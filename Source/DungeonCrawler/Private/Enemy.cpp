// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Ability_Blood_Frenzy.h"
#include "Ability_Leech.h"
#include "AIEnemy.h"
#include "BleedDebuff.h"
#include "NavigationInvokerComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Buff.h"
#include "DebuffActor.h"
#include "DungeonCrawlerGM.h"
#include "DungeonManager.h"
#include "FireDebuff.h"
#include "ImpBuff.h"
#include "InGamePlayerHUD.h"
#include "InteractableBuffActor.h"
#include "LevelUpWidget.h"
#include "LighteningDebuff.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "OrbTeleporter.h"
#include "PlayerCharacterController.h"
#include "PoisonDebuff.h"
#include "ShadowDebuff.h"
#include "WaterDebuff.h"
#include "Weapon.h"
#include "WeaponInstance.h"
#include "Components/BoxComponent.h"
#include "Components/VerticalBox.h"
#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Water		SurfaceType1
#define SURFACE_Lava		SurfaceType2
#define SURFACE_Poison		SurfaceType3
#define SURFACE_Blood		SurfaceType4
#define SURFACE_Wood		SurfaceType5
#define SURFACE_Cobble		SurfaceType6
#define SURFACE_Grass		SurfaceType7

FOnEnemyDeath AEnemy::OnEnemyDeath;

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	RightHandHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandHitBox"));
	RightHandHitBox->SetupAttachment(GetMesh(), FName("hand_r"));
	RightHandHitBox->SetGenerateOverlapEvents(true);

	LeftHandHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandHitBox"));
	LeftHandHitBox->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandHitBox->SetGenerateOverlapEvents(true);


	CenterHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CenterHitBox"));
	CenterHitBox->SetupAttachment(GetMesh(), FName("pelvis_Socket"));
	CenterHitBox->SetGenerateOverlapEvents(true);

	Collisions.Add("RightHandHitBox", RightHandHitBox);
	Collisions.Add("LeftHandHitBox", LeftHandHitBox);
	Collisions.Add("CenterHitBox", CenterHitBox);

	GetMesh()->SetGenerateOverlapEvents(true);
	GetCharacterMovement()->bEnablePhysicsInteraction = false;

	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Navigation Invoker"));

	AIControllerClass = AAIEnemy::StaticClass();
	
}

void AEnemy::Attack()
{
	int pos = CanUseAblity(EAbilityType::Offensive);

	if(pos > -1)
	{
		UseAbility(EAbilityType::Offensive, pos);
		GetWorld()->GetTimerManager().SetTimer(FAttackResetFallback, this, &AEnemy::ResetEnemyState, 2.f, false);

		return;
	}

	if (CanUseSpecial())
	{
		UseSpecial();
		GetWorld()->GetTimerManager().SetTimer(FAttackResetFallback, this, &AEnemy::ResetEnemyState, 2.f, false);

		return;
	}

	if (CanAttack() && Stamina >= AttackStaminaReduction) {

		EnemyState = EEnemyStates::attack;
		FString AttackName = "Attack" + FString::FromInt(AttackCount + 1);

		PlayAnimMontage(AttackMontage, 1, FName(AttackName));
		
		Stamina -= AttackStaminaReduction;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		AttackCount = (AttackCount + 1) % AttackMontage->GetNumSections();

		if (AttackCount == AttackMontage->GetNumSections() - 1) {
			if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
			{
				if (AIEnemyController->features.IsValidIndex(10))
				{
					AIEnemyController->features[10] = 1.f;
				}
			}
		}
	}
	GetWorld()->GetTimerManager().SetTimer(FAttackResetFallback, this, &AEnemy::ResetEnemyState,2.f,false);
}

void AEnemy::UseSpecificAttack(UAnimMontage* MontageToUse, FName SectionToPlay)
{
	if (MontageToUse) {
		PlayAnimMontage(MontageToUse, 1, SectionToPlay);
	}

}

UPrimitiveComponent* AEnemy::GetCollisionByName(FName Name)
{
	return Collisions[Name];
}

