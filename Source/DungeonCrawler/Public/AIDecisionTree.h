// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStates.h"

class AEnemy;
class AIDecisionTreeNode;
/**
 * 
 */
class DUNGEONCRAWLER_API AIDecisionTree
{
public:

	TUniquePtr<AIDecisionTreeNode> Root;


	void Evaluate(TArray<float>& Features,AEnemy*Enemy);

	float calculateRawProbability(float k, AIDecisionTreeNode* currentNode,float featureValue, float IntelligencePenalty);
	float calculateEntropy(float p);
	float calculateAlpha(float entropy,float IntelligencePenalty);
	float calculateFinal(float alpha, float p);
	void UpdateSplits(TArray<float>& Features, int32 Outcome);
	const float maxEntropy = log(2);
	AIDecisionTree() = default;
	~AIDecisionTree() = default;
};
