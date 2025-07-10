// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(Blueprintable)
enum class EAIStates : uint8 {
	none UMETA(DisplayName = "none"),
	idle UMETA(DisplayName = "idle"),
	attack UMETA(DisplayName = "attack"),
	defense UMETA(DisplayName = "defense"),
	dead UMETA(DisplayName = "dead")
};
