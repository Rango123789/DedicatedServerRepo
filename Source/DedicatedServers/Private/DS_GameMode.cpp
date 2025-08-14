// Fill out your copyright notice in the Description page of Project Settings.


#include "DS_GameMode.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

#include "GenericPlatform/GenericPlatformOutputDevices.h"

/*
we did this in GAS course (at least Stephen does, and define it in "project.h" )
DECLARE_LOG_CATEGORY_EXTERN(CustomLogName[/LogTemp/...] , [Log(white)/Warning(yellow)/Error(red)] , All  ) 

DEFINE_LOG_CATEGORY(CustomLogName).
 */
DEFINE_LOG_CATEGORY(GameServerLog);

//don't know why AWS try to find the DefaultPawnClass, isn't already set in BP_GameMode?
ADS_GameMode::ADS_GameMode() :
    ProcessParams(nullptr)
{
    /*UPDATE: this is bad practice, we directly set them in BP__GameMode, lol! Stephen verified it:
        // Set default pawn class to our Blueprinted character
            // the keyword **static** means that the variable PlayerPawnBPClass is only initialized once
            //the first time the constructor ADS_GameMode::ADS_GameMode() is called — no matter how many times this constructor is later executed
            //Performance Optimization: Loading a class reference from disk (especially with ConstructorHelpers) can be expensive. Making it static ensures that this work is done only once.
            //Caching: The result of the class lookup is stored and reused on subsequent calls to the constructor, avoiding repeated class lookups.

            static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));

        //GM::DefaultPawnClass, DefaultPlayerStateClass... are members of GM
            if (PlayerPawnBPClass.Class != NULL)
            {
                DefaultPawnClass = PlayerPawnBPClass.Class;
            }

            UE_LOG(GameServerLog, Log, TEXT("Initializing ADS_GameMode..."));
    */
}

void ADS_GameMode::BeginPlay()
{
    Super::BeginPlay();

/*this only active if 
  - "Development Server" mode is selected in Rider/VS
  -  GameLiftSDK/Pluggin is there (I guess)*/
#if WITH_GAMELIFT
    InitGameLift();
#endif
}

void ADS_GameMode::InitGameLift()
{
#if WITH_GAMELIFT
  
/*step1:  Loading the 'GameLiftServerSDK' module, store it into a pointer of its representative module type.*/
    UE_LOG(GameServerLog, Log, TEXT("Calling InitGameLift...")); //that is the HOSTING function lol
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

/*Step2: Define the ServerParameters for a GameLift Anywhere fleet (can be local var). These are not needed for a GameLift managed EC2 fleet.*/
    FServerParameters ServerParams; 


// default we default back to "old" code, so there wont be any "-glAnywhereFleet" any more to trying to check on lol!
//    if (bIsAnywhere)    
//    {
        UE_LOG(GameServerLog, Log, TEXT("Configuring server parameters for Anywhere..."));
     
        FParse::Value(FCommandLine::Get() , TEXT("-authtoken="), ServerParams.m_authToken);
        FParse::Value(FCommandLine::Get() , TEXT("-fleetid="), ServerParams.m_fleetId);
        FParse::Value(FCommandLine::Get() , TEXT("-hostid="), ServerParams.m_hostId);
        FParse::Value(FCommandLine::Get() , TEXT("-websocketurl="), ServerParams.m_webSocketUrl);
        
        FString glAnywhereProcessId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("-processid="), glAnywhereProcessId))
        {
            ServerParams.m_processId = TCHAR_TO_UTF8(*glAnywhereProcessId);
        }
        else
        {
            // If no ProcessId is passed as a command line argument, generate a randomized unique string.
            FString TimeString = FString::FromInt(std::time(nullptr));
            FString ProcessId = "ProcessId_" + TimeString;
            ServerParams.m_processId = TCHAR_TO_UTF8(*ProcessId);
        }

        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_YELLOW);
        UE_LOG(GameServerLog, Log, TEXT(">>>> WebSocket URL: %s"), *ServerParams.m_webSocketUrl);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Fleet ID: %s"), *ServerParams.m_fleetId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Process ID: %s"), *ServerParams.m_processId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParams.m_hostId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Auth Token: %s"), *ServerParams.m_authToken);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Aws Region: %s"), *ServerParams.m_awsRegion);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Access Key: %s"), *ServerParams.m_accessKey);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Secret Key: %s"), *ServerParams.m_secretKey);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Session Token: %s"), *ServerParams.m_sessionToken);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
