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
// 10/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_FileReaderListener_h
#define Aos_SEInterfaces_FileReaderListener_h

#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/DiskStat.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosFileReadListener : virtual public OmnRCObject
{
public:
	virtual bool dataRead(const u64 &read_id, 
						std::vector<AosBuffPtr> &buffs,
						const AosRundataPtr &rdata) {return false;};
	virtual bool readError(const u64 &read_id, 
						const OmnString &errmsg,
						const AosRundataPtr &rdata) {return false;};
	virtual void getMemory(const u64 read_id, const int size, std::vector<AosBuffPtr> &buffs)
	{ 
		return buffs.push_back(OmnNew AosBuff(size AosMemoryCheckerArgs));
	}

	virtual void fileReadCallBack(
			const u64 &reqId, 
			const int64_t &expected_size,  
			const bool &finished, 
			const AosDiskStat &disk_stat)
	{
	};

};
#endif

