// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnumHitDirection.h"
#include "GameStats.h"
#include "GearType.h"
#include "Item.h"
#include "PlayerCharacter.generated.h"

class UNiagaraSystem;
class UPlayerSaveGame;
class UMainGameInstance;
class ULivesWidget;
class UWidgetComponent;
class UWeaponInstance;
DECLARE_MULTICAST_DELEGATE_FourParams(FOnPlayerDamaged, float&, FDamageEvent const&, AController*, AActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyDamageTaken, AEnemy*,float&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityUsed,const int&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE(FOnPlayerReady);


class AProjectile_Hook;
class UBuff;
class ULiquidDataAsset;
class UItemDataObject;
class ANPC;
class UAbility;
class ADebuffActor;
class UElementDebuff;
class UWeaponItem;
class UGearItem;
class AInGamePlayerHUD;
class UPlayerAnimInstance;
enum class EWeaponType : uint8;
class AItem;
class AEnemy;
class UInputConfigData;
class AWeapon;
class APlayerCharacterState;

UENUM()
enum class PlayerStates : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	MOVING UMETA(DisplayName = "MOVING"),
	DODGING UMETA(DisplayName = "DODGING"),
	ATTACKING UMETA(DisplayName = "ATTACKING"),
	HIT UMETA(DisplayName = "HIT"),
	STUNNED UMETA(DisplayName = "STUNNED"),
	BLOCKING UMETA(DisplayName = "BLOCKING"),
	PARRIED UMETA(DisplayName = "PARRIED"),
	HEALING UMETA(DisplayName = "HEALING"),
	ABILITY UMETA(DisplayName = "ABILITY"),
	DEAD UMETA(DisplayName = "DEAD")

};


UENUM()
enum class EResourceTypes : uint8
{
	Stamina UMETA(DisplayName = "Stamina"),
	Mana UMETA(DisplayName = "Mana")
};

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere ,Category = "Camera")
	UCameraComponent* Cam;

	UPROPERTY(BlueprintReadWrite, EditAnywhere ,Category = "SpringArm")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputConfigData* InputConfig;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* OneHandMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* TwoHandMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* BowMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* MagicMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* MiscMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Montages")
	UAnimMontage* DodgeMontages;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Target Camera Height")
	FVector CamHeightOffset = FVector(0.f);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Gear")
	USkeletalMeshComponent* Chest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
	USkeletalMeshComponent* Legs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear")
	USkeletalMeshComponent* Helm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WidgetComponent")
	UWidgetComponent* CurrentLivesWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WidgetComponent")
	TSubclassOf<ULivesWidget> LivesWidgetClass;

	UPROPERTY(EditAnywhere,Category = "Level Up Pressed")
	UNiagaraSystem* LevelUpPressedWidget;

	float MontageSpeed = 1.f;


	void SetPlayerState(PlayerStates NewPlayerStates);

	void IncrementAttackCount();

	void ResetAttackCount();

	void Dodge();

	void ResetLastInputDirection();
	void SetAttackingWeapon(AWeapon*AttackWeapon);
	void DecreaseElementalProgression();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Weapon")
	TSubclassOf<AWeapon> StarterWeapon;


	void DropItem();


	bool GetWantsToInteract()
	{
		return WantsToInteract;
	}


	void Interact(const FInputActionValue& InputActionValue);
	void FinishedInteract();

	UPROPERTY()
	AActor* OverlappingActor = nullptr;

	void UseItem(UItemDataObject* item);

	void tryEquipGear(AItem* GearToEquip, const EGearType& itemType,  UItemDataObject* ItemInstance);

	void EquipGear(AItem* GearToEquip, const EGearType& itemType,  UItemDataObject* ItemInstance);

	UPROPERTY()
	bool bIsHolding = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Bow Power")
	float BowPower = 0.0;

	FName GetValidSocket(EGearType Slot,AWeapon* Weapon);

	TArray<EGearType> GetValidSlotsForWeapon(EWeaponType WeaponType);

	void RemoveItemFromSlot(EGearType Slot);

	UFUNCTION(BlueprintCallable,Category = "Current Player State")
	PlayerStates GetCurrentState() const
	{
		return ps;
	}

	float GetPlayerMontageTime();

	void PlayerStunned();

	bool CanPlayerDoAction(EResourceTypes ResourceType, const float& AmountNeeded);

	void Block(const FInputActionValue& InputActionValue);

	void SetParrySuccess(bool bNewValue);

	float HitDotProduct = 0.f;

	void SetHitDirection(const EHitDirection& NewHitDirection);

	void DetermineHitAnimation(const float& poise);

	bool IsBackStab(AEnemy*Enemy);

	bool IsParry(AEnemy* Enemy);

	float DeterminePoise(float PoiseDamage);

	void ToggleHitBox(ECollisionEnabled::Type NewCollisionChannel) const;

	void ResetCameraPosition(float DeltaTime);

	APlayerCharacterState* GetPlayerCharacterState() const;

	void ToggleWeaponType();

	USkeletalMeshComponent* FindSkeletalMesh(EGearType GearType);

	UPROPERTY()
	UPlayerAnimInstance* PlayerAnimInstance;

	FVector DeprojectScreen(AWeapon*Weapon);

	UPROPERTY(EditAnywhere,Category = "Dodge Reduction")
	float DodgeReduction = 0.f;

	void UnEquipGear(AItem*GearToUnEquip);

	UPROPERTY()
	TMap<EElementTypes, UElementDebuff*> CurrentDebuffs;

	UPROPERTY()
	TMap<EElementTypes, ULiquidDataAsset*> LiquidDebuffs;

	AWeapon*GetCurrentWeapon() const
	{
		return CurrentWeapon;
	}


	void HandleEnemyAttack(AEnemy* Enemy,float& DamageAmount);

	void HandleDebuffAttack(ADebuffActor*DebuffActor,float& DamageAmount);

	void LiquidEntered(ULiquidDataAsset* LiquidDataAsset);

	void LiquidExited(ULiquidDataAsset* LiquidDataAsset);

	void AbilityHeal();

	UPROPERTY()
	UAbility* EquippedHealAbility;

	UPROPERTY()
	ULiquidDataAsset* CurrentLiquid;

	TQueue<AProjectile_Hook*> Hooks;

	int RecentlyPressedAbility = -1;

	FOnPlayerDamaged OnPlayerDamaged;

	FOnEnemyDamageTaken OnEnemyDamageTaken;

	AProjectile* SpawnProjectile(FVector Size = FVector(1.f),float DamageModifier = 1.f,EProjectileType ProjectileType = EProjectileType::SingleTarget,float Speed = 1.f,float GravityScale = 1.f);

	AWeapon*GetAttackingWeapon() const
	{
		return AttackingWeapon;
	}


	void SetNewSoftPCS(APlayerCharacterState*NewSoftPCS);

	FOnAbilityUsed OnAbilityUsed;

	FVector SpawnLocation = FVector(0.f,0.f,200.f);

	void ResetPlayer(APlayerCharacterState*playerCharacterState);

	void ShowLivesWidget();


	TWeakObjectPtr<UMainPlayerWidget> GetMainPlayerWidget();


	void InitializeActionNameMap();

	void LoadFromGameInstance();

	void LoadFromSavedGame(APlayerCharacterState*PCS, UMainGameInstance*GI,UPlayerSaveGame*Save);

	UFUNCTION()
	void ResetState();

	bool bGetIsDead() const
	{
		return  bDead;
	}

	void PlaySoundBasedOnSurface();

	void PlayPickUpSound();

	void ResetCurrentLivesWidget();

	void GiveStarterItems();

	FOnDeath OnPlayerDeath;

	FTimerHandle FDecreaseElementalTimerHandle;

	void RemoveGearStats(const EGearType& Slot);

	void SetHealAbility(TSubclassOf<UAbility> NewHealAbility);

	void PauseGame();

	void PlayLevelUpWidgetNiagara();

	bool bIsTargetedEnemy(AEnemy*Enemy);

	void ResetTargetedEnemy();


	FOnPlayerReady OnPlayerReady;
