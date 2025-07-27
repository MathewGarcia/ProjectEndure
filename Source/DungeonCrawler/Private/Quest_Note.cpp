// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest_Note.h"

#include "DungeonCrawlerGM.h"
#include "DungeonManager.h"
#include "MainGameInstance.h"
#include "NPC.h"
#include "NPC_Letter_Reciever.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Kismet/GameplayStatics.h"

void UQuest_Note::Execute()
{
	Super::Execute();

	float Roll = FMath::FRandRange(0, 1.f);

	UE_LOG(LogTemp, Warning, TEXT("Roll: %f"), Roll);
	if(Roll >= RollNeeded)
	{
		if(ActorToSpawn)
		{
			if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter())) {
				if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
					UWorld* World = GetWorld();
					if (!World) return;
					if (AActor* FoundActor = UGameplayStatics::GetActorOfClass(World, ADungeonManager::StaticClass()))
					{
						if (ADungeonManager* FoundDungeonManager = Cast<ADungeonManager>(FoundActor))
						{
							TArray<ADungeonPieceActor*> EligiblePieces;
							for (ADungeonPieceActor* DungeonPiece : FoundDungeonManager->CurrentDungeonPieces)
							{
								if (!DungeonPiece) continue;
								float Distance = (player->GetActorLocation() - DungeonPiece->GetActorLocation()).Size();
								if (Distance <= 1000.f && !DungeonPiece->SpawnedActor)
								{
									continue;
								}

								EligiblePieces.Add(DungeonPiece);

							}
							if (EligiblePieces.Num() > 0) {
								int pos = FMath::RandRange(0, EligiblePieces.Num() - 1);
								if(EligiblePieces.IsValidIndex(pos))
								{
									FVector Location = EligiblePieces[pos]->GetActorLocation() + (FVector::UpVector * EligiblePieces[pos]->TileHeight)*EligiblePieces[pos]->floors;
									FActorSpawnParameters params;
									params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
									if(ANPC_Letter_Reciever*Letter_Reciever = World->SpawnActor<ANPC_Letter_Reciever>(ActorToSpawn, Location, FRotator::ZeroRotator, params))
									{
										Letter_Reciever->NoteQuest = this;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void UQuest_Note::StartQuest_Implementation()
{
	Super::StartQuest_Implementation();

	UWorld* World = GetWorld();
	if (!World) return;
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
	{
		MGI->OnDungeonGenerationComplete.AddDynamic(this, &UQuest_Note::Execute);
	}
}
