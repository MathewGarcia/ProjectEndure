#include "ImpBuff.h"
#include "Enemy.h"
void UImpBuff::Execute()
{
	UBuff::Execute();
}

void UImpBuff::BeginBuff(AEnemy* Enemy)
{
	if (!GetWorld() || !Enemy) return;

	Enemy->Damage += IncreaseDamageBy;
	UE_LOG(LogTemp, Error, TEXT("New Enemy Damage = %f"), Enemy->Damage);
	UE_LOG(LogTemp, Warning, TEXT("Buff Started"));
	GetWorld()->GetTimerManager().SetTimer(BuffTimer, [this, Enemy]()
		{
			Enemy->Damage -= IncreaseDamageBy;
		}, ExistTimer, false);
}
