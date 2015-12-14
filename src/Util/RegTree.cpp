////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RegTree.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/RegTree.h"

#include <string.h>
#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Memory.h"
#include "Util/OmnNew.h"


static struct aosRegTreeNode * aosRegTree_createNode()
{
	int i;
	struct aosRegTreeNode *node = 
		(struct aosRegTreeNode *)aos_malloc(sizeof(struct aosRegTreeNode));
	if (!node)
	{
		return 0;
	}

	node->mIsLeaf = 0;
	node->mData = 0;
	node->mNumValidBranches = 0;
	node->mParent = 0;
	for (i=0; i<eAosCharTreeBranchSize; i++)
	{
		node->mBranches[i] = 0;
	}
	return node;
}


struct aosRegTree *aosRegTree_create()
{
	struct aosRegTree *tree = 
		(struct aosRegTree*)aos_malloc(sizeof(struct aosRegTree));
	if (!tree)
	{
		aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	aosRegTree_init(tree);
	return tree;
}


int aosRegTree_init(struct aosRegTree *tree)
{
	struct aosRegTreeNode *node = aosRegTree_createNode();

	tree->mRoot = node;
	tree->mMinChar = eAosMinChar;
	tree->mMaxChar = eAosMaxChar;
	
	if (!node)
	{
		return aosAlarm(eAosAlarmMemErr);
	}
	return 0;
}


//
// If 'key' or its prefix matches a node, it returns the node and 
// foundIndex is the index of 'key' right after the last matched
// character. If not found, it returns 0 and foundIndex is the
// index of key right after the last checked character.
//
int aosRegTree_get(struct aosRegTree *tree, 
				   const char *key, 
				   int *foundIndex, 
				   void **foundNode)
{
	int len = strlen(key);
	char c;
	struct aosRegTreeNode *node = tree->mRoot;
	int index;
	void *dataFound = 0;
	
	*foundIndex = 0;
	if (!tree->mRoot)
	{
		return 0;
	}

	index = 0;	
	while (node && index < len)
	{
		if (node->mIsLeaf)
		{
			*foundIndex = index;
			*foundNode = node->mData;
			return 0;
		}

		c = key[index++];
		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			*foundIndex = index;
			return 0;
		}

		if (node->mBranches)
		{
			node = node->mBranches[c-tree->mMinChar];
		}
		else
		{
			node = 0;
		}
	}

	if (!dataFound)
	{
		*foundIndex = index;
	}
	// return dataFound;
	return 0;
}


