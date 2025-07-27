// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementDebuff.h"

void UElementDebuff::EndBuff()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(FDebuffTimerHandle);
	}
}
