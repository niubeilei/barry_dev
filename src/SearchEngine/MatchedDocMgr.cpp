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
// 10/30/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SearchEngine/MatchedDocMgr.h"

#include "Util/OmnNew.h"


AosMatchedDocMgr::AosMatchedDocMgr()
{
	mHTable = OmnNew AosWordIdHash("", eTablesize, AOS_PTRSIZE);
}


bool
AosMatchedDocMgr::isDocSelected(
		const u64 &docid, 
		const u32 queryid)
{
	// This function checks whether an entry has been created
	// for the doc. If not, it creates one. It then checks
	// whether the 'queryid' is already in the doc's Query List.
	// If yes, it returns true. Otherwise, it appends the 
	// queryid and returns false. 
	//
	u64 ptr = mHTable->getWordId((char *)&docid, 8;
}
#endif
