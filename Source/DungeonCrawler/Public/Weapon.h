// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementTypes.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "WeaponItemData.h"
#include "Weapon.generated.h"

class UWeaponInstance;
class AEnemy;
class AProjectile;
class UBoxComponent;
class UCapsuleComponent;

UENUM()
enum class EWeaponType : uint8
{
	OneHand UMETA(DisplayName = "OneHand"),
	TwoHand UMETA(DisplayNAME = "TwoHand"),
	Bow UMETA(DisplayNAME = "Bow"),
	Mage UMETA(DisplayNAME = "Mage")
};



UCLASS()
class DUNGEONCRAWLER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Collision Comp")
	UCapsuleComponent* CapsuleComponent;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Weapon Type")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Type")
	float PoiseDamage = 0;

	UPROPERTY(EditAnywhere,Category = "Projectile To Spawn")
	TSubclassOf<AProjectile> Projectile;

	UPROPERTY()
	AProjectile* CurrentProjectile;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Weapon Move Set")
	UAnimMontage* TwohandMoveSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Move Set")
	UAnimMontage* OnehandMoveSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Move Set")
	UAnimMontage* Block;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Move Set")
	UAnimMontage* Misc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlendSpaces")
	UBlendSpace* OneHandBlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlendSpaces")
	UBlendSpace* TwoHandBlendSpace;

	UPROPERTY(EditAnywhere,Category = "Stamina Reduction")
	float StaminaReduction = 0.f;

	UPROPERTY(EditAnywhere,Category = "Mana Reduction")
	float ManaReduction;

	void DetermineHitDirection(AEnemy*Enemy);

	UPROPERTY()
	TArray<AActor*> HitActors;

	UPROPERTY()
	FVector StartLocation;

	void Swing();

	void SetIsSwinging(const bool& newVal);

	void StopMontages(UAnimInstance* AnimInstance);

	bool GetIsSwinging() const
	{
		return bIsSwinging;
	}

	float CalculateDamage(AEnemy* Enemy);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Weapon Instance")
	UWeaponInstance* WeaponInstance;

	void InitWeapon();

	void ThrowWeapon(APlayerCharacter*player,AEnemy*CurrentEnemy);

	void ReturnToPlayer();

	int CurrentThrownHits = 0;
	int MaxThrownHits = 5;

	FTransform OriginalTransform;

	void MoveWeapon(float DeltaTime);
	FTimerHandle FLerpTimer;

	UPROPERTY()
	AEnemy* TargetedEnemy;

	float InterpAlphaVal = 0.f;
	FVector InterpStart;
	FVector InterpEnd;

	void ActivateWeapon();

	FTimerHandle FResetWeaponHitActors;

	void AttachElementType() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private :
	bool bIsSwinging = false;

};
