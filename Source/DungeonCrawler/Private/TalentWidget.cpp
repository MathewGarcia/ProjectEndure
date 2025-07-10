// Fill out your copyright notice in the Description page of Project Settings.


#include "TalentWidget.h"

#include "Ability.h"
#include "PlayerCharacterState.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "EmptyAbility.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "TalentTreeWidget.h"
#include "Components/TextBlock.h"

void UTalentWidget::OnButtonClicked() 
{
	if(AbilityClass)
	{
			if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOwningPlayerState()))
			{
				if (UAbility* Ability = NewObject<UAbility>(PCS, AbilityClass))
				{
					if (!bIsAbilityAlreadyLearned(Ability, PCS->LearnedAbilities)) {
						Ability->AbilityTalentRow = AbilityRow;
						RemovePreviousAbilty(AbilityRow, PCS);
						PCS->LearnedAbilities.Add(Ability);


						if (Ability->AbilityActivationType == EAbilityActivationType::Event)
						{

							if (Ability->bShouldExecute_Implementation())
							{
								Ability->execute_Implementation();
							}
							for (int i = 0; i < PCS->EquippedAbilities.Num(); ++i) {

								if (PCS->EquippedAbilities[i]->AbilityTalentRow == AbilityRow)
								{
									PCS->EquippedAbilities[i] = NewObject<UEmptyAbility>(PCS);
									break;
								}

							}
						}
						else if (Ability->AbilityActivationType == EAbilityActivationType::Activate) {

							for (int i = 0; i < PCS->EquippedAbilities.Num(); ++i) {

								if (PCS->EquippedAbilities[i]->AbilityTalentRow == AbilityRow)
								{
									PCS->EquippedAbilities[i] = Ability;
									break;
								}

								if (PCS->EquippedAbilities[i]->GetClass() == UEmptyAbility::StaticClass()) {

									PCS->EquippedAbilities[i] = Ability;
									break;
								}

							}

						}

						if(OwningWidget.IsValid())
						{
							if(UTalentTreeWidget* LocalOwningWidget = OwningWidget.Get())
							{
								LocalOwningWidget->CheckLearnedAbilities();
							}
						}
						PCS->OnAbilitiesUpdated.Broadcast();

						UE_LOG(LogTemp, Warning, TEXT("Learned ability: %s"), *Ability->AbilityName);
					}
				}

			}
		
	}
}

void UTalentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TalentButton->OnClicked.AddUniqueDynamic(this, &UTalentWidget::OnButtonClicked);
	TalentButton->OnHovered.AddUniqueDynamic(this, &UTalentWidget::UTalentWidget::ChangeHoveredText);

		if (AbilityClass) {
			if (UAbility* AbilityInfo = AbilityClass->GetDefaultObject<UAbility>())
			{
				TalentImage->SetBrushFromTexture(AbilityInfo->Image);
			}
		}
}

void UTalentWidget::RemovePreviousAbilty(const EAbilityTalentRow& AbilityRowToCheck, APlayerCharacterState* LocalPCS)
{
	if (!LocalPCS ) return;

	//possibly make this a reverse array.

	for(int i = 0; i < LocalPCS->LearnedAbilities.Num(); ++i)
	{
		if(UAbility*AbilityToCompareTo = LocalPCS->LearnedAbilities[i])
		{
			if(AbilityToCompareTo->AbilityTalentRow == AbilityRowToCheck)
			{
				LocalPCS->LearnedAbilities.RemoveAt(i);
				break;
			}
		}
	}

}

bool UTalentWidget::bIsAbilityAlreadyLearned(UAbility* CheckedAbility, TArray<UAbility*>& LearnedAbilities)
{
	for(UAbility*Ability : LearnedAbilities)
	{
		if(Ability->GetClass() == CheckedAbility->GetClass())
		{
			return true;
		}
	}
	return false;
}

void UTalentWidget::ChangeHoveredText()
{
	if (!OwningWidget.IsValid()) return;

	if (UTalentTreeWidget* TTW = OwningWidget.Get()) {
		if (TTW->DescriptionText) {
			if (UAbility* AbilityDefaultObj = Cast<UAbility>(AbilityClass->GetDefaultObject<UAbility>())) {
				TTW->DescriptionText->SetText(FText::FromString(AbilityDefaultObj->AbilityDescription));
			}
		}
	}
	
}

bool UTalentWidget::bIsAbilityLearned()
{
	if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOwningPlayerState())) {
		for (int i = 0; i < pcs->LearnedAbilities.Num(); ++i)
		{
			if(UAbility*Ability = pcs->LearnedAbilities[i])
			{
				if(Ability->GetClass() == AbilityClass)
				{
					return true;
				}
			}
		}
	}
	return false;
}
