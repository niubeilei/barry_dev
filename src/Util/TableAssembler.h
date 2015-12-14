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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_TableAssembler_h
#define Aos_Util_TableAssembler_h

#include "Rundata/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosTableAssembler : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxTableSize = 10000000
	};

private:
	AosXmlTagPtr 				mConfig;
	AosTransClientPtr 			mTransClient;
	int							mNumPhysicals;
	vector<AosStrU64ArrayPtr>	mTables;

public:
	AosTableAssembler(const AosXmlTagPtr &config, AosRundata *rdata);
	AosTableAssembler(
			const AosXmlTagPtr &config, 
			const AosTransClientPtr &transclt,
			AosRundata *rdata);
	~AosTableAssembler();

	bool addValue(const char* data, const u32 len, AosRundata *rdata);
	bool addValue(const char* data, const u32 len, const int phyid, AosRundata *rdata);
	bool finish(AosRundata *rdata);

private:
	bool sendTable(int phyid,  AosRundata *rdata);
	bool config(const AosXmlTagPtr &conf, AosRundata *rdata);
};

#endif