void AEnemy::OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			if(HitEnemies.Contains(player))
			{
				return;
			}

			HitEnemies.Add(player);
			FVector Direction = (player->GetActorLocation() - OverlappedComponent->GetComponentLocation()).GetSafeNormal();
			float forwardDot = FVector::DotProduct(Direction, player->GetActorForwardVector());
			float rightDot = FVector::DotProduct(Direction, player->GetActorRightVector());

			if(forwardDot > 0.7)
			{
				player->SetHitDirection(EHitDirection::FRONT);
			}
			else if(forwardDot < -0.7)
			{
				player->SetHitDirection(EHitDirection::BACK);

			}
			else if(rightDot > 0.f)
			{
				player->SetHitDirection(EHitDirection::RIGHT);
			}
			else
			{
				player->SetHitDirection(EHitDirection::LEFT);

			}

			float CalculatedDamage = CalculateDamage();

			player->TakeDamage(CalculatedDamage, FDamageEvent(), GetController(), this);
			UE_LOG(LogTemp, Warning, TEXT("Hit Player"));
		}
	}
}

void AEnemy::Dodge(int direction)
{

	if(CanBlock() && ShouldBlock())
	{
		Block();
		return;
	}

	int pos = CanUseAblity(EAbilityType::Defensive);
	if(pos > -1)
	{
		UseAbility(EAbilityType::Defensive,pos);
		return;
	}

	if (CanAttack() && Stamina >= DodgeStaminaReduction) {
		EnemyState = EEnemyStates::dodge;
		switch (direction)
		{
		case 0:
			PlayAnimMontage(DodgeMontage, 1, "DodgeForward");
			break;
		case 1:
			PlayAnimMontage(DodgeMontage, 1, "DodgeBackward");
			break;
		case 2:
			PlayAnimMontage(DodgeMontage, 1, "DodgeLeft");
			break;
		case 3:
			PlayAnimMontage(DodgeMontage, 1, "DodgeRight");
			break;

		default:
			break;
		}
		Stamina -= DodgeStaminaReduction;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	}


}

void AEnemy::SetStamina(float newStamina)
{
	Stamina = newStamina;
}

bool AEnemy::isFriendlyInRange()
{

	

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<AActor*> HitActors;

	if(IsPendingKillPending() || !IsValidLowLevel() || !GetWorld())
	{
		return false;
	}
	if(UKismetSystemLibrary::SphereOverlapActors(GetWorld(),GetActorLocation(),1000.f,ObjectTypes,AEnemy::StaticClass(),ActorsToIgnore,HitActors))
	{
		for(AActor*Actor : HitActors)
		{
			if(AEnemy*Enemy = Cast<AEnemy>(Actor))
			{
				RecentEnemies.AddUnique(Enemy);
			}
		}
		return true;
	}

	
	return false;
}

void AEnemy::StartRechargeStamina()
{
	GetWorld()->GetTimerManager().SetTimer(StaminaRechargeHandle, this, &AEnemy::StaminaRechargeTick , 0.5, true);
}

void AEnemy::BuffFriendlies()
{
	UE_LOG(LogTemp, Warning, TEXT("buff"));

	if (EnemyParameters.BuffObject) {
		PlayAnimMontage(MiscMontages, 1, "Buff");

		for (TWeakObjectPtr<AEnemy> WeakEnemy : RecentEnemies)
		{
			if (!WeakEnemy.IsValid()) continue;

			if (AEnemy* Enemy = WeakEnemy.Get()) {
				UImpBuff* buff = NewObject<UImpBuff>(Enemy, EnemyParameters.BuffObject);
				Enemy->Buffs.Add(buff);
				buff->BeginBuff(Enemy);

				if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(Enemy->GetController()))
				{
					AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());
				}
			}
		}
	}
}

void AEnemy::UseSpecial()
{
	EnemyState = EEnemyStates::attack;
	int SpecialNumber = FMath::RandRange(1, 2);
	FString SpecialName = "Special" + FString::FromInt(SpecialNumber);
	UseSpecificAttack(MiscMontages, FName(SpecialName));
	Stamina -= SpecialStaminaReduction;
	Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	GetWorld()->GetTimerManager().SetTimer(FSpecialCooldown,SpecialTimer,false);
}

bool AEnemy::CanUseSpecial() const
{
	if (!MiscMontages) return false;

	for(int i = 0; i < MiscMontages->GetNumSections(); ++i)
	{
		FName sectionName = MiscMontages->GetSectionName(i);
		if(sectionName.ToString().Contains("Special"))
		{
			return  !GetWorld()->GetTimerManager().IsTimerActive(FSpecialCooldown) && Stamina >= SpecialStaminaReduction && CanAttack();
		}
	}
	return false;
}

void AEnemy::ShootProjectile()
{
	if (!EnemyParameters.ProjectileToSpawn) return;

	FActorSpawnParameters params;
	params.Owner = this;
	if(AProjectile*Projectile = GetWorld()->SpawnActor<AProjectile>(EnemyParameters.ProjectileToSpawn,GetActorLocation(),FRotator(0.f),params))
	{
		if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			FVector Direction = (MGI->localPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			Projectile->LaunchProjectile(1.0, Direction);
		}
	}
}

