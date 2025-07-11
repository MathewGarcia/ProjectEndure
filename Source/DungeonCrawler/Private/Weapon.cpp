// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Enemy.h"
#include "EnvironmentalItem.h"
#include "MainGameInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "WeaponInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Field/FieldSystemObjects.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemObjects.h"           
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Comp"));
	CapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetupAttachment(Root);

}


void AWeapon::DetermineHitDirection(AEnemy* Enemy)
{
	if (!Enemy) return;

	FVector Direction = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	float forwardDot = FVector::DotProduct(Direction, Enemy->GetActorForwardVector());
	float rightDot = FVector::DotProduct(Direction, Enemy->GetActorRightVector());

	if (forwardDot > 0.7)
	{
		Enemy->HitDirection = EHitDirection::FRONT;
		
	}
	else if (forwardDot < -0.7)
	{
		Enemy->HitDirection = EHitDirection::BACK;

	}
	else if (rightDot > 0.f)
	{
		Enemy->HitDirection = EHitDirection::RIGHT;

	}
	else
	{
		Enemy->HitDirection = EHitDirection::LEFT;
	}
}

void AWeapon::Swing()
{
	bIsSwinging = true;

	StartLocation = bIsSkeletal ? SkeletalMeshComponent->GetComponentLocation() : StaticMeshComponent->GetComponentLocation();

}

void AWeapon::SetIsSwinging(const bool& newVal)
{
	bIsSwinging = newVal;
}

void AWeapon::StopMontages(UAnimInstance* AnimInstance)
{
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(TwohandMoveSet))
	{
		AnimInstance->Montage_StopWithBlendOut(0.f, TwohandMoveSet);
	}
	else if (AnimInstance->Montage_IsPlaying(OnehandMoveSet))
	{
		AnimInstance->Montage_StopWithBlendOut(0.f, OnehandMoveSet);
	}

}


void AWeapon::InitWeapon()
{
	UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData);
	if (!WeaponItemData) return;

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		if (APlayerCharacter* player = MGI->localPlayer) {
			if (APlayerCharacterState* pcs = player->GetPlayerCharacterState()) {
				if (!WeaponInstance)
				{
					if (WeaponInstance = NewObject<UWeaponInstance>(pcs,UWeaponInstance::StaticClass()); WeaponInstance)
					{
						WeaponInstance->ItemData = WeaponItemData;
						WeaponInstance->ItemData = WeaponItemData;
						WeaponInstance->Strength = WeaponItemData->Strength;
						WeaponInstance->BaseDamage = WeaponItemData->BaseDamage;
						WeaponInstance->Intellect = WeaponItemData->Intellect;
						WeaponInstance->ElementTypeDamage = WeaponItemData->ElementTypeDamage;
						WeaponInstance->WeaponElementType = WeaponItemData->WeaponElementType;
						WeaponInstance->IronCoreResourceAmt = WeaponItemData->IronCoreResourceAmt;
						WeaponInstance->IronFragmentResourceAmt = WeaponItemData->IronFragmentResourceAmt;
						WeaponInstance->IronStoneResourceAmt = WeaponItemData->IronStoneResourceAmt;
						WeaponInstance->ElementalCoreResourceAmt = WeaponItemData->ElementalCoreResourceAmt;
						WeaponInstance->EXPNeeded = WeaponItemData->EXPNeeded;
						WeaponInstance->Quantity = ItemData->Quantity;
					}
				}
			}
		}
	}
}

void AWeapon::ThrowWeapon(APlayerCharacter* player, AEnemy* CurrentEnemy)
{

	if (!player || !CurrentEnemy || bIsSwinging || TargetedEnemy == CurrentEnemy) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	InterpAlphaVal = 0.f;
	InterpStart = GetActorLocation();
	InterpEnd = CurrentEnemy->GetActorLocation();
	TargetedEnemy = CurrentEnemy;
	bIsSwinging = true;
	HitActors.Empty();

}

void AWeapon::ReturnToPlayer()
{
	bIsSwinging = false;
	TargetedEnemy = nullptr;
	CurrentThrownHits = 0;
	HitActors.Empty();

	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOwner()))
	{
		FName GearSocket = player->GetValidSocket(EGearType::RHand, this);
		AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GearSocket);
	}
}


