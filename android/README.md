# Absolute Zero -> Android (Xash3D FWGS)

Port das libs do mod (client + server) para Android. O APK voce monta com as libs.

## Como usar
1. Copie este pacote por cima da raiz do projeto (ele so traz arquivos novos/alterados).
2. Suba para o GitHub e rode o workflow **Build Android libs (Xash3D FWGS)** (aba Actions).
3. Baixe o artefato `az-android-libs`:
   `arm64-v8a/libclient.so`, `arm64-v8a/libserver.so`, `armeabi-v7a/...`
4. Coloque as libs em `lib/<abi>/` do seu APK do Xash3D FWGS.
   Se o seu launcher espera outros nomes, mude `AZ_CLIENT_NAME` / `AZ_SERVER_NAME`
   (CMake `-D` ou no workflow).

## O que mudou no codigo
- `CMakeLists.txt` + `android/sources_*.cmake`: build com NDK (lista de fontes = a do vcxproj, sem VGUI).
- `android/vgui_stub.h`, `android/voice_status_stub.h`: VGUI e chat de voz viram stubs (`-DNO_VGUI`).
  Sem placar, menus de time/classe, spectator panel e MOTD em VGUI.
- `android/android_compat.h`: forcado em todos os arquivos (stricmp, itoa, min/max, `__declspec`, etc.).
- `android/android_stubs.cpp`: navegador de servidores (winsock) e variaveis que moravam nos arquivos VGUI.
- `cl_dlls/input_android.cpp`: substitui `inputw32.cpp` (mouse via engine; toque e gamepad ficam com o engine).
- Correcoes de portabilidade: `MAKE_STRING` (64 bits), precedencia da macro `STRING`, `gHUD.##y`,
  `extern "C"` em `pm_shared.h`, casts de ponteiro, `Create(..., NULL)` ambiguo, includes com caixa/barra invertida.
- `tri.cpp`: neblina via `pTriAPI->Fog` (sem OpenGL direto). `StudioModelRenderer.cpp`: hook x86 de sombra desativado.

## Verificado aqui (sem NDK/clang)
Os 71 arquivos do client e os 131 do server compilam com GCC e as duas libs linkam sem simbolos faltando.
Exports conferidos: `Initialize`, `HUD_*`, `CL_CreateMove`, `IN_*` (client); `GiveFnptrsToDll`,
`GetEntityAPI`, `GetEntityAPI2` e as entidades (server). **Nao foi compilado com clang/NDK nem testado no aparelho.**

## Se o CI falhar
Mande o log (`build-log-<abi>`). Pontos provaveis: avisos de clang virando erro, `armeabi-v7a` (32 bits), nome das libs.
