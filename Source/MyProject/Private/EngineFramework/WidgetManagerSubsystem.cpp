// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineFramework/WidgetManagerSubsystem.h"
#include "UserWidgets/MainMenu.h"
#include "UserWidgets/Modals/SessionFoundModal.h"
#include "Blueprint/UserWidget.h"
#include "EngineFramework/MultiplayerSessionSubsystem.h"


UWidgetManagerSubsystem::UWidgetManagerSubsystem()
{
	const ConstructorHelpers::FClassFinder<UMainMenu> MainMenuWidgetClass(
		TEXT("/Game/Blueprints/Widgets/WGT_MenuSwitcher"));
	if (!ensure(MainMenuWidgetClass.Class != nullptr))
		return;

	const ConstructorHelpers::FClassFinder<USessionFoundModal> SessionFoundWidgetClass(TEXT("/Game/Blueprints/Widgets/CustomWidgets/WGT_SessionFoundModal"));
	if(!ensure(SessionFoundWidgetClass.Class != nullptr))
		UE_LOG(LogTemp, Warning, TEXT("Could not find specied Session Found Modal"))

	const ConstructorHelpers::FClassFinder<UModalBase> QuitGameWidgetClass(TEXT("/Game/Blueprints/Widgets/CustomWidgets/WGT_VerificationModal"));
	if(!ensure(QuitGameWidgetClass.Class != nullptr))
		UE_LOG(LogTemp, Warning, TEXT("Could not find specied Modal"))
	
	MainMenuClass = MainMenuWidgetClass.Class;
	SessionFoundModalClass = SessionFoundWidgetClass.Class;
	QuitGameModalClass = QuitGameWidgetClass.Class;

	
}

void UWidgetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ISessionController* Controller = GetGameInstance()->GetSubsystem<UMultiplayerSessionSubsystem>();
	if(!ensure(Controller != nullptr))
		return;

	WidgetController = Controller;
	WidgetController->GetOnSessionFound().AddUObject(this, &UWidgetManagerSubsystem::OnSessionsFound_Callback);
}

void UWidgetManagerSubsystem::SetWidgetController(ISessionController* InController)
{
	if (!ensure(WidgetController != nullptr))
	{
		return;
	}
	WidgetController = InController;
}

void UWidgetManagerSubsystem::ShowMainMenu()
{
	if (!ensure(MainMenuClass != nullptr))
		return;
	
	if(!ensure(WidgetController != nullptr))
		return;

	MainMenu = CreateWidget<UMainMenu>(GetGameInstance(), MainMenuClass);
	if (!ensure(MainMenu != nullptr))
		return;


	MainMenu->SetSessionController(WidgetController);
	MainMenu->AddToViewport();
	MainMenu->bIsFocusable = true;
	MainMenu->OnSessionCreation.AddDynamic(this, &UWidgetManagerSubsystem::OnSessionCreated_Callback);
	MainMenu->OnQuitButtonPressed.AddDynamic(this, &UWidgetManagerSubsystem::OnQuitButtonPressed_Callback);

	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller could not be retrieved for input mode setup"))
		return;
	}
	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
	
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
}

void UWidgetManagerSubsystem::ShowSessionFoundModal(USessionFoundModal* Modal)
{
	FString NameOfTheSession;
	if (!FoundSessions[CurrentSessionIndex].Session.SessionSettings.Get(TEXT("SessionName"),NameOfTheSession))
	{
		NameOfTheSession = "DefaultName";
	}
		
	Modal->SetSessionName(NameOfTheSession);
	Modal->SetAvailableSlots(
		FoundSessions[CurrentSessionIndex].Session.NumOpenPublicConnections,
		FoundSessions[CurrentSessionIndex].Session.SessionSettings.NumPublicConnections);
	
	Modal->OnModalInteracted.AddUFunction(this, "OnSessionInteraction_Callback");
	Modal->AddToViewport();
	Modal->StartTimer();
}

void UWidgetManagerSubsystem::HandleFoundSessions()
{
	USessionFoundModal* SessionModal = CreateWidget<USessionFoundModal>(GetGameInstance(), SessionFoundModalClass);
	if(!ensure(SessionModal != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Session Modal. Abording!"))
		return;
	}

	ShowSessionFoundModal(SessionModal);
	
}

void UWidgetManagerSubsystem::OnSessionCreated_Callback()
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller could not be retrieved for input mode setup"))
		return;
	}
	
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
}

void UWidgetManagerSubsystem::OnSessionsFound_Callback(const TArrayView<FOnlineSessionSearchResult>& Sessions, bool bIsSuccessful)
{
	if(bIsSuccessful)
	{
		CurrentSessionIndex = 0;
		FoundSessions = Sessions;
		MainMenu->ShowFindingSessionsWidget(false);
		HandleFoundSessions();
	}
	else
	{
		MainMenu->ShowFindingSessionsWidget(false);
		MainMenu->ShowFindSessionFailed(true);
		FTimerHandle HideFailWidgetTimerHandle;
		FTimerDelegate HideFailWidget;
		HideFailWidget.BindLambda([this]()
		{
			MainMenu->ShowFindSessionFailed(false);
		});
		GetWorld()->GetTimerManager().SetTimer(HideFailWidgetTimerHandle, HideFailWidget, 2.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("Session Find failed."))
	}

}

void UWidgetManagerSubsystem::OnSessionJoin_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Type::Success)
	{
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
		if (!ensure(PlayerController != nullptr))
		{
			UE_LOG(LogTemp, Error, TEXT("Player Controller could not be retrieved for input mode setup"))
			return;
		}

		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(true);
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
	}
}

void UWidgetManagerSubsystem::OnQuitButtonPressed_Callback()
{
	UModalBase* QuitModal = CreateWidget<UModalBase>(GetGameInstance(), QuitGameModalClass);
	if(!ensure(QuitModal != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Quit Modal. Abording!"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Quiting"))
	
	QuitModal->SetModalContext("Exit The Game");
	QuitModal->OnModalInteracted.AddLambda([this](UModalBase* Modal, EModalResult Result)
	{
		if(Result == Accepted)
		{
			Modal->RemoveFromParent();
			GetGameInstance()->GetFirstLocalPlayerController(GetWorld())->ConsoleCommand("quit");
		}
		else
		{
			Modal->RemoveFromParent();
		}
	});

	QuitModal->AddToViewport();
}

void UWidgetManagerSubsystem::HandleSessionModalCancelOrTimeout()
{
	
	USessionFoundModal* SessionModal = CreateWidget<USessionFoundModal>(GetGameInstance(), SessionFoundModalClass);
	if(!ensure(SessionModal != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Session Modal. Abording!"))
		return;
	}

	ShowSessionFoundModal(SessionModal);
}

void UWidgetManagerSubsystem::OnSessionInteraction_Callback(UModalBase* Menu, EModalResult Result)
{
	switch (Result)
	{
	case Accepted:
		// Join Session
		if(CurrentSessionIndex > FoundSessions.Num())
			CurrentSessionIndex = FoundSessions.Num() - 1;
		
		WidgetController->JoinGame(FoundSessions[CurrentSessionIndex]);
		
		break;

	case Aborted:
		Menu->RemoveFromParent();
		
	case Canceled:
		if(++CurrentSessionIndex < FoundSessions.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Modal Closed"))
			
			CurrentSessionIndex = 0;
			
			HandleSessionModalCancelOrTimeout();
			Menu->OnModalInteracted.Clear();
		}
		
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Something went wrong on Session Modal Interaction callback"));
	}
}
