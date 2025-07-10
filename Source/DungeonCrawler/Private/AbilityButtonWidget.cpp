// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityButtonWidget.h"

#include <EmptyAbility.h>

#include "Ability.h"
#include "DragWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UAbilityButtonWidget::UpdateProgressBar()
{
	if (!AssignedAbility) return;

	float CurrentTime = AssignedAbility->Cooldown;

	TWeakObjectPtr<UAbilityButtonWidget> SafeButton = this;

	GetWorld()->GetTimerManager().SetTimer(CDTimerHandle, [this,CurrentTime]() mutable
	{
		if(AbilityCooldownPB)
		{
			CurrentTime -= 1;
			AbilityCooldownPB->SetPercent(( CurrentTime / AssignedAbility->Cooldown));
			if(AbilityCooldownPB->GetPercent() <= 0.f)
			{
				GetWorld()->GetTimerManager().ClearTimer(CDTimerHandle);
			}
		}
	}, 1.f,true);
}

void UAbilityButtonWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOwningPlayerState())) {

		if (UAbility* Ability = Cast<UAbility>(ListItemObject))
		{
			if (image)
			{
				image->SetBrushFromTexture(Ability->Image);
			}

			if (AbilityName)
			{
				AbilityName->SetText(FText::FromString(Ability->AbilityName));
			}

			slotNum = PCS->EquippedAbilities.IndexOfByKey(Ability);

			if (AbilityHotkey)
			{
				FString AbilityString = "Ability" + FString::FromInt(slotNum+1);
				AbilityString += "IA";

				//hard coded for now.
				if (const auto* FoundInputMap = PCS->InputMap.Find(AbilityString))
				{
					FString KeyToPress = FoundInputMap->ToString();
					AbilityHotkey->SetText(FText::FromString(KeyToPress));
				}
			}

			if(AbilityCooldownPB)
			{
				Ability->OnAbilityUsed.AddDynamic(this, &UAbilityButtonWidget::UpdateProgressBar);
			}

			if (Ability->GetClass() != UEmptyAbility::StaticClass()) {
				AssignedAbility = Ability;
			}

		}
	}
}

FReply UAbilityButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	 Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);



	 FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);


	 UE_LOG(LogTemp, Warning, TEXT("Mouse button pressed"));
	 return Reply.NativeReply;
}

void UAbilityButtonWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!AssignedAbility) return;

	UE_LOG(LogTemp, Warning, TEXT("Drag Detected"));

	UDragDropOperation* DragDropOperation = NewObject<UDragDropOperation>(this, UDragDropOperation::StaticClass());

	if(DragWidgetClass)
	{
		UDragWidget* DragVisual = CreateWidget<UDragWidget>(GetWorld(), DragWidgetClass);
		DragVisual->image->SetBrush(image->GetBrush());
		DragVisual->AbilityName->SetText(AbilityName->GetText());
		DragDropOperation->DefaultDragVisual = DragVisual;
	}
	DragDropOperation->Payload = this;
	OutOperation = DragDropOperation;
}

bool UAbilityButtonWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Drag dropped on %d"), slotNum);

	UAbilityButtonWidget* DragSource = Cast<UAbilityButtonWidget>(InOperation->Payload);

	if (DragSource && DragSource != this) {

		if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOwningPlayerState())) {

			UAbility* Temp = PCS->EquippedAbilities[slotNum];
			PCS->EquippedAbilities[slotNum] = PCS->EquippedAbilities[DragSource->slotNum];
			PCS->EquippedAbilities[DragSource->slotNum] = Temp;

			Reinitialize(PCS->EquippedAbilities[slotNum]);
			DragSource->Reinitialize(PCS->EquippedAbilities[DragSource->slotNum]);



		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAbilityButtonWidget::Reinitialize(UAbility* Ability)
{
	if (!Ability) return;


	if (image)
	{
		image->SetBrushFromTexture(Ability->Image);
	}

	if (AbilityName)
	{
		AbilityName->SetText(FText::FromString(Ability->AbilityName));
	}

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOwningPlayerState()))
	{
		slotNum = PCS->EquippedAbilities.IndexOfByKey(Ability);

		if (AbilityHotkey)
		{
			FString AbilityString = "Ability" + FString::FromInt(slotNum + 1) + "IA";

			if (const auto* FoundInputMap = PCS->InputMap.Find(AbilityString))
			{
				FString KeyToPress = FoundInputMap->ToString();
				AbilityHotkey->SetText(FText::FromString(KeyToPress));
			}
		}
	}

	if (Ability->GetClass() != UEmptyAbility::StaticClass()) {
		AssignedAbility = Ability;
	}

}

void UAbilityButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearTimer(CDTimerHandle);
}


