#pragma once
#include "CoreMinimal.h"
#include "Buff.h"
#include "ImpBuff.generated.h"

UCLASS()
class DUNGEONCRAWLER_API UImpBuff : public UBuff
{

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Damage")
	float IncreaseDamageBy;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float ExistTimer;

	FTimerHandle BuffTimer;

	void Execute() override;

	void BeginBuff(AEnemy* Enemy);

};

