#pragma once
#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "WaterDebuff.generated.h"
UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UWaterDebuff : public UElementDebuff
{
	GENERATED_BODY()

public:
	virtual void ExecuteDebuff_Implementation(AActor* Target,AActor*DamageCauser) override;

	virtual void EndBuff() override;
};

