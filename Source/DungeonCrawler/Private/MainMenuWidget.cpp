// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "MainGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	NewPlayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewPlayerButtonClicked);
}

void UMainMenuWidget::OnStartButtonClicked()
{
		UGameplayStatics::OpenLevel(this, FName("Hub"));

	
}

void UMainMenuWidget::OnNewPlayerButtonClicked()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MGI->DeleteGameData();
	}
}

void UMainMenuWidget::OnQuitButtonClicked()
{
}
