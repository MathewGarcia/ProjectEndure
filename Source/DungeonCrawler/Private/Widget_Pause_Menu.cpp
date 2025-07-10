// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Pause_Menu.h"

#include "MainGameInstance.h"
#include "PlayerCharacterController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWidget_Pause_Menu::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddUniqueDynamic(this, &UWidget_Pause_Menu::OnResumeButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UWidget_Pause_Menu::OnQuitButtonClicked);

}

void UWidget_Pause_Menu::OnResumeButtonClicked()
{

	if (!GetOwningPlayerPawn()) return;
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(GetOwningPlayerPawn()->GetController()))
		{
			PCC->ShowHideCursor(false);
			SetVisibility(ESlateVisibility::Hidden);
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			MGI->bGamePaused = false;
		}
	}
}

void UWidget_Pause_Menu::OnQuitButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}
