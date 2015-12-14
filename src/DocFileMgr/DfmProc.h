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
// 2012/09/13	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/04/03
#ifndef Omn_DocFileMgr_DfmProc_h
#define Omn_DocFileMgr_DfmProc_h

#include "Rundata/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "DfmUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosDfmProc : virtual public OmnRCObject
{
public:
	virtual OmnString 	getFnamePrefix(){ return "Data"; };
	virtual u32 		getDocHeaderSize() = 0;
	virtual AosDfmDocPtr  getTempDfmDoc() = 0;
	virtual bool		needDeltaHeader() = 0;

	// Temp.
	virtual u64			parseLocId(const u64 loc_id, const u32 virtual_id) = 0;
};
#endif
