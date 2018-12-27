#pragma once
#ifndef MAIN_H
#define MAIN_H

//=====================================================================================
class IApplicationContext;

//=====================================================================================
#define INIT_MANAGER( NAME, ... ) NAME ::Instance().Init( __VA_ARGS__ )
#define INITPOST_MANAGER( NAME ) NAME ::Instance().InitPost()
#define TICK_MANAGER( NAME, DT ) NAME ::Instance().Tick( DT )
#define FINIT_MANAGER( NAME ) NAME ::Instance().Finalise()

//=====================================================================================
int Launch(  IApplicationContext * a_ClientAppCtx, int a_Args, char *a_Argv[] );

#endif//MAIN_H