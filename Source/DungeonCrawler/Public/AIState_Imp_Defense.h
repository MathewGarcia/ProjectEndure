#pragma once
#include "AIState_Defense.h"
class AIState_Imp_Defense : public AIState_Defense
{



	virtual EAIStates GetState() override;


	virtual TUniquePtr<AIDecisionTreeNode> BuildDecisionTree(AEnemy* Enemy) override;

	TUniquePtr<AIDecisionTreeNode> BuildBuffAllies(AEnemy* Enemy);
};

