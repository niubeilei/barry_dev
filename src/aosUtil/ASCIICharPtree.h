////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ASCIICharPtree.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_ASCIICharPtree_h
#define Omn_AosUtil_ASCIICharPtree_h

#define eAosCharTreeMaxChar 126 
#define eAosCharTreeMinChar 32
#define eAosCharTreeBranchSize eAosCharTreeMaxChar-eAosCharTreeMinChar 


struct aosASCIICharPtreeNode
{
	char						mIsLeaf;
	void *						mData;
	unsigned char				mNumValidBranches;
	char						mBranchChar;
	struct aosASCIICharPtreeNode *	mParent;
	struct aosASCIICharPtreeNode *	mBranches[eAosCharTreeBranchSize];
};

struct aosASCIICharPtree
{
	struct aosASCIICharPtreeNode *	mRoot;
	unsigned char				mMinChar;
	unsigned char				mMaxChar;
};

extern struct aosASCIICharPtree *aosASCIICharPtree_create(void);
extern int    aosASCIICharPtree_init(struct aosASCIICharPtree *tree);
extern struct aosASCIICharPtree * aosASCIICharPtree_create(void);
extern int    aosASCIICharPtree_get(struct aosASCIICharPtree *tree, 
						const char *key, 
						int *foundIndex, 
						void **entry);
extern int aosASCIICharPtree_insert(struct aosASCIICharPtree *tree, 
						const char *key, 
						const int len, 
						void *entry, 
						const int replace);
/*static inline int aosASCIICharPtree_insert1(struct aosASCIICharPtree *tree, 
						const char *key, 
						void *data, 
						const int replace)
{
	return aosASCIICharPtree_insert(tree, key, strlen(key), data, replace);
}
*/

extern int aosASCIICharPtree_remove(struct aosASCIICharPtree *tree, 
						const char *key, 
						void **entry);

#endif

