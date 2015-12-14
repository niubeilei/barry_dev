////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ASCIICharPtree.cpp
// Description:
//	Each node in a Ptree may have m branches, each can be either indexed
//  by a char or   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/ASCIICharPtree.h"

#include <KernelSimu/string.h>
#include "alarm_c/alarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Slab.h"
#include "aosUtil/Memory.h"
#include "Alarm/Alarm.h"
#include "aosUtil/ReturnCode.h"
#include "util_c/modules.h"

static struct AosSlab *sgSlab = 0;
#define AOS_MAX_ASCIICHARPtree_SLAB_ENTRY 10000

static struct aosASCIICharPtreeNode * aosASCIICharPtree_createNode(void)
{
	int i;
	struct aosASCIICharPtreeNode *node;
 
	if (!sgSlab)
	{
		aos_alarm("ASCIICharPtree not created");
		return 0;
	}

	node = (struct aosASCIICharPtreeNode *)AosSlab_get(sgSlab);
	if (!node)
	{
		aos_alarm("Failed to get a memory from the slab");
		return 0;
	}

	node->mIsLeaf = 0;
	node->mData = 0;
	node->mNumValidBranches = 0;
	node->mBranchChar = 0;
	node->mParent = 0;
	for (i=0; i<eAosCharTreeBranchSize; i++)
	{
		node->mBranches[i] = 0;
	}
	return node;
}


struct aosASCIICharPtree * aosASCIICharPtree_create()
{
	struct aosASCIICharPtree *tree = 
		(struct aosASCIICharPtree*)aos_malloc(sizeof(struct aosASCIICharPtree));
	if (!tree)
	{
		aos_alarm("tree is null");
		return 0;
	}

	if (!sgSlab)
	{
		AosSlab_constructor("ASCIICharPtree", 
			sizeof(struct aosASCIICharPtreeNode), 
			AOS_MAX_ASCIICHARPtree_SLAB_ENTRY, &sgSlab);
		if (!sgSlab)
		{
			aos_alarm("slab is null");
			return 0;
		}
	}

	aosASCIICharPtree_init(tree);
	return tree;
}


int aosASCIICharPtree_init(struct aosASCIICharPtree *tree)
{
	struct aosASCIICharPtreeNode *node = aosASCIICharPtree_createNode();

	if (!node)
	{
		aos_alarm("Failed to create the root node. "
			"Seems that the tree was not created yet");
		return -1;
	}

	tree->mRoot = node;
    tree->mMinChar = eAosCharTreeMinChar;
    tree->mMaxChar = eAosCharTreeMaxChar;
	
	if (!node)
	{
		return aos_alarm("node is null");
	}
	return 0;
}


