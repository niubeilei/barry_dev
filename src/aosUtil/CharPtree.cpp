////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPtree.cpp
// Description:
//	Each node in a ptree may have m branches, each can be either indexed
//  by a char or   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "aosUtil/CharPtree.h"

#include <KernelSimu/string.h>
#include "alarm_c/alarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Slab.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "util_c/modules.h"
#include <stdio.h>

#ifdef __KERNEL__
#include <linux/module.h>
#endif

static struct AosSlab *sgSlab = 0;
#define AOS_MAX_CHARPTREE_SLAB_ENTRY 10101

static struct aosCharPtreeNode * aosCharPtree_createNode(void)
{
	int i;
	struct aosCharPtreeNode *node;
 
	if (!sgSlab)
	{
		aos_alarm("CharPtree not created");
		return 0;
	}

	node = (struct aosCharPtreeNode *)AosSlab_get(sgSlab);
	if (!node)
	{
		aos_alarm("Failed to get a memory from the slab");
		return 0;
	}

	node->mIsLeaf = 0;
	node->mData = 0;
	node->mTWC= 0;
	node->mNumValidBranches = 0;
	node->mBranchChar = 0;
	node->mParent = 0;
	node->mAddition = 0;
	for (i=0; i<eAosCharTreeBranchSize; i++)
	{
		node->mBranches[i] = 0;
	}
	return node;
}


struct aosCharPtree * aosCharPtree_create()
{
	struct aosCharPtree *tree = 
		(struct aosCharPtree*)aos_malloc(sizeof(struct aosCharPtree));
	if (!tree)
	{
		aos_alarm("tree is null");
		return 0;
	}

	if (!sgSlab)
	{
		AosSlab_constructor("CharPTree", 
			sizeof(struct aosCharPtreeNode), 
			AOS_MAX_CHARPTREE_SLAB_ENTRY, &sgSlab);
		if (!sgSlab)
		{
			aos_alarm("Slab is null");
			return 0;
		}
	}

	aosCharPtree_init(tree);
	return tree;
}


