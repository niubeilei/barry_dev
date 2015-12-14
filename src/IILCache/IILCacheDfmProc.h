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
// 2013/03/02	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_IILCache_IILCacheDfmProc_h
#define Aos_IILCache_IILCacheDfmProc_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmProc.h"

class AosIILCacheDfmProc : public AosDfmProc
{
	OmnDefineRCObject;

private:

	static AosDfmDocPtr		smDfmDoc;

public:
	virtual OmnString 	getFnamePrefix();
	virtual u32 		getDocHeaderSize();
	virtual bool		finishTrans(const vector<u64> &total_trans, const AosRundataPtr &rdata);
	virtual AosDfmDocPtr  getTempDfmDoc();
	virtual bool		needDeltaHeader();
	virtual u64			parseLocId(const u64 loc_id, const u32 virtual_id);
};
#endif

#endif
