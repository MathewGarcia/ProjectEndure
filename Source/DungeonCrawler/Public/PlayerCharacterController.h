// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void ResetCursor();
	void ShowHideCursor(bool NewVal);

	void PlayCameraShake();

	UPROPERTY(EditAnywhere,Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShake;
protected:
	FInputModeGameAndUI InputUI;
	FInputModeGameOnly InputGameOnly;

};
