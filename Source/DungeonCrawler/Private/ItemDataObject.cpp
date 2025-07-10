// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataObject.h"

void UItemDataObject::SetQuantity(int32 NewQuantity)
{
	if(Quantity != NewQuantity)
	{
		Quantity = NewQuantity;
		OnItemDataChanged.Broadcast();
	}
}
