// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AINPC.h"
#include "AIPlayerClones.generated.h"

class APlayerCharacter;
/**
 * 
 */


UCLASS()
class DUNGEONCRAWLER_API AAIPlayerClones : public AAINPC
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Threshold")
	float Threshold = 1000.f;

	virtual void Execute() override;

	int Index = 0;

	UFUNCTION()
	void UsePlayerAbility(const int& pressedAbility);
protected:
	UPROPERTY()
	APlayerCharacter* Player;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;



};
