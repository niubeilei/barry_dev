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
// This is a utility to select docs.
//
// Modification History:
// 03/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterClt_CounterAssembler_h
#define AOS_CounterClt_CounterAssembler_h

#include "CounterUtil/StatTypes.h"
#include "CounterUtil/CounterOperations.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosCounterAssembler : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		//eDftMaxBuffSize = 1000000
		eDftMaxBuffSize = 1
	};

	vector<OmnMutexPtr>	mLocks;
	vector<AosBuffPtr>	mBuffs;
	int					mNumVirtuals;
	int					mNumPhysicals;

	static u64	smMaxBuffSize;

public:
	AosCounterAssembler();
	~AosCounterAssembler();

	static bool config(const AosXmlTagPtr &conf);
	void init(const u64 &num_phy, const u64 &num_vir);

	bool addCounter(const OmnString &counter_id, 
					const OmnString &cname, 
					const int64_t &cvalue, 
					const u64 &unitime,
					const AosStatType::E statType,
					const AosCounterOperation::E &operation,
					const AosRundataPtr &rdata);

	bool addCounter2(
					const AosCounterOperation::E &operation,
					const OmnString &counter_id,
					const OmnString &cname,
					const int64_t &cvalue,
					const u64 &unitime,
					const AosTimeGran::E time_gran,
					const u16 &entry_type,
					const AosRundataPtr &rdata);
};
#endif

