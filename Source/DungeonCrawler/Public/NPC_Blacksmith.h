// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "NPC_Blacksmith.generated.h"

class UBlacksmithUpgradeUI;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ANPC_Blacksmith : public ANPC
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation() override;

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category = "Upgrade Blacksmith Widget")
	TSubclassOf<UBlacksmithUpgradeUI> UpgradeBlacksmithWidgetClass;

	UPROPERTY()
	UBlacksmithUpgradeUI* BlacksmithUpgradeUI;

};
