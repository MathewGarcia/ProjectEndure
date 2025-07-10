// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffActor.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADebuffActor::ADebuffActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitComponent = CreateDefaultSubobject <UCapsuleComponent>(TEXT("Hit Component"));
	HitComponent->SetupAttachment(GetMesh());
	HitComponent->SetGenerateOverlapEvents(true);


}

void ADebuffActor::OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(OtherActor && OtherActor != GetOwner())
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
		{
			if(AEnemy*enemy = Cast<AEnemy>(GetOwner()))
			{
				player->TakeDamage(enemy->EnemyParameters.ElementDamage*5,FDamageEvent(),nullptr,this);

			}
		}
		if(AEnemy*enemy = Cast<AEnemy>(OtherActor))
		{
			if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner())) {
				if (AWeapon*Weapon = player->GetCurrentWeapon()) {
					if (Weapon->ItemData) {
						if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData)) {
							enemy->TakeDamage(WeaponItemData->ElementTypeDamage, FDamageEvent(), nullptr, this);
						}
					}
				}

			}
		}
	}
}

// Called when the game starts or when spawned
void ADebuffActor::BeginPlay()
{
	Super::BeginPlay();

	HitComponent->OnComponentBeginOverlap.AddDynamic(this, &ADebuffActor::OnHitBoxBeginOverlap);

	GetMesh()->PlayAnimation(AnimToPlay,false);
}

// Called every frame
void ADebuffActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

