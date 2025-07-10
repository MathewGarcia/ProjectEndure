#pragma once

#include "CoreMinimal.h"
#include "GearType.generated.h"

UENUM(Blueprintable)
enum class EGearType : uint8
{
	none UMETA(DisplayName = "none"),
	Head UMETA(DisplayName = "Head"),
	LHand UMETA(DisplayName = "LHand"),
	RHand UMETA(DisplayName = "RHand"),
	Chest UMETA(DisplayName = "Chest"),
	Legs UMETA(DisplayName = "Legs")
};