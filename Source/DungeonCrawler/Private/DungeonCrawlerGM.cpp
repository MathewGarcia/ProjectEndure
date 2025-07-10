// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonCrawlerGM.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerSaveGame.h"

void ADungeonCrawlerGM::IncrementRoundCount(int NewRoundCount)
{
	roundCount += NewRoundCount;
	OnRoundEndDelegate.Broadcast();
}

void ADungeonCrawlerGM::SetRoundCount(int NewRoundCount)
{
	roundCount = NewRoundCount;
}

void ADungeonCrawlerGM::InitializeRound()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MGI->LoadGameData();

		if(MGI->LoadType == ELoadType::GameInstance)
		{
			roundCount = MGI->GIRound;
		}
		else if (UPlayerSaveGame* Save = MGI->GetSavedGame())
		{
			if (MGI->bLoadingSucceeded && MGI->LoadType == ELoadType::SaveGame) {
				roundCount = Save->Round;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't get save"));
		}
	}


}

void ADungeonCrawlerGM::Loading()
{

		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (APlayerCharacter* player = MGI->localPlayer)
			{
				if (UMainPlayerWidget* MPW = player->GetMainPlayerWidget().Get())
				{
					if (bIsLoading) {
						MPW->SetLoadVisibility(ESlateVisibility::Visible);
					}
					else
					{
						MPW->SetLoadVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
			GetWorld()->GetTimerManager().ClearTimer(FRemoveFogUITimer);
		}
	
}


void ADungeonCrawlerGM::SetPlayer(APlayerCharacter* player)
{
	SafePlayer = player;
}

void ADungeonCrawlerGM::SetDungeonReady()
{
	bDungeonReady = true;
}

void ADungeonCrawlerGM::CheckReady()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		if (APlayerCharacter* player = MGI->localPlayer)
		{
			if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
			{
				if (PCS->bInDungeon)
				{
					if (bDungeonReady)
					{
						bIsLoading = false;
						Loading();
					}
				}
				else
				{
					bIsLoading = false;
					Loading();
				}
			}
		}
}

void ADungeonCrawlerGM::InitializeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(FRemoveFogUITimer, this, &ADungeonCrawlerGM::CheckReady, 0.5, true);
}

void ADungeonCrawlerGM::BeginPlay()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
			MGI->OnDungeonGenerationComplete.AddDynamic(this, &ADungeonCrawlerGM::SetDungeonReady);
	}

	InitializeTimer();
	InitializeRound();
}
