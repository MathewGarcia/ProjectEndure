// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUpWidget.h"
#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void ULevelUpWidget::UpdateRemainingPointsText(const int& Remaining)
{
	if(RemainingPointsText)
	{
		RemainingPointsText->SetText(FText::FromString(FString::FromInt(Remaining)));
	}
}

void ULevelUpWidget::HealthDecreasedButtonClicked()
{
	if (!PCS) return;

	if (spentHealthPoints > 0) {

		FString currentText = HealthStatLevel->GetText().ToString();

		int currentNumber = FCString::Atoi(*currentText);

		HealthStatLevel->SetText(FText::AsNumber(currentNumber - 1));
		PCS->playerStats.UpdateLevelPoints(+1);
		--spentHealthPoints;
		RemoveRecentFromStatTrack(EPlayerStatType::Health);
	}
}

void ULevelUpWidget::HealthIncreaseButtonClicked()
{
	if (!PCS) return;

	if (PCS->playerStats.levelPoints == 0) return;


	FString currentText = HealthStatLevel->GetText().ToString();

	int currentNumber = FCString::Atoi(*currentText);
	HealthStatLevel->SetText(FText::AsNumber(currentNumber + 1));
	PCS->playerStats.UpdateLevelPoints(-1);
	++spentHealthPoints;
	PCS->playerStats.StatTracker.Push(EPlayerStatType::Health);

}

void ULevelUpWidget::StaminaDecreaseButtonClicked()
{
	if (!PCS) return;

	if (spentStaminaPoints > 0) {
		FString currentText = StaminaStatLevel->GetText().ToString();

		int currentNumber = FCString::Atoi(*currentText);
		StaminaStatLevel->SetText(FText::AsNumber(currentNumber - 1));
		PCS->playerStats.UpdateLevelPoints(+1);
		--spentStaminaPoints;
		RemoveRecentFromStatTrack(EPlayerStatType::Stamina);

	}

}

void ULevelUpWidget::StaminaIncreaseButtonClicked()
{
	if (!PCS) return;

	if (PCS->playerStats.levelPoints == 0) return;

	FString currentText = StaminaStatLevel->GetText().ToString();

	int currentNumber = FCString::Atoi(*currentText);
	StaminaStatLevel->SetText(FText::AsNumber(currentNumber + 1));
	PCS->playerStats.UpdateLevelPoints(-1);
	++spentStaminaPoints;
	PCS->playerStats.StatTracker.Push(EPlayerStatType::Stamina);

}

void ULevelUpWidget::StrengthDecreaseButtonClicked()
{
	if (!PCS) return;

	if (spentStrengthPoints > 0) {
		FString currentText = StrengthStatLevel->GetText().ToString();

		int currentNumber = FCString::Atoi(*currentText);
		StrengthStatLevel->SetText(FText::AsNumber(currentNumber - 1));
		PCS->playerStats.UpdateLevelPoints(+1);

		--spentStrengthPoints;
		RemoveRecentFromStatTrack(EPlayerStatType::Strength);

	}
}

void ULevelUpWidget::StrengthIncreaseButtonClicked()
{
	if (!PCS) return;
	if (PCS->playerStats.levelPoints == 0) return;

	FString currentText = StrengthStatLevel->GetText().ToString();

	int currentNumber = FCString::Atoi(*currentText);
	StrengthStatLevel->SetText(FText::AsNumber(currentNumber + 1));
	PCS->playerStats.UpdateLevelPoints(-1);

	++spentStrengthPoints;
	PCS->playerStats.StatTracker.Push(EPlayerStatType::Strength);

}

void ULevelUpWidget::IntellectDecreaseButtonClicked()
{
	if (!PCS) return;

	if (spentIntellectPoints > 0) {
		FString currentText = IntellectStatLevel->GetText().ToString();

		int currentNumber = FCString::Atoi(*currentText);
		IntellectStatLevel->SetText(FText::AsNumber(currentNumber - 1));
		PCS->playerStats.UpdateLevelPoints(+1);

		--spentIntellectPoints;
		RemoveRecentFromStatTrack(EPlayerStatType::Intellect);

	}

}

