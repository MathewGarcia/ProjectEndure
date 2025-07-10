#pragma once

UENUM()
enum class EElementTypes : uint8
{
	Physical UMETA(DisplayName = "Physical"),
	Fire UMETA(DisplayName = "Fire"),
	Water UMETA(DisplayName = "Water"),
	Shadow UMETA(DisplayName = "Shadow"),
	Lightening UMETA(DisplayName = "Lightening"),
	Bleed UMETA(DisplayName = "Bleed"),
	Poison UMETA(DisplayName = "Poison"),
	NONE UMETA(DisplayName = "NONE")
};