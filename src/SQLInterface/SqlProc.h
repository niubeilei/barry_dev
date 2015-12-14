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
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SQLInterface_SqlProc_h
#define Omn_SQLInterface_SqlProc_h

#include "Rundata/Ptrs.h"
#include "SQLInterface/Ptrs.h"
#include "SQLInterface/SqlReqid.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"



class AosSqlProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosSqlReqid::E		mId;
	OmnString			mReqidName;

public:
	AosSqlProc(const OmnString &name, const AosSqlReqid::E reqid, const bool regflag);
	~AosSqlProc();

	// SqlProc Interface
	virtual bool proc(const AosRundataPtr &rdata) = 0;

	static AosSqlProcPtr getProc();
	
private:
};
#endif