void ULevelUpWidget::IntellectIncreaseButtonClicked()
{
	if (!PCS) return;
	if (PCS->playerStats.levelPoints == 0) return;

	FString currentText = IntellectStatLevel->GetText().ToString();

	int currentNumber = FCString::Atoi(*currentText);
	
	IntellectStatLevel->SetText(FText::AsNumber(currentNumber + 1));
	PCS->playerStats.UpdateLevelPoints(-1);

	++spentIntellectPoints;
	PCS->playerStats.StatTracker.Push(EPlayerStatType::Intellect);

}

void ULevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PCS = Cast<APlayerCharacterState>(GetOwningPlayerState());

	Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	if(Player)
	{
		if(APlayerController*PC = Cast<APlayerController>(Player->Controller))
		{
			PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD());

			if(PlayerHUD)
			{
				PlayerHUD->LevelUpWidget = this;
			}
		}
	}

	if(PCS)
	{
		HealthStatLevel->SetText(FText::AsNumber(PCS->playerStats.HealthLevel));
		StaminaStatLevel->SetText(FText::AsNumber(PCS->playerStats.Stamina));
		StrengthStatLevel->SetText(FText::AsNumber(PCS->playerStats.Strength));
		IntellectStatLevel->SetText(FText::AsNumber(PCS->playerStats.Intellect));
		RemainingPointsText->SetText(FText::AsNumber(PCS->playerStats.levelPoints));

		PCS->playerStats.OnPlayerStatLevelChanged.AddUniqueDynamic(this, &ULevelUpWidget::RefreshAllStats);

		PCS->playerStats.OnPlayerLevelChanged.AddDynamic(this,&ULevelUpWidget::updateLevelUp);
		PCS->playerStats.OnLevelPointsUpdated.AddDynamic(this, &ULevelUpWidget::UpdateRemainingPointsText);
	}

	HealthDecreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::HealthDecreasedButtonClicked);
	HealthIncreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::HealthIncreaseButtonClicked);
	StaminaDecreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::StaminaDecreaseButtonClicked);
	StaminaIncreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::StaminaIncreaseButtonClicked);
	StrengthDecreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::StrengthDecreaseButtonClicked);
	StrengthIncreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::StrengthIncreaseButtonClicked);
	IntellectDecreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::IntellectDecreaseButtonClicked);
	IntellectIncreaseButton->OnClicked.AddDynamic(this, &ULevelUpWidget::IntellectIncreaseButtonClicked);
	Cancel->OnClicked.AddDynamic(this, &ULevelUpWidget::CancelOnClick);
	Complete->OnClicked.AddDynamic(this, &ULevelUpWidget::OnClickFinish);

}

void ULevelUpWidget::UpdateEXPProgressBar(const float& Val)
{
	if(EXP)
	{
		EXP->SetPercent(Val);
	}
}

void ULevelUpWidget::RefreshAllStats()
{
	if (HealthStatLevel && StaminaStatLevel && StrengthStatLevel && IntellectStatLevel && RemainingPointsText) {

		LevelText->SetText(FText::AsNumber(PCS->playerStats.playerLevel));
		HealthStatLevel->SetText(FText::AsNumber(PCS->playerStats.HealthLevel));
		StaminaStatLevel->SetText(FText::AsNumber(PCS->playerStats.Stamina));
		StrengthStatLevel->SetText(FText::AsNumber(PCS->playerStats.Strength));
		IntellectStatLevel->SetText(FText::AsNumber(PCS->playerStats.Intellect));
		RemainingPointsText->SetText(FText::AsNumber(PCS->playerStats.levelPoints));

	}
}

