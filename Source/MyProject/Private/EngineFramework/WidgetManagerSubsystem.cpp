// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineFramework/WidgetManagerSubsystem.h"
#include "UserWidgets/MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "EngineFramework/MultiplayerSessionSubsystem.h"


UWidgetManagerSubsystem::UWidgetManagerSubsystem()
{
	const ConstructorHelpers::FClassFinder<UMainMenu> MainMenuWidgetClass(
		TEXT("/Game/Blueprints/Widgets/WGT_MenuSwitcher"));
	if (!ensure(MainMenuWidgetClass.Class != nullptr))
		return;

	MainMenuClass = MainMenuWidgetClass.Class;
}

void UWidgetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ISessionController* Controller = GetGameInstance()->GetSubsystem<UMultiplayerSessionSubsystem>();
	if(!ensure(Controller != nullptr))
		return;

	WidgetController = Controller;
	WidgetController->GetOnSessionFound().AddUObject(this, &UWidgetManagerSubsystem::OnSessionFound_Callback);
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

	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
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

void UWidgetManagerSubsystem::OnSessionCreated_Callback()
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
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

void UWidgetManagerSubsystem::OnSessionFound_Callback(const TArrayView<FOnlineSessionSearchResult>& FoundSessions, bool bIsSuccessful)
{
	if(bIsSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *FoundSessions[0].GetSessionIdStr())
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find any sessions"))
	}

}

void UWidgetManagerSubsystem::OnSessionJoin_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result != EOnJoinSessionCompleteResult::Type::Success)
		return;
	
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Player Controller could not be retrieved for input mode setup"))
		return;
	}
	
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
}
