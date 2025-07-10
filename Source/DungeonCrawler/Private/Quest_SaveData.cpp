#include "Quest_SaveData.h"
#include "Quest.h"
#include "PlayerCharacterState.h"


void FQuest_SaveData::Init(APlayerCharacterState*PCS) {
	if (!PCS) return;

	if (QuestToSpawn) {
		if(UQuest*QuestSpawned = NewObject<UQuest>(PCS,QuestToSpawn))
		{
			QuestSpawned->StartQuest_Implementation();
			QuestSpawned->DeserializeQuest(this);
		}
	}
}
