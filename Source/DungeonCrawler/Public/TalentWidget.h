// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability_Talent_Row_Enum.h"
#include "Blueprint/UserWidget.h"
#include "TalentWidget.generated.h"

class UTalentTreeWidget;
class UTextBlock;
class APlayerCharacterState;
class UImage;
class UAbility;
/**
 * 
 */
class UButton;

UCLASS()
class DUNGEONCRAWLER_API UTalentWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,Category = "Ability Row")
	EAbilityTalentRow AbilityRow;

	UPROPERTY(meta=(BindWidget))
	UButton* TalentButton;

	UPROPERTY(EditAnywhere,Category = "Ability")
	TSubclassOf<UAbility>AbilityClass;

	UPROPERTY(meta=(BindWidget))
	UImage* TalentImage;

	UFUNCTION()
	void OnButtonClicked();

	virtual void NativeConstruct() override;

	void RemovePreviousAbilty(const EAbilityTalentRow& AbilityRowToCheck,APlayerCharacterState*LocalPCS);

	bool bIsAbilityAlreadyLearned(UAbility*CheckedAbility,TArray<UAbility*>& LearnedAbilities);

	UPROPERTY()
	TWeakObjectPtr<UTalentTreeWidget> OwningWidget;

	UFUNCTION()
	void ChangeHoveredText();

	bool bIsAbilityLearned();
};
