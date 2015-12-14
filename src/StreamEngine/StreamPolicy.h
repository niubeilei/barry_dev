////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 03/17/2015 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_StreamPolicy_h
#define AOS_StreamEngine_StreamPolicy_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"
#include <map>

#define STREAMPOLICY_RR  "rr"
#define STREAMPOLICY_LOCAL  "local"
#define STREAMPOLICY_SHUFFLE  "shuffle"

//
//This class helps to decide which downstream node will be
//used based on some specified policy
//
//It mostly bases on some load balance policy
//
class AosStreamPolicy : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Policy 
	{
		eRR,
		eLocal,
		eShuffle,
		eInvalid
	};

private:
	Policy 	mPolicy;

public:
	AosStreamPolicy();
	AosStreamPolicy(const OmnString &policy);
	AosStreamPolicy(const Policy &policy);

	~AosStreamPolicy();

	int getPhyId(int rddId);
	u32 getLogicPid(const int svr_id);

	//getter/setter
	void setPolicy(const Policy &policy) { mPolicy = policy; }

	bool getTargetProcess(
					int &svr_id, 
					u32 &logic_pid, 
					const int rddId,
					const u64 &service_docid);



private:
	OmnString idToStr(const Policy &id);
	Policy strToId(const OmnString &str);

};

#endif
