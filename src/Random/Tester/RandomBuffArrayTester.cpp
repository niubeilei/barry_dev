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
// 2013/05/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/Tester/RandomBuffArrayTester.h"

#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "Util/UtUtil.h"
#include "Rundata/Rundata.h"
#include "Random/Ptrs.h"
#include "Random/RandomBuffArray.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosRandomBuffArrayTester::AosRandomBuffArrayTester()
{
}


bool 
AosRandomBuffArrayTester::start()
{
	int tries = 10;
	for (int i=0; i<tries; i++) 
	{
		testQuery1();
		testQuery2();
	}
	return true;
}


bool
AosRandomBuffArrayTester::testQuery1()
{
	const int record_len = 50;
	AosRandomBuffArray buff_array(record_len);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	int num_records = rand() % 1000;
	AosBuffPtr buff = buff_array.createNRecordToBuff(num_records, rdata);
	aos_assert_r(buff, false);

	const char *data = buff->data();
	const int64_t num_entries = buff->dataLen() / record_len;
	if (num_entries <= 0) return true;

	int idx1 = rand() % num_entries;
	int idx2 = rand() % num_entries;
	if (idx2 < idx1)
	{
		int nn = idx1;
		idx1 = idx2;
		idx2 = nn;
	}

	OmnString value1(&data[idx1*record_len], record_len);
	OmnString value2(&data[idx2*record_len], record_len);

	AosBitmapObjPtr bitmap;
	bool rslt = buff_array.query(rdata, eAosOpr_ge, value1, eAosOpr_le, value2, bitmap);
	aos_assert_r(rslt, false);
	aos_assert_r(bitmap, false);

	u64 docid;
	int num_checks = idx2 - idx1 + 1;
	OmnScreen << "nn: " << num_checks << endl;
	for (int i=0; i<num_checks; i++)
	{
		int idx = idx1 + i;
		OmnString record(&data[idx*record_len]);
		vector<OmnString> parts;
		int nn = AosStrSplit::splitStrByChar(record, "_", parts, 5);
		aos_assert_r(parts.size() == 3, false);
		rslt = AosStr2U64(parts[2].data(), parts[2].length(), true, docid);
		aos_assert_r(rslt, false);
		aos_assert_r(bitmap->checkDoc(docid), false);
		bitmap->removeDocid(docid);
	}
	aos_assert_r(bitmap->isEmpty(), false);
    return true;
}


bool
AosRandomBuffArrayTester::testQuery2()
{
	const int record_len = 50;
	AosRandomBuffArray buff_array(record_len);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	int num_records = rand() % 1000;
	AosBuffPtr buff = buff_array.createNRecordToBuff(num_records, rdata);
	aos_assert_r(buff, false);

	const char *data = buff->data();
	const int64_t num_entries = buff->dataLen() / record_len;
	if (num_entries <= 0) return true;

	int idx1 = rand() % num_entries;
	int idx2 = rand() % num_entries;
	if (idx2 < idx1)
	{
		int nn = idx1;
		idx1 = idx2;
		idx2 = nn;
	}

	OmnString value1(&data[idx1*record_len], record_len);
	OmnString value2(&data[idx2*record_len], record_len);

	u64 start_idx = 0;
	switch (rand() % 10)
	{
	case 0:
		 start_idx = num_records;
		 break;

	case 1:
		 start_idx = 0;
		 break;

	case 2:
		 start_idx = rand();
		 break;

	default:
		 start_idx = rand() % num_entries;
	}
	// start_idx = 0;

	u64 page_size;
	switch (rand() % 10)
	{
	case 0:
		 page_size = 0;
		 break;

	case 1:
		 page_size = num_entries;
		 break;

	case 2:
		 page_size = rand();
		 break;

	default:
		 page_size = rand() % num_entries;
	}
	// page_size = 10;

	vector<OmnString> values;
	vector<u64> docids;
	bool rslt = buff_array.query(rdata, eAosOpr_ge, value1, eAosOpr_le, value2, start_idx, 
			page_size, values, docids);
	aos_assert_r(rslt, false);

	if (start_idx + idx1 >= num_entries || page_size == 0)
	{
		aos_assert_r(docids.size() == 0, false);
		aos_assert_r(values.size() == 0, false);
		return true;
	}

	int num_checks = idx2 - idx1 + 1;
	if (start_idx >= num_checks)
	{
		aos_assert_r(docids.size() == 0, false);
		aos_assert_r(values.size() == 0, false);
		return true;
	}

	aos_assert_r(num_checks >= 0, false);
	u32 end_idx = start_idx + page_size;
	if (end_idx > num_checks) end_idx = start_idx + num_checks;

	u32 queried_entries = num_checks - start_idx;
	if (queried_entries > page_size) queried_entries = page_size;
	aos_assert_r(queried_entries == docids.size(), false);
	aos_assert_r(queried_entries == values.size(), false);

	u64 docid;
	OmnScreen << "nn: " << num_checks << endl;
	for (int idx=start_idx; idx<end_idx; idx++)
	{
		OmnString record(&data[(idx+idx1)*record_len]);
		vector<OmnString> parts;
		int nn = AosStrSplit::splitStrByChar(record, "_", parts, 5);
		aos_assert_r(parts.size() == 3, false);
		rslt = AosStr2U64(parts[2].data(), parts[2].length(), true, docid);
		aos_assert_r(rslt, false);

		OmnString ss = values[idx-start_idx];
		u64 dd = docids[idx-start_idx];
		aos_assert_r(record == ss, false);
		aos_assert_r(docid == dd, false);
	}
    return true;
}

