#pragma once
UENUM()
enum class EItemType : uint8
{
	Consumeable UMETA(DisplayName = "Consumeable"),
	Armor UMETA(DisplayName = "Armor"),
	Weapon UMETA(DisplayName = "Weapon"),
	Currency UMETA(DisplayName = "Currency")
};