void AEnemy::FacePlayer()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		FVector Direction = (MGI->localPlayer->GetActorLocation() - GetActorLocation());
		Direction.Z = 0;
		FRotator DirectionToFace = Direction.Rotation();
		SetActorRotation(DirectionToFace);
	}
}

void AEnemy::UseAbility(EAbilityType AbilityType,int pos)
{

	switch (AbilityType)
	{
	case EAbilityType::Offensive:
		if (EnemyParameters.OffensiveAbilities.IsValidIndex(pos)) {
			EnemyParameters.OffensiveAbilities[pos]->execute();
			Stamina -= EnemyParameters.OffensiveAbilities[pos]->staminaCost;
			Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);

		}
		break;
	case EAbilityType::Defensive:
		if (EnemyParameters.DefensiveAbilities.IsValidIndex(pos)) {
			EnemyParameters.DefensiveAbilities[pos]->execute();
			Stamina -= EnemyParameters.DefensiveAbilities[pos]->staminaCost;
			Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		}
		break;
	}
}

int AEnemy::CanUseAblity(EAbilityType AbilityType)
{
	switch (AbilityType)
	{
	case EAbilityType::Offensive:
		for(int i = 0; i < EnemyParameters.OffensiveAbilities.Num(); ++i)
		{
			UAbility* Ability = EnemyParameters.OffensiveAbilities[i];
			if(Ability->bShouldExecute() && CanAttack())
			{
				return i;
			}
		}
		break;
	case EAbilityType::Defensive:
		for (int i = 0; i < EnemyParameters.DefensiveAbilities.Num(); ++i)
		{
			UAbility* Ability = EnemyParameters.DefensiveAbilities[i];
			if (Ability->bShouldExecute() && CanAttack())
			{
				return i;
			}
		}
		break;
	}
	return -1;
}

void AEnemy::AbilityLogic(EAbilityType AbilityType, int pos)
{
	switch (AbilityType)
	{
	case EAbilityType::Offensive:
		if (EnemyParameters.OffensiveAbilities.IsValidIndex(pos)) {
			EnemyParameters.OffensiveAbilities[pos]->Logic();
			Stamina -= EnemyParameters.OffensiveAbilities[pos]->staminaCost;
			Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);

		}
		break;
	case EAbilityType::Defensive:
		if (EnemyParameters.DefensiveAbilities.IsValidIndex(pos)) {
			EnemyParameters.DefensiveAbilities[pos]->Logic();
			Stamina -= EnemyParameters.DefensiveAbilities[pos]->staminaCost;
			Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		}
		break;
	}
}

void AEnemy::DetermineHitAnimation(const float& poise)
{
	auto PlayIfSectionExists = [&](const FName& SectionName)
		{
			if (MiscMontages && MiscMontages->GetSectionIndex(SectionName) != INDEX_NONE)
			{
				PlayAnimMontage(MiscMontages, 1.0f, SectionName);
			}
			else
			{
				PlayAnimMontage(MiscMontages, 1.0f, FName("Hit"));
			}
		};

	if (poise < 0)
	{
		switch (HitDirection)
		{
		case EHitDirection::FRONT:
			PlayIfSectionExists(FName("HitFRONT"));
			break;
		case EHitDirection::BACK:
			PlayIfSectionExists(FName("HitBACK"));
			break;
		case EHitDirection::LEFT:
			PlayIfSectionExists(FName("HitLEFT"));
			break;
		case EHitDirection::RIGHT:
			PlayIfSectionExists(FName("HitRIGHT"));
			break;
		default:
			PlayAnimMontage(MiscMontages, 1.0f, FName("Hit"));
			break;
		}
	}
	else
	{
		switch (HitDirection)
		{
		case EHitDirection::FRONT:
			PlayIfSectionExists(FName("LightHitFRONT"));
			break;
		case EHitDirection::BACK:
			PlayIfSectionExists(FName("LightHitBACK"));
			break;
		case EHitDirection::LEFT:
			PlayIfSectionExists(FName("LightHitLEFT"));
			break;
		case EHitDirection::RIGHT:
			PlayIfSectionExists(FName("LightHitRIGHT"));
			break;
		default:
			PlayAnimMontage(MiscMontages, 1.0f, FName("Hit"));
			break;
		}
	}
}


bool AEnemy::CanAttack() const
{
	return EnemyState == EEnemyStates::none;
}

