////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPtree.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_CharPTree_h
#define Omn_AosUtil_CharPTree_h

#define eAosCharTreeMaxChar 255 
#define eAosCharTreeMinChar 32
#define eAosCharTreeBranchSize (eAosCharTreeMaxChar-eAosCharTreeMinChar+1)


struct aosCharPtreeNode
{
	char						mIsLeaf;
	void *						mData;
	char *						mAddition;
	unsigned char				mNumValidBranches;
	char						mBranchChar;
	char						mTWC;
	struct aosCharPtreeNode *	mParent;
	struct aosCharPtreeNode *	mBranches[eAosCharTreeBranchSize];
};

struct aosCharPtree
{
	struct aosCharPtreeNode *	mRoot;
	unsigned char				mMinChar;
	unsigned char				mMaxChar;
};

extern struct aosCharPtree *aosCharPtree_create(void);
extern int    aosCharPtree_init(struct aosCharPtree *tree);

extern int    aosCharPtree_get(struct aosCharPtree *tree, 
						const char *key, 
						int *foundIndex, 
						void **entry);
extern int aosCharPtree_insert(struct aosCharPtree *tree, 
						const char *key, 
						int len, 
						void *entry, 
						const int replace);

extern int aosCharPtree_remove(struct aosCharPtree *tree, 
						const char *key, 
						void **entry);




//add by zzh, 2006/07/04
//The following functions have not been tortoured and the above non-pack functions
//are used many times, so I use the differente function names instead of overlay the old functions.
//The above old functions can be overlaid after tortoure of the new functions

//New functions:
//	1. pack
//	2. support the wildcard at the end of the key
//	3. support the wildcard at the beginning of the key. ( by a new interface and create a new tree)
//
//For the ip address and netmask matching, we can use the ptree too:
//  1. convert the ip address to a u32( for example: u32 ip;)
//  2. convert the netmask to a unsigned char( unsigned char mask_bits; )
//  3. the mask_bits is between 1 and 32
//  4. get the first 32 bits of the ip(u32), the result is a string, 
//  	the length of the string is mask_bits, 
//  	the string is a stream of '1' and '0'
//  5. use the mask_bits char long string as the key, with a tail wildcard.
//     we can match any ip address in the subnet.
//

//  the proto type is same as aosCharPtree_get()
int aosCharPtree_get_pack(struct aosCharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **entry);

//the proto type is same as aosCharPtree_remove()
int aosCharPtree_remove_pack(struct aosCharPtree *tree, 
						const char *key, 
						void **entry);

//the proto type is same as aosCharPtree_remove()
//except that: 
//	add a new parameter: char twc
//	twc stands for "Tail WildCard"
//	twc!=0 means there is a wildcard at the end of the key
//	twc==0 means there is no wildcard at the end of the key
int aosCharPtree_insert_pack(struct aosCharPtree *tree, 
						char *key, 
						int len, 
						void *entry, 
						const int replace,
						char twc);

// 	aosCharPtree_get_pack_recursion() will first call aosCharPtree_get_pack()
// 	if aosCharPtree_get_pack() fails, remove one char at the beginning of the key
// 	and call aosCharPtree_get_pack() again until the length of key is 1 or 
// 	the aosCharPtree_get_pack() success
int aosCharPtree_get_pack_recursion(struct aosCharPtree *tree, 
						const char *key, 
						int *indexFound, 
						void **entry);


int aosCharPtree_print(struct aosCharPtree *tree); 

struct aosCharPtreeNode *aosCharPtree_listFirst( struct aosCharPtree * tree, struct aosCharPtreeNode * node );
struct aosCharPtreeNode *aosCharPtree_listNext( 
		struct aosCharPtree * tree, 
		struct aosCharPtreeNode * node, 
		unsigned char c );

int aosCharPtree_fullKey(struct aosCharPtree * tree, struct aosCharPtreeNode * node, char * key, int len);
#endif

