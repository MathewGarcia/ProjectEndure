// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_Letter_Reciever.h"

#include "InteractableTextWidget.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Quest_Note.h"
#include "Components/BoxComponent.h"

void ANPC_Letter_Reciever::BeginPlay()
{
	Super::BeginPlay();
	if (BoxComponent) {
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPC_Letter_Reciever::BeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ANPC_Letter_Reciever::OverlapEnd);
	}
}

void ANPC_Letter_Reciever::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ANPC_Letter_Reciever::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			player->OverlappingActor = nullptr;
			TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
			if (MPWPtr.IsValid())
			{
				UMainPlayerWidget* MPW = MPWPtr.Get();
				if (MPW && MPW->InteractionsWidget)
				{
					MPW->InteractionsWidget->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

void ANPC_Letter_Reciever::Interact_Implementation()
{
	Super::Interact_Implementation();
	if (!NoteQuest) return;
	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(NoteQuest->GetOuter()))
	{
		if(PCS->CurrentQuests.Contains(NoteQuest))
		{
			NoteQuest->CompleteQuest();
			//play goodbye animation?
			UE_LOG(LogTemp, Warning, TEXT("Thank you!!"))
		}
	}
}
