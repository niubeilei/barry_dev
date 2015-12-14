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
// 03/03/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_ReqDistr_Samples_Processor_h
#define Aos_Proggie_ReqDistr_Samples_Processor_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Util/RCObjImp.h"

class AosMyProcessor : public AosNetReqProc
{
	OmnDefineRCObject;

public:
	AosMyProcessor() {}
	~AosMyProcessor() {}

	virtual bool	procRequest(const OmnConnBuffPtr &req);
};
#endif

