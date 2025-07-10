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
	AcceptButton->OnClicked.AddDynamic(this, &UWidget_Quest::OnAcceptClicked);
	DenyButton->OnClicked.AddDynamic(this, &UWidget_Quest::OnDenyClicked);
}

void UWidget_Quest::InitInfo()
{
	if (!QuestDataAsset) return;

	QuestText->SetText(QuestDataAsset->QuestName);

	DescriptionText->SetText(QuestDataAsset->QuestDescription);

	for(const auto& pair : QuestDataAsset->RewardsMap)
	{
		
		PossibleRewards->AddItem(pair.Key);

	}

	EXPTextBlock->SetText(FText::FromString(FString::Printf(TEXT("EXP: %f"), QuestDataAsset->RewardEXP)));
}

void UWidget_Quest::OnAcceptClicked()
{
	if (!QuestDataAsset || !QuestDataAsset->QuestToSpawn) return;

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOwningPlayerState()))
	{
		if(UQuest*Quest = NewObject<UQuest>(PCS,QuestDataAsset->QuestToSpawn))
		{
			if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(PCS->GetPawn()->GetController())) {
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

void UWidget_Quest::OnDenyClicked()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		if (APlayerCharacter* player = MGI->localPlayer) {
			if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
			{
				SetVisibility(ESlateVisibility::Collapsed);
				PCC->ResetCursor();
			}
		}
	}
}
