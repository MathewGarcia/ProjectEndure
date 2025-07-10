// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Elemental_Beam.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UAbility_Elemental_Beam::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->PlayAnimMontage(MontageToPlay, player->MontageSpeed);
		}
	}
}

bool UAbility_Elemental_Beam::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
		}
	}
	return false;
}

void UAbility_Elemental_Beam::Logic()
{
	Super::Logic();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {

			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			ChannelBeam(player);
			player->bUseControllerRotationYaw = true;
			GetWorld()->GetTimerManager().SetTimer(FCustomTick,[this,safePlayer]
			{
					if (!safePlayer.IsValid()) return;

					if (APlayerCharacter* localPlayer = safePlayer.Get()) {
						ChannelBeam(localPlayer);
					}
			},0.2, true);

			GetWorld()->GetTimerManager().SetTimer(FDuration, [this,safePlayer]()
			{
					if (!safePlayer.IsValid()) return;
				if(GetWorld()->GetTimerManager().IsTimerActive(FCustomTick))
				{
					GetWorld()->GetTimerManager().ClearTimer(FCustomTick);
					GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
					if(APlayerCharacter*localPlayer = safePlayer.Get())
					{
						localPlayer->StopAnimMontage(MontageToPlay);
						localPlayer->bUseControllerRotationYaw = false;

					}
				}
			}, Duration, false);
		}
	}
}

void UAbility_Elemental_Beam::ChannelBeam(APlayerCharacter* player)
{
	if (!player) return;

	int32 ViewportSizeX, ViewportSizeY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Center of the screen
	float ScreenCenterX = ViewportSizeX * 0.5f;
	float ScreenCenterY = ViewportSizeY * 0.5f;

	FVector WorldOrigin;
	FVector WorldDirection;

	// Deproject the screen position to a world space ray
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(
		ScreenCenterX,
		ScreenCenterY,
		WorldOrigin,
		WorldDirection
	);

	// Line trace from the center of the screen to get an impact point.
	FVector TraceEnd = WorldOrigin + WorldDirection * 5000.f;
	FHitResult CameraHit;
	FVector TargetPoint = TraceEnd;
	UKismetSystemLibrary::DrawDebugLine(player->GetWorld(), WorldOrigin,TraceEnd,FLinearColor::Red,123);
	if (player->GetWorld()->LineTraceSingleByChannel(CameraHit, WorldOrigin, TraceEnd, ECC_Pawn))
	{
	
		TargetPoint = CameraHit.ImpactPoint;
	}
	//Trace from the head of the player to the hit point.
	FVector BeamStart = player->GetActorLocation() + FVector(0, 0, 50); // optional offset
	FVector ToBeam = (TargetPoint - BeamStart);
	FVector BeamDirection = ToBeam.GetSafeNormal();
	float BeamDistance = ToBeam.Size();
	FVector BeamEnd = BeamStart + BeamDirection * 1000.f;

	if(NiagaraSystem)
	{
		if(UNiagaraComponent*NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem,player->GetMesh(),"hand_r",FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::Type::SnapToTarget,true))
		{


			NiagaraComp->SetVariableFloat("User.Velocity", BeamDistance);

			NiagaraComp->SetVariableVec3("User.TargetLocation",BeamDirection);
		}
	}

	//create sphere multi for objects with new beam values.
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(player);

	if (UKismetSystemLibrary::SphereTraceMultiForObjects(
		player->GetWorld(),
		BeamStart,
		BeamEnd,
		25.f,
		ObjectTypes,
		true,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true))
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor()))
			{
				if (AWeapon* Weapon = player->GetAttackingWeapon())
				{
					float Damage = Weapon->CalculateDamage(Enemy);
					Enemy->TakeDamage(Damage, FDamageEvent(), player->GetController(), player);
				}
			}
		}
	}
}
