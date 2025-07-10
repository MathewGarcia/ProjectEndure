#pragma once
#include "CoreMinimal.h"
#include "ElementTypes.h"
#include "GameStats.generated.h"

class UMainPlayerWidget;
class AInGamePlayerHUD;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelChanged, const int&, level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStatLevelChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelPointsUpdated, const int& , NewValue);

UENUM(Blueprintable)
enum class EPlayerStatType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Stamina UMETA(DisplayName = "Stamina"),
	Intellect UMETA(DisplayName = "Intellect"),
	Strength UMETA(DisplayName = "Strength"),
	Life UMETA(DisplayName = "Life")
};

USTRUCT(Blueprintable)
struct DUNGEONCRAWLER_API FGameStats
{
	GENERATED_BODY()

	FGameStats() : Stamina(10), Strength(10), Intellect(10), HealthLevel(10),ManaLevel(10) {};

	FGameStats(int newStam, int newStrength, int newIntellect,int health,int mana) : Stamina(newStam), Strength(newStrength), Intellect(newIntellect), HealthLevel(health),ManaLevel(mana){};

	//LEVELS i.e, 10 strength
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player Stats")
	int Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int Intellect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player Stats")
	int HealthLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int ManaLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player Stats")
	int32 softCap = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaRateAmount = 5.f;

	UPROPERTY()
	FOnPlayerLevelChanged OnPlayerLevelChanged;

	UPROPERTY()
	int32 playerLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 maxPlayerLevel = 60;

	UPROPERTY(EditAnywhere, Category = "Player Resistances")
	TMap<EElementTypes, float> Resistances;

	UPROPERTY(EditAnywhere,Category= "Water Resistance")
	float WaterResistance = 0.0; //slows enemy, if hit with fire the enemy will steam. Burning them causing a dot.

	UPROPERTY(EditAnywhere, Category = "Fire Resistance")
	float FireResistance = 0.0; //Burn debuff is added causing the player to become weaker to physical attacks.

	UPROPERTY(EditAnywhere, Category = "Lightening Resistance")
	float LighteningResistance = 0.0; //cause a lightening strike to come down and stun the player.

	UPROPERTY(EditAnywhere, Category = "Shadow Resistance")
	float ShadowResistance = 0.0; //Shadow hand appear attempting to strike the enemy.

	UPROPERTY(EditAnywhere, Category = "Bleed Resistance")
	float BleedResistance = 0.0; //Bleed causing 2x damage.

	UPROPERTY(EditAnywhere, Category = "Poison Resistance")
	float PoisonResistance = 0.0; //Damage over time is caused.

	float PhysicalResistance = 0.0f; //regular damage.

	UPROPERTY()
	int32 levelPoints = 0;

	//TOTAL STATS

	UPROPERTY()
	float currentHealth;

	UPROPERTY()
	float totalHealth = 100;

	UPROPERTY()
	float currentStamina;

	UPROPERTY()
	float totalStamina = 100;

	UPROPERTY()
	float currentMana;

	UPROPERTY()
	float totalMana = 100;

	UPROPERTY()
	float totalStrength = 0;

	UPROPERTY()
	float totalIntellect = 0;

	//TODO eventually factor poise in. 
	UPROPERTY()
	float totalPoise = 100.f;

	UPROPERTY()
	float currentPoise = 0.f;

	UPROPERTY()
	float currentWaterProgression = 0.f;


	UPROPERTY()
	float currentFireProgression = 0.f;


	UPROPERTY()
	float currentLighteningProgression = 0.f;

	UPROPERTY()
	float currentShadowProgression = 0.f;

	UPROPERTY()
	float currentBleedResistance = 0.f;

	UPROPERTY()
	float currentPoisonResistance = 0.f;



	UPROPERTY()
	float totalEXPGained = 0.f;

	UPROPERTY(Transient)
	APlayerController* Owner;

	UPROPERTY()
	TMap<EElementTypes, float> ElementProgression;

	EElementTypes IncreaseElementProgression(EElementTypes ElementType, float val);

	UPROPERTY()
	FOnPlayerStatLevelChanged OnPlayerStatLevelChanged;

	//[type,type,type..]
	UPROPERTY()
	TArray<EPlayerStatType> StatTracker;

	UPROPERTY()
	FOnLevelPointsUpdated OnLevelPointsUpdated;

	void UpdateLevelPoints(int Amount)
	{
		levelPoints += Amount;
		OnLevelPointsUpdated.Broadcast(levelPoints);
	}

	void StartDecreasingElementalProgression();
	float getExp()
	{
		return currentExp;
	}

	void initElementProgressionMap()
	{
		ElementProgression.Add(EElementTypes::Water, currentWaterProgression);
		ElementProgression.Add(EElementTypes::Fire, currentFireProgression);
		ElementProgression.Add(EElementTypes::Lightening, currentLighteningProgression);
		ElementProgression.Add(EElementTypes::Shadow, currentShadowProgression);
		ElementProgression.Add(EElementTypes::Bleed, currentBleedResistance); //progression
		ElementProgression.Add(EElementTypes::Poison, currentPoisonResistance); //progression
	}

	void CalculateAllStats()
	{
		totalStamina += CalculateTotalStat(75, Stamina, 0.1);
		totalHealth += CalculateTotalStat(100, HealthLevel, 0.1);
		totalMana += CalculateTotalStat(50, ManaLevel, 0.1);
		totalStrength += CalculateTotalStat(30, Strength, 0.1);
		totalIntellect += CalculateTotalStat(30, Intellect, 0.1);

		UE_LOG(LogTemp, Warning, TEXT("STAT: STAMINA , %f"), totalStamina);
		UE_LOG(LogTemp, Warning, TEXT("STAT: HEALTH , %f"), totalHealth);
		UE_LOG(LogTemp, Warning, TEXT("STAT: MANA , %f"), totalMana);
		UE_LOG(LogTemp, Warning, TEXT("STAT: STRENGTH , %f"), totalStrength);
		UE_LOG(LogTemp, Warning, TEXT("STAT: INTELLECT , %f"), totalIntellect);


	}

