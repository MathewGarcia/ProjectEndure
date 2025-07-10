#pragma once
#include "CoreMinimal.h"
#include "Ability.h"
#include "ElementTypes.h"
#include "Projectile.h"
#include "EnemyParameters.generated.h"

class AItem;
class AEnemy;
class UBuff;
USTRUCT()
struct FEnemyParameters 
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Category = "Params")
	bool bCanBuff = false;

	UPROPERTY(EditAnywhere, Category = "Params")
	float BuffRadius = 1000.f;

	UPROPERTY(EditAnywhere,Category = "Buff")
	TSubclassOf<UBuff> BuffObject;

	UPROPERTY(EditAnywhere,Category = "Params")
	bool bHasLongRange = false;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileToSpawn;

	UPROPERTY(EditAnywhere,Category = "Enemy Abilities")
	TArray<TSubclassOf<UAbility>> AbilityClasses;

	UPROPERTY()
	TArray<UAbility*> OffensiveAbilities;

	UPROPERTY()
	TArray<UAbility*> DefensiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Water Resistance")
	float WaterResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Fire Resistance")
	float FireResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Lightening Resistance")
	float LighteningResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Shadow Resistance")
	float ShadowResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Bleed Resistance")
	float BleedResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Poison Resistance")
	float PoisonResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Physical Resistance")
	float PhysicalResistance = 0.0;

	UPROPERTY(EditAnywhere,Category = "Params")
	float poiseDamage = 0.f;

	UPROPERTY(EditAnywhere,Category = "Params")
	float maxPoise = 100.f;

	UPROPERTY(EditAnywhere, Category = "Params")
	float poiseResistance = 0.f;

	UPROPERTY(EditAnywhere, Category = "Params")
	float ElementDamage = 0.f; //scalable

	UPROPERTY(EditAnywhere, Category = "Params")
	EElementTypes ElementType;

	UPROPERTY(EditAnywhere, Category = "Params")
	float ResistanceDamage = 0.f; //scalable

	UPROPERTY(EditAnywhere, Category = "Params")
	bool bIsBoss = false;

	float currentPoise;

	UPROPERTY(EditAnywhere, Category = "Items")
	TMap<TSubclassOf<AItem>,float> ItemDropMap;

	UPROPERTY(EditAnywhere, Category = "Params")
	float Exp = 50.f; //scalable


	static bool ShouldBuff(TArray<TWeakObjectPtr<AEnemy>>RecentEnemies, const TSubclassOf<UBuff>& Buff);

	UPROPERTY()
	TMap<EElementTypes, float> Resistances;


	float currentWaterProgression = 0.f;
	float currentFireProgression = 0.f;
	float currentLighteningProgression = 0.f;
	float currentShadowProgression = 0.f;
	float currentBleedResistance = 0.f;
	float currentPoisonResistance = 0.f;

	TMap<EElementTypes, float> ElementProgression;

	EElementTypes IncreaseElementProgression(EElementTypes CurrentElementType, float val);


	void initElementProgressionMap()
	{
		ElementProgression.Add(EElementTypes::Water, currentWaterProgression);
		ElementProgression.Add(EElementTypes::Fire, currentFireProgression);
		ElementProgression.Add(EElementTypes::Lightening, currentLighteningProgression);
		ElementProgression.Add(EElementTypes::Shadow, currentShadowProgression);
		ElementProgression.Add(EElementTypes::Bleed, currentBleedResistance);
		ElementProgression.Add(EElementTypes::Poison, currentPoisonResistance);
	}

	void InitResistances() {
		Resistances.Add(EElementTypes::Physical, PhysicalResistance);
		Resistances.Add(EElementTypes::Water, WaterResistance);
		Resistances.Add(EElementTypes::Bleed, BleedResistance);
		Resistances.Add(EElementTypes::Fire, FireResistance);
		Resistances.Add(EElementTypes::Shadow, ShadowResistance);
		Resistances.Add(EElementTypes::Lightening, LighteningResistance);
		Resistances.Add(EElementTypes::Poison, PoisonResistance);
	}

};

