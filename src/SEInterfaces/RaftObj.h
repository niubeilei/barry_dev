////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/06/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RaftObj_h
#define Aos_SEInterfaces_RaftObj_h

#include "Jimo/Jimo.h"
#include "UtilData/BlobSEReqEntry.h"


class AosRaftObj : virtual public AosJimo
{
protected:

public:
	virtual ~AosRaftObj();

	virtual bool start(AosRundata *rdata) = 0;
	virtual bool stop(AosRundata *rdata) = 0;
	virtual bool appendEntry(AosRundata *rdata, AosBuffPtr buff) = 0;
};

#endif

