// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityButtonWidget.generated.h"

class UProgressBar;
class UAbility;
class UButton;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityButtonWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(BindWidget))
	UImage* image;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(BindWidget))
	UTextBlock* AbilityName;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AbilityCooldownPB;

	int32 slotNum = -1;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* AbilityButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AbilityHotkey;

	UPROPERTY(EditAnywhere,Category = "Drag Widget Class")
	TSubclassOf<UUserWidget> DragWidgetClass;

	UPROPERTY()
	UAbility* AssignedAbility;

	UFUNCTION()
	void UpdateProgressBar();

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void Reinitialize(UAbility* Ability);

	FTimerHandle CDTimerHandle;

	virtual void NativeDestruct() override;

	
	
};
