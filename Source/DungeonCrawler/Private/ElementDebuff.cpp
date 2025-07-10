// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementDebuff.h"

#include "PlayerCharacter.h"

void UElementDebuff::EndBuff()
{
	GetWorld()->GetTimerManager().ClearTimer(FDebuffTimerHandle);
	
}