// 
// It inserts 'data' into the tree. This tree allows longest match. 
// When inserting, if it encounters a leaf but it has not consumed
// all its length yet, it will continue branch until to its length.
// 
// Errors:
// 1. If 'key' contains invalid characters, eAosAlarmInvalidChar is 
//    returned and 'data' is not inserted. 
// 2. If it failed to allocate memory, eAosAlarmMemErr is returned.
// 3. If 'replace' is 0 and the entry already exists in the tree, 
//    it returns eAosAlarmEntryAlreadyExist.
//
int aosRegTree_insert(struct aosRegTree *tree, 
						const char *key, 
						const int len, 
						void *data, 
						const int replace)
{
	int strLen = strlen(key);
	int index = 0;
	struct aosRegTreeNode *node = tree->mRoot;
	struct aosRegTreeNode *prevNode = 0;
	unsigned char c;
	int i;

	// 
	// First, check whether the key is valid
	//
	for (index = 0; index<strLen; index++)
	{
		if (key[index] < tree->mMinChar || key[index] > tree->mMaxChar)
		{
			return aosAlarm4(eAosAlarmInvalidChar, index, key[index], key, 0);
		}
	}

	index = 0;
	while (node && index < strLen)
	{
		prevNode = node;
		c = (unsigned char)key[index++];
		node = node->mBranches[c-tree->mMinChar];
	}

	index--;
	if (!prevNode)
	{
		// 
		// This should never happen (unless the system ran out of memory 
		// at the time of creating the tree). We will give it another try
		// here. 
		//
		if (tree->mRoot)
		{
			// 
			// This is a program error. Should NEVER happen.
			//
			return aosAlarm(eAosAlarmProgErr);
		}

		tree->mRoot = aosRegTree_createNode();
		if (!tree->mRoot)
		{
			// 
			// Still no memory. Return error.
			//
			return aosAlarm(eAosAlarmMemErr);
		}
		prevNode = tree->mRoot;
	}

	if (index >= strLen)
	{
		// 
		// This means there is already a node that matches 'key'. 
		// If the node is not already a leaf yet, make it a leaf.
		// If it is a leaf, check whether we should replace it.
		//
		if (prevNode->mIsLeaf)
		{
			if (replace)
			{
				prevNode->mData = data;
				return 0;
			}

			return aosAlarm4(eAosAlarmEntryAlreadyExist, (unsigned long)prevNode, 
				(unsigned long)prevNode->mData, 0, 0);
		}

		// 
		// It is not a leaf yet. Make it a leaf
		//
		prevNode->mIsLeaf = 1;
		prevNode->mData = data;
		return 0;
	}

	for (i=index; i<strLen; i++)
	{
		c = (unsigned char)((unsigned char)key[i] - (unsigned char)tree->mMinChar);
		prevNode->mBranches[c] = aosRegTree_createNode();
		if (!prevNode->mBranches[c])
		{
			// 
			// Run out of memory. Need to remove all the nodes created. 
			// 
			aosAlarm(eAosAlarmNotImplementedYet);
			return aosAlarm(eAosAlarmMemErr);
		}
		prevNode->mNumValidBranches++;
		prevNode->mBranches[c]->mParent = prevNode;
		prevNode = prevNode->mBranches[c];
	}

	prevNode->mIsLeaf = 1;
	prevNode->mData = data;
	return 0;
}


int aosRegTree_remove(struct aosRegTree *tree, const char *key)
{
	struct aosRegTreeNode *node = tree->mRoot;
	struct aosRegTreeNode *prevNode = 0;
	int len = strlen(key);
	int index = 0;
	unsigned char c;

	while (node && index < len)
	{
		c = (unsigned char)key[index++];
		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			return aosAlarm4(eAosAlarmInvalidChar, index-1, c, key, 0);
		}

		prevNode = node;
		node = node->mBranches[c];
	}

	if (index < len)
	{
		// 
		// It didn't find the node
		//
		return eAosRc_NodeNotFound;
	}

	if (!prevNode)
	{
		// 
		// This should never happen
		//
		return aosAlarm(eAosAlarmProgErr);
	}

	node = prevNode;
	if (!prevNode->mIsLeaf)
	{
		// 
		// If it consumed all 'key' but the matched node is not a leaf, 
		// it means 'key' does not identify a leaf. Nothing can be 
		// deleted. 
		// 
		return aosAlarm4(eAosAlarmMatchedIsNotLeaf, 0, 0, key, 0);
	}

	node->mIsLeaf = 0;
	node->mData = 0;
	if (node->mNumValidBranches != 0)
	{
		return 0;
	}

	// 
	// It is not a leaf, and no more branches. Time to delete this node.
	//
	prevNode = node->mParent;
	while (node)
	{
		// 
		// We shall never delete root.
		//
		if (node == tree->mRoot)
		{
			return 0;
		}

		aos_free(node);
		if (!prevNode)
		{
			break;
		}

		if (prevNode->mNumValidBranches == 0)
		{
			// 
			// This should never happen.
			//
			return aosAlarm(eAosAlarmBranchNumErr);
		}

		prevNode->mNumValidBranches--;
		if (!prevNode->mIsLeaf && prevNode->mNumValidBranches == 0)
		{
			node = prevNode;
			prevNode = node->mParent;
		}
		else
		{
			// 
			// That's it.
			//
			return 0;
		}
	}

	//
	// It should not come to this point.
	//
	return 0;
}
