// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterState.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayerWidget.generated.h"

class USizeBox;
class UOverlay;
class UExpandableArea;
class UInteractableTextWidget;
class UHorizontalBox;
class UTextBlock;
class UVerticalBox;
class UProgressBar;
class APlayerCharacterState;
class UTileView;
class UEquipmentUIWidget;
class UGearSlotWidget;
class UInventoryWidget;
class UWidget_GearInfo;
/**
 * 
 */

UCLASS()
class DUNGEONCRAWLER_API UMainPlayerWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UEquipmentUIWidget* EquipmentUI;

	UPROPERTY(meta = (BindWidget))
	UTileView* AbilityList;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	UPROPERTY()
	APlayerCharacterState* PCS;

	UFUNCTION()
	void WeaponUpdated(AWeapon* Weapon);

	UPROPERTY(EditAnywhere, Category = "Empty slot ablity class")
	TSubclassOf<UAbility> EmptySlotAbilityClass;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TArray<UTexture2D*> crosshairs;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Health;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Stamina;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Mana;

	TMap<FName, UProgressBar*> ProgressBars;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BossInfo;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossName;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Fire;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Lightning;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Water;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Shadow;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Bleed;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Poison;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* FireHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* LightningHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WaterHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* ShadowHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BleedHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PoisonHB;

	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta = (BindWidget))
	UInteractableTextWidget* InteractionsWidget;

	UPROPERTY(meta=(BindWidget))
	UExpandableArea* ExpandableEquipment;


	UPROPERTY(meta = (BindWidget))
	UExpandableArea* ExpandableStats;

	UPROPERTY(meta = (BindWidget))
	UExpandableArea* ExpandableTalents;

	UPROPERTY(meta = (BindWidget))
	UExpandableArea* ExpandableBag;

	UPROPERTY(meta = (BindWidget))
	UWidget_GearInfo* GearInfo;

	UPROPERTY(meta = (BindWidget))
	UOverlay* LoadScreenOverlay;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (BindWidget))
	UTextBlock* RoundCountText;

	UPROPERTY(meta = (BindWidget))
	USizeBox* HealthSizeBox;

	UPROPERTY(meta = (BindWidget))
	USizeBox* StaminaSizeBox;

	UPROPERTY(EditAnywhere,Category = "Base Width")
	float BaseWidth = 60.f;

	TMap<FName, USizeBox*> SizeBoxMap;

	UFUNCTION()
	void AbilitiesUpdated();

	virtual void NativeConstruct() override;

	void UpdateProgressBar(FName ProgressBarToFind, const float& value);

	void SetHorizontalBoxVisibility(EElementTypes ElementType,ESlateVisibility NewVisiblity);

	void RebindDelegates();

	void ExpandEquipment(bool NewVal);
	void ExpandStats(bool NewVal);
	void ExpandTalents(bool NewVal);
	void ExpandBag(bool NewVal);

	void SetLoadVisibility(ESlateVisibility NewVisibility);

	void UpdateHealthSizeBox();

	void UpdateStaminaSizeBox();

};
