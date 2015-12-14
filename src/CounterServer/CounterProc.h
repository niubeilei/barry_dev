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
// 05/26/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_COunterSvr_CounterProc_h
#define Omn_COunterSvr_CounterProc_h

#include "CounterServer/Ptrs.h"
#include "CounterUtil/CounterOperations.h"
#include "Rundata/Rundata.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosCounterProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosCounterOperation::E	mId;

public:
	AosCounterProc(const AosCounterOperation::E id, const bool regflag);
	~AosCounterProc() {}

	// CounterProc Interface
	// virtual bool proc(
	// 		const AosXmlTagPtr &request, 
	// 		const AosRundataPtr &rdata) = 0;
	virtual bool proc(
					const AosBuffNatTransPtr &trans, 
					AosVirCtnrSvr* virtual_server, 
					const AosRundataPtr &rdata) = 0;

	virtual AosCounterProcPtr clone() = 0;

	static AosCounterProcPtr  getProc(const OmnString &procid);
	static AosCounterProcPtr  getProc(const AosCounterOperation::E code);

private:
	bool 	registerProc();
};
#endif
#endif
