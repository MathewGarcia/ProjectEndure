#pragma once
#include "AIState_Attack.h"
class DUNGEONCRAWLER_API AIState_Attack_Mostrosity : public AIState_Attack
{
	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;
	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;
	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;

};

