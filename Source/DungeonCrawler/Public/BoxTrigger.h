// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxTrigger.generated.h"

class ALevelMusicActor;
class AEnemy;
class UBoxComponent;
UCLASS()
class DUNGEONCRAWLER_API ABoxTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxTrigger();

	UPROPERTY(EditAnywhere, Category = "Box Trigger Mesh")
	UStaticMeshComponent* TriggerMesh;

	UPROPERTY(EditAnywhere, Category = "Box Trigger")
	UBoxComponent* Box;


	UPROPERTY(EditAnywhere,Category = "Boss")
	TWeakObjectPtr<AEnemy> EnemyReference;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TWeakObjectPtr<ALevelMusicActor> MusicActorPtr;

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
