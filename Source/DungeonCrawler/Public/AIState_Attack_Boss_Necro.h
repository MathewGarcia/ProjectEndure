#pragma once
#include "AIState_Attack.h"
class DUNGEONCRAWLER_API AIState_Attack_Boss_Necro : public AIState_Attack
{
public:

	virtual TUniquePtr<AIDecisionTreeNode> BuildInRange(AEnemy* Enemy) override;
	virtual TUniquePtr<AIDecisionTreeNode> BuildNotInRange(AEnemy* Enemy) override;
	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;

	virtual void TickState(float DeltaTime) override;
};

