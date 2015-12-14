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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_Testers_RecordTester_h
#define Aos_DataBlob_Testers_RecordTester_h

#include "DataBlob/Ptrs.h"
#include "DataRecord/Ptrs.h"
#include "DataRecord/DataRecord.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosRecordTester : virtual public OmnTestPkg
{
private:
	u64								mMaxRcds;
	u64								mMoreRcds;
	u64								mMaxSize;
	int								mRcdSize;
	bool							mWithDocid;
	bool							mSetFieldNull;
	bool							mSetTnailNull;
	OmnString						mType;
	OmnString						mTooShortPolicy;
	OmnString						mTooLongPolicy;
	OmnString						mNullPolicy;
	vector<u64>						mU64Keys;
	vector<OmnString>				mStrKeys;
	vector<u64>						mDocids;
	AosDataRecordPtr				mDataRecord;
	AosDataBlobPtr					mBlob;

public:
	AosRecordTester();
	~AosRecordTester();

	bool run(const AosRundataPtr &rdata);

private:
	int				getPageSize();
	int 			getRcdSize();
	int				getMaxSize(const AosXmlTagPtr &conf);
	int				getMaxRcds(const AosXmlTagPtr &conf);
	bool			initKey();
	bool			initU64U64();
	bool			initStrU64();
	bool			initCustom();
	bool			initDocid();
	bool			splitKey(const OmnString &key, const int64_t idx);
	bool			basicTest(const AosRundataPtr &rdata);
	bool			appendValue(const AosRundataPtr &rdata);
	bool			addMoreData(const AosRundataPtr &rdata);
	bool			initDataRecord(const AosRundataPtr &rdata);
	bool			checkSerialize(const AosRundataPtr &rdata);
	bool			checkValueByIdx(const AosRundataPtr &rdata);
	bool			checkValueInOrder(const AosRundataPtr &rdata);
	bool			rewriteRecordByIdx(const AosRundataPtr &rdata);
	bool			rewriteRecordInOrder(const AosRundataPtr &rdata);
	bool			checkFullInBackground(const AosRundataPtr &rdata);
	bool			checkFullInMultiThreads(const AosRundataPtr &rdata);
	bool			checkFullInCurrentThread(const AosRundataPtr &rdata);
	bool			initKeyValue(const u64 &idx, const AosRundataPtr &rdata);
	bool			oneTry(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	OmnString		setTnailNull();
	OmnString		getType();
	OmnString		setFieldNull();
	OmnString		withDocid();
	OmnString		getTooShortPolicy();
	OmnString		getTooLongPolicy();
	OmnString		getNullPolicy();
	AosXmlTagPtr	createConfig(const AosXmlTagPtr &conf);

};
#endif