private:

	EHitDirection HitDirection;

	UPROPERTY()
	AWeapon* AttackingWeapon;

	bool bParrySuccess = false;

	float MouseSens = 0.3;

	PlayerStates ps = PlayerStates::NONE;

	UPROPERTY()
	AEnemy* TargetedEnemy;

	bool CamTargeted = false;

	UPROPERTY()
	APlayerController* PC;

	int attackCount = 0;

	FTimerHandle FResetAttackCount;

	FTimerHandle FResetLastInputDirectionTimer;

	FVector2D InputDirection;

	UPROPERTY()
	TSoftObjectPtr<APlayerCharacterState> SoftPCS;


	bool WantsToInteract = false;

	FTimerHandle BuildPoise;

	FVector TargetCamLocation;

	FVector OriginalCamLocation;

	UPROPERTY()
	AInGamePlayerHUD* PlayerHUD;

	FTimerHandle StaminaTimerHandle;

	void updateGearSlot(UItemDataObject*Item);

	UPROPERTY()
	AWeapon* CurrentWeapon;

	bool PlayerInterrupted();

	FTimerHandle LiquidTimerHandle;

	bool bDead = false;

	FTimerHandle FResetAttackFallBack;
	float ResetAttackFallBackTime = 2.f;

	EPhysicalSurface MostRecentSurfaceType;

	UPROPERTY(EditAnywhere,Category = "Side offset")
	float MaxSideOffset = 200.f;
	// how fast the camera recenters
	const float LateralRecenterSpeed = 5.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void Move(const FInputActionValue& Val);

	void Look(const FInputActionValue& Val);


	void Target(const FInputActionValue& Val);

	void UseAbility1(const FInputActionValue& Val);
	void UseAbility2(const FInputActionValue& Val);
	void UseAbility3(const FInputActionValue& Val);
	void UseAbility4(const FInputActionValue& Val);
	void UseAbility5(const FInputActionValue& Val);
	void UseAbility6(const FInputActionValue& Val);

	void Release(const FInputActionValue& Val);

	void ApplyElementDebuff(EElementTypes ElementType,AActor*DamageCauser);

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void InteractBag(const FInputActionValue& Val);
	void InteractEquipment(const FInputActionValue& Val);
	void InteractStats(const FInputActionValue& Val);
	void InteractTalent(const FInputActionValue& Val);

	bool bIsBagOpen = false;
	bool bIsEquipmentOpen = false;
	bool bIsTalentsOpen = false;
	bool bIsStatsOpen = false;


public:

	void Attack();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

};
