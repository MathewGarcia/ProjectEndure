// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"

void ATutorialGameMode::StartPlay()
{

	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MGI->LoadType = ELoadType::Tutorial;
		FTimerHandle FDelay;
		GetWorld()->GetTimerManager().SetTimer(FDelay, [MGI]
		{
			if(IsValid(MGI))
			{
				if(APlayerCharacter*player = MGI->localPlayer)
				{
					if(UMainPlayerWidget*MPW = player->GetMainPlayerWidget().Get())
					{
						MPW->SetLoadVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
		},0.5, false);
	}
	Super::StartPlay();


}
