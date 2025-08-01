// Fill out your copyright notice in the Description page of Project Settings.


#include "AoEHealActor.h"

#include "AoE_Heal.h"
#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "TimerManager.h"

// Sets default values
AAoEHealActor::AAoEHealActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	RootComponent = BoxComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

}

void AAoEHealActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(OtherActor)
	{
		if(APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{

			if (OwningObject && player)
			{
				if (!GetWorldTimerManager().IsTimerActive(FHealingTimer))
				{
					GetWorldTimerManager().SetTimer(FHealingTimer, [this, player]()
						{
							if (!player || !OwningObject) return;
							if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
							{
								PCS->playerStats.currentHealth += OwningObject->HealingAmount;
								PCS->playerStats.currentMana += OwningObject->HealingAmount;

								if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
									if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
									{
										if (UMainPlayerWidget* MainWidget = PlayerHUD->GetMainUIWidget())
										{
											float healthRatio = PCS->GetTotalHealth() > 0 ? PCS->playerStats.currentHealth / PCS->GetTotalHealth() : 0.f;
											float manaRatio = PCS->GetTotalMana() > 0 ? PCS->playerStats.currentMana / PCS->GetTotalMana() : 0.f;
											MainWidget->UpdateProgressBar("Health", healthRatio);
											MainWidget->UpdateProgressBar("Mana", manaRatio);
										}
									}
								}

							}
						}, OwningObject->HealingRate, true);
				}
			}
		}
	}
}

void AAoEHealActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			if (OwningObject)
			{
				if (GetWorldTimerManager().IsTimerActive(FHealingTimer))
				{
					GetWorldTimerManager().ClearTimer(FHealingTimer);
				}
			}
		}
	}
}


// Called when the game starts or when spawned
void AAoEHealActor::BeginPlay()
{
	Super::BeginPlay();

	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAoEHealActor::BeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AAoEHealActor::OverlapEnd);
	}
	
}

// Called every frame
void AAoEHealActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

