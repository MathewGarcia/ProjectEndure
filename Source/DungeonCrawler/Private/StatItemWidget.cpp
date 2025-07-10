// Fill out your copyright notice in the Description page of Project Settings.


#include "StatItemWidget.h"

#include "GameStatsObject.h"
#include "Components/TextBlock.h"

void UStatItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if(UGameStatsObject*GameStats = Cast<UGameStatsObject>(ListItemObject))
	{
		if(DecreaseButton)

			if(StatText)
			{
			
			}
	}

}

void UStatItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