void AEnemy::Block()
{
	if(CanBlock())
	{
		EnemyState = EEnemyStates::blocking;

		PlayAnimMontage(MiscMontages, 1, "Block");
	}
}

bool AEnemy::CanBlock() const
{
	return EnemyState == EEnemyStates::none && MiscMontages->IsValidSectionName("Block");
}

void AEnemy::EndBlock()
{
	EnemyState = EEnemyStates::none;
}

bool AEnemy::ShouldBlock() const
{
	/*we should block if:
	 *	1. We can afford it. i.e., we have enough to dodge, but also want to conserve stamina but what defines this?
	 *	2. Keep the player close and wait for the move set to end.
	 *	3.The player is already attacking , we can't dodge.
	 *
	 */

	float score = 0.f;
	float attackingWeight = 0.f;

	if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		//stam*x
		float normalizedStamina = (Stamina / MaxStamina);
		if(GI->localPlayer)
		{
			if(GI->localPlayer->GetCurrentState() == PlayerStates::ATTACKING)
			{
				//current state*1.f
				attackingWeight = 1.0f;
			}
	

			float Distance = (GI->localPlayer->GetActorLocation() - GetActorLocation()).Size();

			float normalizedDistance = FMath::Clamp(1-(Distance/400.f),0.f,1.f); //400 is considered in range

			score = normalizedStamina * 0.3 + attackingWeight * 0.4 + normalizedDistance * 0.3;

		}

	}


	return score >= 0.38f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ScaleEnemyAccordingToRound();

	Health = MaxHealth;
	Stamina = MaxStamina;
	EnemyParameters.currentPoise = EnemyParameters.maxPoise;

	StartRechargeStamina();

	RightHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHitBoxBeginOverlap);
	LeftHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHitBoxBeginOverlap);
	CenterHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHitBoxBeginOverlap);


	for (UClass* AbilityClass : EnemyParameters.AbilityClasses)
	{
		UAbility* Ability = NewObject<UAbility>(this, AbilityClass);

		switch (Ability->AbilityActivationType)
		{
		case EAbilityActivationType::Activate:
			if (Ability->AbilityType == EAbilityType::Offensive)
			{
				EnemyParameters.OffensiveAbilities.AddUnique(Ability);

			}
			else {
				EnemyParameters.DefensiveAbilities.AddUnique(Ability);
			}
			break;

		case EAbilityActivationType::Timer:
			Ability->Logic();
			break;

		case EAbilityActivationType::Event:
			break;
		}
	}


	EnemyParameters.InitResistances();
	EnemyParameters.initElementProgressionMap();

	float ShouldSetRandom = FMath::FRandRange(0.f, 1.f);

	if (ShouldSetRandom >= 0.25 && !EnemyParameters.bIsBoss && EnemyType != EEnemyType::mage) {
		SetRandomElementType();
	}

}

