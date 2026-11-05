// Stubs do navegador de servidores (hud_servers.cpp usa winsock + VGUI, nao portado).
#include "hud_servers.h"
#include "hud.h"

// Variaveis que antes eram definidas nos arquivos VGUI (vgui_TeamFortressViewport.cpp / vgui_ScorePanel.cpp)
int g_iTeamNumber;
int g_iUser1;
int g_iUser2;
int g_iUser3;
hud_player_info_t   g_PlayerInfoList[MAX_PLAYERS+1];   // player info from the engine
extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS+1];  // additional player info sent directly to the client dll

// gViewPort e criado em VGui_Startup() (ver android/vgui_stub.h)


void ServersInit( void ) {}
void ServersShutdown( void ) {}
void ServersThink( double ) {}
void ServersCancel( void ) {}
void ServersList( void ) {}
void BroadcastServersList( int ) {}
int ServersGetCount( void ) { return 0; }
const char *ServersGetInfo( int ) { return ""; }
int ServersIsQuerying( void ) { return 0; }
