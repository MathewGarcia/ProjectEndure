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
            if (MontageToPlay)
                player->PlayAnimMontage(MontageToPlay, player->MontageSpeed);
        }
    }
}

bool UAbility_Elemental_Beam::bShouldExecute_Implementation()
{
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
            UWorld* World = GetWorld();
            return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
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
            UWorld* World = player->GetWorld();
            if (!World) return;
            TWeakObjectPtr<APlayerCharacter> safePlayer = player;
            ChannelBeam(player);
            player->bUseControllerRotationYaw = true;
            if (World->GetTimerManager().IsTimerActive(FCustomTick)) {
                World->GetTimerManager().ClearTimer(FCustomTick);
            }
            if (World->GetTimerManager().IsTimerActive(FDuration)) {
                World->GetTimerManager().ClearTimer(FDuration);
            }
            if (World->GetTimerManager().IsTimerActive(FCooldown)) {
                World->GetTimerManager().ClearTimer(FCooldown);
            }
            World->GetTimerManager().SetTimer(FCustomTick,[this,safePlayer,World]
            {
                if (!safePlayer.IsValid()) return;
                if (APlayerCharacter* localPlayer = safePlayer.Get()) {
                    ChannelBeam(localPlayer);
                }
            },0.2, true);
            World->GetTimerManager().SetTimer(FDuration, [this,safePlayer,World]()
            {
                if (!safePlayer.IsValid()) return;
                if(World->GetTimerManager().IsTimerActive(FCustomTick))
                {
                    World->GetTimerManager().ClearTimer(FCustomTick);
                    World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
                    if(APlayerCharacter*localPlayer = safePlayer.Get())
                    {
                        if (MontageToPlay)
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
	if (!player || !player->GetWorld()) return;
    UWorld* World = player->GetWorld();
    int32 ViewportSizeX = 0, ViewportSizeY = 0;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
        PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
    } else {
        return;
    }
    // Center of the screen
    float ScreenCenterX = ViewportSizeX * 0.5f;
    float ScreenCenterY = ViewportSizeY * 0.5f;
    FVector WorldOrigin;
    FVector WorldDirection;
    if (!UGameplayStatics::GetPlayerController(World, 0)->DeprojectScreenPositionToWorld(
        ScreenCenterX,
        ScreenCenterY,
        WorldOrigin,
        WorldDirection
    )) {
        return;
    }
    // Line trace from the center of the screen to get an impact point.
    FVector TraceEnd = WorldOrigin + WorldDirection * 5000.f;
    FHitResult CameraHit;
    FVector TargetPoint = TraceEnd;
    UKismetSystemLibrary::DrawDebugLine(World, WorldOrigin,TraceEnd,FLinearColor::Red,123);
    if (World->LineTraceSingleByChannel(CameraHit, WorldOrigin, TraceEnd, ECC_Pawn))
    {
        TargetPoint = CameraHit.ImpactPoint;
    }
    //Trace from the head of the player to the hit point.
    FVector BeamStart = player->GetActorLocation() + FVector(0, 0, 50); // optional offset
    FVector ToBeam = (TargetPoint - BeamStart);
    FVector BeamDirection = ToBeam.GetSafeNormal();
    float BeamDistance = ToBeam.Size();
    FVector BeamEnd = BeamStart + BeamDirection * 1000.f;
    if(NiagaraSystem && player->GetMesh())
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
        World,
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
