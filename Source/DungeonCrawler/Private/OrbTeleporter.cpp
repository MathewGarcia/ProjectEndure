// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbTeleporter.h"

#include "DungeonCrawlerGM.h"
#include "DungeonManager.h"
#include "Enemy.h"
#include "GearItemInstanceSaveData.h"
#include "LevelMusicActor.h"
#include "PlayerCharacter.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "Quest.h"
#include "Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

AOrbTeleporter::AOrbTeleporter()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	if(SkeletalMesh)
	{
		SkeletalMesh->SetVisibility(false);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

TSoftObjectPtr<UWorld> AOrbTeleporter::SelectReward()
{
	float TotalWeight = 0.f;
	for(const auto& pair : RoomMap)
	{
		TotalWeight += pair.Value;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	
	for (const auto& pair : RoomMap)
	{
		Roll -= pair.Value;
		if(Roll <= 0.f)
		{
			UE_LOG(LogTemp, Error, TEXT("Returning key!"));
			return pair.Key;
		}
	}

	return nullptr;
}

void AOrbTeleporter::LevelLoaded()
{
	if (!LoadedLevel) return;


	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		//access the player
		if (APlayerCharacter* player = MGI->localPlayer)
		{
			if(UMainPlayerWidget*MPW = player->GetMainPlayerWidget().Get())
			{
				MPW->SetLoadVisibility(ESlateVisibility::Visible);
			}
		}
	}

	if(ULevel* SubLevel = LoadedLevel->GetLoadedLevel())
	{
		for(AActor*Actor : SubLevel->Actors)
		{
			if (AOrbTeleporter* FoundTeleporter = Cast<AOrbTeleporter>(Actor))
				{
					ChildTeleporter = FoundTeleporter;
					FoundTeleporter->ChildTeleporter = this;
					FoundTeleporter->TeleportType = EOrbTeleportType::Exit;
					FoundTeleporter->LoadedLevel = LoadedLevel;

					if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
					{
						//access the player
						if (APlayerCharacter* player = MGI->localPlayer)
						{

							FTimerHandle FDelay;
							GetWorld()->GetTimerManager().SetTimer(FDelay, [player,FoundTeleporter]
							{
									player->SetActorLocation(FoundTeleporter->GetActorLocation());
									player->SpawnLocation = FoundTeleporter->GetActorLocation();

							}, 0.5, false);
						}
					}
					//check our teleporter type
					if (TeleportType == EOrbTeleportType::Boss)
					{
						FoundTeleporter->BoxComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
					}
					if (ADungeonManager* DungeonManager = Cast<ADungeonManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonManager::StaticClass())))
					{
						DungeonManager->SetResetTeleporter(FoundTeleporter);
					}
					break;
				}
		}
	}


	HandleTeleport();


}

void AOrbTeleporter::Interact_Implementation()
{

	if(TeleportType == EOrbTeleportType::Reward)
	{
		if (!LoadedLevel) {
			if (TSoftObjectPtr<UWorld> ReturnWorld = SelectReward(); !ReturnWorld.IsNull())
			{
				FVector SpawnLocation = FVector(0.f, 0.f, -5000);
				bool bOutSuccess = false;
				LoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), ReturnWorld.GetLongPackageName(), SpawnLocation, FRotator::ZeroRotator, bOutSuccess);
				LoadedLevel->OnLevelLoaded.AddDynamic(this, &AOrbTeleporter::LevelLoaded);
				if (!bOutSuccess || !LoadedLevel)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load Teleporter Level"));
					return;
				}
			}
		}
		else
		{
			if (!LoadedLevel->IsLevelVisible()) {
				LoadedLevel->SetShouldBeVisible(true);
				HandleTeleport();
			}
		}
	}
	else if(TeleportType == EOrbTeleportType::Exit)
	{
		if (LoadedLevel)
		{
			LoadedLevel->SetShouldBeVisible(false);
		}
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			MGI->SaveGameData();
		}
			HandleTeleport();

		
	}
	else if(TeleportType == EOrbTeleportType::Boss)
	{

		if(ALevelMusicActor*MusicActor = Cast<ALevelMusicActor>(UGameplayStatics::GetActorOfClass(GetWorld(),ALevelMusicActor::StaticClass())))
		{
			if (MusicActor->MusicManagerComponent->IsPlaying()) {
				MusicActor->MusicManagerComponent->Stop();
			}
		}
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			MGI->SaveGameData();
		}
			if (LoadedLevel) {
				//LoadedLevel->SetShouldBeVisible(true);
				HandleTeleport();
			}
		else if (TSoftObjectPtr<UWorld> ReturnWorld = SelectReward(); !ReturnWorld.IsNull())
		{
			FVector SpawnLocation = FVector(0.f, 0.f, -5000);
			bool bOutSuccess = false;
			LoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), ReturnWorld.GetLongPackageName(), SpawnLocation, FRotator::ZeroRotator, bOutSuccess);
			LoadedLevel->OnLevelLoaded.AddDynamic(this, &AOrbTeleporter::LevelLoaded);
			if (!bOutSuccess || !LoadedLevel)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to load Teleporter Level"));
			}

		}

	}
	else if (TeleportType == EOrbTeleportType::OneWay)
	{
		TSoftObjectPtr<UWorld> SelectedWorld = SelectReward();

		if(!SelectedWorld.IsNull())
		{
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
			{
				MGI->UpdateInDungeon(true);
				MGI->SaveGameData();
			}
			QuickSaveData();
			const FString LevelName = SelectedWorld.GetLongPackageName();
			UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));

		}
	}
	else if(TeleportType == EOrbTeleportType::Exfil)
	{

		float PossibleReturnHub = FMath::FRandRange(0, 1.f);

		if (PossibleReturnHub >= 0.25f)
		{
			//before we change worlds we want to save the data the current player has.

			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
			{
				MGI->UpdateInDungeon(false);
			}
			QuickSaveData();
			UGameplayStatics::OpenLevel(GetWorld(), "Hub",true);
		}
		else
		{
			if (ADungeonManager* DungeonManager = Cast<ADungeonManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonManager::StaticClass())))
			{
				DungeonManager->ResetDungeon();
			}
		}
	}
}