void AEnemy::ApplyElementDebuff(EElementTypes ElementType,AActor*DamageCauser)
{
	if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{

		if (TSubclassOf<UElementDebuff>* FoundDebuff = GI->ElementDebuffs.Find(ElementType))
		{
			switch (ElementType)
			{
			case EElementTypes::Physical:
				break;
			case EElementTypes::Fire:
				if (UFireDebuff* FireDebuff = NewObject<UFireDebuff>(this, *FoundDebuff))
				{
					FireDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::Water:
				if (UWaterDebuff* WaterDebuff = NewObject<UWaterDebuff>(this, *FoundDebuff))
				{
					WaterDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::Shadow:

				if (UShadowDebuff* ShadowDebuff = NewObject<UShadowDebuff>(this, *FoundDebuff))
				{
					ShadowDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::Lightening:
				if (ULighteningDebuff* LighteningDebuff = NewObject<ULighteningDebuff>(this, *FoundDebuff))
				{
					LighteningDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::Bleed:
				if (UBleedDebuff* BleedDebuff = NewObject<UBleedDebuff>(this, *FoundDebuff))
				{
					BleedDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::Poison:
				if (UPoisonDebuff* PoisonDebuff = NewObject<UPoisonDebuff>(this, *FoundDebuff))
				{
					PoisonDebuff->ExecuteDebuff_Implementation(this, DamageCauser);
				}
				break;
			case EElementTypes::NONE:
				break;
			}
		}
	}
}

void AEnemy::CalculateElementProgression(APlayerCharacter* DamageCauser)
{

	if (!DamageCauser) return;

	if(APlayerCharacterState*PCS = DamageCauser->GetPlayerCharacterState())
	{
		if(AWeapon*Weapon = Cast<AWeapon>(PCS->PlayerGear[EGearType::RHand]))
		{
			if (!Weapon->ItemData) return;

			if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData)) {

				EElementTypes ElementType = EnemyParameters.IncreaseElementProgression(WeaponItemData->WeaponElementType, WeaponItemData->ElementTypeProgressionDamage);
				if (ElementType != EElementTypes::NONE)
				{
					ApplyElementDebuff(ElementType, Weapon);
				}
			}

		}
	}
}

void AEnemy::HandlePlayerAttack(APlayerCharacter* player, float DamageAmount)
{
	if (!player->GetAttackingWeapon()) return;

	CalculateElementProgression(player);

	if (EnemyState != EEnemyStates::blocking) {
		if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (GI->BloodSystems.IsValidIndex(0))
			{
				// This spawns a Niagara system once, auto-destroys it when done
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GI->BloodSystems[0], GetActorLocation());
			}
		}

		if (UWeaponInstance*PlayerWeaponInstance = player->GetAttackingWeapon()->WeaponInstance)
		{
			if (PlayerWeaponInstance->WeaponElementType != EElementTypes::NONE && PlayerWeaponInstance->WeaponElementType != EElementTypes::Physical) {
				ApplyElementDamageParticle(PlayerWeaponInstance->WeaponElementType);
				if (EElementTypes* Weakness = WeaknessMap.Find(PlayerWeaponInstance->WeaponElementType))
				{
					if (EnemyParameters.ElementType == *Weakness)
					{
						DamageAmount *= 2;
						if (CriticalHitMaterial) {
							FTimerHandle FWeaknessTH;
							UMaterialInterface* OgMaterial = GetMesh()->GetMaterial(0);
							GetMesh()->SetMaterial(0, CriticalHitMaterial);
								GetWorld()->GetTimerManager().SetTimer(FWeaknessTH, [this, OgMaterial]
									{
										GetMesh()->SetMaterial(0, OgMaterial);
									}, 0.1, false);
						}
					}
				}
			}
		}

		player->OnEnemyDamageTaken.Broadcast(this, DamageAmount);


		Health -= DamageAmount;
		Health = FMath::Clamp(Health, 0, MaxHealth);

		if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
		{
			PCC->PlayCameraShake();
		}


		if (APlayerCharacterState*PCS = player->GetPlayerCharacterState())
		{
			if(UAbility**FoundAbility = PCS->LearnedAbilities.FindByPredicate([](UAbility*Ability)
			{
					return Ability && Ability->IsA(UAbility_Leech::StaticClass());
			}))
			{
				if (UAbility_Leech* FoundLeech = Cast<UAbility_Leech>(*FoundAbility)) {
					FoundLeech->DamageDealt = DamageAmount;
					if (FoundLeech->bShouldExecute_Implementation())
					{
						FoundLeech->execute_Implementation();
					}
				}

			}


			if (AWeapon* playerWeapon = player->GetCurrentWeapon())
			{
				EnemyParameters.currentPoise -= playerWeapon->PoiseDamage * (1.f - EnemyParameters.poiseResistance);

				EnemyParameters.currentPoise = FMath::Clamp(EnemyParameters.currentPoise, 0.0f, EnemyParameters.maxPoise);

			}
		}
		if (EnemyState == EEnemyStates::backstabbed)
		{
			FVector Location = player->GetActorForwardVector() * 5.f;
			FVector RightLocation = GetActorRightVector() * 10.0f;

			SetActorLocation(GetActorLocation() + Location + RightLocation, true);

			PlayAnimMontage(MiscMontages, 1, "BackstabRecieved");

			GetWorld()->GetTimerManager().SetTimer(FBackstabTimer, 1.5f, false);


		}
		else if (EnemyState == EEnemyStates::parried)
		{
			FVector Location = player->GetActorForwardVector() * 5.f;
			FVector RightLocation = GetActorRightVector() * 10.0f;

			SetActorLocation(GetActorLocation() + Location + RightLocation, true);

			//play parried animation.
			PlayAnimMontage(MiscMontages, 1, "ParryRecieved");
		}
		else
		{
			EnemyState = EEnemyStates::hit;
			DetermineHitAnimation(EnemyParameters.currentPoise);
		}
	}
	else
	{
		EnemyParameters.currentPoise -= Damage * EnemyParameters.poiseResistance * 2;
		PlayAnimMontage(MiscMontages, 1, "BlockSuccess");
	}

}

void AEnemy::HandleDebuffAttack(ADebuffActor* DebuffActor, float DamageAmount)
{
	Health -= DamageAmount;
	Health = FMath::Clamp(Health, 0, MaxHealth);
	EnemyState = EEnemyStates::hit;
	DetermineHitAnimation(EnemyParameters.currentPoise);
}

TSubclassOf<AItem> AEnemy::ItemToDrop()
{
	float TotalWeight = 0.f;
	for(const auto&pair : EnemyParameters.ItemDropMap)
	{
		TotalWeight += pair.Value;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);

	for(const auto&pair : EnemyParameters.ItemDropMap)
	{
		Roll -= pair.Value;
		if(Roll <= 0.f)
		{
			return pair.Key;
		}
	}

	return nullptr;
}

void AEnemy::DropItem(TSubclassOf<AItem> itemToDrop)
{
	if (!itemToDrop) return;

	FVector Location = GetActorLocation();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if(AItem*ItemToSpawn = GetWorld()->SpawnActor<AItem>(itemToDrop,Location,FRotator(0.f),params))
	{
		
	}
}

void AEnemy::SlowEnemySpeed(float Time)
{
	if(UCharacterMovementComponent*CMC = GetCharacterMovement())
	{
		CMC->MaxWalkSpeed /= 2;
		FTimerHandle FSlowTimer;
		GetWorld()->GetTimerManager().SetTimer(FSlowTimer, [CMC]()
		{
				CMC->MaxWalkSpeed *= 2;
		}, Time, false);
	}
}

float AEnemy::CalculateDamage()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if(APlayerCharacter*player = MGI->localPlayer)
		{
			if(APlayerCharacterState*PCS = player->GetPlayerCharacterState())
			{
				float ElementalDamage = 0.f;
				float PhysicalDamage = Damage * (1-PCS->playerStats.Resistances[EElementTypes::Physical]);
				if (EnemyParameters.ElementType != EElementTypes::NONE) {
					ElementalDamage = EnemyParameters.ElementDamage * (1 - PCS->playerStats.Resistances[EnemyParameters.ElementType]);
				}

				return PhysicalDamage + ElementalDamage;
			}
		}
	}
	return Damage;
}

