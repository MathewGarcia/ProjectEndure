// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableTextWidget.h"

#include "Components/TextBlock.h"

void UInteractableTextWidget::UpdateInteractTextBlock(const FString& Text)
{
	if(InteractTextBlock)
	{
		InteractTextBlock->SetText(FText::FromString(Text));
	}
}
