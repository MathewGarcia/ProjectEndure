// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyParameters.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AEnemy*);

class APlayerCharacterState;
class AInteractableBuffActor;
class ADebuffActor;
class APlayerCharacter;
class AWeapon;
class UElementDebuff;
enum class EHitDirection : uint8;
class UNavigationInvokerComponent;

UENUM(Blueprintable)
enum class EEnemyType : uint8
{
	none UMETA(Display = "none"),
	ogre UMETA(Display = "ogre"),
	skeleton UMETA(Display = "skeleton"),
	imp UMETA(Display = "imp"),
	mage UMETA(Display = "mage"),
	knight UMETA(Display = "knight"),
	necroboss UMETA(DisplayName = "necroboss"),
	monstrosityboss UMETA(Displayname = "monstrosityboss"),
	salamanderboss UMETA(Displayname = "salamanderboss")
};

UENUM(Blueprintable)
enum class EEnemyStates : uint8
{
	none UMETA(Display = "none"),
	attack UMETA(Display = "attack"),
	dodge UMETA(Display = "dodge"),
	hit UMETA(Display = "hit"),
	backstabbed UMETA(Display = "backstabbed"),
	parried UMETA(Display = "parried"),
	blocking UMETA(Display = "blocking"),
	roaming UMETA(Display = "roaming"),
	fakeDead UMETA(Display = "fakeDead"),
	idleAttack UMETA(Display = "idleAttack")
};
UCLASS()
class DUNGEONCRAWLER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	static FOnEnemyDeath OnEnemyDeath;
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere,Category = "Navigation Invoker")
	UNavigationInvokerComponent* NavigationInvoker;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float MaxHealth;//scalable

	UPROPERTY(EditAnywhere,Category = "Enemy Info")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float AttackDistance;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float MaxStamina; //scalable

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	int AttackCount;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float IntelligencePenalty;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	int DodgeStaminaReduction;

	UPROPERTY(EditAnywhere, Category = "Anim Montages")
	int SpecialStaminaReduction;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	int AttackStaminaReduction;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	EEnemyType EnemyType;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	FEnemyParameters EnemyParameters;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float Damage;//sclable

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float Passivity = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float MontageCheck = 0.5f;


	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	float DropChance = 0.f;

	UPROPERTY(EditAnywhere, Category = "Enemy Name")
	FText EnemyName;

	UPROPERTY(EditAnywhere,Category = "Special Timer")
	float SpecialTimer = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Stamina Amount Rate")
	float StaminaAmount = 5.f;

	UPROPERTY(EditAnywhere,Category = "Weakness Map")
	TMap<EElementTypes, EElementTypes> WeaknessMap;

	virtual void Attack();

	void UseSpecificAttack(UAnimMontage*MontageToUse, FName SectionToPlay);

	UPROPERTY(EditAnywhere,Category= "Anim Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages")
	UAnimMontage* MiscMontages;


	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* DeathAnimation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "EnemyState")
	EEnemyStates EnemyState;

	UPROPERTY(EditAnywhere,Category = "Critical Hit")
	UMaterialInstance* CriticalHitMaterial;

	UPROPERTY()
	TArray<UBuff*> Buffs;

	virtual UPrimitiveComponent* GetCollisionByName(FName Name);

	UPROPERTY()
	TMap<FName, UPrimitiveComponent*> Collisions;

	FOnEnemyDeath InstancedOnEnemyDeath;

	UFUNCTION()
	void OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Dodge(int direction);

	FTimerHandle StaminaRechargeHandle;

	TArray<TWeakObjectPtr<AEnemy>> RecentEnemies;

	float GetStamina() const
	{
		return Stamina;
	}

	void SetStamina(float newStamina);

	UPROPERTY(EditAnywhere, Category = "Attack Boxes")
	UCapsuleComponent* RightHandHitBox;

	UPROPERTY(EditAnywhere, Category = "Attack Boxes")
	UCapsuleComponent* LeftHandHitBox;

	UPROPERTY(EditAnywhere, Category = "Attack Boxes")
	UCapsuleComponent* CenterHitBox;

	bool isFriendlyInRange();

	void StartRechargeStamina();

	void BuffFriendlies();

	void UseSpecial();

	bool CanUseSpecial() const;

	void ShootProjectile();

	void FacePlayer();

	void UseAbility(EAbilityType AbilityType,int pos);

	int CanUseAblity(EAbilityType AbilityType);

	void AbilityLogic(EAbilityType AbilityType, int pos);

	UPROPERTY()
	EHitDirection HitDirection;

	void DetermineHitAnimation(const float& poise);

	bool CanAttack() const;

	UPROPERTY()
	TArray<AActor*> HitEnemies;

	void Block();

	bool CanBlock() const;

	void EndBlock();

	bool ShouldBlock() const;

	UPROPERTY(EditAnywhere,Category = "BlockWeight")
	float BlockWeight;

	void ApplyElementDebuff(EElementTypes ElementType,AActor*DamageCauser);

	void CalculateElementProgression(APlayerCharacter*DamageCauser);

	TMap<EElementTypes, UElementDebuff*> CurrentDebuffs;


	void HandlePlayerAttack(APlayerCharacter* player, float DamageAmount);

	void HandleDebuffAttack(ADebuffActor* DebuffActor, float DamageAmount);

	TSubclassOf<AItem> ItemToDrop();
	void DropItem(TSubclassOf<AItem> itemToDrop);

	bool bMovementCompleted = true;
	bool bWaitCompleted = true;

	void SlowEnemySpeed(float Time);

	float CalculateDamage();

	void StopDebuffs();

	UFUNCTION()
	void StaminaRechargeTick();

	UPROPERTY(EditAnywhere, Category = "Boss Item")
	TSubclassOf<AInteractableBuffActor> InteractableBuffActor;
	void SpawnBossBuff();

	bool bDead = false;

	void HideBossHealthBarUI(APlayerCharacterState* pcs);

	void UpdateBossHealthUI();

	void ActivateBossTeleporter();

	void UpdatePlayerEXPUI(APlayerCharacterState*pcs);

	virtual void ResetEnemy();

	bool bShouldEnemyReset();

	UFUNCTION()
	void ResetEnemyState();

	void SetRandomElementType();

	EPhysicalSurface MostRecentSurfaceType;

	void PlaySoundBasedOnSurface();

	void ApplyElementDamageParticle(EElementTypes WeaponElementType);

	FTimerHandle FBackstabTimer;

	void Destroyed() override;

	void ScaleEnemyAccordingToRound();

	void ToggleEnemy(bool newVal);

	bool bIsEnabled = true;

private:
	FTimerHandle BuildPoise;

	FTimerHandle FAttackResetFallback;

	FTimerHandle FSpecialCooldown;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Stamina;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

};
