#include "DedicatedServers.h"
#include "Modules/ModuleManager.h"

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, FPSTemplate, "FPSTemplate" );
IMPLEMENT_MODULE( FDefaultModuleImpl, DedicatedServers );

/*
we did this in GAS course (at least Stephen does, declare it in .h and define it in "project.cpp" )
DECLARE_LOG_CATEGORY_EXTERN(CustomLogName[/LogTemp/...] , [Log(white)/Warning(yellow)/Error(red)] , All  ) 

DEFINE_LOG_CATEGORY(CustomLogName).
 */
DEFINE_LOG_CATEGORY(GameServerLog);