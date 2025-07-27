#include "EnemyParameters.h"
#include "Enemy.h"
#include "Buff.h"

bool FEnemyParameters::ShouldBuff(TArray<TWeakObjectPtr<AEnemy>>RecentEnemies, const TSubclassOf<UBuff>& Buff)
{

		for (TWeakObjectPtr<AEnemy> WeakEnemy: RecentEnemies)
		{
			if (!WeakEnemy.IsValid()) return false;

			if (AEnemy* Enemy = WeakEnemy.Get()) {

				for (UBuff* BuffInstance : Enemy->Buffs)
				{
					if (BuffInstance && BuffInstance->IsA(UBuff::StaticClass()))
					{
						return false;

					}
				}
			}
		}

		return true;
	
}

EElementTypes FEnemyParameters::IncreaseElementProgression(EElementTypes CurrentElementType, float val)
{
	if (CurrentElementType == EElementTypes::NONE) return EElementTypes::NONE;

	if (float* currentVal = ElementProgression.Find(CurrentElementType))
		{
		float resistance = 0.f;
		if (Resistances.Contains(CurrentElementType)) {
			resistance = Resistances[CurrentElementType];
		}
		*currentVal += val * (1.f - resistance);

		*currentVal = FMath::Clamp(*currentVal, 0.f, 1.f);

		UE_LOG(LogTemp, Error, TEXT("%s Enemy Progression : %f"), *UEnum::GetValueAsString(CurrentElementType), *currentVal)

			if (*currentVal >= 1.f)
			{
				*currentVal = 0.f;
				return CurrentElementType;
			}
		}
		return EElementTypes::NONE;
}
