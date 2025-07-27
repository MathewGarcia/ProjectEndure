// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Blacksmith.h"

#include "BlacksmithUpgradeUI.h"
#include "InGamePlayerHUD.h"
#include "InteractableTextWidget.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "Components/BoxComponent.h"

void ANPC_Blacksmith::Interact_Implementation()
{
	Super::Interact_Implementation();

	UE_LOG(LogTemp, Error, TEXT("I am the blacksmith"));

	UWorld* World = GetWorld();
	if (!World) return;
	if(UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
	{
		if(APlayerCharacter*player = MGI->localPlayer)
		{
			if(APlayerCharacterController*PCC = Cast<APlayerCharacterController>(player->GetController()))
			{
				if(UpgradeBlacksmithWidgetClass && !BlacksmithUpgradeUI)
				{
					BlacksmithUpgradeUI = CreateWidget<UBlacksmithUpgradeUI>(World, UpgradeBlacksmithWidgetClass);

					if(BlacksmithUpgradeUI)
					{
						BlacksmithUpgradeUI->AddToViewport();
					}
				}
				else if (BlacksmithUpgradeUI)
				{
					BlacksmithUpgradeUI->SetVisibility(ESlateVisibility::Visible);
				}
				PCC->ShowHideCursor(true);
			}
		}
	}

}


void ANPC_Blacksmith::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
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

void ANPC_Blacksmith::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);


	if (OtherActor)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			player->OverlappingActor = nullptr;
			if(BlacksmithUpgradeUI)
			{
				BlacksmithUpgradeUI->SetVisibility(ESlateVisibility::Hidden);
			}
			TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
			if (MPWPtr.IsValid())
			{
				UMainPlayerWidget* MPW = MPWPtr.Get();
				if (MPW && MPW->InteractionsWidget)
				{
					MPW->InteractionsWidget->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController()))
			{
				PCC->ResetCursor();
			}
		}
	}
}

void ANPC_Blacksmith::BeginPlay()
{
	Super::BeginPlay();
	if (BoxComponent) {
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPC_Blacksmith::BeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ANPC_Blacksmith::OverlapEnd);
	}
}