//    }

    UE_LOG(GameServerLog, Log, TEXT("Initializing the GameLift Server..."));

/*Step3: call GameLiftSDKModule::InitSDK(ServerParameters) will establish a local connection with GameLift's agent to enable further communication.*/
    bool bIsAnywhere =
        !ServerParams.m_fleetId.IsEmpty()
        && !ServerParams.m_hostId.IsEmpty()
        && !ServerParams.m_authToken.IsEmpty()
        && !ServerParams.m_webSocketUrl.IsEmpty();

    //I update the code in case of EC2 as well
    FGameLiftGenericOutcome InitSdkOutcome;
    if (bIsAnywhere) InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParams);    
    else                   InitSdkOutcome = GameLiftSdkModule->InitSDK();   

    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(GameServerLog, Log, TEXT("GameLift InitSDK succeeded!"));
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(GameServerLog, Log, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(GameServerLog, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
        return;
    }

/*Step4A: Bind your custom callbacks with custom reponses/behavivors with FProcessParameters::delegates (it is currently as member) when "the session is created/started, updated, terminated.*/

    ProcessParams = MakeShared<FProcessParameters>();

    ProcessParams->OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(GameServerLog, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    ProcessParams->OnTerminate.BindLambda([=]()
        {
            UE_LOG(GameServerLog, Log, TEXT("Game Server Process is terminating"));
            GameLiftSdkModule->ProcessEnding();
        });

    ProcessParams->OnHealthCheck.BindLambda([]()
        {
            UE_LOG(GameServerLog, Log, TEXT("Performing Health Check"));
            return true; /*In this case, we're always healthy!*/
        });
    
/*Step4B: Ready ProcessParameters::port, LogsParameters...*/
    ProcessParams->port = FURL::UrlConfig.DefaultPort;
    
    TArray<FString> CommandLineTokens; //we didn't
    TArray<FString> CommandLineSwitches;
    FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

    for (FString SwitchStr : CommandLineSwitches)
    {
        FString Key;
        FString Value;

        if (SwitchStr.Split("=", &Key, &Value))
        {
            //== is always strictly case-senstive, so we use FString::Equals for some leeway!
            if (Key.Equals("port" , ESearchCase::IgnoreCase))
            {
                ProcessParams->port = FCString::Atoi(*Value);
                break;
            }
        }
    }

    TArray<FString> Logfiles;
    Logfiles.Add("FPSTemplate/Saved/Logs/FPSTemplate.log");
    ProcessParams->logParameters = Logfiles;

    UE_LOG(GameServerLog, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParams);

    //log out the outcome:
    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(GameServerLog, Log, TEXT("Process Ready!"));
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(GameServerLog, Log, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(GameServerLog, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(GameServerLog, Log, TEXT("InitGameLift completed!"));
#endif
}   

/*Need a test to prove it:
void ADS_GameMode::InitGameLift_Practice()
{

//stage1: load the "GameLiftServerSDK" module from Plugins/..../"GameLiftServerSDK", store it into a pointer of its module-representative type 
    FGameLiftServerSDKModule* GameLiftServerSDKModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
    
//stage2: init connections with AWS GameLift Servers (backend + EC2 server packed game (if non-anywhere)) by calling "InitSDK":
    FServerParameters ServerParams;
   bool bIsAnywhereFleet = FParse::Param(FCommandLine::Get(), TEXT("glAnywhere"));
    //I check the FParse::Value( , , &Out) , it will only change Out when the internal evaluation return true, so doing these directly will make the code much cleaner! yeah!
   if (bIsAnywhereFleet)
   {
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereAccessKey="), ServerParams.m_accessKey);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereAuthToken="), ServerParams.m_authToken);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereAwsRegion="), ServerParams.m_awsRegion);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereFleetId="), ServerParams.m_fleetId);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereProcessId="), ServerParams.m_processId);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereHostId="), ServerParams.m_hostId);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereSecretKey="), ServerParams.m_secretKey);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereSessionToken="), ServerParams.m_sessionToken);
       FParse::Value(FCommandLine::Get() , TEXT("glAnywhereWebSocketUrl="), ServerParams.m_webSocketUrl);
   }

//stage3: either of the case we must init connection with AWS Servers by the API InitSKD call:
   if (bIsAnywhereFleet) GameLiftServerSDKModule->InitSDK(ServerParams);
   else                  GameLiftServerSDKModule->InitSDK();

//stage4: the order you bind callback didn't matter, note that OnStartGameSession's and OnTerminate's callbacks need their own conventional calls at the end, so that AWS Servers know it:
   ProcessParams = MakeShared<FProcessParameters>();    

   ProcessParams->OnStartGameSession.BindLambda(
    [=](Aws::GameLift::Server::Model::GameSession GameSession)
        {
            //your custom code:

            //conventional call for this OnStartGameSession's callback:
            GameLiftServerSDKModule->ActivateGameSession();
        }
   );

    ProcessParams->OnTerminate.BindLambda(
 [=]()
         {
             //your custom code:

             //conventional call for this OnStartGameSession's callback:
             GameLiftServerSDKModule->ProcessEnding();
         }
    );

    //this is 'DECLARE_DELEGATE_RetVal(bool, FOnHealthCheck)', so it requires the callback to return bool
    ProcessParams->OnHealthCheck.BindLambda(
    []()
        {
            //your checking and custom return:

            //this is the default if you didn't return anything above
            return true;
        }
    );

    //stephen didn't add this yet:
    ProcessParams->OnUpdateGameSession.BindLambda(
        [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSession)
        {
            //your custom code:
        }
    );

    ProcessParams->port = FURL::UrlConfig.DefaultPort;
    FParse::Value(FCommandLine::Get() , TEXT("port="), ProcessParams->port);

    //to be exact, we should call it LogFilePaths?
    TArray<FString> LogFiles;
    LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
    ProcessParams->logParameters = LogFiles;

//stage5: call Module->ProcessReady(ProcessParams) and see if it succeeds
    FGameLiftGenericOutcome GameLiftGenericOutcome = GameLiftServerSDKModule->ProcessReady(*ProcessParams);

    if(GameLiftGenericOutcome.IsSuccess())
    {
        UE_LOG(GameServerLog,Log, TEXT("Process Ready!") )
    }
    else
    {
        UE_LOG(GameServerLog, Log, TEXT("ERROR: Process Ready Failed!"));
    }
}

*/

/****OVERAL STEPs:
    //stage1:
    FGameLiftServerSDKModule* GameLiftModule = FModuleManager::LoadModuleChecked<T>("GameLiftServerSDK") ;

    //stage2: for Anywhere Fleet. EC2 fleet dont need ServerParams, 
    FServerParameters ServerParams;
    ServerParameters.var_i = ;

    Warning: you can't return at stage2, because you still need to do stage3+ in case it is EC2 fleet lol!
    
    //stage3: init connections with AWS GameLift Servers (backend + EC2 server packed game (if non-anywhere)) by calling "InitSDK": 
    if(AnywhereFleet) GameLiftModule->InitSDK(ServerParams);
    else GameLiftModule->InitSDK()
    
    //stage4: bind your custom callbacks and tell AWS GameLift Servers it's all done in the end by calling "ProcessReady()":
    FProcessParameters ProcessParams;
    ProcessPrams.delegate_i.AddLambda( [](_Depends_)
      {
        //your custom actions:
          ...
        //conventional calls to let AWS know when you ready to start/activate game session (after it has been OFFICALLY created) or to close the session (either by you or because game server crash or else):
        GameLiftModule->ActivateGameSession(), ProcessEnding()
      });
    GameLiftModule->ProcessReady(ProcessParams)
 */

/*BACKUP before default back to old versio
void ADS_GameMode::InitGameLift()
{
#if WITH_GAMELIFT
  
/*step1:  Loading the 'GameLiftServerSDK' module, store it into a pointer of its representative module type.#1#
    UE_LOG(GameServerLog, Log, TEXT("Calling InitGameLift...")); //that is the HOSTING function lol

    /*This method checks whether the module actually exists. If the module does not exist, an assertion will be triggered.
      * If the module was already loaded previously, the existing instance will be returned.
      * "GameLiftServerSDK" does exist  in Plugins/GameLiftPlugin/Source /GameLiftServer/Source/GameLiftServerSDK/GameLiftServerSDK.Build.cs
      * but It is surprised that that module is of type 'FGameLiftServerSDKModule'?
     #1#
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

/*Step2: Define the ServerParameters for a GameLift Anywhere fleet (can be local var). These are not needed for a GameLift managed EC2 fleet.#1#

    /*do NOT confuse with FProcessParameters (that is much longer class), they defined in different header files even.
    FServerParams:: ( has these members to be assigned)
        AuthToken, SessionToken;
        AccessKey, SecretKey;
        HostId, FleetId, ProcessId;
        WebSocketURL;
        awsRegion;
    #1#
    FServerParameters ServerParametersForAnywhere; 

    /*you gotta run your game via command like at first place:
    "path to your game .exe" -glAnywhereAuthToken="argument1"
                             -glAnywhereWebSocketUrl="argument2"
                             -glAnywhereFleetId="argument3"
                             -glAnywhereProcessId="argument4"
                             -glAnywhereHostId="argument5"
    - conventions:
         -Xi= "argument_i" - think of Xi is pre-defined variable of command lines itself.
         its could be less arguments, that some of them will be auto-stored as we call some KEY arguments.
    - Those arguments are saved into some file.
    
    - FParse::Value(FCommandLine::Get(), TEXT("X_i=") , C++_var) will assign "C++_var = argument_i from the saved file (we dont care how, it is pretty much like the save game system in GAS right?)
         Why "Xi="? not "Xi"? well it is just command convention in the case you want to assign that value for third-party variable (here C++) from the command line perspective.
         If it returns "true", it means we did successfully pass in an AuthToken, it doesn't necessarily mean the passed-in token is correct
    - FParse::Param(FCommandLine::Get(), TEXT("Xi")) check if the specified command-line argument "Xi" exist in the stream (i.e without propagating its value to C++_var at all this time).
    #1#
    bool bIsAnywhereActive = false;
    if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
    {
        bIsAnywhereActive = true;
    }

    if (bIsAnywhereActive)
    {
        UE_LOG(GameServerLog, Log, TEXT("Configuring server parameters for Anywhere..."));

        // If GameLift Anywhere is enabled, parse command line arguments and pass them in the ServerParameters object.
        FString glAnywhereWebSocketUrl = "";
        
        /*the websocket URL to communicate to GameLift; the GameLift server SDK uses websockets so it needs to know the websocket URL
         *in other words an end point that it can use to access GameLift so this is how it's going to communicate with GameLift so we can basically copy these - sound like between "server game" and "backend: GameLift", not between client games and sever game#1#
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereWebSocketUrl="), glAnywhereWebSocketUrl))
        {
            ServerParametersForAnywhere.m_webSocketUrl = TCHAR_TO_UTF8(*glAnywhereWebSocketUrl);
        }

        FString glAnywhereFleetId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereFleetId="), glAnywhereFleetId))
        {
            ServerParametersForAnywhere.m_fleetId = TCHAR_TO_UTF8(*glAnywhereFleetId);
        }

        FString glAnywhereProcessId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereProcessId="), glAnywhereProcessId))
        {
            ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*glAnywhereProcessId);
        }
        else
        {
            // If no ProcessId is passed as a command line argument, generate a randomized unique string.
            FString TimeString = FString::FromInt(std::time(nullptr));
            FString ProcessId = "ProcessId_" + TimeString;
            ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*ProcessId);
        }

        FString glAnywhereHostId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereHostId="), glAnywhereHostId))
        {
            ServerParametersForAnywhere.m_hostId = TCHAR_TO_UTF8(*glAnywhereHostId);
        }

        //The WebSocket URL (GameLiftServerSDKEndPoint)
            /*so we're going to have to make it Anywhere Fleet in "the AWS console" specifying that we'll be using "our build of the Unreal Engine dedicated server project" and that has to be passed in as a command line argument as well#1#
        FString glAnywhereAuthToken = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAuthToken="), glAnywhereAuthToken))
        {
            ServerParametersForAnywhere.m_authToken = TCHAR_TO_UTF8(*glAnywhereAuthToken);
        }

        FString glAnywhereAwsRegion = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAwsRegion="), glAnywhereAwsRegion))
        {
            ServerParametersForAnywhere.m_awsRegion = TCHAR_TO_UTF8(*glAnywhereAwsRegion);
        }

        FString glAnywhereAccessKey = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAccessKey="), glAnywhereAccessKey))
        {
            ServerParametersForAnywhere.m_accessKey = TCHAR_TO_UTF8(*glAnywhereAccessKey);
        }

        FString glAnywhereSecretKey = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSecretKey="), glAnywhereSecretKey))
        {
            ServerParametersForAnywhere.m_secretKey = TCHAR_TO_UTF8(*glAnywhereSecretKey);
        }

        FString glAnywhereSessionToken = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSessionToken="), glAnywhereSessionToken))
        {
            ServerParametersForAnywhere.m_sessionToken = TCHAR_TO_UTF8(*glAnywhereSessionToken);
        }

        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_YELLOW);
        UE_LOG(GameServerLog, Log, TEXT(">>>> WebSocket URL: %s"), *ServerParametersForAnywhere.m_webSocketUrl);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Fleet ID: %s"), *ServerParametersForAnywhere.m_fleetId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Process ID: %s"), *ServerParametersForAnywhere.m_processId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParametersForAnywhere.m_hostId);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Auth Token: %s"), *ServerParametersForAnywhere.m_authToken);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Aws Region: %s"), *ServerParametersForAnywhere.m_awsRegion);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Access Key: %s"), *ServerParametersForAnywhere.m_accessKey);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Secret Key: %s"), *ServerParametersForAnywhere.m_secretKey);
        UE_LOG(GameServerLog, Log, TEXT(">>>> Session Token: %s"), *ServerParametersForAnywhere.m_sessionToken);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(GameServerLog, Log, TEXT("Initializing the GameLift Server..."));

/*Step3: call GameLiftSDKModule::InitSDK(ServerParameters) will establish a local connection with GameLift's agent to enable further communication.
 *       call InitSDK(__) for AnywhereFleet and InitSDK() for EC2 fleet (I'm not sure if the update code will default back to InitSDK() from InitSDK(__) in case it is EC2 fleet, but better off be careful
 *       
 *FGameLiftSDKModule::InitSDK(), ProcessReady() is uniquely defined for that module type (unlike IModuleInterface::StartupModule(), ShutdownModule() overriden from its conventional parent)
 *The call return "FGameLiftGenericOutcome" that has values help you know whether the initialization of th module succeeds or not
 *-fail when:  ServerParameters::AuthToken or else isn't correct or invalid
 *-succeed when: all ServerParameters::vars are valid and correct#1#
    //I update the code in case of EC2 as well
    FGameLiftGenericOutcome InitSdkOutcome;
    if (bIsAnywhereActive) InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);    
    else                   InitSdkOutcome = GameLiftSdkModule->InitSDK();   

    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(GameServerLog, Log, TEXT("GameLift InitSDK succeeded!"));
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(GameServerLog, Log, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(GameServerLog, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
        return;
    }

/*Step4A: Bind your custom callbacks with custom reponses/behavivors with FProcessParameters::delegates (it is currently as member) when "the session is created/started, updated, terminated.

[DOC, modified by RAN] The following delegates that allow Amazon GameLift Servers to send messages or prompts to a game server process. Your must implement each of these functions in your game server code. For more information, see ProcessParameters(C++) (C#) (Unreal) (Go) .    
    (Optional) OnHealthCheck – Amazon GameLift Servers calls this delegate regularly to request a health status report from the server.
    
    OnStartGameSession – Amazon GameLift Servers calls this delegate in response to the client request CreateGameSession().
    
    oOnProcessTerminate – Amazon GameLift Servers forces the server process to stop, letting it shut down gracefully.
    
    (Optional) OnUpdateGameSession – Amazon GameLift Servers delivers an updated game session object to the game server or provides a status update on a match backfill request. The FlexMatch backfill feature requires this callback.
 #1#
    ProcessParams = MakeShared<FProcessParameters>();

    /*When a game session is created, Amazon GameLift Servers sends an activation request to the game server and passes along the game session object containing game properties and other settings.
     Here is where a game server should take action based on the game session object.
     Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()#1#
    ProcessParams->OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(GameServerLog, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    /*OnProcessTerminate callback. Amazon GameLift Servers will invoke this callback before shutting down an instance hosting this game server.
      It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
      In this case, we simply tell Amazon GameLift Servers we are indeed going to shutdown.#1#
    ProcessParams->OnTerminate.BindLambda([=]()
        {
            UE_LOG(GameServerLog, Log, TEXT("Game Server Process is terminating"));
            GameLiftSdkModule->ProcessEnding();
        });

    /*  This is the HealthCheck callback.
      Amazon GameLift Servers will invoke this callback every 60 seconds or so.
      Here, a game server might want to check the health of dependencies and such.
      Simply return true if healthy, false otherwise.
      The game server has 60 seconds to respond with its health status. Amazon GameLift Servers will default to 'false' if the game server doesn't respond in time.
      -->if the game server crashes or end itself without calling ProcessEnding() GameLift will no longer receive true and so it will end "server process" too.
      -->EITHER process heath=unhealthy OR didn't response for 3 consecutive times, the AWS Servers will end the server process. 
      #1#
    ProcessParams->OnHealthCheck.BindLambda([]()
        {
            UE_LOG(GameServerLog, Log, TEXT("Performing Health Check"));
            return true; /*In this case, we're always healthy!#1#
        });
    
/*Step4B: Ready ProcessParameters::port, LogsParameters...#1#
    //GameServer.exe -port=7777 LOG=server.mylog
    ProcessParams->port = FURL::UrlConfig.DefaultPort;
    
    TArray<FString> CommandLineTokens; //we didn't
    TArray<FString> CommandLineSwitches;
    /* if you first run: "Path/to/GameServer.exe" -key1=value1 -key2=value2 -key3=value3
     ; then FCommandLine::Parse(FCommandLine::Get(), StringArray1, StringArray2 ) will result in this:
     StringArray1 = {} (empty array -- only in this case)
     StringArray2 = {"-key1=value1", "-key2=value2", "-key3=value3"}
     *Overal:
    StringArray1 → Positional arguments (e.g. file names, bare strings)
    StringArray2 → Option flags starting with - (key-value pairs, switches)

    *A positional argument is a command-line input that:
    -Does not start with - or --
    -Is identified by its position/order in the command
    -Usually passed before or after options

    GameServer.exe MyMap.umap -port=7777 -log
    <=>MyMap.umap → positional argument (StringArray1)

    myscript.sh input.txt output.txt -verbose
    <=>input.txt, output.txt → positional arguments

    UE4Editor-Cmd.exe MyProject.uproject MyMap.umap -run=Cook -targetplatform=WindowsNoEditor
    <=>MyProject.uproject and MyMap.umap → positional arguments
     #1#
    FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

    /* FString::Split( [Separator], OutString1 , OutString2)
       FString string = "string1[Separator]string2"
       string.Split( [Separator], OutString1 , OutString2)
       <=> OutString1 = string1, OutString2 = string2
     * In commandline: "Path/to/file.exe" -a=b
       '-a' is called a flag, option, or switch (so 'a' is called "option name", "flag name", or "key")
        'b' is "the value" or "argument" assigned to that flag
       '-a=b'	option-value pair or key-value pair
       #1#
    for (FString SwitchStr : CommandLineSwitches)
    {
        FString Key;
        FString Value;

        if (SwitchStr.Split("=", &Key, &Value))
        {
            //== is always strictly case-senstive, so we use FString::Equals for some leeway!
            if (Key.Equals("port" , ESearchCase::IgnoreCase))
            {
                ProcessParams->port = FCString::Atoi(*Value);
                break;
            }
        }
    }

    /*My idea, need VERIFIED:
    int32 PortNumber = 0;
    if(FParse::Value( FCommandLine::Get(), TEXT("port=") , PortNumber ))
    {
        ProcessParameters->port = PortNumber;
    }
    
    FString PortNumberInString;
    if(FParse::Value( FCommandLine::Get(), TEXT("port=") , PortNumberInString ))
    {
        ProcessParameters->port =FCString::Atoi(*PortNumberInString);
    }
    #1#    

    //Here, "the game server" tells "Amazon GameLift Servers" where to find "game session log files".
    //At the end of a game session, Amazon GameLift Servers uploads everything in the specified 
    //location and stores it in the cloud for access later.
    TArray<FString> Logfiles;
    //this is incorrect of the DOC:
        //Logfiles.Add(TEXT("GameServerLog/Saved/Logs/GameServerLog.log"));
    //this is correct one: TEXT("ProjectName/Saved/Logs/ProjectName.log") - if you check the relative path to the log file in your project it will be a DYNAMIC log file that is overriden everytime you compile/run the game (and the last version one will be stored as _backup versions, yeah! )
    Logfiles.Add("FPSTemplate/Saved/Logs/FPSTemplate.log");
    ProcessParams->logParameters = Logfiles;

/*Step5: call GameLiftSdkModule->ProcessReady(__) passing in "ProcessParameters" (that has its delegates fully bound to your custom callbacks) to tell Amazon GameLift Servers it's ready to host game sessions (so after the GameLift Servers knows it, you can actually "create session" - which will learn in some next lesson):
 * GameLift will receive this [ProcessParameters::delegates] and auto-broadcast those delegates (in turn trigger bound callbacks) when appropriate.
 * However, you didn't actually "create any session" yet nor did you "end any session yet" (not sure how, but we will learn this don't worry)
 * As I said, FGameLiftServerSDKModule::ProcessReady() is uniquely defined by FGameLiftServerSDKModule.
 *
 * Warning: we also have Aws::GameLift::Internal::GameLiftServerState::OnStartGameSession, OnUpdateGameSession, OnTerminateProcess that are functions (not delegates), I suppose they will also be auto-called by AWS GameLift at the same time 
 - I did NOT find any clue that those functions in turn call FProcessParameters::delegates, but regardless your custom callbacks bound to  FProcessParameters::delegates and built-in functions of the same name should be auto-called at very similar/same time I believe!
 - we didn't even learn about the 'GameLiftServerState' class yet
 #1#
    //The game server calls ProcessReady() to tell Amazon GameLift Servers it's ready to host game sessions.
    UE_LOG(GameServerLog, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParams);

    //log out the outcome:
    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(GameServerLog, Log, TEXT("Process Ready!"));
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(GameServerLog, Log, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(GameServerLog, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
        UE_LOG(GameServerLog, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(GameServerLog, Log, TEXT("InitGameLift completed!"));
#endif
}   */