void AEnemy::StopDebuffs()
{
	for(const auto & pair : CurrentDebuffs)
	{
		if(UElementDebuff*Debuff = pair.Value)
		{
			Debuff->EndBuff();
		}
	}

	CurrentDebuffs.Empty();
}

void AEnemy::StaminaRechargeTick()
{
	Stamina += StaminaAmount;
	Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	UE_LOG(LogTemp, Warning, TEXT("Enemy Stamina Recharge!! : %f"), Stamina);
}

void AEnemy::SpawnBossBuff()
{
	if (InteractableBuffActor)
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector Location = GetActorLocation();
		FHitResult HitResult;
		FVector End = Location + (FVector::DownVector * 1000.f);
		if(GetWorld()->LineTraceSingleByChannel(HitResult,Location,End,ECC_Visibility))
		{
			FVector SpawnLoc = HitResult.ImpactPoint + (FVector::UpVector * 20.f);
			GetWorld()->SpawnActor<AInteractableBuffActor>(InteractableBuffActor,SpawnLoc , FRotator::ZeroRotator, params);

		}
		else {
			GetWorld()->SpawnActor<AInteractableBuffActor>(InteractableBuffActor, Location, FRotator::ZeroRotator, params);
		}
	}
}

void AEnemy::HideBossHealthBarUI(APlayerCharacterState* pcs)
{
	if (UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (!MainGameInstance->localPlayer) return;

		if (APlayerController* PC = Cast<APlayerController>(MainGameInstance->localPlayer->GetController()))
		{
			if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
			{
				if (EnemyParameters.bIsBoss) {
					if (UMainPlayerWidget* MainPlayerHUDWidget = InGamePlayerHUD->GetMainUIWidget())
					{
						MainPlayerHUDWidget->BossInfo->SetVisibility(ESlateVisibility::Hidden);
					}
				}

			}
		}
	}
}

void AEnemy::UpdateBossHealthUI()
{
	if (UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		//might break an edge case??
		if (!MainGameInstance->localPlayer) return;

		if (APlayerController* PC = Cast<APlayerController>(MainGameInstance->localPlayer->GetController()))
		{
			if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
			{
				if (UMainPlayerWidget* MainPlayerHUDWidget = InGamePlayerHUD->GetMainUIWidget())
				{
					MainPlayerHUDWidget->UpdateProgressBar("BossHealth", Health / MaxHealth);
				}
			}
		}
	}
}

