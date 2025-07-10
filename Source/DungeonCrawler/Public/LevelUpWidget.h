// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalentTreeWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpWidget.generated.h"

class UProgressBar;
enum class EPlayerStatType : uint8;
class AInGamePlayerHUD;
class APlayerCharacter;
class FOnLevelChanged;
class APlayerCharacterState;
class UTextBlock;
class UButton;
/**
 * 
 */

UCLASS()
class DUNGEONCRAWLER_API ULevelUpWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingPointsText;

	UFUNCTION()
	void UpdateRemainingPointsText(const int& Remaining);

	UPROPERTY(meta=(BindWidget))
	UButton* HealthDecreaseButton;

	UFUNCTION()
	void HealthDecreasedButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* HealthIncreaseButton;

	UFUNCTION()
	void HealthIncreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthStatLevel;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* StaminaStatLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* StaminaDecreaseButton;

	UFUNCTION()
	void StaminaDecreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* StaminaIncreaseButton;

	UFUNCTION()
	void StaminaIncreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StrengthStatLevel;


	UPROPERTY(meta = (BindWidget))
	UButton* StrengthDecreaseButton;

	UFUNCTION()
	void StrengthDecreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* StrengthIncreaseButton;

	UFUNCTION()
	void StrengthIncreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IntellectStatLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* IntellectDecreaseButton;

	UFUNCTION()
	void IntellectDecreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* IntellectIncreaseButton;

	UFUNCTION()
	void IntellectIncreaseButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* Cancel;

	UPROPERTY(meta = (BindWidget))
	UButton* Complete;


	UPROPERTY(meta = (BindWidget))
	UProgressBar* EXP;

	UPROPERTY()
	APlayerCharacterState* PCS;

	UPROPERTY()
	APlayerCharacter* Player;


	UPROPERTY()
	AInGamePlayerHUD*PlayerHUD;

	void UpdateEXPProgressBar(const float& Val);

	UFUNCTION()
	void RefreshAllStats();


	void RebindDelegates(APlayerCharacterState*NewPCS);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void CancelOnClick();

	UFUNCTION()
	void OnClickFinish();


	UFUNCTION()
	void updateLevelUp(const int& newLevel);


	void RemoveRecentFromStatTrack(EPlayerStatType PlayerStatType);

private:

	int spentHealthPoints = 0;
	int spentStaminaPoints = 0;
	int spentStrengthPoints = 0;
	int spentIntellectPoints = 0;
};
