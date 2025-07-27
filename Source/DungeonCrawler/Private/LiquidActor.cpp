// Fill out your copyright notice in the Description page of Project Settings.


#include "LiquidActor.h"

#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ALiquidActor::ALiquidActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = BoxComponent;
}

void ALiquidActor::OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if(OtherActor && OtherActor->GetClass() != StaticClass())
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
		{
			player->LiquidEntered(this,ElementType);
		}
	}*/
}

void ALiquidActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (OtherActor && OtherActor->GetClass() != StaticClass())
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			player->LiquidExited(this,ElementType);
		}
	}*/
}

// Called when the game starts or when spawned
void ALiquidActor::BeginPlay()
{
	Super::BeginPlay();
	if (BoxComponent) {
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALiquidActor::OnHitBoxBeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ALiquidActor::OverlapEnd);
	}
}

// Called every frame
void ALiquidActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

