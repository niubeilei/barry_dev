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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryCaller_h
#define Aos_SEInterfaces_QueryCaller_h

#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"


class AosQueryCaller : virtual public OmnRCObject
{
private:

public:

	virtual bool queryFinished(const AosRundataPtr &rdata,
					const AosQueryRsltObjPtr &data,
					const AosBitmapObjPtr	&bitmap,
					const AosBitmapQueryTermObjPtr &caller) = 0;
	virtual bool queryFailed(const AosRundataPtr &rdata,
					const OmnString &errmsg) = 0;
	virtual int getPageSize() const = 0;
	virtual u64 getStartIdx() const = 0;
	virtual i64 getBlockSize() const = 0;
	virtual int getInitBlockSize() = 0;
	virtual int nextBlockSize(const int crt_block_size) = 0;
};

#endif