int aosCharPtree_init(struct aosCharPtree *tree)
{
	struct aosCharPtreeNode *node = aosCharPtree_createNode();

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

static int __aosCharPtree_print(struct aosCharPtree *tree, struct aosCharPtreeNode * node, int depth)
{
	int i;
	char tmp[1024]="";

	if (!node)
	{
		return 0;
	}

	for(i=0;i<depth-1;i++)
	{
//		printf("    ");
		strcat(tmp,"    ");
	}

//	printf("|-- ");
	strcat(tmp,"|-- ");

	if(node->mIsLeaf)
	{
		//printf("(");
		strcat(tmp,"(");
	}

//	printf("%c",node->mBranchChar+tree->mMinChar);
	sprintf( &tmp[strlen(tmp)], "%c",node->mBranchChar+tree->mMinChar);

	if(node->mAddition)
	{
//		printf("%s",node->mAddition);
		sprintf(&tmp[strlen(tmp)],"%s",node->mAddition);
	}

	if(node->mIsLeaf)
	{
//		printf("=%d)",(int)node->mData);
		sprintf( &tmp[strlen(tmp)],"=%ld)",(unsigned long)node->mData);
	}
	
//	printf(",%d\n",node->mNumValidBranches);
	sprintf( &tmp[strlen(tmp)], ",%d,%c",node->mNumValidBranches,node->mTWC?'*':' ');

	
#ifdef __KERNEL__
	printk(KERN_INFO "%s", tmp);
#else
	printf("%s\n",tmp);
#endif

	
	for(i=tree->mMinChar; i<=tree->mMaxChar; i++)
	{
		__aosCharPtree_print(tree,node->mBranches[i-tree->mMinChar],depth+1);
	}

	return 0;
}

int aosCharPtree_print(struct aosCharPtree *tree)
{
	int i;
	char tmp[128];


	sprintf(tmp,"ROOT,%d",tree->mRoot->mNumValidBranches);

#ifdef __KERNEL__
	printk(KERN_INFO "%s", tmp);
#else
	printf("%s\n",tmp);
#endif

	for(i=tree->mMinChar; i<=tree->mMaxChar; i++)
	{
		__aosCharPtree_print(tree,tree->mRoot->mBranches[i-tree->mMinChar],1);
	}
	
	return 0;
}



static int common_prefix_length(const char * s1, const char * s2)
{
	int i=0;

	while(s1[i]&&s2[i])
	{
		if(s1[i]!=s2[i])
			return i;
		i++;
	}
	return i;
}



static int is_prefix_of(const char * sub, const char * str)
{
	int i=0;

	while(sub[i]&&str[i])
	{
		if(sub[i]!=str[i])
			return 0;
		i++;
	}

	if(0==sub[i])
		return 1;
	else
		return -1;
}


enum{
	MATCH=0,
	NOT_MATCH,
	NOT_MATCH_ADD_BRANCHE,
	NOT_MATCH_SPLIT,
	NOT_MATCH_SPLIT_ADD
};


//
// Input Parameters:
// tree: 
// key:
//
// Output Parameters:
// index
// node
//
// Return Values:
// 0 exact match
//   node is the matched node
//   index is the length of the key
// 1 not match, but the matched node is not a leaf node
//   node is the matched node
//   index is the length of the key
// 2 not match
//   node is the node of the key, but the node's addition is not a prefix of the key
//   index is the position of the key, where to compare to addition
// 3 not match
//   node is parent node
//   index is the position of the key, where will be the index node->child[]
static int __aosCharPtree_get(struct aosCharPtree *tree, 
						const char *key, 
						int *index, 
						struct aosCharPtreeNode **node)
{
	int len = strlen(key);
	unsigned char c;

	*node = tree->mRoot;

	if (len <= 0)
	{
		return eAosAlarmEmptyKey;
	}

	if (!tree->mRoot)
	{
		return aos_alarm("root is null");
	}

	*index = 0;	
	while( *index<len && 0==(*node)->mTWC )
	{
		c=key[*index];
		if ((*node)->mBranches[c-tree->mMinChar])
		{
			*node=(*node)->mBranches[c-tree->mMinChar];
			(*index)++;

			int ret = is_prefix_of((*node)->mAddition,&key[*index]); 
			if(ret>0)
			{
				(*index) += strlen((*node)->mAddition);
				continue;
			}
			else if(ret==0)
			{
				// not prefix
				// yahoo.com
				// yahoo.inc
				return NOT_MATCH_SPLIT_ADD;
			}
			else
			{
				// yahoo.com, 
				// yahoo
				//(*index) += strlen((*node)->mAddition);
				return NOT_MATCH_SPLIT;
			}
		}
		else
		{
			// yahoo.com, yahoo.inc
			// yahoo.cn
			return NOT_MATCH_ADD_BRANCHE;
		}
	};

	if((*node)->mIsLeaf)
	{
		return MATCH;
	}
	else
	{
		//yahoo.com, yahoo.inc
		//yahoo.
		return NOT_MATCH;
	}
}

//
// If 'key' or its prefix matches a node, it returns the node and 
// foundIndex is the index of 'key' right after the last matched
// character. If not found, it returns 0 and foundIndex is the
// index of key right after the last checked character.
//
int aosCharPtree_get_pack(struct aosCharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **entry)
{
	struct aosCharPtreeNode * node;
	int ret;

	ret = __aosCharPtree_get(tree, key, indexFound, &node);

	if(ret==MATCH )
	{
		*entry = node->mData;
	}
	else if(ret==NOT_MATCH_ADD_BRANCHE && node->mTWC)
	{
		*entry = node->mData;
		ret = MATCH;
	}
	return ret;
}
int aosCharPtree_get_pack_recursion(struct aosCharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **entry)
{
	int i;
	int len;
	int ret;

	len=strlen(key);
	for(i=0;i<len;i++)
	{
		ret = aosCharPtree_get_pack(tree, &key[i], indexFound, entry);

		if(ret==MATCH )
		{
			return ret;
		}
	}

	return NOT_MATCH;
}


//
// If 'key' or its prefix matches a node, it returns the node and 
// foundIndex is the index of 'key' right after the last matched
// character. If not found, it returns 0 and foundIndex is the
// index of key right after the last checked character.
//
int aosCharPtree_get(struct aosCharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **nodeFound)
{
	int len = strlen(key);
	unsigned char c;
	struct aosCharPtreeNode *node = tree->mRoot;
	int index;

	if (len <= 0)
	{
		return eAosAlarmEmptyKey;
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

// Not match
// the node's whole string 
// Input parameters: 
// 		tree: the ptree
// 		data: the data we will insert to the ptree
// 		key: the key of the data
// 		pos: 
// 		node: the node's whole string is identical to key[0]..key[pos-1]
// 			the node->mBranches[key[pos]] must be zero
// 			the key[pos] must not be zero
/*
static int __aosCharPtree_insert_add_branche(
		struct aosCharPtree * tree,
		void * data,
		char * key,
		int pos,
		struct aosCharPtreeNode * node,
		char twc
		)

{
	unsigned char c;

	c = key[pos] - tree->mMinChar;
	node->mBranches[c] = aosCharPtree_createNode();
	if (!node->mBranches[c])
	{
				// 
				// Run out of memory. Need to remove all the nodes created. 
				// 
				aos_alarm("Not implemented yet");
				return aos_alarm("Fatal");
	}
	node->mNumValidBranches++;
	node->mBranches[c]->mParent = node;
	node->mBranches[c]->mBranchChar = c;
	node->mBranches[c]->mData = data;
	node->mBranches[c]->mIsLeaf = 1;
	node->mBranches[c]->mTWC= twc;
	//node->mBranches[c]->mAddition = strdup( &key[pos+1] );
	node->mBranches[c]->mAddition = (char*)aos_malloc(strlen(key)-pos);
	strcpy(node->mBranches[c]->mAddition,&key[pos+1]);
	return 0;
}
*/


static int __aosCharPtree_insert_split(
		struct aosCharPtree * tree,
		void * data,
		char * key,
		int pos,
		struct aosCharPtreeNode * node ,
		char twc
		)
{
	int i;
	char * tmp;
	struct aosCharPtreeNode * new_parent;

	new_parent=aosCharPtree_createNode();
	if(!new_parent)
	{
		aos_alarm("Not implemented yet");
		return -1;
	}
			
	new_parent->mParent = node->mParent;
	new_parent->mBranchChar = node->mBranchChar;
	new_parent->mNumValidBranches = 1;
	new_parent->mParent->mBranches[(int)new_parent->mBranchChar] = new_parent;
	new_parent->mData=data;
	new_parent->mIsLeaf=1;
	new_parent->mTWC=twc;

	i=common_prefix_length(node->mAddition,&key[pos]);
	new_parent->mAddition = (char*)aos_malloc(i+1);
	strncpy(new_parent->mAddition, node->mAddition,i);	
	new_parent->mAddition[i]=0;
	new_parent->mBranches[node->mAddition[i]-tree->mMinChar]=node;

	node->mParent = new_parent;
	node->mBranchChar = node->mAddition[i]-tree->mMinChar;
	tmp=(char*)aos_malloc(strlen(node->mAddition)-i);
	strncpy(tmp,&node->mAddition[i+1],strlen(node->mAddition)-i);
	aos_free(node->mAddition);
	node->mAddition=tmp;

	return 0;
}


static int __aosCharPtree_insert_split_add(
		struct aosCharPtree * tree,
		void * data,
		char * key,
		int pos,
		struct aosCharPtreeNode * parent,
		char twc
		)
{
	int i;
	char * tmp;
	struct aosCharPtreeNode * new_parent;

	new_parent=aosCharPtree_createNode();
	if(!new_parent)
	{
		aos_alarm("Not implemented yet");
		return -1;
	}
			
	new_parent->mParent = parent->mParent;
	new_parent->mBranchChar = parent->mBranchChar;
	new_parent->mNumValidBranches = 2;
	new_parent->mData = parent->mData;
	i=common_prefix_length(parent->mAddition,&key[pos]);
	new_parent->mAddition = (char*)aos_malloc(i+1);
	strncpy(new_parent->mAddition, parent->mAddition,i);	
	new_parent->mAddition[i]=0;
	new_parent->mBranches[parent->mAddition[i]-tree->mMinChar]=parent;
			

	struct aosCharPtreeNode * new_node=aosCharPtree_createNode();
	if(!new_node)
	{
		aos_alarm("Not implemented yet");
		return -1;
	}
	new_parent->mBranches[key[i+pos]-tree->mMinChar]=new_node;
	new_node->mParent = new_parent;
	new_node->mBranchChar = key[i+pos]-tree->mMinChar;
	new_node->mData = data;
	new_node->mAddition = (char*)aos_malloc(strlen(key)-i-pos);
	strncpy(new_node->mAddition,&key[i+pos+1],strlen(key)-i-pos);
	new_node->mAddition[strlen(key)-i-pos]=0;
	new_node->mIsLeaf = 1;
	new_node->mTWC=twc;

	parent->mParent->mBranches[(int)parent->mBranchChar] = new_parent;
	parent->mParent = new_parent;
	parent->mBranchChar = parent->mAddition[i]-tree->mMinChar;
	tmp=(char*)aos_malloc(strlen(parent->mAddition)-i);
	strncpy(tmp,&parent->mAddition[i+1],strlen(parent->mAddition)-i);
	aos_free(parent->mAddition);
	parent->mAddition=tmp;

	return 0;
}

/*
static int __aosCharPtree_insert_pack(struct aosCharPtree *tree, 
						char *key, 
						int key_len, 
						void *data, 
						const int replace,
						char twc)
{
	int ret;
	int pos;
	struct aosCharPtreeNode * node;


	ret = __aosCharPtree_get(tree,key,&pos,&node);

	switch(ret)
	{
		case MATCH:
			// Match
			if(replace)
			{
				node->mData=data;
				node->mTWC=twc;
			}
			return 0;
		case NOT_MATCH:
			// Not match
			// The node found whose whole string is equal to the key
			// But the node is not a leaf node
			node->mIsLeaf=1;
			node->mData=data;
			node->mTWC=twc;
			return 0;
		case NOT_MATCH_SPLIT_ADD:
			// Not match. 
			// The whole string of the parent of the node is the prefix of the key
			// The whole string of the node is diffrent and the key
			return __aosCharPtree_insert_split_add(tree,data,key,pos,node,twc);
		case NOT_MATCH_SPLIT:
			// Not match, but there is one node whose whole string for example is s.
			// key is the prefix of s
			// For exapmle: key=a
			// The tree is ROOT->abc

			return __aosCharPtree_insert_split(tree,data,key,pos,node,twc);
		case NOT_MATCH_ADD_BRANCHE:
			// Not match
			// the node's whole string 
			return __aosCharPtree_insert_add_branche(tree,data,key,pos,node,twc);
		default:
		//	aosAlarm(eAosAlarmNotImplementedYet);
			break;

	}
	return 0;
}
*/


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
/*
int aosCharPtree_insert_pack(struct aosCharPtree *tree, 
						char *key, 
						int key_len, 
						void *data, 
						const int replace,
						char twc)
{
	int index = 0;
	unsigned char c;
	
	if (key_len < 0)
	{
		return eAosAlarmEmptyKey;
	}
	if ( key_len==0 )
		key_len=strlen(key);

	// 
	// First, check whether the key is valid
	//
	for (index = 0; index<key_len; index++)
	{
		c = (unsigned char)key[index];
		if (c < tree->mMinChar || c > tree->mMaxChar)
		{
			aos_alarm("Character out of bound: %u, %u, %u, %s", 
				index, c, tree->mMaxChar, key);
			return -1;
		}
	}
	
	return __aosCharPtree_insert_pack(tree, key, key_len, data, replace,twc);
}
*/


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
int aosCharPtree_insert(struct aosCharPtree *tree, 
						const char *key, 
						int len, 
						void *data, 
						const int replace)
{
	int index = 0;
	struct aosCharPtreeNode *node = tree->mRoot;
	struct aosCharPtreeNode *prevNode = 0;
	unsigned char c;
	int i;

	if (len < 0)
	{
		return eAosAlarmEmptyKey;
	}
	if ( len==0 )
		len=strlen(key);

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
			return aos_alarm("Program error");
		}

		tree->mRoot = aosCharPtree_createNode();
		if (!tree->mRoot)
		{
			// 
			// Still no memory. Return error.
			//
			return aos_alarm("Memory error");
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
		prevNode->mBranches[c] = aosCharPtree_createNode();
		if (!prevNode->mBranches[c])
		{
			// 
			// Run out of memory. Need to remove all the nodes created. 
			// 
			aos_alarm("Not implemented yet");
			return aos_alarm("Memory error");
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

/*
static int __aosCharPtree_merge(struct aosCharPtree *tree, struct aosCharPtreeNode *parent)
{
	char * add;
	struct aosCharPtreeNode * child;
	int i;

	if(parent->mNumValidBranches!=1 || parent->mIsLeaf )
		return 0;

	if(parent->mParent==0)
		return 0;

	i=0;
	while(parent->mBranches[i]==0)
		i++;
	child=parent->mBranches[i];
	
	parent->mParent->mBranches[(int)parent->mBranchChar] = child;
	child->mParent=parent->mParent;
	add=(char*)aos_malloc(strlen(child->mAddition)+1+strlen(parent->mAddition)+1);
	strcpy(add,parent->mAddition);
	add[strlen(add)+1]=0;
	add[strlen(add)]=child->mBranchChar + tree->mMinChar;
	strcat(add,child->mAddition);
	aos_free(child->mAddition);
	aos_free(parent->mAddition);
	child->mBranchChar=parent->mBranchChar;
	child->mAddition=add;
	AosSlab_release(sgSlab, parent);
	return 0;
}
*/


int aosCharPtree_remove_pack(struct aosCharPtree *tree, 
						const char *key, 
						void **entry)
{
	struct aosCharPtreeNode *node;
	struct aosCharPtreeNode *parent;
	//struct aosCharPtreeNode *prevNode = 0;
	int len = strlen(key);
	//int index = 0;
	int pos;
	int ret;
	//char c;

	if (len <= 0)
	{
		return eAosAlarmEmptyKey;
	}

	if(entry)
		*entry = 0;

	ret = __aosCharPtree_get(tree,key,&pos,&node);
	if(ret!=MATCH)
	{
		return ret;
	}

	if(entry)
		*entry=node->mData;
	parent=node->mParent;
	if( node->mNumValidBranches > 0 )
	{
		node->mIsLeaf = 0;	
		node->mData = 0;
		__aosCharPtree_merge(tree,node);
	}
	else
	{
		parent->mNumValidBranches--;
		parent->mBranches[(int)node->mBranchChar]=0;
		aos_free(node->mAddition);
		AosSlab_release(sgSlab, node);
		__aosCharPtree_merge(tree,parent);
	}

	return 0;

}


int aosCharPtree_remove(struct aosCharPtree *tree, 
						const char *key, 
						void **entry)
{
	struct aosCharPtreeNode *node = tree->mRoot;
	struct aosCharPtreeNode *prevNode = 0;
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
		return aos_alarm("Program error");
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

			aos_alarm("Alarm");
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
			return aos_alarm("Alarm");
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
	return aos_alarm("Fatal");
}

struct aosCharPtreeNode *aosCharPtree_listFirst( struct aosCharPtree * tree, struct aosCharPtreeNode * node )
{
	int i;
	struct aosCharPtreeNode * found;
	if(node==0)
	{
		return 0;
	}

	if(node->mIsLeaf)
	{
		return node;		
	}

	for(i=0;i<tree->mMaxChar-tree->mMinChar+1;i++)
	{
		found=aosCharPtree_listFirst( tree, node->mBranches[i] );
		if(found)
			return found;
	}
	return 0;
}

struct aosCharPtreeNode *aosCharPtree_listNext( 
		struct aosCharPtree * tree, 
		struct aosCharPtreeNode * node, 
		unsigned char c )
{
	int i;
	struct aosCharPtreeNode * found;

	if(node==0)
	{
		return 0;
	}

	for(i=c+1;i<tree->mMaxChar-tree->mMinChar+1;i++)
	{
		found=aosCharPtree_listFirst( tree, node->mBranches[i] );
		if(found)
			return found;
	}

	return aosCharPtree_listNext( tree, node->mParent, node->mBranchChar );
	
}

int aosCharPtree_fullKey(struct aosCharPtree * tree, struct aosCharPtreeNode * node, char * key, int len)
{
	struct aosCharPtreeNode * p;
		int i;
	int pos=0;

	if(node==0)
		return 0;

	p=node;
	if(p->mTWC)
	{
		if(pos<len)
			key[pos++]='*';
		else
			return -1;

	}


	while(p->mParent)
	{
		int al;
	
		al=strlen(p->mAddition);
		for(i=0;i<al;i++)
		{
			if(pos<len)
				key[pos++]=p->mAddition[al-i-1];//+tree->mMinChar;
			else
				return -1;
		}

		if(p->mBranchChar)
		{
			if(pos<len)
				key[pos++]=p->mBranchChar+tree->mMinChar;
			else
				return -1;
		}
		p=p->mParent;
	}

	for(i=0;i<(pos+1)/2;i++)
	{
		unsigned char ch;
		ch=key[i];
		key[i]=key[pos-i-1];
		key[pos-i-1]=ch;
	}
	
	if(pos<len)
		key[pos]=0;
	else
		return -1;
	return pos;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosCharPtree_init );
EXPORT_SYMBOL( aosCharPtree_remove );
EXPORT_SYMBOL( aosCharPtree_create );
EXPORT_SYMBOL( aosCharPtree_insert );
EXPORT_SYMBOL( aosCharPtree_get );
EXPORT_SYMBOL( aosCharPtree_listNext );
EXPORT_SYMBOL( aosCharPtree_listFirst );
EXPORT_SYMBOL( aosCharPtree_fullKey );
#endif
#endif
