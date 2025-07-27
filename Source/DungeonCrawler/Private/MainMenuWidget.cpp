// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "MainGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (StartButton) {
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartButtonClicked);
	}
	if (QuitButton) {
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	}
	if (NewPlayerButton) {
		NewPlayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewPlayerButtonClicked);
	}
}

void UMainMenuWidget::OnStartButtonClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Hub"));
}

void UMainMenuWidget::OnNewPlayerButtonClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
	{
		MGI->DeleteGameData();
	}
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	// Implement quit logic if needed
}
