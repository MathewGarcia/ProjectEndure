// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementTypes.h"
#include "Projectile.generated.h"

UENUM()
enum class EProjectileType : uint8
{
	SingleTarget UMETA(DisplayName = "SingleTarget"),
	AoE UMETA(DisplayName = "Area of Effect")
};


class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class DUNGEONCRAWLER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditAnywhere,Category = "Projectile Mesh")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Projectile Hit Box")
	USphereComponent* HitBox;

	UPROPERTY(EditAnywhere, Category = "Projectile Movement Component")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile Type")
	EProjectileType ProjectileType = EProjectileType::SingleTarget;

	UPROPERTY(EditDefaultsOnly, Category = "LevelUpSound")
	USoundBase* ContactSound;

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	void LaunchProjectile(float Speed, const FVector& Direction);

	float DamageModifier = 1.f;

	void Explode();

	virtual void Destroyed() override;

	void PlaySound();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