void AWeapon::MoveWeapon(float DeltaTime)
{
			InterpAlphaVal += DeltaTime * 10.f;
			FVector NewLocation = FMath::Lerp(InterpStart, InterpEnd, InterpAlphaVal);
			SetActorLocation(NewLocation, true);

	if(InterpAlphaVal >= 1.f)
	{
		FVector CurrentWeaponLocation = CapsuleComponent->GetComponentLocation();
		const FQuat Quat = CapsuleComponent->GetComponentQuat();
		float Radius = CapsuleComponent->GetScaledCapsuleRadius();
		float Height = CapsuleComponent->GetScaledCapsuleHalfHeight();
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());
		FHitResult Hit;
		FCollisionQueryParams params;
		params.bTraceComplex = true;
		params.AddIgnoredActors(ActorsToIgnore);
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		DrawDebugCapsule(GetWorld(), CurrentWeaponLocation, Height, Radius, Quat, FColor::Red);


		//Sphere overlap.
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		if (GetWorld()->SweepSingleByObjectType(Hit, StartLocation, CurrentWeaponLocation, Quat, ObjectQueryParams, FCollisionShape::MakeCapsule(Radius, Height), params))
		{
			if (AEnemy* Enemy = Cast<AEnemy>(Hit.GetActor()))
			{

				if(HitActors.Contains(Enemy))
				{
					ReturnToPlayer();
					return;
				}

				DetermineHitDirection(Enemy);
				HitActors.Add(Enemy);

				float Damage = CalculateDamage(Enemy);
				Enemy->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
				TArray<AActor*> InnerActorsToIgnore;
				InnerActorsToIgnore.Add(this);
				InnerActorsToIgnore.Add(Enemy);
				TArray<AActor*> OutActors;
				if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), 1000.f, ObjectTypes, AEnemy::StaticClass(), InnerActorsToIgnore, OutActors))
					{
						AEnemy* ClosestEnemy = nullptr;
						UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation(), 1000.f, 12, FLinearColor::White, 123, 10);
						float closestDistance = 1000000.0f;
						for (AActor* Actor : OutActors)
						{
							if (AEnemy* enemy = Cast<AEnemy>(Actor)) {
								if(HitActors.Contains(enemy))
								{
									continue;
								}
								float Distance = (Actor->GetActorLocation() - GetActorLocation()).Size();
								if (Distance < closestDistance)
								{

									ClosestEnemy = enemy;
								}
							}
						}

					if(ClosestEnemy)
						{
						TargetedEnemy = ClosestEnemy;
						InterpAlphaVal = 0.f;
						InterpStart = GetActorLocation();
						InterpEnd = TargetedEnemy->GetActorLocation();
						++CurrentThrownHits;
						}
					else
					{
						ReturnToPlayer();
					}
					}

			}

		}
	}

}

void AWeapon::ActivateWeapon()
{
	FVector CurrentWeaponLocation = CapsuleComponent->GetComponentLocation();
	const FQuat Quat = CapsuleComponent->GetComponentQuat();
	float Radius = CapsuleComponent->GetScaledCapsuleRadius();
	float Height = CapsuleComponent->GetScaledCapsuleHalfHeight();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> Hits;
	FCollisionQueryParams params;
	params.bTraceComplex = true;
	params.AddIgnoredActors(ActorsToIgnore);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	DrawDebugCapsule(GetWorld(), CurrentWeaponLocation, Height, Radius, Quat, FColor::Red);
	if (GetWorld()->SweepMultiByObjectType(Hits, StartLocation, CurrentWeaponLocation, Quat, ObjectQueryParams, FCollisionShape::MakeCapsule(Radius, Height), params))
	{
		for (auto Hit : Hits)
		{
			if (AEnemy* Enemy = Cast<AEnemy>(Hit.GetActor()))
			{

				if (HitActors.Contains(Enemy))
				{
					continue;
				}
				DetermineHitDirection(Enemy);

				HitActors.Add(Enemy);


				float Damage = CalculateDamage(Enemy);

				Enemy->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
			}

		}

	}
}

