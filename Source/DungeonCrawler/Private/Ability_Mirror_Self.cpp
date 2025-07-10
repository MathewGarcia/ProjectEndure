// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Mirror_Self.h"

#include "AIPlayerClones.h"
#include "GearArmorItem.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "WeaponInstance.h"

void UAbility_Mirror_Self::execute_Implementation()
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

bool UAbility_Mirror_Self::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
		}
	}
	return false;
}

void UAbility_Mirror_Self::Logic()
{
	Super::Logic();

	UE_LOG(LogTemp, Warning, TEXT("Playing,%p"), this);

	if (PlayerToSpawn) {
		if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
		{
			if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
			{
				for (int i = 0; i < CloneMax; ++i)
				{
					FActorSpawnParameters params;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					FVector ForwardVector = player->GetActorForwardVector() * FMath::FRandRange(-Offset, Offset);
					FVector RightVector = player->GetActorRightVector() * FMath::FRandRange(-Offset, Offset);

					FVector NewLocation = player->GetActorLocation() + (ForwardVector + RightVector);
					if (APlayerCharacter* ClonedPlayer = GetWorld()->SpawnActor<APlayerCharacter>(PlayerToSpawn, NewLocation, player->GetActorRotation(), params))
					{
						if (AAIPlayerClones* ClonedController = Cast<AAIPlayerClones>(ClonedPlayer->GetController()))
						{
							if (APlayerCharacterState* ClonedPCS = GetWorld()->SpawnActor<APlayerCharacterState>(PCS->GetClass(),params)) {
								ClonedPlayer->SetNewSoftPCS(ClonedPCS);
								ClonedController->SetPlayerState(ClonedPCS);
								ClonePlayerStats(ClonedPlayer, ClonedPCS, PCS);
								PlayerClones.Add(ClonedPlayer);
								AIPlayerClonesControllers.Add(ClonedController);
								PlayerCharacterStatesClones.Add(ClonedPCS);

									ClonedController->Possess(ClonedPlayer);
									ClonedController->Index = i;
							}

								player->OnAbilityUsed.AddUFunction(ClonedController, "UsePlayerAbility");
							
						}


					}
				}
				TWeakObjectPtr<APlayerCharacterState>SafePlayerState = PCS;
				TWeakObjectPtr<UAbility_Mirror_Self>SafeAbility = this;
				GetWorld()->GetTimerManager().SetTimer(FPlayerLiveCheck, [SafePlayerState, SafeAbility]
					{
						if (!SafePlayerState.IsValid() || !SafeAbility.IsValid()) return;

					if(APlayerCharacterState*LocalPCS= SafePlayerState.Get())
					{
						if(LocalPCS->playerStats.currentHealth <= 0)
						{
							if (UAbility_Mirror_Self* LocalAbility = SafeAbility.Get()) {
								for (AAIPlayerClones* Controller : LocalAbility->AIPlayerClonesControllers)
								{
									Controller->Destroy();
								}
								for (APlayerCharacterState* ClonedState : LocalAbility->PlayerCharacterStatesClones)
								{
									ClonedState->Destroy();
								}
								for (APlayerCharacter* Clone : LocalAbility->PlayerClones)
								{
									if (Clone->GetCurrentWeapon())
									{
										Clone->GetCurrentWeapon()->Destroy();
									}
									Clone->Destroy();
								}
								LocalAbility->GetWorld()->GetTimerManager().ClearTimer(LocalAbility->FPlayerLiveCheck);
								LocalAbility->GetWorld()->GetTimerManager().ClearTimer(LocalAbility->FDuration);

							}
						}
					}

				}, 0.1, true);
				GetWorld()->GetTimerManager().SetTimer(FDuration, [SafeAbility]
				{
						if (!SafeAbility.IsValid()) return;

						if (UAbility_Mirror_Self* LocalAbility = SafeAbility.Get()) {
							for (AAIPlayerClones* Controller : LocalAbility->AIPlayerClonesControllers)
							{
								Controller->Destroy();
							}
							for(APlayerCharacterState*ClonedState : LocalAbility->PlayerCharacterStatesClones)
							{
								ClonedState->Destroy();
							}
							for (APlayerCharacter* Clone : LocalAbility->PlayerClones)
							{
								if(Clone->GetCurrentWeapon())
								{
									Clone->GetCurrentWeapon()->Destroy();
								}
								Clone->Destroy();
							}

							if (LocalAbility->GetWorld()->GetTimerManager().IsTimerActive(LocalAbility->FPlayerLiveCheck)) {
								LocalAbility->GetWorld()->GetTimerManager().ClearTimer(LocalAbility->FPlayerLiveCheck);
							}


						}
					
				}, Duration, false);

				GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}
		}
	}
}

