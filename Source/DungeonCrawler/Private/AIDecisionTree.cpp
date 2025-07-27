// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDecisionTree.h"

#include "AIDecisionTreeNode.h"
#include "Enemy.h"

void AIDecisionTree::Evaluate(TArray<float>& Features,AEnemy*Enemy)
{
    //feature indices: 0 is player distance, 1 is player health, 2 is enemy health, 3 is player stamina, 4 is player state, 5 is montage time, 6 is player facing away?, 7 
    if (!Enemy || !Root.IsValid()) return;
    AIDecisionTreeNode* CurrentNode = Root.Get();
    while(CurrentNode && CurrentNode->NodeType != ENodeType::Leaf)
    {
        if (!CurrentNode || !CurrentNode->LeftChild.IsValid() || !CurrentNode->RightChild.IsValid() || Features.Num() <= CurrentNode->featureIndex) break;
        float featureValue = Features[CurrentNode->featureIndex];
        float rawProbability = calculateRawProbability(1,CurrentNode,featureValue,Enemy->IntelligencePenalty);
        float entropy = calculateEntropy(rawProbability);
        float alpha = calculateAlpha(entropy,Enemy->IntelligencePenalty); 
        float probabilityFinal = calculateFinal(alpha, rawProbability);
        if(FMath::FRandRange(0.0f,1.0f) < probabilityFinal)
        {
            CurrentNode = CurrentNode->LeftChild.Get();
        }
        else
        {
            CurrentNode = CurrentNode->RightChild.Get();
        }
    }
    if(CurrentNode)
    {
        CurrentNode->execute();
    }
}

float AIDecisionTree::calculateRawProbability(float k, AIDecisionTreeNode* currentNode, float featureValue, float IntelligencePenalty)
{
    if (!currentNode) return 0.0f;
    // When IntelligencePenalty is 0, make the decision deterministic.
    if (FMath::IsNearlyZero(IntelligencePenalty))
    {
        return (featureValue < currentNode->SplitValue) ? 1.0f : 0.0f;
    }
    float adjustedK = FMath::Lerp(10.0f, k, IntelligencePenalty);
    float rawProb = 1.0f / (1.0f + FMath::Exp(-adjustedK * (currentNode->SplitValue - featureValue)));
    return FMath::Clamp(rawProb, 0.0f, 1.0f);
}

float AIDecisionTree::calculateEntropy(float p)
{
    float clampedP = FMath::Clamp(p, 0.0001f, 0.9999f);
    return -(clampedP * log(clampedP) + (1 - clampedP) * log(1 - clampedP));
}

float AIDecisionTree::calculateAlpha(float entropy, float IntelligencePenalty)
{
    float a = IntelligencePenalty * (entropy / maxEntropy);
    return FMath::Clamp(a, 0.0f, 1.0f);
}

float AIDecisionTree::calculateFinal(float alpha, float p)
{
    float fp = (1 - alpha)* p + alpha * 0.5;
    return FMath::Clamp(fp, 0.0f, 1.0f);
}


void AIDecisionTree::UpdateSplits(TArray<float>& Features, int32 Outcome)
{
    // No implementation yet
}
