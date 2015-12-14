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
#ifndef Omn_CounterServer_RetrieveCounterSet_h
#define Omn_CounterServer_RetrieveCounterSet_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/CounterProc.h"
#include "Rundata/Rundata.h"
#include "CounterServer/Ptrs.h"


class AosRetrieveCounterSet : virtual public AosCounterProc 
{

public:
	AosRetrieveCounterSet(const ReqId id, const bool regflag);
	~AosRetrieveCounterSet() {}

	// RetrieveCounterSet Interface
	bool proc(const AosXmlTagPtr &request, 
			  const AosRundataPtr &rdata);
	AosCounterProcPtr clone();
};
#endif

#endif
