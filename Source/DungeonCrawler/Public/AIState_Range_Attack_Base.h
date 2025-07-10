#pragma once
#include "AIState_Attack.h"
class DUNGEONCRAWLER_API AIState_Range_Attack_Base : public AIState_Attack
{

	virtual void TickState(float DeltaTime) override;
	
	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;
};