//
// If 'key' or its prefix matches a node, it returns the node and 
// foundIndex is the index of 'key' right after the last matched
// character. If not found, it returns 0 and foundIndex is the
// index of key right after the last checked character.
//
int aosASCIICharPtree_get(struct aosASCIICharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **nodeFound)
{
	int len = strlen(key);
	unsigned char c;
	struct aosASCIICharPtreeNode *node = tree->mRoot;
	int index;

	if (len <= 0)
	{
		return -eAosAlarmEmptyKey;
	}

	*indexFound = -1;
	*nodeFound = 0;
	if (!tree->mRoot)
	{
		return aos_alarm("root is null");
	}

	index = 0;	
	while (node && index < len)
	{

		c = (unsigned char)key[index++];

		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			*indexFound = index-2;
			return aos_alarm(key);
		}

		if (node->mBranches)
		{
			if ((node = node->mBranches[c-tree->mMinChar]) == 0)
			{
//				*indexFound = index-2;
				break;
			}

			if (node->mData)
			{
				*indexFound = index-1;
				*nodeFound = node->mData;
			}
		}
		else
		{
			node = 0;
		}
	}

	if (index >= len && *nodeFound)
	{
		// 
		// Found
		//
		return 0;
	}
	return eAosRc_NotFound;
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
int aosASCIICharPtree_insert(struct aosASCIICharPtree *tree, 
						const char *key, 
						const int len, 
						void *data, 
						const int replace)
{
	int index = 0;
	struct aosASCIICharPtreeNode *node = tree->mRoot;
	struct aosASCIICharPtreeNode *prevNode = 0;
	unsigned char c;
	int i;

	if (len <= 0)
	{
		return eAosAlarmEmptyKey;
	}

	// 
	// First, check whether the key is valid
	//
	for (index = 0; index<len; index++)
	{
		c = (unsigned char)key[index];
		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			aos_alarm("Character out of bound: %u, %u, %u, %s", 
				index, c, tree->mMaxChar, key);
			return -1;
		}
	}

	index = 0;
	while (node && index < len)
	{

		prevNode = node;
		c = key[index++];
		node = node->mBranches[c-tree->mMinChar];
	}

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
			return aos_alarm("program error");
		}

		tree->mRoot = aosASCIICharPtree_createNode();
		if (!tree->mRoot)
		{
			// 
			// Still no memory. Return error.
			//
			return aos_alarm("memory error");
		}
		prevNode = tree->mRoot;
	}

	if (index >= len && node)
	{
		// 
		// This means there is already a node that matches 'key'. 
		// If the node is not already a leaf yet, make it a leaf.
		// If it is a leaf, check whether we should replace it.
		//
		if (node->mIsLeaf)
		{
			if (replace)
			{
				node->mData = data;
				return 0;
			}

			return aos_alarm("Alarm");
		}

		// 
		// It is not a leaf yet. Make it a leaf
		//
		node->mIsLeaf = 1;
		node->mData = data;
		return 0;
	}

	index--;
	for (i=index; i<len; i++)
	{
		c = key[i] - tree->mMinChar;
		prevNode->mBranches[c] = aosASCIICharPtree_createNode();
		if (!prevNode->mBranches[c])
		{
			// 
			// Run out of memory. Need to remove all the nodes created. 
			// 
			aos_alarm("Branch null");
			return aos_alarm("Alarm");
		}
		prevNode->mNumValidBranches++;
		prevNode->mBranches[c]->mParent = prevNode;
		prevNode->mBranches[c]->mBranchChar = c;
		prevNode = prevNode->mBranches[c];
	}

	prevNode->mIsLeaf = 1;
	prevNode->mData = data;
	return 0;
}


int aosASCIICharPtree_remove(struct aosASCIICharPtree *tree, 
						const char *key, 
						void **entry)
{
	struct aosASCIICharPtreeNode *node = tree->mRoot;
	struct aosASCIICharPtreeNode *prevNode = 0;
	int len = strlen(key);
	int index = 0;
	char c;

	if (len <= 0)
	{
		return eAosAlarmEmptyKey;
	}

	if(entry)
		*entry = 0;

	while (node && index < len)
	{
		c = key[index++];
		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			return aos_alarm("Alarm");
		}

		prevNode = node;
		node = node->mBranches[c-tree->mMinChar];
	}

	if (index < len)
	{
		// 
		// It didn't find the node
		//
		return 0;
	}

	if (!prevNode)
	{
		// 
		// This should never happen
		//
		return aos_alarm("Node null");
	}

	if (!node || !node->mIsLeaf)
	{
		// 
		// If it consumed all 'key' but the matched node is not a leaf, 
		// it means 'key' does not identify a leaf. Nothing can be 
		// deleted. 
		// 
		return 0;
	}

	if(entry) 
		*entry = node->mData;
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
		if (!prevNode)
		{
			if (node == tree->mRoot)
			{
				return 0;
			}

			aos_alarm("root null");
			break;
		}

		if (prevNode->mNumValidBranches == 0)
		{
			// 
			// This should never happen.
			//
			return aos_alarm("Alarm");
		}

		if (prevNode->mBranches[(unsigned char)node->mBranchChar] != node)
		{
			return aos_alarm("Fatal");
		}
		prevNode->mBranches[(unsigned char)node->mBranchChar] = 0;
		prevNode->mNumValidBranches--;

		// 
		// We shall never delete root.
		//
		if (node == tree->mRoot)
		{
			return 0;
		}

		AosSlab_release(sgSlab, node);
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
	return aos_alarm("Alarm");
}


