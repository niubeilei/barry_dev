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
// Modification History:
// 05/013/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_RaidFileReq_h
#define AOS_DocFileMgr_RaidFileReq_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

struct AosRaidFileReq : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	AosRaidFileReq(){};
	~AosRaidFileReq(){};

	virtual u64 	getTotalReq() = 0;
	
	virtual bool	addRequest(
						const u64 &reqid,
						const OmnFilePtr &ff,
						const int64_t &seek_pos,
						const int64_t &bytes_to_read) = 0;

	virtual bool	sendRequest() = 0;

	virtual void	clear() = 0;

	virtual bool	getErrorCode() {return false;}
private:
	bool	sanityCheck();
};
#endif
