// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "ProjectF/Character/BaseCharacter.h"
#include "ProjectF/Player/BasePlayerController.h"
#include "ProjectF/Player/BasePlayerState.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseGameMode)

ABaseGameMode::ABaseGameMode()
{
	PlayerControllerClass = ABasePlayerController::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
	DefaultPawnClass = ABaseCharacter::StaticClass();
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

#if WITH_GAMELIFT
	InitGameLift();
#endif
}

void ABaseGameMode::InitGameLift()
{
#if WITH_GAMELIFT
	UE_LOG(LogTemp, Log, TEXT("Initializing the GameLift Server"));

	//Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	//Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
	// 이때 fleet은 이 서버 프로그램을 가동하는 hosting resources를 나타낸다. AWS EC2 fleet이 될 수도 있고, 직접 서버를 가동하면 개인 하드웨어가 될 수도 있다.
	FServerParameters ServerParameters;

	bool bIsAnywhereActive;
	SetServerParameters(bIsAnywhereActive, ServerParameters);

	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	//Use InitSDK(serverParameters) for a GameLift Anywhere fleet. 
	//Use InitSDK() for a GameLift managed EC2 fleet.
	FGameLiftGenericOutcome InitSdkOutcome;
	if (bIsAnywhereActive)
	{
		InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParameters);
	}
	else
	{
		InitSdkOutcome = GameLiftSdkModule->InitSDK(); 
	}

	if (!InitSdkOutcome.IsSuccess())
	{
		UE_LOG(LogTemp, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogTemp, Log, TEXT("ERROR: InitSDK failed : ("));
		FGameLiftError GameLiftError = InitSdkOutcome.GetError();
		UE_LOG(LogTemp, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
		UE_LOG(LogTemp, SetColor, TEXT("%s"), COLOR_NONE);
		return;
	}

	ProcessParameters = MakeShared<FProcessParameters>();

	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogTemp, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();
	};
	ProcessParameters->OnStartGameSession.BindLambda(OnGameSession);

	// ProcessParameters.OnTerminate : 이 게임 서버의 호스팅 인스턴스가 종료되기 전에 추가 작업을 수행하기 위해 호출된다.
	auto OnProcessTerminate = [=]()
	{
		UE_LOG(LogTemp, Log, TEXT("Game Server process is terminating."));
		GameLiftSdkModule->ProcessEnding();
	};
	ProcessParameters->OnTerminate.BindLambda(OnProcessTerminate);

	//GameLift invokes this callback approximately every 60 seconds.
	// false를 반환하면 서버가 종료된다.
	auto OnHealthCheck = []() 
	{
		UE_LOG(LogTemp, Log, TEXT("Performing Health Check"));
		return true;
	};
	ProcessParameters->OnHealthCheck.BindLambda(OnHealthCheck);

	// 플레이어가 연결할 Port 설정
	// Command Line Arguments로 넘길 경우 해당 Port를 사용하고, 아니라면 UE의 기본 포트인 7777 사용 
	int32 Port = FURL::UrlConfig.DefaultPort;
	ParseCommandLinePort(Port);
	ProcessParameters->port = Port;

	// GameSession Log를 저장할 위치 저장
	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("ProjectF/Saved/Logs/ProjectF.log"));
	ProcessParameters->logParameters = LogFiles;

	//The game server calls ProcessReady() to tell GameLift it's ready to host game sessions.
	UE_LOG(LogTemp, Log, TEXT("Calling Process Ready."));
	FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

    if (!ProcessReadyOutcome.IsSuccess())
    {
    	UE_LOG(LogTemp, SetColor, TEXT("%s"), COLOR_RED);
    	UE_LOG(LogTemp, Log, TEXT("ERROR: Process Ready Failed!"));
    	FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
    	UE_LOG(LogTemp, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
    	UE_LOG(LogTemp, SetColor, TEXT("%s"), COLOR_NONE);
    	return;
    }

	UE_LOG(LogTemp, Log, TEXT("InitGameLift completed!"));
#endif
}

void ABaseGameMode::SetServerParameters(bool& bIsAnywhereActive, FServerParameters& OutServerParameters)
{
#if WITH_GAMELIFT
	// Anywhere Fleet을 사용하는지 체크
	FString AnywhereFleet;
	if (!FParse::Value(FCommandLine::Get(), TEXT("-anywherefleet"), AnywhereFleet))
	{
		bIsAnywhereActive = false;
		return;
	}

	bIsAnywhereActive = true;
	
	//AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	// 서버를 Command Line으로 돌릴 때, Command Line Argument를 이용해 AuthToken 정보를 저장한다.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
	{
		UE_LOG(LogTemp, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
	}
	
	//The Host/compute-name of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
	{
		UE_LOG(LogTemp, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
	}
	
	//The Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
	{
		UE_LOG(LogTemp, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
	}
	
	//The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
	{
		UE_LOG(LogTemp, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
	}
	
	//The PID of the running process
	OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(LogTemp, Log, TEXT("PID: %s"), *OutServerParameters.m_processId);
#endif
}

void ABaseGameMode::ParseCommandLinePort(int32& OutPort)
{
#if WITH_GAMELIFT
	// Command Line에서 Token과 switch로 나눈다.
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	// port=???? 꼴의 Switch를 찾아 OutPort를 해당 Value로 설정한다.
	for (const FString& Switch : CommandLineSwitches)
	{
		FString Key;
		FString Value;
		if (Switch.Split("=", &Key, &Value))
		{
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
			{
				OutPort = FCString::Atoi(*Value);
				return;
			}
		}
	}
#endif
}
