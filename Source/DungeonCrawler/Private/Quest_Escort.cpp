// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest_Escort.h"

#include "DungeonCrawlerGM.h"
#include "DungeonManager.h"
#include "NPC_Escort.h"
#include "QuestDataAsset_Escort.h"
#include "Kismet/GameplayStatics.h"

void UQuest_Escort::StartQuest_Implementation()
{
	Super::StartQuest_Implementation();

	UWorld* World = GetWorld();

	if(!QuestDataAsset || !OwningNPC || !World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error getting quest data"));
		return;
	}

	if(ADungeonCrawlerGM*GM = Cast<ADungeonCrawlerGM>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnRoundEndDelegate.AddDynamic(this, &UQuest_Escort::Execute);
	}

	if (UQuestDataAsset_Escort* DA_Quest_Escort = Cast<UQuestDataAsset_Escort>(QuestDataAsset)) {
		FVector SpawnLocation = OwningNPC->GetActorLocation();
		FRotator SpawnRotation = OwningNPC->GetActorRotation();
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				if (ANPC_Escort* EscortActor = World->SpawnActor<ANPC_Escort>(DA_Quest_Escort->EscortNPC, SpawnLocation, SpawnRotation,params))
				{
					EscortActor->ParentQuest = this;
					if (ADungeonManager* DungeonManager = Cast<ADungeonManager>(UGameplayStatics::GetActorOfClass(World, ADungeonManager::StaticClass()))) {
						TArray<ADungeonPieceActor*> EligibleLocations;

						for(int i = 1; i < DungeonManager->CurrentDungeonPieces.Num(); ++i)
						{
							if(ADungeonPieceActor*DungeonPiece = DungeonManager->CurrentDungeonPieces[i])
							{
								float Distance = (DungeonPiece->GetActorLocation() - SpawnLocation).Size();
								if(Distance <= 1000.f)
								{
									continue;
								}
								EligibleLocations.Add(DungeonPiece);

							}
						}

						if(!EligibleLocations.IsEmpty())
						{
							int pos = FMath::RandRange(0, EligibleLocations.Num() - 1);
							if(EligibleLocations.IsValidIndex(pos))
							{
								EscortActor->StartEscort(EligibleLocations[pos]->GetActorLocation());
							}
						}

					}
				}
	}
}

void UQuest_Escort::Execute()
{
	Super::Execute();

	EndQuest();
}
