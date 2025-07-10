// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBuffActor.h"
#include "BuffSelectionWidget.h"
#include "InGamePlayerHUD.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"

UBuffDataAsset* AInteractableBuffActor::ChooseBuff()
{
	float TotalWeight = 0;

	for(auto const& pair : BuffsMap)
	{
		TotalWeight += pair.Value;
	}

	float RandomRoll = FMath::FRandRange(0, TotalWeight);

	for (auto const& pair : BuffsMap)
	{
		RandomRoll -= pair.Value;

		if(RandomRoll <= 0)
		{
			return pair.Key;
		}
	}

	return nullptr;
}

void AInteractableBuffActor::OnBuffSelected()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (MGI->localPlayer)
		{
			if (APlayerController* PC = Cast<APlayerController>(MGI->localPlayer->GetController()))
			{
				if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(PC)) {
					PCC->ResetCursor();
					Destroy();

				}

			}

		}

	}
}

void AInteractableBuffActor::Interact_Implementation()
{
	Super::Interact_Implementation();

	//add the buffs to the chosen buffs array.
	for(int i = 0; i < MaxBuffsDrop; ++i)
	{
		ChosenBuffs.Add(ChooseBuff());
	}

	for (auto const& pair : BuffsMap)
	{
		AllBuffs.Add(pair.Key);
	}
	//then we need to pull up the interface.

	if (BuffSelectionWidgetClass && !BuffSelectionWidget)
	{
		BuffSelectionWidget = CreateWidget<UBuffSelectionWidget>(GetWorld(), BuffSelectionWidgetClass);

		if (BuffSelectionWidget)
		{
			if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
			{
				if(MGI->localPlayer)
				{
					if(APlayerController*PC = Cast<APlayerController>(MGI->localPlayer->GetController()))
					{
						if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(PC)) {
							if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
							{
								PCC->ShowHideCursor(true);
								InGamePlayerHUD->BuffSelectionWidget = BuffSelectionWidget;
								BuffSelectionWidget->Init(ChosenBuffs, AllBuffs);
								BuffSelectionWidget->OnBuffSelected.AddDynamic(this, &AInteractableBuffActor::OnBuffSelected);
								BuffSelectionWidget->AddToViewport();
							}
						}
					}
				}
			}
		

		}
	}
	else if (BuffSelectionWidget)
	{
		BuffSelectionWidget->SetVisibility(ESlateVisibility::Visible);
	}



}

