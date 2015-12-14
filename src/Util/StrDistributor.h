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
// Modification History:
// 07/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_StrDistributor_h
#define Snt_Util_StrDistributor_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;

class AosStrDistributor : virtual public OmnRCObject
{
public:
	AosStrDistributor();
	~AosStrDistributor();

	bool distribute(
				const AosBuffArrayPtr &data, 
				const int num_buckets,
				vector<AosBuffArrayPtr> &buckets,
				const int key_start, 
				const int key_len,
				const int record_len,
				AosRundata *rdata);

private:
	bool createIndex(const vector<OmnString> keys, AosRundata *rdata);
};
#endif
