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
#ifndef Omn_CounterServer_RetrieveCountersByQry_h
#define Omn_CounterServer_RetrieveCountersByQry_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/CounterProc.h"
#include "Rundata/Rundata.h"
#include "CounterServer/Ptrs.h"


class AosRetrieveCountersByQry : virtual public AosCounterProc 
{

public:
	AosRetrieveCountersByQry(const ReqId id, const bool regflag);
	~AosRetrieveCountersByQry() {}

	// RetrieveCountersByQry Interface
	bool proc(const AosXmlTagPtr &request, 
			  const AosRundataPtr &rdata);

	bool getSingleCountersByQuery(const AosVirCtnrSvrPtr &vserver,
								  const AosXmlTagPtr &request,
								  const AosRundataPtr &rdata);

	bool getMultiCountersByQuery(const AosVirCtnrSvrPtr &vserver,
								  const AosXmlTagPtr &request,
								  const AosRundataPtr &rdata);

	bool getSubCountersByQuery(const AosVirCtnrSvrPtr &vserver,
								  const AosXmlTagPtr &request,
								  const AosRundataPtr &rdata);
	AosCounterProcPtr clone();
};
#endif

#endif
