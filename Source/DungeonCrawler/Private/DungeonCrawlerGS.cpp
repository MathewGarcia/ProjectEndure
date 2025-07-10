// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonCrawlerGS.h"

#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void ADungeonCrawlerGS::BeginPlay()
{
	Super::BeginPlay();

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
       player = Cast<APlayerCharacter>(PlayerController->GetPawn());

    }


}
