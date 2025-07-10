// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "OrbTeleporter.generated.h"

class ULevelStreamingDynamic;
/**
 * 
 */
UENUM()
enum class EOrbTeleportType : uint8
{
	Reward UMETA(DisplayName = "Reward"),
	Exit UMETA(DisplayName = "Exit"),
	Boss UMETA(DisplayName = "Boss"),
	OneWay UMETA(DisplayName = "OneWay"),
	Exfil UMETA(DisplayName = "Exfil")
};

UCLASS()
class DUNGEONCRAWLER_API AOrbTeleporter : public AInteractableActor
{
	GENERATED_BODY()


public:

	AOrbTeleporter();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Child Teleporter")
	AOrbTeleporter* ChildTeleporter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Child Teleporter")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Teleport Type")
	EOrbTeleportType TeleportType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Map")
	TMap<TSoftObjectPtr<UWorld>, float> RoomMap;

	TSoftObjectPtr<UWorld> SelectReward();

	UFUNCTION()
	void LevelLoaded();

	virtual void Interact_Implementation() override;

	void HandleTeleport();

	void QuickSaveData();

private:

	UPROPERTY()
	ULevelStreamingDynamic* LoadedLevel = nullptr;

	FTimerHandle FBossCheckTimer;

	FDelegateHandle OrbDelegateHandle;
};