	float CalculateTotalStat(int totalPerPoint, int currentStatLevel, float reductionFactor)
	{
		float total = 0.0f;
		for(int i = 1; i <= currentStatLevel; ++i)
		{
			total+= totalPerPoint / (1 + (currentStatLevel * reductionFactor));
		}

		return total;

	}

	float AddToStat(int totalPerPoint,int currentStat)
	{
		return totalPerPoint / (1 + (currentStat * 0.1));
	}

	void RemovePointFromStat(EPlayerStatType StatType)
	{
		switch (StatType)
		{
		case EPlayerStatType::Health:
			--HealthLevel;
			totalHealth = CalculateTotalStat(100, HealthLevel, 0.1);
			break;
		case EPlayerStatType::Stamina:
			--Stamina;
			totalStamina = CalculateTotalStat(75, Stamina, 0.1);
			break;
		case EPlayerStatType::Intellect:
			--Intellect;
			totalIntellect = CalculateTotalStat(30, Intellect, 0.1);
			break;
		case EPlayerStatType::Strength:
			--Strength;
			totalStrength = CalculateTotalStat(30, Strength, 0.1);
			break;
		}
		OnPlayerStatLevelChanged.Broadcast();
	}

	void levelUp()
	{

		++playerLevel;
		UpdateLevelPoints(1);
		OnPlayerLevelChanged.Broadcast(playerLevel);
		//next level is 90 lets say. we have 2 more exp.
		//owed exp = nextLevelExp - currentExp
		//owedexp = 30 - 90
		float owedExp =  currentExp - nextLevelExp;
		//20%
		nextLevelExp *= 2.5;
	
		currentExp = 0;

		if(owedExp > 0)
		{
			UpdateEXP(owedExp);
		}
		UE_LOG(LogTemp, Warning, TEXT("PLAYER LEVEL UP , %d"), playerLevel);

	}

	//exp is essentially the exp we have gained or lost.
	void UpdateEXP(float exp)
	{
		//current exp is 1500, the loss is -1600.
		//1500 - 1600
		//-100
		currentExp += exp;
		totalEXPGained += exp;
		//currentExp = FMath::Clamp(currentExp, -1.0f, nextLevelExp);
		if(exp <= 0)
		{
			//100 owed exp
			float owedExp = currentExp*-1;
			if(owedExp > 0.f)
			{
				DeLevel(owedExp);
			}
		}
		//now lets do the level up.
		//current exp is 0, the gain is 90
		//exp needed to level is 30
		//current exp >= nextlevelexp. we level
		if(currentExp >= nextLevelExp && playerLevel < maxPlayerLevel)
		{
		
			levelUp();
		}

	}


	//100 passed in
	void DeLevel(float owedExp)
	{
		/* go down a level.
	 *
	 * if we are less than 0 exp, we should drop back down.
	 *
	 *the function to do that is 1.2% so we simply just subtract that to get exp needed.
	 * nextLevelExp-= nextLevelExp*0.2;
	 *
	 * i have to get the cost of the weapon expNeeded then subtract.
	 *
	 * if exp is negative then we are subtracting, so we can just send it to delevel.
	 *
	 *owedExp = costExp - currentExp;
	 *
	 * so currentExp = nextLevelExp - owedExp
	 *
	 * if we owe more exp, then keep subtracting, this will introduce a new variable of float owedExp.
	 *
	 *currentExp -= owedExp.
	 *
	 *while owedExp <= 0?
	 */
		if(playerLevel == 0 || owedExp <= 0)
		{
			return;
		}

		if (StatTracker.Num() > 0) {
			EPlayerStatType LastStatChanged = StatTracker.Pop();
			RemovePointFromStat(LastStatChanged);
		}
		--playerLevel;
		OnPlayerLevelChanged.Broadcast(playerLevel);
		//say the next level is now 1300
		nextLevelExp -= nextLevelExp * 0.2;
		// 1300 - 100 = 1200
		currentExp = nextLevelExp - owedExp;
		//owed exp is 100 - (the difference of the max level exp and current exp) i.e, 1300 - 1200 = 100. owed exp is now 0.
		owedExp = owedExp - (nextLevelExp - currentExp);
		currentExp = FMath::Clamp(currentExp, -1.0f, nextLevelExp);

		if(currentExp < 0.f)
		{
			UpdateEXP(owedExp);
		}
	}

	void IncreaseStamina()
	{
		currentStamina += StaminaRateAmount;
	}

	void InitResistances() {

		Resistances.Add(EElementTypes::Water, WaterResistance);
		Resistances.Add(EElementTypes::Bleed,BleedResistance);
		Resistances.Add(EElementTypes::Fire,FireResistance);
		Resistances.Add(EElementTypes::Shadow,ShadowResistance);
		Resistances.Add(EElementTypes::Lightening,LighteningResistance);
		Resistances.Add(EElementTypes::Poison,PoisonResistance);
		Resistances.Add(EElementTypes::Physical, PhysicalResistance);


	}

	float GetNextLevelEXP()
	{
		return nextLevelExp;
	}

	float GetEXP()
	{
		return currentExp;
	}
private:
	float nextLevelExp = 1000;
	float currentExp;
	FTimerHandle FResetTimerHandle;
	FTimerHandle FResetResistancesTimerHandle;


};