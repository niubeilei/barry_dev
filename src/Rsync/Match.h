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

#include "Util/HashUtil.h"
#include <sys/types.h>
#include "Rsync.h"
#include <vector>
#include <hash_map>
#include "aosUtil/Types.h"

class AOSMatch
{
private:
	typedef hash_map<const u32, vector<u32>, u32_hash, u32_cmp> map_t;
	typedef hash_map<const u32, vector<u32>, u32_hash, u32_cmp>::iterator itr_t;

 	map_t			mMap;

public:
    AOSMatch();
    virtual ~AOSMatch();

    void buildHashTable(Sum_Struct *s);
	int matchSums(Sum_Struct *s,
			char *block_buf,
			int len,
			u32 checksum,
			int lastmatch);
};

#endif // MATCH_H



