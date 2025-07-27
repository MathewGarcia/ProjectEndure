// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Iron_Skin.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Iron_Skin::execute_Implementation()
{
	Super::execute_Implementation();
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
        {
            if (MontageToPlay)
                player->PlayAnimMontage(MontageToPlay, 1.f);
        }
    }
	
}

bool UAbility_Iron_Skin::bShouldExecute_Implementation()
{
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
            UWorld* World = PCS->GetWorld();
            return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown)&& player->CanPlayerDoAction(EResourceTypes::Stamina,staminaCost);
        }
    }
    return false;
}

void UAbility_Iron_Skin::Logic()
{
	Super::Logic();
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        UWorld* World = PCS->GetWorld();
        if (!World) return;
        if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
            TWeakObjectPtr<APlayerCharacter>SafePlayer = player;
            if (UMeshComponent* Mesh = player->GetMesh()) {
                if (IronSkinMaterial && Mesh->GetMaterial(0)) {
                    if (UMaterial* OriginalMaterial = Cast<UMaterial>(Mesh->GetMaterial(0))) {
                        TWeakObjectPtr<UMaterial> OriginalMaterialSafe = OriginalMaterial;
                        TWeakObjectPtr<UMeshComponent> SafeMesh = Mesh;
                        Mesh->SetMaterial(0, IronSkinMaterial);
                        //increase physical resistance by 20%
                        float& ResistanceRef = PCS->playerStats.Resistances[EElementTypes::Physical];
                        float Original = ResistanceRef;
                        const float min = 0.2;
                        const float PercentBonus = ResistanceRef * 0.2;
                        float Bonus = FMath::Max(PercentBonus, min);
                        ResistanceRef += Bonus;
                        FTimerHandle FDuration;
                        if (World->GetTimerManager().IsTimerActive(FDuration)) {
                            World->GetTimerManager().ClearTimer(FDuration);
                        }
                        World->GetTimerManager().SetTimer(FDuration, [this, Original, PCS, OriginalMaterialSafe, SafeMesh]()
                            {
                                if (!OriginalMaterialSafe.IsValid() || !SafeMesh.IsValid()) return;
                                PCS->playerStats.Resistances[EElementTypes::Physical] = Original;
                                if (UMeshComponent* LocalMesh = SafeMesh.Get())
                                {
                                    if (UMaterial* LocalMaterial = OriginalMaterialSafe.Get()) {
                                        LocalMesh->SetMaterial(0, LocalMaterial);
                                    }
                                }
                            }, Duration, false);
                    }
                }
            }
        }
        if (World->GetTimerManager().IsTimerActive(FCooldown)) {
            World->GetTimerManager().ClearTimer(FCooldown);
        }
        World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
    }
}
