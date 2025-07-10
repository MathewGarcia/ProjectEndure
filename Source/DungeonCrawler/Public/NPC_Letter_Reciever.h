// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "NPC_Letter_Reciever.generated.h"

class UQuest_Note;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ANPC_Letter_Reciever : public ANPC
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void Interact_Implementation() override;


	UPROPERTY()
	UQuest_Note* NoteQuest;

};
