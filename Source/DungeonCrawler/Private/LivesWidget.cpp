// Fill out your copyright notice in the Description page of Project Settings.


#include "LivesWidget.h"
#include "Components/TextBlock.h"


void ULivesWidget::UpdateLivesText(int newVal)
{
	if (!LivesText) return;
	LivesText->SetText(FText::FromString(FString::FromInt(newVal)));
}
