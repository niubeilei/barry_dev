////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 03/08/2012 Created by Chen Dign
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_VirtualFileNames_h
#define AOS_StorageMgr_VirtualFileNames_h

#include "StorageMgr/StrKey.h"
#include "Util/String.h"

class AosFilekey
{
public:
	OmnString getSizeIdMapFileKey(const u64 &virtualid, const u32 siteid)
	{
		OmnString fkey = AOSSTRKEY_SIZEID_MAP;
		fkey << "_" << virtualid << "_" << siteid;
		return fkey;
	}
};
#endif

