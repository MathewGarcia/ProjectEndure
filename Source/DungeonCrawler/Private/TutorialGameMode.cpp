// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"

void ATutorialGameMode::StartPlay()
{
    UWorld* World = GetWorld();
    if (!World) {
        Super::StartPlay();
        return;
    }
    if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
    {
        MGI->LoadType = ELoadType::Tutorial;
        FTimerHandle FDelay;
        World->GetTimerManager().SetTimer(FDelay, [MGI]
        {
            if(IsValid(MGI))
            {
                if(APlayerCharacter*player = MGI->localPlayer)
                {
                    TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
                    if (MPWPtr.IsValid())
                    {
                        UMainPlayerWidget* MPW = MPWPtr.Get();
                        if (MPW) {
                            MPW->SetLoadVisibility(ESlateVisibility::Collapsed);
                        }
                    }
                }
            }
        },0.5, false);
    }
    Super::StartPlay();
}
