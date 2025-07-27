// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacterState.h"

#include "Ability.h"
#include "EmptyAbility.h"
#include "GearItemData.h"
#include "ItemData.h"
#include "Item.h"
#include "ItemDataObject.h"
#include "MainGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

static bool AreWeaponsStackable(
    const UWeaponInstance* A,
    const UWeaponInstance* B)
{
    if (!A || !B) {
        UE_LOG(LogTemp, Warning, TEXT("AreWeaponsStackable: One weapon is null"));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("=== STACKABLE CHECK ==="));
    UE_LOG(LogTemp, Warning, TEXT("Weapon A: Element=%s, Level=%d, Damage=%d"),
        *UEnum::GetValueAsString(A->WeaponElementType), A->CurrentWeaponLevel, A->BaseDamage);
    UE_LOG(LogTemp, Warning, TEXT("Weapon B: Element=%s, Level=%d, Damage=%d"),
        *UEnum::GetValueAsString(B->WeaponElementType), B->CurrentWeaponLevel, B->BaseDamage);

    // Same asset?
    if (A->ItemData != B->ItemData) {
        UE_LOG(LogTemp, Warning, TEXT("Different ItemData - NOT STACKABLE"));
        return false;
    }

    bool bSameElement = A->WeaponElementType == B->WeaponElementType;
    bool bSameDamage = A->BaseDamage == B->BaseDamage;

    UE_LOG(LogTemp, Warning, TEXT("Same Element: %s, Same Damage: %s"),
        bSameElement ? TEXT("YES") : TEXT("NO"),
        bSameDamage ? TEXT("YES") : TEXT("NO"));

    bool bStackable = bSameElement && bSameDamage;
    UE_LOG(LogTemp, Warning, TEXT("RESULT: %s"), bStackable ? TEXT("STACKABLE") : TEXT("NOT STACKABLE"));

    return bStackable;
}

static bool AreGearsStackable(
    const UGearItemInstance* A,
    const UGearItemInstance* B)
{
    if (!A || !B) return false;

    // Same asset?
    if (A->ItemData != B->ItemData) return false;

    // Same upgrade tier?
    return  A->GearLevel == B->GearLevel;
 
}


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


void APlayerCharacterState::AddItemToInventory(UItemDataObject* Item)
{
    if (!Item || !Item->ItemData) return;

    UItemDataObject* ClonedItem = nullptr;

    if (UWeaponInstance* WeaponInstanceItem = Cast<UWeaponInstance>(Item)) {
        UWeaponInstance* ClonedWeapon = DuplicateObject<UWeaponInstance>(WeaponInstanceItem, this);
        ClonedItem = ClonedWeapon;

        for (UItemDataObject* InvItem : Inventory)
        {
            if (UWeaponInstance* ExistingWeapon = Cast<UWeaponInstance>(InvItem))
            {
                if (AreWeaponsStackable(ExistingWeapon, ClonedWeapon))
                {
                    // Found a stackable clone → just bump its count
                    ExistingWeapon->SetQuantity(ExistingWeapon->Quantity + ClonedWeapon->ItemData->Quantity);
                    FInventoryUpdated.Broadcast(Inventory);
                    return;
                }
            }
        }
    }
    else if (UGearItemInstance* GearItemInstance = Cast<UGearItemInstance>(Item))
    {
        UGearItemInstance* ClonedGear = DuplicateObject<UGearItemInstance>(GearItemInstance, this);
        ClonedItem = ClonedGear;

        for (UItemDataObject* InvItem : Inventory)
        {
            if (UGearItemInstance* ExistingGear = Cast<UGearItemInstance>(InvItem))
            {
                if (AreGearsStackable(ExistingGear, ClonedGear))
                {
                    // Found a stackable clone → just bump its count
                    ExistingGear->SetQuantity(ExistingGear->Quantity + ClonedGear->ItemData->Quantity);
                    FInventoryUpdated.Broadcast(Inventory);
                    return;
                }
            }
        }
    }
    else
    {
        ClonedItem = DuplicateObject<UItemDataObject>(Item, this);

        // Try to stack onto an existing clone
        for (UItemDataObject* Existing : Inventory)
        {
            if (!Existing) continue;
            // Match on same asset + any deep checks you need:
            if (Existing->ItemData == ClonedItem->ItemData)
            {
                // Found a stackable clone → just bump its count
                Existing->SetQuantity(Existing->Quantity + ClonedItem->ItemData->Quantity);
                FInventoryUpdated.Broadcast(Inventory);
                return;
            }
        }
    }

    // Add the cloned item, not the original
    if (ClonedItem)
    {
        Inventory.Add(ClonedItem);
        FInventoryUpdated.Broadcast(Inventory);
    }
}





void APlayerCharacterState::RemoveItemFromInventory(UItemDataObject* Item)
{
    if (!Item) return;

    int32 Index = Inventory.IndexOfByKey(Item);
    if (Index == INDEX_NONE) return;           // nothing to remove

    UItemDataObject* Entry = Inventory[Index];
    if (Entry->Quantity > 1)
    {
        Entry->SetQuantity(Entry->Quantity - 1);
    }
    else
    {
        Inventory.RemoveAt(Index);
    }
    FInventoryUpdated.Broadcast(Inventory);
}

void APlayerCharacterState::RemoveItemFromInventory(UItemDataObject* Item,int Amount)
{
    if (!Item) return;

    int32 Index = Inventory.IndexOfByKey(Item);
    if (Index == INDEX_NONE) return;           // nothing to remove

    UItemDataObject* Entry = Inventory[Index];
    if (Entry->Quantity > 1)
    {
        Entry->SetQuantity(Entry->Quantity - Amount);
    }
    else
    {
        Inventory.RemoveAt(Index);
    }
    FInventoryUpdated.Broadcast(Inventory);
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
    if (!NewPCS) return;
    NewPCS->LearnedAbilitiesSaveData.Empty();
    for (UAbility* Ability : LearnedAbilities)
    {
        if (Ability) {
            SerializeObject(Ability,NewPCS);
        }
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
                    if(EquippedAbilities[i] && EquippedAbilities[i]->GetClass() == UEmptyAbility::StaticClass())
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
            if (GainLifeNiagaraSystem && player->GetMesh()) {
                UNiagaraFunctionLibrary::SpawnSystemAttached(GainLifeNiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);
            }
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
        if (LevelUpNiagaraSystem && player->GetMesh()) {
            UNiagaraFunctionLibrary::SpawnSystemAttached(LevelUpNiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);
        }
        UWorld* World = GetWorld();
        if(World)
        {
            if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
            {
                if (MGI->LevelUpSound) {
                    UGameplayStatics::PlaySound2D(World, MGI->LevelUpSound);
                }
            }
        }
    }
}

void APlayerCharacterState::ReBindDelegates()
{
    playerStats.OnPlayerLevelChanged.Clear();
    playerStats.OnPlayerLevelChanged.AddDynamic(this, &APlayerCharacterState::PlayerLevelUp);
}

