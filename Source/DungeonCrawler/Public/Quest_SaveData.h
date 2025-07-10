#pragma once
#include "CoreMinimal.h"
#include "VariableVariant.h"
#include "Quest_SaveData.generated.h"


class APlayerCharacterState;
class UQuest;

USTRUCT()
struct DUNGEONCRAWLER_API FQuest_SaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UQuest> QuestToSpawn;

	UPROPERTY()
	FVariableVariant QuestMetaData;

	void Init(APlayerCharacterState*PCS);

};
