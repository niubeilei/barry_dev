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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataShuffler_Testers_DataShufflerTester_h
#define Aos_DataShuffler_Testers_DataShufflerTester_h

#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "DataShuffler/Ptrs.h"
#include "DataCacher/Ptrs.h"

#include <vector>
using namespace std;

class AosDocidShufflerTester : public OmnTestPkg
{

public:
	enum
	{
		eMaxEntries = 1000000
	};

private:
	u64 	mCrtId;
	int64_t mCrtVirServerNum;
	int 	mNumServers;
	int 	mNumVirtuals;

public:
	AosDocidShufflerTester();
	~AosDocidShufflerTester() {}

	virtual bool		start();

private:
	bool basicTest();
	bool test(const AosXmlTagPtr &config, const AosRundataPtr &rdata);

	bool shuffle(
			const AosDataShufflerPtr &dataShuffler,
			const AosXmlTagPtr &config,
			const AosRundataPtr &rdata);

	AosDataCacherPtr generateData(
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);

	bool appendEntry(
			const AosDataShufflerPtr &dataShuffler,
			const AosRundataPtr &rdata);

	bool verifyData2(
			const vector<AosDataCacherPtr> &dataCachers,
			const AosRundataPtr &rdata);

	bool verifyData1(const AosRundataPtr &rdata);
	bool verifyDocid(
				const char *record,
				const int index,
				const AosRundataPtr &rdata);
	int	route(const u64 &distid);
};


#endif

