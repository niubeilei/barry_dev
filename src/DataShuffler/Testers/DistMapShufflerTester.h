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
#ifndef Aos_DataShuffler_Testers_DistMapShufflerTester_h
#define Aos_DataShuffler_Testers_DistMapShufflerTester_h

#include "Tester/TestPkg.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "DataShuffler/Ptrs.h"
#include "DataCacher/Ptrs.h"

#include <vector>
using namespace std;

class AosDistMapShufflerTester : public OmnTestPkg
{
public:
	enum
	{
		eMaxEntries = 10000
	};

private:
	u64 	mCrtId;
	AosBuffArrayPtr mMap;
	vector<OmnString>	mArray;
public:
	AosDistMapShufflerTester();
	~AosDistMapShufflerTester() {}

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
	bool verifyResult(
				const char* record,
				const int index,
				const AosRundataPtr &rdata);
	int	route(const OmnString &key);

	bool createMap(const AosRundataPtr &rdata);
	OmnString	generateEntryStr();
	int	comp(const OmnString key);
};


#endif

