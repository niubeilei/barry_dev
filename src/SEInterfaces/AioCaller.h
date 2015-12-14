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
// 04/28/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_AioCaller_h
#define Aos_SEInterfaces_AioCaller_h

#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosAioCaller : virtual public OmnRCObject
{

public:
	struct Parameter 
	{
		u64 reqId;
		int64_t expected_size;
		bool finished;
		int64_t offset;

		bool serializeFrom(const AosBuffPtr &buff)
		{
			reqId = buff->getU64(0);
			expected_size = buff->getI64(-1);
			finished = buff->getU8(0);
			offset = buff->getI64(-1);
			return true;
		}

		bool serializeTo(const AosBuffPtr &buff)
		{
			buff->setU64(reqId);
			buff->setI64(expected_size);
			buff->setU8(finished);
			buff->setI64(offset);
			return true;
		}
	};

public:

	virtual AosBuffPtr getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl) = 0;
	//virtual void dataRead(u64 &reqid, const AosBuffPtr &buff, bool &finished) = 0;
	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff) = 0;
	virtual void readError(u64 &reqid, OmnString &errmsg) = 0;
	virtual void dataWrite(const u64 &reqid) = 0;
	virtual void writeError(u64 &reqid, OmnString &errmsg) = 0;
};
#endif

