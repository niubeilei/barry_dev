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

#include "AOS_Match.h"
#include "AOS_MD5.h"
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
AOS_Match::AOS_Match(){}
AOS_Match::~AOS_Match(){}
bool AOS_Match::cmp(target t1,target t2)
{
    return t1.t < t2.t;
}
void AOS_Match::build_hash_table(Sum_Struct *s)//以16位的校验码为key，建立hash表
{
    // This function build hash table based on Sum_Struct
    int i;
    target tem_target;
    for (i=0;i<s->count;i++)
    {
        tem_target.i = i;
        tem_target.t = gettag(s->sums[i].sum1);
        targets.push_back(tem_target);
    }
    sort(targets.begin(),targets.end(), AOS_Match::cmp);
    for (i=0; i<TABLESIZE; i++)
        tag_table[i] = NULL_TAG;

    for (i=s->count-1;i>=0;i--)
    {
        tag_table[targets[i].t] = i;
    }
}
int AOS_Match::match_sums(Sum_Struct *s,char *block_buf,int len,uint32 checksum1,int lastmatch)
{
    // This function compare if this block can match by 3 step
    // if this block match, then return the block id it matched
    // if not ,then return -1
    tag t = gettag(checksum1);
    int j = tag_table[t];
    if(j != NULL_TAG)// find this 16bit value in hashtable
    {
        int i = targets[j].i;
        if(checksum1 == s->sums[i].sum1)// this checksum1 exit,then compare MD5
        {
            char md5[33];
            bzero(md5,33);
            AOS_Checksum::get_checksum2(block_buf,len,md5);
            //MD5::TransTo32ByteMd5(block_buf,md5);
            for(int k = lastmatch; k < s->count; k++)
            {
                if(strcmp(s->sums[k].sum2,md5) == 0)
                {
                    return k;
                }
            }
        }
    }
    return -1;
}