void AEnemy::ActivateBossTeleporter()
{
	TArray<AActor*> Teleporters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbTeleporter::StaticClass(), Teleporters);

	for(AActor*TeleporterActor : Teleporters)
	{
		if(AOrbTeleporter*Teleporter = Cast<AOrbTeleporter>(TeleporterActor))
		{
			if(Teleporter->TeleportType == EOrbTeleportType::Reward)
			{
				Teleporter->BoxComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			}
		}
	}
}

void AEnemy::UpdatePlayerEXPUI(APlayerCharacterState* pcs)
{
	if (!IsValid(pcs)) return;

	if (APlayerController* PlayerController= Cast<APlayerController>(pcs->GetPawn()->GetController())) {
		if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PlayerController->GetHUD())) {
			if (ULevelUpWidget* LevelUpWidget = Cast<ULevelUpWidget>(InGamePlayerHUD->LevelUpWidget))
			{
				LevelUpWidget->UpdateEXPProgressBar(pcs->playerStats.getExp() / pcs->playerStats.GetNextLevelEXP());
			}
		}
	}
}

void AEnemy::ResetEnemy()
{
	if(AAIEnemy*EnemyController = Cast<AAIEnemy>(GetController()))
	{
		EnemyController->ChangeAIState(EnemyController->IdleState.Get());
	}
	Health = MaxHealth;
	Stamina = MaxStamina;
	
}

