// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM()
enum class EHitDirection : uint8
{
	FRONT UMETA(DisplayName = "FRONT"),
	BACK UMETA(DisplayName = "BACK"),
	LEFT UMETA(DisplayName = "LEFT"),
	RIGHT UMETA(DisplayName = "RIGHT"),
};