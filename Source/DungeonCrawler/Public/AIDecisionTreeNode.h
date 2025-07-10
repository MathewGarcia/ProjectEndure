// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStates.h"

/**
 * 
 */

enum class ENodeType : uint8
{
	Leaf,
	Decision
};


class DUNGEONCRAWLER_API AIDecisionTreeNode
{
public:

	ENodeType NodeType;

	int32 featureIndex;

	float SplitValue;

	TUniquePtr<AIDecisionTreeNode> LeftChild;
	TUniquePtr<AIDecisionTreeNode> RightChild;

	TFunction<void()>ExecuteCallback;

	void execute()
	{
		if(ExecuteCallback)
		{
			ExecuteCallback();
		}
	}


	AIDecisionTreeNode() = default;
	~AIDecisionTreeNode() = default;
};

namespace DecisionTreeHelper
{
	inline TUniquePtr<AIDecisionTreeNode> MakeDecisionNode(int32 FeatureIndex, float SplitValue)
	{
		TUniquePtr<AIDecisionTreeNode> Node = MakeUnique<AIDecisionTreeNode>();
		Node->featureIndex = FeatureIndex;
		Node->SplitValue = SplitValue;
		Node->NodeType = ENodeType::Decision;
		return Node;
	}

	inline TUniquePtr<AIDecisionTreeNode> MakeLeafNode(TFunction<void()> ExecuteCallback)
	{
		TUniquePtr<AIDecisionTreeNode> Node = MakeUnique<AIDecisionTreeNode>();
		Node->NodeType = ENodeType::Leaf;
		Node->ExecuteCallback = ExecuteCallback;
		return Node;
	}


}