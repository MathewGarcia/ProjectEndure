// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_QuestGiver.h"

#include "InteractableTextWidget.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterState.h"
#include "QuestDataAsset.h"
#include "Widget_Quest.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"

void ANPC_QuestGiver::Interact_Implementation()
{
	if (!ChosenQuest.IsValid()) return;
	UWorld* World = GetWorld();
	if (!World) return;
	if (QuestWidgetClass && !QuestWidget)
	{
		QuestWidget = CreateWidget<UWidget_Quest>(World, QuestWidgetClass);
		if (QuestWidget)
		{
			QuestWidget->QuestDataAsset = ChosenQuest.Get();
			QuestWidget->OwningNPC = this;
			QuestWidget->AddToViewport();
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance())) {
				if (APlayerCharacter* player = MGI->localPlayer) {
					if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
					{
						PCC->ShowHideCursor(true);
					}
				}
			}
		}
	}
	else if (QuestWidget)
	{
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance())) {
			if (APlayerCharacter* player = MGI->localPlayer) {
				if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
				{
					PCC->ShowHideCursor(true);
				}
			}
		}
		QuestWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

UQuestDataAsset* ANPC_QuestGiver::GetQuest()
{
	float totalWeight = 0.f;
	for(const auto&pair : QuestsMap)
	{
		totalWeight += pair.Value;
	}
	if (totalWeight <= 0) return nullptr;
	float Roll = FMath::FRandRange(0.f, totalWeight);
	for(const auto&pair : QuestsMap)
	{
		Roll -= pair.Value;
		if(Roll <= 0)
		{
			return pair.Key;
		}
	}
	return nullptr;
}

void ANPC_QuestGiver::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (OtherActor)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
        {
            player->OverlappingActor = this;
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
            if (MPWPtr.IsValid())
            {
                UMainPlayerWidget* MPW = MPWPtr.Get();
                if (MPW && MPW->InteractionsWidget)
                {
                    UInteractableTextWidget* InteractableTextWidget = MPW->InteractionsWidget;
                    if (APlayerCharacterState* PlayerCharacterState = player->GetPlayerCharacterState()) {
                        if (FKey* InputKey = PlayerCharacterState->InputMap.Find("InteractIA")) {
                            FKey Key = (*InputKey);
                            FString String = FString::Printf(TEXT("Press %s to Interact"), *Key.ToString());
                            InteractableTextWidget->UpdateInteractTextBlock(String);
                            InteractableTextWidget->SetVisibility(ESlateVisibility::Visible);
                        }
                    }
                }
            }
        }
    }
}

void ANPC_QuestGiver::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    if (OtherActor)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
        {
            player->OverlappingActor = nullptr;
            if(QuestWidget)
            {
                QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
            }
            if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
            {
                PCC->ShowHideCursor(false);
            }
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
            if (MPWPtr.IsValid())
            {
                UMainPlayerWidget* MPW = MPWPtr.Get();
                if (MPW && MPW->InteractionsWidget)
                {
                    MPW->InteractionsWidget->SetVisibility(ESlateVisibility::Collapsed);
                }
            }
        }
    }
}

void ANPC_QuestGiver::BeginPlay()
{
    Super::BeginPlay();
    ChosenQuest = GetQuest();
    if (BoxComponent) {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPC_QuestGiver::BeginOverlap);
        BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ANPC_QuestGiver::OverlapEnd);
    }
}
