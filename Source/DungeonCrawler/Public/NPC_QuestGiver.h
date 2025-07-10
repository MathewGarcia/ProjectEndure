// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "NPC_QuestGiver.generated.h"

class UQuestDataAsset;
class UWidget_Quest;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ANPC_QuestGiver : public ANPC
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,Category = "Widget To Spawn")
	TSubclassOf<UWidget_Quest> QuestWidgetClass;


	virtual void Interact_Implementation() override;

	UPROPERTY(EditAnywhere,Category = "Quests")
	TMap<UQuestDataAsset*, float> QuestsMap;

	UQuestDataAsset* GetQuest();

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	TWeakObjectPtr<UQuestDataAsset> ChosenQuest;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UWidget_Quest* QuestWidget;
};
