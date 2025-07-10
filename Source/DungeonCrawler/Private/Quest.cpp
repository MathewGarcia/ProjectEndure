// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest.h"

#include "DungeonCrawlerGM.h"
#include "Item.h"
#include "MainGameInstance.h"
#include "NPC.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "QuestDataAsset.h"

void UQuest::StartQuest_Implementation()
{
	//add the quest to the player "current quests" 
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		PCS->CurrentQuests.Add(this);
	}
}

void UQuest::CompleteQuest()
{


	if (!QuestDataAsset || !OwningNPC) return;


	FVector SpawnLocation = OwningNPC->GetActorLocation() + (OwningNPC->GetActorForwardVector() * 50.f);
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	int RandomAmount = FMath::RandRange(1, QuestDataAsset->RewardsMap.Num() - 1);

	for(int i = 0; i < RandomAmount; ++i)
	{
		if (UItemDataAsset* Reward = GetReward()) {
			GetWorld()->SpawnActor<AItem>(Reward->ItemToSpawn, SpawnLocation, FRotator::ZeroRotator, params);
		}
	}

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		PCS->playerStats.UpdateEXP(QuestDataAsset->RewardEXP);
	}
	EndQuest();
}

void UQuest::EndQuest()
{
	//remove from current quests and destroy object.


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
			PCS->CurrentQuests.Remove(this);
			//BeginDestroy();
		
	}

}

void UQuest::Execute()
{

}

void UQuest::OnDeath()
{
}

void UQuest::SerializeQuest(FQuest_SaveData& Out)
{

	if (!QuestDataAsset) return;


	Out.QuestToSpawn = GetClass();
	Out.QuestMetaData.StringVar = QuestDataAsset->QuestName.ToString();
}

void UQuest::DeserializeQuest(FQuest_SaveData* In)
{

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if(UQuestDataAsset**FoundQuest = MGI->Quests.Find(In->QuestMetaData.StringVar))
		{
			QuestDataAsset = (*FoundQuest);
		}
	}
}

UItemDataAsset* UQuest::GetReward()
{
	if (!QuestDataAsset) return nullptr;

	float total = 0.f;

	for(const auto& pair : QuestDataAsset->RewardsMap)
	{
		total += pair.Value;
	}

	float Roll = FMath::FRandRange(0, total);

	for(const auto& pair : QuestDataAsset->RewardsMap)
	{
		Roll -= pair.Value;

		if(Roll <= 0)
		{
			return pair.Key;
		}
	}

	return nullptr;
}

