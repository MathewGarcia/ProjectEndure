// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"

#include "DungeonCrawlerGM.h"
#include "GearArmorItem.h"
#include "PlayerCharacter.h"
#include "PlayerSaveGame.h"
#include "Quest.h"
#include "Quest_SaveData.h"
#include "Weapon.h"
#include "WeaponInstance.h"
#include "Kismet/GameplayStatics.h"

void UMainGameInstance::SaveGameData()
{
    UPlayerSaveGame* PlayerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));

    if (!PlayerSaveGame) return;
    bool bSaved = false;
    if(localPlayer)
    {
	    if(APlayerCharacterState*PCS = localPlayer->GetPlayerCharacterState())
	    {
            PlayerSaveGame->playerStats = FGameStats(PCS->playerStats);
            PCS->SaveAbilities(PCS);
            PlayerSaveGame->LearnedAbilities = PCS->LearnedAbilitiesSaveData;
            PlayerSaveGame->Lives = PCS->GetCurrentLives();
            PlayerSaveGame->HealAbility = PCS->HealAbility;

            PlayerSaveGame->CurrentQuestSaveData.Empty();

            for(int i = 0; i < PCS->CurrentQuests.Num(); ++i)
            {
	            if(UQuest*Quest = PCS->CurrentQuests[i])
	            {
                    FQuest_SaveData QuestSaveData = FQuest_SaveData();

                    Quest->SerializeQuest(QuestSaveData);

                    PlayerSaveGame->CurrentQuestSaveData.Add(QuestSaveData);
	            }
            }

            if (ADungeonCrawlerGM* GM = Cast<ADungeonCrawlerGM>(GetWorld()->GetAuthGameMode())) {
                PlayerSaveGame->Round = GM->GetRoundCount();
            }
            for (UItemDataObject* Item : PCS->Inventory)
            {
                if (UWeaponInstance* Weapon = Cast<UWeaponInstance>(Item))
                {
                    //going to need to have separate arrays... 1 for weapons, gear and regular items.
                    FWeaponInstanceSaveData WeaponToSave = FWeaponInstanceSaveData();
                    Weapon->SerializeWeapon(WeaponToSave);
                    PlayerSaveGame->SavedInventoryWeapons.Add(WeaponToSave);

                }
                else if (UGearItemInstance* Gear = Cast<UGearItemInstance>(Item))
                {
                    FGearItemInstanceSaveData GearToSave = FGearItemInstanceSaveData();
                    Gear->SeralizeGearItem(GearToSave);
                    PlayerSaveGame->SavedInventoryGear.Add(GearToSave);
                }
                else
                {
                    FItemInstance Data = FItemInstance();

                    if (Item->ItemData) {
                        Data.ItemDataID = Item->ItemData->ItemName.ToString();
                        Data.Quantity = Item->Quantity;
                        PlayerSaveGame->SavedInventory.Add(Data);
                    }
                }
            }
			if(AWeapon*CurrentWeapon = localPlayer->GetCurrentWeapon())
			{
                if (CurrentWeapon->WeaponInstance) {
                    CurrentWeapon->WeaponInstance->SerializeWeapon(PlayerSaveGame->SavedWeapon);
                }


                if(UGearItemInstance**ChestItem = PCS->PlayerGearInstances.Find(EGearType::Chest))
                {
                    FGearItemInstanceSaveData ChestItemInstanceSaveData;
                    (*ChestItem)->SeralizeGearItem(ChestItemInstanceSaveData);
                    PlayerSaveGame->SavedGear.Add(ChestItemInstanceSaveData);
                }
                if (UGearItemInstance** LegItem = PCS->PlayerGearInstances.Find(EGearType::Legs))
                {
                    FGearItemInstanceSaveData LegItemInstanceSaveData;
                    (*LegItem)->SeralizeGearItem(LegItemInstanceSaveData);
                    PlayerSaveGame->SavedGear.Add(LegItemInstanceSaveData);
                }
                if (UGearItemInstance** HeadItem = PCS->PlayerGearInstances.Find(EGearType::Head))
                {
                    FGearItemInstanceSaveData HeadItemInstanceSaveData;
                    (*HeadItem)->SeralizeGearItem(HeadItemInstanceSaveData);
                    PlayerSaveGame->SavedGear.Add(HeadItemInstanceSaveData);
                }
                if (UGearItemInstance** LHandItem = PCS->PlayerGearInstances.Find(EGearType::LHand))
                {
                    FGearItemInstanceSaveData LHandItemInstanceSaveData;
                    (*LHandItem)->SeralizeGearItem(LHandItemInstanceSaveData);
                    PlayerSaveGame->SavedGear.Add(LHandItemInstanceSaveData);
                }
                
			}
	    }

         bSaved = UGameplayStatics::SaveGameToSlot(PlayerSaveGame, "Test", 0);
    }

	if(bSaved)
	{
        UE_LOG(LogTemp, Error, TEXT("SAVED GAME"));
	}
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FAILED TO SAVE GAME"));

    }

}

void UMainGameInstance::LoadGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("Test"), 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Loading saved game"));
        SavedGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Test", 0));
        UE_LOG(LogTemp, Warning, TEXT("Loaded saved game"));
        bLoadingSucceeded = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No save found, creating default"));
        SavedGame = Cast<UPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveGame::StaticClass()));
        bLoadingSucceeded = false;

    }

}

void UMainGameInstance::DeleteGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("Test"), 0))
    {
        UGameplayStatics::DeleteGameInSlot("Test", 0);
    }
}

void UMainGameInstance::UpdateInDungeon(bool bInDungeon)
{
    UPlayerSaveGame* PlayerSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot("Test", 0));

    if(PlayerSaveGame)
    {
        PlayerSaveGame->bInDungeon = bInDungeon;
        UGameplayStatics::SaveGameToSlot(PlayerSaveGame, "Test", 0);
    }

}

void UMainGameInstance::PlayDodgeSound(AActor* Actor)
{
    if (!Actor) return;

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DodgeSound, Actor->GetActorLocation(), Actor->GetActorRotation(), 1, 1, 0, Attenuation);

}

void UMainGameInstance::Init()
{
    Super::Init();

    if (UWorld* World = GetWorld())
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            localPlayer = Cast<APlayerCharacter>(PlayerController->GetPawn());
        }
    }
}
