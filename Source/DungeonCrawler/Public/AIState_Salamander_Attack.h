#pragma once
#include "CoreMinimal.h"
#include "AIState_Attack.h"


/**
 *
 */
class DUNGEONCRAWLER_API AIState_Salamander_Attack : public AIState_Attack
{
	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;

};



