// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "NPC_Escort.generated.h"

class UQuest_Escort;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ANPC_Escort : public ANPC
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Health")
	float MaxHealth;
	
	void StartEscort(FVector Location);

	UPROPERTY()
	UQuest_Escort* ParentQuest;
private:
	float Health;

	bool bInitialized = false;

	bool bAlreadyAwarded = false;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

};
