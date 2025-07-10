// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "BlacksmithUpgradeUI.generated.h"

class UItemDataObject;
class UButton;
class UItemDataAsset;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBlacksmithUpgradeUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item List View",meta = (BindWidget))
	UListView* ItemListView;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

	void SetUpgradeItemData(UObject* SelectedItem);


	virtual void NativeConstruct() override;

	void HandleOnEntryWidgetGenerated(UUserWidget& GeneratedWidget);

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnCloseButtonClicked();

	UPROPERTY()
	UItemDataObject* UpgradeItemData;

	void InitializeList();
};
