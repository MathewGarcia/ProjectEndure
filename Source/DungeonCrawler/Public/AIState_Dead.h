// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStateBase.h"

/**
 * 
 */
class DUNGEONCRAWLER_API AIState_Dead : public AIStateBase
{
public:
	AIState_Dead();

	virtual void TickState(float DeltaTime) override;

	virtual void OnEnterState() override;

	virtual void OnExitState() override;

	virtual EAIStates GetState() override;

};
