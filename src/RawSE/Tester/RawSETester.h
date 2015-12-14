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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_Tester_RawSETester_h
#define Aos_CubeComm_Tester_RawSETester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "RawSE/RawSE.h"
#include "RawSE/Ptrs.h"
#include <ext/hash_map>
#include <map>
#include <vector>

class AosRawSETester : public OmnTestPkg
{
public:
	enum
	{
		eDefaultTries = 100,
		eMaxBuffSize = 500000000   //500M
	};
	typedef std::vector< std::pair<u32, u32> > v_seed_repeat_t;
	typedef __gnu_cxx::hash_map<const AosRawFile::Identifier, v_seed_repeat_t,
			AosRawSE::AosRawFileTagHash, AosRawSE::aos_compare_raw_file_tag> map_t;
	typedef map_t::iterator itr_t;

private:
	int									mTries;
	int									mBatchSize;
	int									mTestCount;
	u64									muFileCreated;
	u32									mSeedCount;
	i64									mTestDurationSec;
	u32									mMaxSiteID;
	u32									mMaxCubeID;
	u64									mMaxAseID;
	u64									mReadTestCount;
	u64									mCreateTestCount;
	u64									mDelTestCount;
	u64									mAppendTestCount;
	u64									mReadFailedCount;
	u64									mCreateFailedCount;
	u64									mDelFailedCount;
	u64									mAppendFailedCount;
	AosRundataPtr						mRundata;
	AosRawSEPtr							mRawSE;
	OmnFilePtr							mFile;
	map_t								mRawFileSeedMap;		//index of seeds
	std::map<u32, OmnString>			mSeedMap;				//seedID and seed content
	std::vector<AosRawFile::Identifier>	mvFile;
	static AosBuffPtr					ptrBuff;

public:
	AosRawSETester();
	~AosRawSETester() {}

	virtual bool		start();

private:
	bool	basicTest();

	bool	runOneTest();

	bool	oneBatchTests(const int batch_size);

	bool	testAppendData();

	bool	testCreateFile();

	bool	testDeleteFile();

	bool	testReadData();

	bool	loadSeed();

	u32		pickSiteid();

	u32		pickCubeid();

	u64		pickAseid();

	u32		pickSeed();

	OmnString	createData(const AosRawFile::Identifier &raw_file);
};
#endif

