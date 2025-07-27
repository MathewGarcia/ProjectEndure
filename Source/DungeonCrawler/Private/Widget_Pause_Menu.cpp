// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Pause_Menu.h"

#include "MainGameInstance.h"
#include "PlayerCharacterController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWidget_Pause_Menu::NativeConstruct()
{
	Super::NativeConstruct();
	if (ResumeButton) {
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UWidget_Pause_Menu::OnResumeButtonClicked);
	}
	if (QuitButton) {
		QuitButton->OnClicked.AddUniqueDynamic(this, &UWidget_Pause_Menu::OnQuitButtonClicked);
	}
}

void UWidget_Pause_Menu::OnResumeButtonClicked()
{
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return;
	UWorld* World = GetWorld();
	if (!World) return;
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance())) {
		if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(PlayerPawn->GetController()))
		{
			PCC->ShowHideCursor(false);
			SetVisibility(ESlateVisibility::Hidden);
			UGameplayStatics::SetGamePaused(World, false);
			MGI->bGamePaused = false;
		}
	}
}

void UWidget_Pause_Menu::OnQuitButtonClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;
	UGameplayStatics::OpenLevel(World, "MainMenu");
}
