// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentalItem.generated.h"

class AItem;
class UNiagaraSystem;
class UNiagaraComponent;
class UGeometryCollectionComponent;

UCLASS()
class DUNGEONCRAWLER_API AEnvironmentalItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentalItem();

	UPROPERTY(EditAnywhere,Category = "Static Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "NiagaraSystemToPlay")
	UNiagaraSystem* NiagaraSystemToPlay;


	UPROPERTY(EditAnywhere, Category = "Item To Drop")
	TArray<TSubclassOf<AItem>> ItemToDropClass;


	virtual void Destroyed() override;

	void DropItem();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
