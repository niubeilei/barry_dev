////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RegTree.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_RegTree_h
#define Omn_Util_RegTree_h

enum
{
	eAosCharTreeBranchSize = 95,
};


struct aosRegTreeNode
{
	char						mIsLeaf;
	void *						mData;
	unsigned char				mNumValidBranches;
	struct aosRegTreeNode *	mParent;
	struct aosRegTreeNode *	mBranches[eAosCharTreeBranchSize];
};

struct aosRegTree
{
	struct aosRegTreeNode *	mRoot;
	char						mMinChar;
	char						mMaxChar;
};

extern struct aosRegTree *aosRegTree_create();
extern int    aosRegTree_init(struct aosRegTree *tree);
extern struct aosRegTree * aosRegTree_create();
extern int    aosRegTree_get(struct aosRegTree *tree, 
						const char *key, 
						int *foundIndex, 
						void **foundNode);
extern int aosRegTree_insert(struct aosRegTree *tree, 
						const char *key, 
						const int len, 
						void *data, 
						const int replace);
extern int aosRegTree_remove(struct aosRegTree *tree, const char *key);

#endif

