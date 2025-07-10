// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoEHealActor.generated.h"

class UAoE_Heal;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class DUNGEONCRAWLER_API AAoEHealActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAoEHealActor();

	UPROPERTY(EditAnywhere,Category = "Box Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "NiagaraComponent")
	UNiagaraComponent* NiagaraComponent;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	UAoE_Heal* OwningObject;


	FTimerHandle FOwningObjectTimerCheck;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTimerHandle FHealingTimer;

};
