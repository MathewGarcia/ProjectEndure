// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementTypes.h"
#include "LiquidActor.generated.h"

class UBoxComponent;

UCLASS()
class DUNGEONCRAWLER_API ALiquidActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALiquidActor();

	UPROPERTY(EditAnywhere,Category = "Box Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "LiquidType")
	EElementTypes ElementType;

	UPROPERTY(EditAnywhere, Category = "LiquidType")
	float ElementResistanceDamageRate;

	UPROPERTY(EditAnywhere, Category = "LiquidType")
	float ElementRawDamage;

	UFUNCTION()
	void OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle LiquidTimerHandle;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
