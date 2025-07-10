// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest_Slay_Enemies.h"

#include "Enemy.h"
#include "QuestDataAsset_Slay_Enemies.h"

void UQuest_Slay_Enemies::StartQuest_Implementation()
{
	Super::StartQuest_Implementation();

	AEnemy::OnEnemyDeath.AddUObject(this,&UQuest_Slay_Enemies::OnEnemyKilled);

}


void UQuest_Slay_Enemies::OnEnemyKilled(AEnemy* Enemy)
{
	if (!QuestDataAsset) return;


	if (UQuestDataAsset_Slay_Enemies* DA_Quest = Cast<UQuestDataAsset_Slay_Enemies>(QuestDataAsset))
	{
		if (Enemy->GetClass() == DA_Quest->EnemyClass) {
			++Progress;
			if (Progress >= DA_Quest->amountToKill)
			{
				CompleteQuest();
				AEnemy::OnEnemyDeath.RemoveAll(this);
			}
		}
	}
}
