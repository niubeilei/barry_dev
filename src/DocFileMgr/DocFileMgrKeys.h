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
// 09/19/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DocFileMgrKeys_h
#define AOS_DocFileMgr_DocFileMgrKeys_h

#define AOSFILEKEY_SIZEID_MAP				"__zkd_0001"			// Ketty 2012/08/24 

class AosDocFileMgrkey
{
public:
	inline static OmnString getSizeIdMapFileKey(
			const int virtualid,
			const u32 siteid,
			const u32 sizeid)
	{
		OmnString fkey = AOSFILEKEY_SIZEID_MAP;
		fkey << "_" << virtualid << "_" << siteid << "_" << sizeid;
		return fkey;
	}

	// Linda, 2012/12/10
	inline static OmnString getGroupedMapFileKey(
			const int virtualid,
			const u32 siteid)
	{
		OmnString fkey = AOSFILEKEY_SIZEID_MAP;
		fkey << "_" << virtualid << "_" << siteid;
		return fkey;
	}

};
#endif