void AWeapon::AttachElementType() const
{
	if(WeaponInstance)
	{
		if (WeaponType == EWeaponType::Mage ||WeaponType == EWeaponType::Bow || WeaponInstance->WeaponElementType == EElementTypes::NONE || WeaponInstance->WeaponElementType == EElementTypes::Physical) return;

		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
			if(UNiagaraSystem**NS = MGI->WeaponParticleMap.Find(WeaponInstance->WeaponElementType))
			{
				if(UNiagaraComponent*NC = UNiagaraFunctionLibrary::SpawnSystemAttached(*NS, StaticMeshComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale,true))
				{
					NC->SetVariableStaticMesh("User.WeaponMesh", StaticMeshComponent->GetStaticMesh());
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();


	if (CapsuleComponent->OnComponentBeginOverlap.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("On Component Begin Overlap successfully bound!"));
	}

	if(ItemData && !WeaponInstance)
	{

		InitWeapon();
	}
}

float AWeapon::CalculateDamage(AEnemy* Enemy)
{

	if (!WeaponInstance) return 0.f;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOwner()))
	{
		if(APlayerCharacterState*PCS = player->GetPlayerCharacterState())
		{
			if (UWeaponItemData*WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
				float TotalStrength = 0.0f;
				if (WeaponType == EWeaponType::OneHand)
				{
					//because we are using one hand/half our strength
					TotalStrength = PCS->GetTotalStrength() / 2;
				}
				else if(WeaponType == EWeaponType::TwoHand)
				{
					TotalStrength = PCS->GetTotalStrength();
				}


				float BaseDamage = (WeaponInstance->BaseDamage + (TotalStrength * WeaponItemData->StrengthScale) + (PCS->GetTotalIntellect() * WeaponItemData->IntellectScale)) * (1.f - Enemy->EnemyParameters.PhysicalResistance);

				float ElementDamage = WeaponInstance->ElementTypeDamage*(1.f - Enemy->EnemyParameters.Resistances[WeaponInstance->WeaponElementType]);

				float Damage = (BaseDamage + ElementDamage) * PCS->DamageMultiplier;
				UE_LOG(LogTemp, Warning, TEXT("DAMAGE LOG: %f, TOTAL STRENGTH: %f"), Damage, PCS->GetTotalStrength());

				return Damage;
			}
		}
	}
	return 0.f;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner())) {
		if (bIsSwinging && CurrentThrownHits < MaxThrownHits && TargetedEnemy && player->GetCurrentState() == PlayerStates::ABILITY)
		{
			MoveWeapon(DeltaTime);
		}
		else if (bIsSwinging)
		{
			FVector CurrentWeaponLocation = CapsuleComponent->GetComponentLocation();
			const FQuat Quat = CapsuleComponent->GetComponentQuat();
			float Radius = CapsuleComponent->GetScaledCapsuleRadius();
			float Height = CapsuleComponent->GetScaledCapsuleHalfHeight();
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(GetOwner());
			TArray<FHitResult> Hits;
			FCollisionQueryParams params;
			params.bTraceComplex = true;
			params.AddIgnoredActors(ActorsToIgnore);
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
			ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
			ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
			DrawDebugCapsule(GetWorld(), CurrentWeaponLocation, Height, Radius, Quat, FColor::Red);
			if (GetWorld()->SweepMultiByObjectType(Hits, StartLocation, CurrentWeaponLocation, Quat, ObjectQueryParams, FCollisionShape::MakeCapsule(Radius, Height), params))
			{
				for (auto Hit : Hits)
				{
					if (AEnemy* Enemy = Cast<AEnemy>(Hit.GetActor()))
					{

						if (HitActors.Contains(Enemy))
						{
							continue;
						}

						DetermineHitDirection(Enemy);

						HitActors.Add(Enemy);

						float Damage = 0.0f;

							if (player->GetCurrentState() == PlayerStates::ATTACKING && player->IsBackStab(Enemy))
							{
								if (UAnimInstance* AnimInstance = player->GetMesh()->GetAnimInstance())
								{
									if (!AnimInstance->Montage_IsPlaying(Misc)) {
										StopMontages(AnimInstance);
										player->PlayAnimMontage(Misc, 1, "Backstab");
										Enemy->EnemyState = EEnemyStates::backstabbed;
										Damage = CalculateDamage(Enemy) * 1.5;
									}
								}
							}
							else if (player->GetCurrentState() == PlayerStates::ATTACKING && player->IsParry(Enemy))
							{
								if (UAnimInstance* AnimInstance = player->GetMesh()->GetAnimInstance())
								{
									if (!AnimInstance->Montage_IsPlaying(Misc)) {
										StopMontages(AnimInstance);
										player->PlayAnimMontage(Misc, 1, "ParryAttack");
										Damage = CalculateDamage(Enemy) * 2;
									}
								}
							}
							else {

								Damage = CalculateDamage(Enemy);
							}

						Enemy->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());

					}
					else if (AEnvironmentalItem* EnvironmentalItem = Cast<AEnvironmentalItem>(Hit.GetActor()))
					{
						if (EnvironmentalItem->ActorHasTag("Destructible")) {

							EnvironmentalItem->TakeDamage(100.f, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());

							UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnvironmentalItem->NiagaraSystemToPlay, EnvironmentalItem->GetActorLocation());
							if (NiagaraComponent && EnvironmentalItem->Mesh && EnvironmentalItem->Mesh->GetStaticMesh())
							{
								NiagaraComponent->SetVariableStaticMesh(FName("User.BrokenMesh"), EnvironmentalItem->Mesh->GetStaticMesh());
							}
						}
					}
				}
			}
		}
	}


}

