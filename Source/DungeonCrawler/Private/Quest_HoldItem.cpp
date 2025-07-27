// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest_HoldItem.h"

#include "DungeonCrawlerGM.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "QuestDataAsset_HoldItem.h"

void UQuest_HoldItem::Execute()
{
	Super::Execute();

	++Rounds;
	UE_LOG(LogTemp, Warning, TEXT("New Round count %d"), Rounds);
	if(UQuestDataAsset_HoldItem* HoldItem = Cast<UQuestDataAsset_HoldItem>(QuestDataAsset))
	{
		if(Rounds >= HoldItem->MaxRounds)
		{
			CompleteQuest();
		}
	}

}

void UQuest_HoldItem::OnDeath()
{
	Super::OnDeath();

	EndQuest();
}

void UQuest_HoldItem::StartQuest_Implementation()
{
	Super::StartQuest_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		UWorld* World = GetWorld();
		if (!World) return;
		if (ADungeonCrawlerGM* GM = Cast<ADungeonCrawlerGM>(World->GetAuthGameMode()))
		{
			if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {

				GM->OnRoundEndDelegate.AddDynamic(this, &UQuest_HoldItem::Execute);
				player->OnPlayerDeath.AddDynamic(this, &UQuest_HoldItem::OnDeath);
			}
		}
	}
}

void UQuest_HoldItem::SerializeQuest(FQuest_SaveData& Out)
{
	Super::SerializeQuest(Out);

	Out.QuestMetaData.IntVar = Rounds;
}

void UQuest_HoldItem::DeserializeQuest(FQuest_SaveData* In)
{
	if (!In) return;

	Super::DeserializeQuest(In);

	Rounds = In->QuestMetaData.IntVar;
	UE_LOG(LogTemp, Warning, TEXT("Rounds %d"), Rounds);
}
