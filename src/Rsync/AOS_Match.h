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

#ifndef AOS_Rsync_MATCH_H
#define AOS_Rsync_MATCH_H

#include <sys/types.h>
#include "AOS_Rsync.h"
#include <vector>

typedef unsigned int uint32;
typedef unsigned short tag;
#define TABLESIZE (1<<16)
#define NULL_TAG ((tag)-1)
#define gettag(sum) (((sum)>>16) + ((sum)&0xFFFF))

typedef struct
{
    tag t;
    int i;
}target;

class AOS_Match
{
    public:
        vector<target> targets;
        tag tag_table[TABLESIZE];
    public:
        AOS_Match();
        virtual ~AOS_Match();
        static bool cmp(target t1, target t2);
        void build_hash_table(Sum_Struct *s);//以16位的校验码为key，建立hash表
        int match_sums(Sum_Struct *s,char *block_buf,int len,uint32 checksum1,int lastmatch);
};

#endif // MATCH_H