void ULevelUpWidget::RebindDelegates(APlayerCharacterState* NewPCS)
{

	if (!NewPCS) return;

	NewPCS->playerStats.OnPlayerStatLevelChanged.AddUniqueDynamic(this, &ULevelUpWidget::RefreshAllStats);
	NewPCS->playerStats.OnPlayerLevelChanged.AddDynamic(this, &ULevelUpWidget::updateLevelUp);
	NewPCS->playerStats.OnLevelPointsUpdated.AddUniqueDynamic(this, &ULevelUpWidget::UpdateRemainingPointsText);

}


void ULevelUpWidget::CancelOnClick()
{
	if(PCS)
	{
		PCS->playerStats.UpdateLevelPoints(spentHealthPoints);
		PCS->playerStats.UpdateLevelPoints(spentStaminaPoints);
		PCS->playerStats.UpdateLevelPoints(spentStrengthPoints);
		PCS->playerStats.UpdateLevelPoints(spentIntellectPoints);

	}
	spentHealthPoints = 0;
	spentStaminaPoints = 0;
	spentStrengthPoints = 0;
	spentIntellectPoints = 0;

	RefreshAllStats();

}

void ULevelUpWidget::OnClickFinish()
{

	if (spentHealthPoints <= 0 && spentStaminaPoints <= 0 && spentStrengthPoints <= 0 && spentIntellectPoints <= 0) return;
	
	int finalHealthLevel = FCString::Atoi(*HealthStatLevel->GetText().ToString());
	int finalStaminaLevel = FCString::Atoi(*StaminaStatLevel->GetText().ToString());
	int finalStrengthLevel = FCString::Atoi(*StrengthStatLevel->GetText().ToString());
	int finalIntellectLevel = FCString::Atoi(*IntellectStatLevel->GetText().ToString());


	if(PCS)
	{

		if(spentHealthPoints > 0)
		{
			PCS->playerStats.HealthLevel = finalHealthLevel;
			PCS->playerStats.totalHealth += PCS->playerStats.AddToStat(100,PCS->playerStats.HealthLevel);

			if(Player)
			{
				if(UMainPlayerWidget*MPW = Player->GetMainPlayerWidget().Get())
				{
					MPW->UpdateHealthSizeBox();
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("New Health %f"), PCS->playerStats.totalHealth);
		}
		if(spentStaminaPoints > 0)
		{
			PCS->playerStats.Stamina = finalStaminaLevel;
			PCS->playerStats.totalStamina += PCS->playerStats.AddToStat(75, PCS->playerStats.Stamina);


			if (Player)
			{
				if (UMainPlayerWidget* MPW = Player->GetMainPlayerWidget().Get())
				{
					MPW->UpdateStaminaSizeBox();
				}
			}
		}
		if(spentStrengthPoints > 0)
		{
			PCS->playerStats.Strength = finalStrengthLevel;
			PCS->playerStats.totalStrength += PCS->playerStats.AddToStat(30, PCS->playerStats.Strength);
		}
		if(spentIntellectPoints > 0)
		{
			PCS->playerStats.Intellect = finalIntellectLevel;
			PCS->playerStats.totalIntellect += PCS->playerStats.AddToStat(30, PCS->playerStats.Intellect);

		}
	}

	if(PlayerHUD)
	{
		PlayerHUD->TalentTreeWidget->CheckAllRows();
	}

	if(Player)
	{
		Player->PlayLevelUpWidgetNiagara();
	}

	spentHealthPoints = 0;
	spentStaminaPoints = 0;
	spentStrengthPoints = 0;
	spentIntellectPoints = 0;
}

void ULevelUpWidget::updateLevelUp(const int& newLevel)
{
	if(LevelText)
	{
		LevelText->SetText(FText::AsNumber(newLevel));

	}
	
}

void ULevelUpWidget::RemoveRecentFromStatTrack(EPlayerStatType PlayerStatType)
{
	if (!PCS) return;

	if(PCS->playerStats.StatTracker.Num() > 0)
	{
		for(int i = PCS->playerStats.StatTracker.Num() - 1; i >= 0; --i)
		{
			if(PCS->playerStats.StatTracker[i] == PlayerStatType)
			{
				PCS->playerStats.StatTracker.RemoveAt(i);
				break;
			}
		}
	}
}
