#pragma once
#include "AIState_Defense.h"
class DUNGEONCRAWLER_API AIState_Range_Defense : public AIState_Defense
{
	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;

	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;
};

