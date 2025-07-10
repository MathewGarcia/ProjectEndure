// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


class UItemDataObject;
class APlayerCharacter;
class UBoxComponent;
class UBaseItem;
UCLASS()
class DUNGEONCRAWLER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();


	UPROPERTY(EditAnywhere,Category = "Root")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere,Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere,Category = "Static Mesh Component")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Item Collision")
	UBoxComponent* collisionBox;

	UPROPERTY(EditAnywhere, Category = "Item Type")
	EItemType ItemType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Is Skeletal")
	bool bIsSkeletal;

	UFUNCTION()
	void ItemOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void PickUp(APlayerCharacter*player);

	UPROPERTY(EditAnywhere,Category = "Item Info")
	UItemDataAsset* ItemData;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