bool AEnemy::bShouldEnemyReset()
{
	if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
	{
		if (AIEnemyController->CurrentState == AIEnemyController->AttackState.Get()) {
			if (UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
			{
				if(APlayerCharacter*player = MainGameInstance->localPlayer)
				{
					if(player->bGetIsDead())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void AEnemy::ResetEnemyState()
{
	EnemyState = EEnemyStates::none;
}

void AEnemy::SetRandomElementType()
{
	int RandomNum = FMath::RandRange(0, 6);

	switch(RandomNum)
	{
	case 0:
		EnemyParameters.ElementType = EElementTypes::NONE;
		break;
	case 1:
		EnemyParameters.ElementType = EElementTypes::Poison;
		break;
	case 2:
		EnemyParameters.ElementType = EElementTypes::Fire;
		break;
	case 3:
		EnemyParameters.ElementType = EElementTypes::Shadow;
		break;
	case 4:
		EnemyParameters.ElementType = EElementTypes::Lightening;
		break;
	case 5:
		EnemyParameters.ElementType = EElementTypes::Water;
		break;
	case 6:
		EnemyParameters.ElementType = EElementTypes::Physical;
		break;
	default: 		
		EnemyParameters.ElementType = EElementTypes::NONE;
		break;
	}

	if(EnemyParameters.ElementType != EElementTypes::NONE)
	{
		EnemyParameters.ElementDamage = FMath::FRandRange(15.f,100.f);
		EnemyParameters.ResistanceDamage = FMath::FRandRange(0.1, 1.f);
	}
}

void AEnemy::PlaySoundBasedOnSurface()
{

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {

		USoundBase* Sound = nullptr;

		if (MostRecentSurfaceType == SURFACE_Water || MostRecentSurfaceType == SURFACE_Lava || MostRecentSurfaceType == SURFACE_Poison || MostRecentSurfaceType == SURFACE_Blood)
		{
		 Sound = MGI->Liquid[FMath::RandRange(0, MGI->Liquid.Num() - 1)];

		}
		else if (MostRecentSurfaceType == SURFACE_Cobble)
		{
			 Sound = MGI->Cobble[FMath::RandRange(0, MGI->Cobble.Num() - 1)];
		}
		else if (MostRecentSurfaceType == SURFACE_Grass)
		{
			 Sound = MGI->Grass[FMath::RandRange(0, MGI->Grass.Num() - 1)];

		}
		else if (MostRecentSurfaceType == SURFACE_Wood)
		{
			Sound = MGI->Wood[FMath::RandRange(0, MGI->Wood.Num() - 1)];

		}
		else
		{
			Sound = MGI->Cobble[FMath::RandRange(0, MGI->Cobble.Num() - 1)];
		}

		if (Sound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), FRotator::ZeroRotator, 1, 1, 0, MGI->Attenuation);
		}

	}
}

void AEnemy::ApplyElementDamageParticle(EElementTypes WeaponElementType)
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if(UNiagaraSystem**NS = MGI->ElementalDamageMap.Find(WeaponElementType))
		{
			if(UNiagaraComponent*NC = UNiagaraFunctionLibrary::SpawnSystemAttached(*NS, GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
			{
				NC->SetVariableObject("User.EnemyMesh", GetMesh());
			}
		}
	}
}

void AEnemy::Destroyed()
{
	Super::Destroyed();
	if (EnemyParameters.bIsBoss) {
		if (ADungeonManager* DungeonManager = Cast<ADungeonManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonManager::StaticClass())))
		{
			if (AOrbTeleporter* Teleporter = Cast<AOrbTeleporter>(UGameplayStatics::GetActorOfClass(GetWorld(), AOrbTeleporter::StaticClass()))) {
				DungeonManager->ReactivateTeleporter(Teleporter);
			}
		}
	}
}

void AEnemy::ScaleEnemyAccordingToRound()
{
	if (ADungeonCrawlerGM* GM = GetWorld()->GetAuthGameMode<ADungeonCrawlerGM>()) {
		float Scale = FMath::Pow(1.1f, GM->GetRoundCount()); // Grows 10% per round

		MaxHealth *= Scale;
		MaxStamina *= Scale;
		EnemyParameters.ElementDamage *= Scale;
		Damage *= Scale;

	}
}


void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(GetWorld()->GetTimerManager().IsTimerActive(StaminaRechargeHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeHandle);
	}
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bShouldEnemyReset())
	{
		ResetEnemy();
	}


	UPrimitiveComponent* Base = GetCharacterMovement()->CurrentFloor.HitResult.Component.Get();
	if (Base && Base->GetBodyInstance())
	{
		UPhysicalMaterial* PhysMat = Base->GetBodyInstance()->GetSimplePhysicalMaterial();

		if (PhysMat)
		{

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMat);
			MostRecentSurfaceType = SurfaceType;
		}
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{

	if(AAIEnemy*AIEnemyController = Cast<AAIEnemy>(GetController()))
	{
		if(AIEnemyController->CurrentState == AIEnemyController->IdleState.Get())
		{
			AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());
		}
	}


	if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser)) {
		HandlePlayerAttack(player, DamageAmount);
	}
	else if(ADebuffActor*DebuffActor = Cast<ADebuffActor>(DamageCauser))
	{
		HandleDebuffAttack(DebuffActor,DamageAmount);
	}
	else
	{

		Health -= DamageAmount;
		Health = FMath::Clamp(Health, 0, MaxHealth);
	}

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		USoundBase* Sound = MGI->Hit[FMath::RandRange(0, MGI->Hit.Num() - 1)];
		if (Sound) {
			UGameplayStatics::PlaySound2D(GetWorld(), Sound);
		}
	}

	if(EnemyParameters.bIsBoss)
	{
		UpdateBossHealthUI();
	}

	if(Health <= 0)
	{
		if (!bDead) {


			AEnemy::OnEnemyDeath.Broadcast(this);
			InstancedOnEnemyDeath.Broadcast(this);

			if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser))
			{
				if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(player->GetPlayerState()))
				{
					SetActorEnableCollision(false);
					pcs->playerStats.UpdateEXP(EnemyParameters.Exp);
					StopDebuffs();

					if (EnemyParameters.bIsBoss) {
						HideBossHealthBarUI(pcs);
						SpawnBossBuff();

					}
							
					UpdatePlayerEXPUI(pcs);

					if(player->bIsTargetedEnemy(this))
					{
						player->ResetTargetedEnemy();
					}

					//check for bloodfrenzy
					if (UAbility** FoundAbility = pcs->LearnedAbilities.FindByPredicate([](UAbility* Ability)
						{
							return Ability && Ability->IsA(UAbility_Blood_Frenzy::StaticClass());
						}))
					{
						if (UAbility_Blood_Frenzy* Blood_Frenzy = Cast<UAbility_Blood_Frenzy>(*FoundAbility)) {
							++Blood_Frenzy->KillCount;
							if (Blood_Frenzy->bShouldExecute_Implementation())
							{
								Blood_Frenzy->execute_Implementation();
							}
						}

					}


				}
			}

			if (EnemyType != EEnemyType::skeleton) {
				GetMesh()->PlayAnimation(DeathAnimation, false);
			}
			else
			{
				GetMesh()->SetSimulatePhysics(true);
			}

			if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
			{
				if (FMath::FRandRange(0.f, 1.f) >= DropChance) {
					DropItem(ItemToDrop());
				}
				if (AIEnemyController->DeadState) {
					AIEnemyController->ChangeAIState(AIEnemyController->DeadState.Get());
				}
			}

			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
			bDead = true;
		}

	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

