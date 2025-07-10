// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterState.h"

#include "Ability.h"
#include "Ability_Intelligence_Penalty.h"
#include "EmptyAbility.h"
#include "ItemData.h"
#include "Item.h"
#include "ItemDataObject.h"
#include "MainGameInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "VariableVariant.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacterState::APlayerCharacterState()
{
	PlayerGear.Add(EGearType::Head, nullptr);
	PlayerGear.Add(EGearType::LHand, nullptr);
	PlayerGear.Add(EGearType::RHand, nullptr);
	PlayerGear.Add(EGearType::Chest, nullptr);
	PlayerGear.Add(EGearType::Legs, nullptr);
}

void APlayerCharacterState::BeginPlay()
{
	Super::BeginPlay();

	playerStats.CalculateAllStats();

	playerStats.currentHealth = playerStats.totalHealth;
	playerStats.currentPoise = playerStats.totalPoise;
	playerStats.currentStamina = playerStats.totalStamina;
	playerStats.currentMana = playerStats.totalMana;
	currentLives = MaxLives;

	playerStats.InitResistances();
	playerStats.initElementProgressionMap();
	playerStats.OnPlayerLevelChanged.AddDynamic(this, &APlayerCharacterState::PlayerLevelUp);

	DeserializeAbilities();
}


void APlayerCharacterState::updateStrength(float strengthAmt)
{
	playerStats.totalStrength += strengthAmt;
}

void APlayerCharacterState::updateStamina(float staminaAmt)
{
	playerStats.totalStamina += staminaAmt;
}

void APlayerCharacterState::updateHealth(float healthAmt)
{
	playerStats.totalHealth += healthAmt;
	UE_LOG(LogTemp, Warning, TEXT("New HP %f"), playerStats.totalHealth);
}

void APlayerCharacterState::updateIntellect(float intellectAmt)
{
	playerStats.totalIntellect += intellectAmt;
}

void APlayerCharacterState::updateMana(float manaAmt)
{
	playerStats.totalMana += manaAmt;
}

void APlayerCharacterState::AddItemToInventory(UItemDataObject* item)
{
	for(UItemDataObject* Item : Inventory)
	{
		if (!Item) break;
		if (!Item->ItemData) break;
		if (!item || !item->ItemData) break;

		if(Item->ItemData->ItemName.EqualTo(item->ItemData->ItemName) && Item->Quantity > 0)
		{
			Item->SetQuantity(Item->Quantity + item->Quantity);
			return;
		}
	}

	Inventory.Add(item);
	FInventoryUpdated.Broadcast(Inventory);
}

void APlayerCharacterState::RemoveItemFromInventory(UItemDataObject* itemToRemove)
{

	for(int i = 0; i < Inventory.Num(); ++i)
	{
		if(Inventory[i] && Inventory[i]->ItemData && Inventory[i]->ItemData->ItemName.EqualTo(itemToRemove->ItemData->ItemName))
		{
			if(Inventory[i]->Quantity > 1)
			{
				itemToRemove->SetQuantity(Inventory[i]->Quantity - 1);
				return;
			}
			Inventory.RemoveAt(i);
			FInventoryUpdated.Broadcast(Inventory);

		}
	}


}

void APlayerCharacterState::RemoveItemFromInventory(UItemDataObject* itemToRemove, int Amount)
{
	if (!itemToRemove) return;

	for (int i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i] && Inventory[i]->ItemData && Inventory[i]->ItemData->ItemName.EqualTo(itemToRemove->ItemData->ItemName))
		{
			if (Inventory[i]->Quantity > 1)
			{
				itemToRemove->SetQuantity(Inventory[i]->Quantity - Amount);
				return;
			}
			Inventory.RemoveAt(i);
			FInventoryUpdated.Broadcast(Inventory);

		}
	}
}

void APlayerCharacterState::SerializeObject(UObject* Object, APlayerCharacterState*NewPCS)
{
	if (!NewPCS) return;

	if(UAbility*Ability = Cast<UAbility>(Object))
	{
		FAbilitySaveData AbilitySaveData = FAbilitySaveData();
		Ability->SerializeObject(AbilitySaveData);
		NewPCS->LearnedAbilitiesSaveData.Add(AbilitySaveData);
	}

}

void APlayerCharacterState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);


	if(APlayerCharacterState* NewPCS = Cast<APlayerCharacterState>(PlayerState))
	{
		NewPCS->LearnedAbilitiesSaveData.Reset();

		SaveAbilities(NewPCS);

	}


}

void APlayerCharacterState::SaveAbilities(APlayerCharacterState* NewPCS)
{
	NewPCS->LearnedAbilitiesSaveData.Empty();
	for (UAbility* Ability : LearnedAbilities)
	{
		SerializeObject(Ability,NewPCS);
	}
}

void APlayerCharacterState::DeserializeAbilities()
{
	for (const FAbilitySaveData& AbilitySaveData : LearnedAbilitiesSaveData)
	{
		if(UAbility*Ability = NewObject<UAbility>(this,AbilitySaveData.AbilityClass))
		{
			Ability->DeserializeObject(AbilitySaveData);
			LearnedAbilities.Add(Ability);
			if(Ability->AbilityActivationType == EAbilityActivationType::Activate)
			{
				for(int i = 0; i < EquippedAbilities.Num(); ++i)
				{
					if(EquippedAbilities[i]->GetClass() == UEmptyAbility::StaticClass())
					{
						EquippedAbilities[i] = Ability;
						break;
					}
				}
			}
			OnAbilitiesUpdated.Broadcast();
		}
	}
}

void APlayerCharacterState::UpdateLives(int amount)
{
	if (amount > 0)
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(GetPawn()))
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(GainLifeNiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);
		}
	}
	currentLives += amount;
	currentLives = FMath::Clamp(currentLives, 0, MaxLives);
}

void APlayerCharacterState::SetCurrentLives(int amount)
{
	currentLives = amount;
}

void APlayerCharacterState::PlayerLevelUp(const int& level)
{
	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetPawn()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(LevelUpNiagraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);

		if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), MGI->LevelUpSound);
		}
	}
}

void APlayerCharacterState::ReBindDelegates()
{
	playerStats.OnPlayerLevelChanged.Clear();
	playerStats.OnPlayerLevelChanged.AddDynamic(this, &APlayerCharacterState::PlayerLevelUp);
}