void AOrbTeleporter::HandleTeleport()
{

	if(ADungeonCrawlerGM*GM = GetWorld()->GetAuthGameMode<ADungeonCrawlerGM>())
	{
		GM->bIsLoading = true;
		GM->Loading();
		GM->InitializeTimer();
	}
	if (ADungeonManager* DungeonManager = Cast<ADungeonManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonManager::StaticClass())))
	{
	
		if (TeleportType == EOrbTeleportType::Boss)
		{
				DungeonManager->CleanWorld();
			
		}
		else if (TeleportType == EOrbTeleportType::Exit)
		{
			DungeonManager->ResetDungeon();
		}
	}

	if(UMainGameInstance* MainGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (MainGI->localPlayer)
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [MainGI]()
			{
				if (ADungeonCrawlerGM* GM = MainGI->GetWorld()->GetAuthGameMode<ADungeonCrawlerGM>())
				{
					GM->bIsLoading = false;
					GM->Loading();
				}
			}, 0.5f, false);
		}
	}
	
}

void AOrbTeleporter::QuickSaveData()
{
	//access the game instance
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		//access the player
		if (APlayerCharacter* player = MGI->localPlayer)
		{
			//access the player state
			if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
			{
				MGI->LoadType = ELoadType::GameInstance;
				//save the player stats
				MGI->GIPlayerStats = PCS->playerStats;
				//save the learned abilites
				PCS->SaveAbilities(PCS);
				MGI->GILearnedAbilities = PCS->LearnedAbilitiesSaveData;

				MGI->GICurrentQuestSaveData.Empty();
				MGI->GIHealAbility = PCS->HealAbility;

				for (int i = 0; i < PCS->CurrentQuests.Num(); ++i)
				{
					if (UQuest* Quest = PCS->CurrentQuests[i])
					{
						FQuest_SaveData QuestSaveData = FQuest_SaveData();

						Quest->SerializeQuest(QuestSaveData);

						MGI->GICurrentQuestSaveData.Add(QuestSaveData);
					}
				}


				if (ADungeonCrawlerGM* GM = Cast<ADungeonCrawlerGM>(GetWorld()->GetAuthGameMode())) {
					MGI->GIRound = GM->GetRoundCount();
				}

				for(UItemDataObject*Item : PCS->Inventory)
				{
					if(UWeaponInstance*Weapon = Cast<UWeaponInstance>(Item))
					{
						//going to need to have separate arrays... 1 for weapons, gear and regular items.
						FWeaponInstanceSaveData WeaponToSave = FWeaponInstanceSaveData();
						Weapon->SerializeWeapon(WeaponToSave);
						MGI->GIInventoryWeapons.Add(WeaponToSave);

					}
					else if(UGearItemInstance*Gear = Cast<UGearItemInstance>(Item))
					{
						FGearItemInstanceSaveData GearToSave = FGearItemInstanceSaveData();
						Gear->SeralizeGearItem(GearToSave);
						MGI->GIInventoryGear.Add(GearToSave);
					}
					else
					{
						FItemInstance Data = FItemInstance();

						if (Item->ItemData) {
							Data.ItemDataID = Item->ItemData->ItemName.ToString();
							Data.Quantity = Item->Quantity;
							MGI->GIInventory.Add(Data);
						}
					}
				}
				//save the weapon
				player->GetCurrentWeapon()->WeaponInstance->SerializeWeapon(MGI->GICurrentWeapon);

				MGI->GICurrentLives = PCS->GetCurrentLives();

				//loop through each gear item and save it.
				for (const auto& pair : PCS->PlayerGearInstances)
				{
					if (UGearItemInstance* GearItemInstance = pair.Value)
					{
						FGearItemInstanceSaveData GearItemInstanceSaveData = FGearItemInstanceSaveData();

						GearItemInstance->SeralizeGearItem(GearItemInstanceSaveData);

						MGI->GICurrentGear.Add(GearItemInstanceSaveData);
					}
				}
			}
		}
	}
}

