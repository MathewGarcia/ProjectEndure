// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TalentTreeWidget.generated.h"

class UAbility;
class UTextBlock;
class AInGamePlayerHUD;
class APlayerCharacter;
class APlayerCharacterState;
class UHorizontalBox;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UTalentTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* StrengthVB;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* IntellectVB;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionText;


	void fillRows(TArray<UHorizontalBox*>& rowToFill,UVerticalBox*rowToSearch);

	void EnableRow(TArray<UHorizontalBox*>rowToCheck, const int& playerStat);

	void setAllRowsDisabled(TArray<UHorizontalBox*> rowToHide);



	void CheckAllRows();

	void CheckLearnedAbilities();


	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	AInGamePlayerHUD* PlayerHUD;
	
	UPROPERTY()
	APlayerCharacterState* PCS;
private:
	TArray<UHorizontalBox*> StrengthRows;

	TArray<UHorizontalBox*> IntellectRows;

protected:
	virtual void NativeConstruct() override;

	void SetTalentWidgetOwner();


};
