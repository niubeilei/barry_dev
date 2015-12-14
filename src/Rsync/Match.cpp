////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
//
// Modification History:
// 2015/02/03 Created by Koala Ren
////////////////////////////////////////////////////////////////////////////

#include "Match.h"
#include "MD5.h"
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <iostream>

//////////////////////////////////////////////////////////////
// This class is used to build a hash table which has 2^16 entries based on 32 bit checksum
// and then compare md5 value
// if they both hit, this block can match
// there are three steps to compare
// 1. search hash table if hit then step 2
// 2. compare 32bit checksum, if equal then step 3
// 3  compare 256bit md5 value, if equal ,this block match
// 2. Otherwise, it should generate:
//////////////////////////////////////////////////////////////

using namespace std;

AOSMatch::AOSMatch(){}
AOSMatch::~AOSMatch(){}


void 
AOSMatch::buildHashTable(Sum_Struct *s)//
{
	for(int i =0; i < s->count; i++)
   	{
	   	itr_t itr = mMap.find(s->sums[i].checksum);
	   	if (itr == mMap.end())
	   	{
			// It is not in the hashmap yet.
		   	vector<u32> vec;
		   	vec.push_back(s->sums[i].id);
			mMap[s->sums[i].checksum] = vec;
		}
		else
		{
			itr->second.push_back(s->sums[i].id);
		}
   }
}


int 
AOSMatch::matchSums(
		Sum_Struct *s, 
		char *block_buf, 
		int len,
		u32 checksum1, 
		int last_match)
{
	itr_t itr = mMap.find(checksum1);
    if (itr != mMap.end())
	{
		char md5[33];
		bzero(md5,33);
		AOSChecksum::getMD5(block_buf, len, md5);
		
		for (u32 i=0; i<itr->second.size(); i++)
		{
			u32 id = itr->second[i];
			if (strcmp(s->sums[id].md5, md5) == 0)
			{
				return id;	
			}
		}
	}
	return -1;
}