void UAbility_Mirror_Self::ClonePlayerStats(APlayerCharacter* ClonedPlayer, APlayerCharacterState* ClonedPlayerState,
	APlayerCharacterState* OGPlayerState)
{


	if (UGearItemInstance** ChestItemInstance = OGPlayerState->PlayerGearInstances.Find(EGearType::Chest); ChestItemInstance && IsValid(*ChestItemInstance)) {
		if (UGearItemInstance* NewGearItemInstance = DuplicateObject(*ChestItemInstance, ClonedPlayerState)) {
			if (AGearArmorItem* ChestArmorItem = NewGearItemInstance->ItemData->ItemToSpawn->GetDefaultObject<AGearArmorItem>())
			{
				ClonedPlayer->tryEquipGear(ChestArmorItem, EGearType::Chest, NewGearItemInstance);
			}
		}
	}
	if (UGearItemInstance** LegItemInstance = OGPlayerState->PlayerGearInstances.Find(EGearType::Legs); LegItemInstance && IsValid(*LegItemInstance)) {
		if (UGearItemInstance* NewGearItemInstance = DuplicateObject(*LegItemInstance, ClonedPlayerState)) {
			if (AGearArmorItem* LegArmorItem = NewGearItemInstance->ItemData->ItemToSpawn->GetDefaultObject<AGearArmorItem>())
			{
				ClonedPlayer->tryEquipGear(LegArmorItem, EGearType::Legs, NewGearItemInstance);
			}
		}
	}

	if (UGearItemInstance** HeadItemInstance = OGPlayerState->PlayerGearInstances.Find(EGearType::Head); HeadItemInstance && IsValid(*HeadItemInstance)) {
		if (UGearItemInstance* NewGearItemInstance = DuplicateObject(*HeadItemInstance, ClonedPlayerState)) {
			if (AGearArmorItem* HeadArmorItem = NewGearItemInstance->ItemData->ItemToSpawn->GetDefaultObject<AGearArmorItem>())
			{
				ClonedPlayer->tryEquipGear(HeadArmorItem, EGearType::Head, NewGearItemInstance);
			}
		}
	}

	if (AItem** RHandItem = OGPlayerState->PlayerGear.Find(EGearType::RHand); RHandItem && IsValid(*RHandItem))
	{
		if(AWeapon*PlayerWeapon = Cast<AWeapon>(*RHandItem))
		{
			FActorSpawnParameters Params;
			Params.Owner = ClonedPlayer;
			if(AWeapon*ClonedWeapon = GetWorld()->SpawnActor<AWeapon>(PlayerWeapon->GetClass(),Params))
			{
				UWeaponInstance* NewWeaponInstance = DuplicateObject(ClonedWeapon->WeaponInstance, ClonedPlayerState);
				ClonedWeapon->WeaponInstance = NewWeaponInstance;
				ClonedPlayer->tryEquipGear(ClonedWeapon, EGearType::RHand,NewWeaponInstance);
			}
		}
	}

	ClonedPlayerState->playerStats = FGameStats(OGPlayerState->playerStats);

	ClonedPlayerState->EquippedAbilities.SetNum(OGPlayerState->EquippedAbilities.Num());
	for(int i = 0; i < OGPlayerState->EquippedAbilities.Num(); ++i)
	{
		if (UAbility* Ability = OGPlayerState->EquippedAbilities[i]) {
			if (UAbility* NewAbility = DuplicateObject(Ability,ClonedPlayerState)) {
				ClonedPlayerState->EquippedAbilities[i] = NewAbility;
			}
		}
	}
}

