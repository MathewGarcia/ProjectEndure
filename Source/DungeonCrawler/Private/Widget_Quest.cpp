// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Quest.h"

#include "MainGameInstance.h"
#include "NPC.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterState.h"
#include "Quest.h"
#include "QuestDataAsset.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWidget_Quest::NativeConstruct()
{
	Super::NativeConstruct();

	InitInfo();
	if (AcceptButton) {
		AcceptButton->OnClicked.AddDynamic(this, &UWidget_Quest::OnAcceptClicked);
	}
	if (DenyButton) {
		DenyButton->OnClicked.AddDynamic(this, &UWidget_Quest::OnDenyClicked);
	}
}

void UWidget_Quest::InitInfo()
{
	if (!QuestDataAsset) return;
	if (QuestText) {
		QuestText->SetText(QuestDataAsset->QuestName);
	}
	if (DescriptionText) {
		DescriptionText->SetText(QuestDataAsset->QuestDescription);
	}
	if (PossibleRewards && QuestDataAsset) {
		for(const auto& pair : QuestDataAsset->RewardsMap)
		{
			PossibleRewards->AddItem(pair.Key);
		}
	}
	if (EXPTextBlock) {
		EXPTextBlock->SetText(FText::FromString(FString::Printf(TEXT("EXP: %f"), QuestDataAsset->RewardEXP)));
	}
}

void UWidget_Quest::OnAcceptClicked()
{
	if (!QuestDataAsset || !QuestDataAsset->QuestToSpawn) return;
	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOwningPlayerState()))
	{
		if(UQuest*Quest = NewObject<UQuest>(PCS,QuestDataAsset->QuestToSpawn))
		{
			APawn* Pawn = PCS->GetPawn();
			if (Pawn) {
				if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(Pawn->GetController())) {
					PCC->ResetCursor();
					Quest->QuestDataAsset = QuestDataAsset;
					Quest->OwningNPC = OwningNPC;
					Quest->StartQuest();
					SetVisibility(ESlateVisibility::Collapsed);
					if(OwningNPC)
					{
						OwningNPC->DisableBoxComponent();
					}
				}
			}
		}
	}
}

void UWidget_Quest::OnDenyClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance())) {
		if (APlayerCharacter* player = MGI->localPlayer) {
			if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
			{
				SetVisibility(ESlateVisibility::Collapsed);
				PCC->ResetCursor();
			}
		}
	}
}
