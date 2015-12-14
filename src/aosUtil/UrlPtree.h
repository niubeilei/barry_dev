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

#ifndef Omn_AosUtil_UrlPTree_h
#define Omn_AosUtil_UrlPTree_h


#include "aosUtil/CharPtree.h"

struct aosUrlPtree
{
	struct aosCharPtree *tree1;
	struct aosCharPtree *tree2;
};

struct aosUrlPtree * aosUrlPtree_create(void);
int aosUrlPtree_insert( struct aosUrlPtree * tree, char * str, void * value, int replace );
int aosUrlPtree_get( struct aosUrlPtree * tree, char * str, void ** value );
int aosUrlPtree_print(struct aosUrlPtree *tree);

int aosUrlPtree_remove(struct aosUrlPtree *tree, 
						const char *key, 
						void **entry);

#endif

