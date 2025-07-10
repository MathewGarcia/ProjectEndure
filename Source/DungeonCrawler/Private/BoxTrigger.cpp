// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxTrigger.h"

#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "AIEnemy.h"
#include "Boss_Wave.h"
#include "InGamePlayerHUD.h"
#include "LevelMusicActor.h"
#include "MainPlayerWidget.h"
#include "Components/AudioComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

// Sets default values
ABoxTrigger::ABoxTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Mesh"));
	RootComponent = TriggerMesh;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	Box->SetGenerateOverlapEvents(true);
	Box->SetupAttachment(RootComponent);
}

void ABoxTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!MusicActorPtr.IsValid()) return;

	if (OtherActor) {
		if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
		{

			if(ALevelMusicActor*MusicActor = MusicActorPtr.Get())
			{
				MusicActor->SetAndPlayMusic();
			}

			if(EnemyReference.IsValid())
			{
				if(AEnemy*Enemy = Cast<AEnemy>(EnemyReference.Get()))
				{
					Enemy->SetActorLocation(Box->Bounds.Origin);
					FVector DirectionToPlayer = (player->GetActorLocation() - Enemy->GetActorLocation());
					Enemy->SetActorRotation(DirectionToPlayer.Rotation());
					if(ABoss_Wave*WaveEnemy = Cast<ABoss_Wave>(Enemy))
					{
						WaveEnemy->StartBoss();
					}

					if(AAIEnemy*AIEnemyController = Cast<AAIEnemy>(Enemy->GetController()))
					{
						AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());
					}
					Box->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
					if(APlayerController*PC = Cast<APlayerController>(player->GetController()))
					{
						if(AInGamePlayerHUD*InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
						{
							if(UMainPlayerWidget*MainUIWidget = InGamePlayerHUD->GetMainUIWidget())
							{
								if(MainUIWidget->BossInfo)
								{
									if (MainUIWidget->BossName)
									{
										MainUIWidget->BossName->SetText(Enemy->EnemyName);
									}
									MainUIWidget->BossInfo->SetVisibility(ESlateVisibility::Visible);
								}
								MainUIWidget->UpdateProgressBar("BossHealth", Enemy->Health / Enemy->MaxHealth);
							}
						}
					}
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("SOMETHING OVERLAPPED"));
	}
}

// Called when the game starts or when spawned
void ABoxTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &ABoxTrigger::BeginOverlap);
}

// Called every frame
void ABoxTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

