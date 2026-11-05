# Half-Life: Absolute Zero -> Android (Xash3D FWGS)
# Gera libclient_android_<arch>.so e libaz_android_<arch>.so para armeabi-v7a / arm64-v8a.
#
# Uso (NDK):
#   cmake -S . -B build -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 -DCMAKE_BUILD_TYPE=Release
#   cmake --build build
cmake_minimum_required(VERSION 3.18)
project(az_android C CXX)

# O Xash3D FWGS procura as libs com o nome <base>_android_<arch>.so (ex.: libaz_android_arm64.so
# para o server, que vem do "gamedll" do liblist.gam, e libclient_android_arm64.so para o client).
set(AZ_CLIENT_NAME "client" CACHE STRING "Nome base da lib do client")
set(AZ_SERVER_NAME "az" CACHE STRING "Nome base da lib do server (= nome do gamedll no liblist.gam)")
set(AZ_LIB_SUFFIX "" CACHE STRING "Sufixo da arquitetura; vazio = automatico pelo ANDROID_ABI")
if(NOT AZ_LIB_SUFFIX)
    if(ANDROID_ABI STREQUAL "arm64-v8a")
        set(AZ_LIB_SUFFIX "_android_arm64")
    elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
        set(AZ_LIB_SUFFIX "_android_armv7a")
    elseif(ANDROID_ABI STREQUAL "x86_64")
        set(AZ_LIB_SUFFIX "_android_amd64")
    elseif(ANDROID_ABI STREQUAL "x86")
        set(AZ_LIB_SUFFIX "_android_i386")
    endif()
endif()
option(AZ_BUILD_CLIENT "Compilar a lib do client" ON)
option(AZ_BUILD_SERVER "Compilar a lib do server" ON)

include(${CMAKE_SOURCE_DIR}/android/sources_client.cmake)
include(${CMAKE_SOURCE_DIR}/android/sources_server.cmake)

# Registrador de crash (grava az_crash.log com a pilha de chamadas)
list(APPEND AZ_CLIENT_SOURCES android/crash_handler.cpp)
list(APPEND AZ_SERVER_SOURCES android/crash_handler.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(AZ_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/android
    ${CMAKE_SOURCE_DIR}/cl_dlls
    ${CMAKE_SOURCE_DIR}/cl_dlls/hud
    ${CMAKE_SOURCE_DIR}/cl_dlls/hl
    ${CMAKE_SOURCE_DIR}/dlls
    ${CMAKE_SOURCE_DIR}/dlls/entity
    ${CMAKE_SOURCE_DIR}/dlls/weapon
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/pm_shared
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_SOURCE_DIR}/game_shared
    ${CMAKE_SOURCE_DIR}/game_shared/util
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/utils/common
)

set(AZ_COMMON_DEFS NO_VGUI AZ_ANDROID _LINUX LINUX POSIX CLIENT_WEAPONS)
set(AZ_COMMON_FLAGS
    "SHELL:-include ${CMAKE_SOURCE_DIR}/android/android_compat.h"
    -fno-strict-aliasing
    -w                          # codigo de 2000-2005: silencia warnings para o log ficar legivel
)
set(AZ_CXX_FLAGS
    -fms-extensions             # 'extra qualification' em construtores, etc. (aceito pelo MSVC)
    -Wno-c++11-narrowing
    -Wno-invalid-offsetof
    -Wno-address-of-temporary   # (float*)&Vector(0,0,0): o temporario vive ate o fim da expressao
    -Wno-non-pod-varargs
    -fexceptions -frtti
)

set(AZ_C_FLAGS
    -Wno-implicit-function-declaration
    -Wno-incompatible-pointer-types
    -Wno-int-conversion
    -Wno-int-to-pointer-cast
)

function(az_setup target outname)
    target_include_directories(${target} PRIVATE ${AZ_INCLUDES})
    target_compile_definitions(${target} PRIVATE ${AZ_COMMON_DEFS})
    target_compile_options(${target} PRIVATE ${AZ_COMMON_FLAGS}
        $<$<COMPILE_LANGUAGE:CXX>:${AZ_CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:C>:${AZ_C_FLAGS}>)
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME ${outname}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/${ANDROID_ABI})
    # vec3_origin / vJumpOrigin / vJumpAngles sao definidos em mais de um arquivo (pm_shared + common/view),
    # o MSVC do projeto original tolerava isso.
    target_link_options(${target} PRIVATE -Wl,--allow-multiple-definition)
    target_link_libraries(${target} PRIVATE m dl log)
endfunction()

if(AZ_BUILD_CLIENT)
    add_library(az_client SHARED ${AZ_CLIENT_SOURCES})
    target_compile_definitions(az_client PRIVATE CLIENT_DLL)
    az_setup(az_client ${AZ_CLIENT_NAME}${AZ_LIB_SUFFIX})
endif()

if(AZ_BUILD_SERVER)
    add_library(az_server SHARED ${AZ_SERVER_SOURCES})
    target_compile_definitions(az_server PRIVATE VALVE_DLL QUIVER VOXEL QUAKE2)
    az_setup(az_server ${AZ_SERVER_NAME}${AZ_LIB_SUFFIX})
endif()
