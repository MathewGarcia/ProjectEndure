// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelMusicActor.generated.h"

UCLASS()
class DUNGEONCRAWLER_API ALevelMusicActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelMusicActor();

	UPROPERTY(EditAnywhere, Category = "Level Sounds")
	UAudioComponent* MusicManagerComponent;

	UPROPERTY(EditAnywhere, Category = "Level Sounds")
	USoundBase* LevelSong;


	UPROPERTY(EditAnywhere, Category = "Level Sounds")
	bool bShouldPlay = true;

	void SetAndPlayMusic();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
