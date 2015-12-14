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

#include "aosUtil/UrlPtree.h"
#include "aosUtil/Memory.h"
/*
#include <KernelSimu/string.h>
#include "aos/aosKernelAlarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Slab.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Modules.h"
#include "aosUtil/ReturnCode.h"
*/
#ifdef __KERNEL__
#include <linux/module.h>
#endif

	
struct aosUrlPtree * aosUrlPtree_create(void)
{
	struct aosUrlPtree *tree;

	tree = (struct aosUrlPtree*)aos_malloc(sizeof(tree));

	tree->tree1=aosCharPtree_create();
	aosCharPtree_init(tree->tree1);

	tree->tree2=aosCharPtree_create();
	aosCharPtree_init(tree->tree2);

	return tree;
}

int aosUrlPtree_insert( struct aosUrlPtree * tree, char * str, void * value, int replace )
{
	struct aosCharPtree * t;
	int twc=0;
	
	if(str[0]=='*')
	{
		str++;
		t=tree->tree2;
	}
	else
	{
		t=tree->tree1;
	}

	char *s=str;
	if(str[strlen(str)-1]=='*')
	{
		twc=1;
		s=(char*)aos_malloc(strlen(str));
		strncpy(s,str,strlen(str));
		s[strlen(str)-1]=0;
	}

	aosCharPtree_insert_pack(t,s,0,value,replace,twc);
	if(s!=str)
		aos_free(s);

	return 0;
}

int aosUrlPtree_get( struct aosUrlPtree * tree, char * str, void ** value )
{
	int pos;

	if( 0 == aosCharPtree_get_pack(tree->tree1,str,&pos,value) )
	{
		return 0;
	}


	return aosCharPtree_get_pack_recursion(tree->tree2,str,&pos,value); 
}


int aosUrlPtree_print(struct aosUrlPtree *tree)
{
	aosCharPtree_print(tree->tree1);
	aosCharPtree_print(tree->tree2);
	return 0;
}

int aosUrlPtree_remove(struct aosUrlPtree *tree, 
						const char *key, 
						void **entry)
{

	if(key[0]=='*')
	{
		return aosCharPtree_remove_pack(tree->tree2,&key[1],entry);
	}
	else
	{
		return aosCharPtree_remove_pack(tree->tree1,key,entry);
	}

}
#ifdef __KERNEL__
EXPORT_SYMBOL( aosUrlPtree_remove );
EXPORT_SYMBOL( aosUrlPtree_create );
EXPORT_SYMBOL( aosUrlPtree_insert );
EXPORT_SYMBOL( aosUrlPtree_get );
EXPORT_SYMBOL( aosUrlPtree_print );
#endif

