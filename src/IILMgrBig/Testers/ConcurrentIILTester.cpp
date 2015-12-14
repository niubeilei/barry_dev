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
// 2013/03/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/Testers/ConcurrentIILTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILClient/IILClient.h"
#include "IILMgrBig/IILMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/ValueRslt.h"
#include "Util/BuffArray.h"

static char *sgStrings[AosConcurrentIILTester::eNumStrings];
static OmnString sgDftIILName = "_zt44_test";
static u64 sgDocid = 10000;

AosConcurrentIILTester::AosConcurrentIILTester()
:
mRecordLen(eDftRecordLen),
mAllNums(0)
{
}


bool 
AosConcurrentIILTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(AosGetDftSiteId());
	OmnScreen << "    Start Tester ..." << endl;

	config();
	init();
	grandTorturer();
	return true;
}


bool
AosConcurrentIILTester::init()
{
	int len = (mRecordLen - sizeof(u64) - 1)/2;
	for (int i=0; i<eNumStrings; i++)
	{
		sgStrings[i] = OmnNew char[len+1];
		OmnRandom::letterDigitStr(sgStrings[i], len, len);
		sgStrings[i][len] = 0;
	}
	return true;
}


bool
AosConcurrentIILTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	mIILName = tag->getAttrStr("iilname", sgDftIILName);
	if (mIILName == "") mIILName = sgDftIILName;

	mRecordLen = tag->getAttrInt("record_len", eDftRecordLen);
	if (mRecordLen <= 0) mRecordLen = eDftRecordLen;

	mCompareFunc = OmnNew AosFunStrU641(mRecordLen);
	mKeyLen = (mRecordLen - sizeof(u64) - 1)/2*2;
	mBuffArray = OmnNew AosBuffArray(mCompareFunc, true, false, eBuffLength);

	mBatchSize = tag->getAttrInt("batch_size", eDftBatchSize);
	if (mBatchSize <= 0) mBatchSize = eDftBatchSize;

	mPageSize = tag->getAttrInt("page_size", eDftPageSize);
	if (mPageSize <= 0) mPageSize = eDftPageSize;
	
	OmnString type = tag->getAttrStr("type", "index");
	mOpr = AosDataColOpr::toEnum(type);
	if (!AosDataColOpr::isValid(mOpr))
	{
		OmnAlarm << "operator error!" << enderr;
	}
	if (type != "index")
	{
		mIncType = AosIILUtil::IILIncType_toEnum(type);
	}
	return true;
}


bool 
AosConcurrentIILTester::basicTest()
{
	return true;
}


bool 
AosConcurrentIILTester::grandTorturer()
{
	vector<int> weights1;
	int total = 20; 	weights1.push_back(total); 	// type = index
	total += 20; 	 	weights1.push_back(total);	// type = norm
	total += 20; 	 	weights1.push_back(total);	// type = set
	total += 20; 	 	weights1.push_back(total);	// type = noupdate
	total += 20; 	 	weights1.push_back(total);	// type = min
	total += 20; 	 	weights1.push_back(total);	// type = max
	
	vector<int> weights;
	total = 100; 		weights.push_back(total); 	// Create Add contents
	total += 10; 	 	weights.push_back(total);	// Check 
	total += 10; 	 	weights.push_back(total);	// Check 

	bool rslt;
	OmnScreen << "MMMMM: mTries: " << mTries << endl;
	OmnString name = mIILName;
	for (int j=0; j<4000; j++)
	{
		mIILName = name;
		mIILName << "_" << j;
		mBuffArray->clear();
		mAllNums = 0;
		OmnString inctype = "index";
		int type = pickOperation(weights1);
		switch(type)
		{
			case 0:
				inctype = "index";
				break;
			case 1:
				inctype = "norm";
				break;
			case 2:
				inctype = "set";
				break;
			case 3:
				inctype = "noupdate";
				break;
			case 4:
				inctype = "min";
				break;
			case 5:
				inctype = "max";
				break;
			default:
				break;
		}
		mOpr = AosDataColOpr::toEnum(inctype);
		if (!AosDataColOpr::isValid(mOpr))
		{
			OmnAlarm << "operator error!" << enderr;
		}
		if (inctype != "index")
		{
			mIncType = AosIILUtil::IILIncType_toEnum(inctype);
		}
OmnScreen << "MMMMMM j:" << j << " IncType : " << inctype << endl;
		for (int i=0; i<mTries; i++)
		{
			int opr = pickOperation(weights);
			rslt = false;
			switch (opr)
			{
				case 0:
					OmnScreen << "MMMMM j:" << j << "  i:"<< i << ", Add contents" << endl;
					rslt = addContents();
					break;

				case 1: 
					OmnScreen << "MMMMM j:" << j << "  i:" << i << ", Check " << endl;
					rslt = checkResults();
					break;

				case 2:
					OmnScreen << "MMMMM j:" << j << "  i:"<< i << ", Merge " << endl;
					rslt = mergeIIL();
					break;

				default:
					OmnAlarm << "Invalid operator: " << opr << enderr;
					rslt = false;
					break;
			}

			if (!rslt)
			{
				OmnAlarm << "Failed operation: " << opr << enderr;
			}
		}
	}
	return true;
}


int
AosConcurrentIILTester::pickOperation(const vector<int> &weights)
{
	int num = rand() % 120;
	for(int i=0; i<weights.size(); i++)
	{
		if (num <= weights[i]) return i;
	}
	return 0;
}

bool
AosConcurrentIILTester::addContents()
{
	// It randomly generates some contents and then add the contents
	// to the IIL.
	aos_assert_r(mBatchSize > 0, false);
	int nn = 150;//rand() % mBatchSize;
	//int idx = eNumStrings;
	//int delta = (rand() % 4) + 1;
	int idx = eNumStrings-1;
	int delta = (rand() % 3) + 1;
	char buff[mRecordLen+1];
	int str_len = (mRecordLen-sizeof(u64)-1)/2;

	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(
			mCompareFunc, true, false, nn * mRecordLen + 1000);
	for (int i=0; i<nn; i++)
	{
		memcpy(buff, sgStrings[idx], str_len);
		idx += delta; 
		if (idx >= eNumStrings)
		{
			idx -= eNumStrings;
		}
		memcpy(&buff[str_len], sgStrings[idx], str_len);
		idx += delta;
		if (idx >= eNumStrings)
		{
			idx -= eNumStrings;
		}
		buff[str_len*2] = 0;
		
		if (sgDocid == 0)
		{
			sgDocid = 10000;
		}
		u64 docid = sgDocid--;
		memcpy(&buff[mRecordLen-sizeof(u64)], &docid, sizeof(u64));
		OmnScreen << "@@@@@@@ add Entry value : " << buff << "  docid : " << docid << endl;
		buff_array->addValue(buff, mRecordLen, mRundata);
	}

	buff_array->sort();

	// Add to the buff
	mBuffArray->merge(mRundata, buff_array, mOpr);
	mAllNums += nn;
	OmnScreen << "####### addtoiil : " << mAllNums << "  mBuffArray : " << mBuffArray->getNumEntries() << endl;
	// Add to the IIL.
	// (TBD)
	addToIIL(buff_array, mRundata);
	return true;
}

bool
AosConcurrentIILTester::addToIIL(
		const AosBuffArrayPtr &buff_array,
		const AosRundataPtr &rdata)
{
	AosBuffPtr buff = buff_array->getBuff();
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(mRecordLen > 0, rdata, false);                    
	char *entries = buff->data();
	int num = buff->dataLen() / mRecordLen;
	if (num <= 0) return true;
	bool rslt;
	switch(mOpr)
	{
		case AosDataColOpr::eNormal:
		case AosDataColOpr::eNoUpdate:
		case AosDataColOpr::eMinValue:
		case AosDataColOpr::eMaxValue:
		case AosDataColOpr::eSetValue:
			rslt = AosIILMgrObj::getIILMgr()->StrIncBlockSafe(    
					mIILName, entries, mRecordLen, num, 0, mIncType, false, rdata);
			break;
		default:
			AosRundataParmPtr parm = rdata->getParm(AosRundataParm::eIILExecutor);
			AosIILExecutorObjPtr executor;
			if (parm)
			{
				AosRundataParm::Type type = parm->getType();
				aos_assert_rr(type == AosRundataParm::eIILExecutor, rdata, false);
				executor = (AosIILExecutorObj *)parm.getPtr();
			}
			rslt = AosIILMgrObj::getIILMgr()->StrAddBlockSafe(    
					mIILName, entries, mRecordLen, num, executor, false, rdata);
			break;
	}
	aos_assert_r(rslt, false);
	return true;
}

bool
AosConcurrentIILTester::checkResults()
{
	// The IIL should be the same as 'mBuffArray'. It randomly
	// picks a range. It then queries the IIL. 
	int num_rcds = mBuffArray->getNumEntries();
	if (num_rcds <= 0) return true;
	int start = OmnRandom::intByRange(
					0, 0, 50, 
					1, num_rcds/6, 50,
					num_rcds/6+1, num_rcds/3, 50, 
					num_rcds/3+1, num_rcds/2, 50);
	if (start >= mBuffArray->size()) return true;

	int end = OmnRandom::intByRange(
					start, start, 50, 
					start+1, start+num_rcds/6, 50, 
					start+num_rcds/6+1, start+num_rcds/3, 50, 
					start+num_rcds/3+1, num_rcds-1, 50);
	
//start =0;
//end = num_rcds-1;
	if (start > end) 
	{
		int nn = start;
		start = end;
		end = nn;
	}

	if (end > num_rcds)
	{
		OmnAlarm << " check results failed " << enderr;
		return true;
	}

	// The values being picked are:
	// 		[start, end]
	int record_len;
	char *start_record = mBuffArray->getRecord(start, record_len);
	aos_assert_r(start_record, false);
	aos_assert_r(record_len == mRecordLen, false);

	// Need to find the first entry (starting from 'start') whose value
	// is the same as 'start_record'.
	while (start > 0)
	{
		char *prev_record = start_record-record_len;
		if (strncmp(prev_record, start_record, mKeyLen) != 0) break;
		start_record -= record_len;
		start--;
	}

	char *end_record = mBuffArray->getRecord(end, record_len);
	aos_assert_r(end_record, false);
	aos_assert_r(record_len == mRecordLen, false);

	// Need to find the last entry (starting from 'end') whose
	// value is the same as 'end_record'.
	while (end < mBuffArray->size()-1)
	{
		char *next_record = end_record+record_len;
		if (strncmp(next_record, end_record, mKeyLen) != 0) break;
		end_record += record_len;
		end++;
	}
	// Now we obtained two records:
	// 		[start_record, end_record]
	// This is the condition to query the IIL.
	/*AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_r(queryobj, false);
	int64_t startidx = 0;
	int psize = (rand() % mPageSize) + 1;
	
	AosOpr opr = eAosOpr_range_ge_le;
	bool reverse = false;
	AosValueRslt value1(start_record, mRecordLen);
	AosValueRslt value2(end_record, mRecordLen);
	AosQueryRsltObjPtr query_rslt;
	char *record = start_record;
	*/
OmnScreen << "num_rcds:" << num_rcds << " start:" << start << " end:" << end << " start_record:" << start_record << " end_record:" << end_record << endl;	
	u32 psize = (rand() % mPageSize) + 1;
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();   
	query_rslt->setWithValues(true);
	//query_rslt->setOperator(mOpr);
	//query_context->setOpr(eAosOpr_an);
	//query_context->setStrValue("*");
	//query_context->setPageSize(1000000);
	query_context->setOpr(eAosOpr_range_ge_le);
	query_context->setStrValue(start_record);
	query_context->setStrValue2(end_record);
	query_context->setPageSize(psize);
	query_context->setBlockSize(1000000);
	
	char *record = start_record;
	if (record != end_record)
	{
		//bool rslt = queryobj->runQuery(startidx, psize, mIILName, 
		//				value1, value2, opr, query_rslt, reverse, true, mRundata);
		bool rslt = AosIILClient::getSelf()->querySafe3(mIILName, query_rslt, 0, query_context, mRundata);
		aos_assert_r(rslt, false);
		aos_assert_r(query_rslt, false);
		//aos_assert_r(psize > 0, false);

		bool finished = false;
		u64 docid;
		OmnString value;
		int loop = 0;
		while (!finished && record != end_record)
		{
			if (loop > psize) break;
			bool rslt = query_rslt->nextDocidValue(docid, value, finished, mRundata);
			aos_assert_r(rslt, false);
			OmnScreen << "value : " << value << "  docid : " << docid << endl;
			OmnScreen << "recor : " << record << "  docid : " << *(u64*)&record[mRecordLen-8] << endl;
			aos_assert_r(strncmp(record, value.data(), mKeyLen) == 0, false);
			aos_assert_r(*(u64*)&record[mRecordLen-8] == docid, false);
			record += mRecordLen;
			loop++;
		}
	}

	//aos_assert_r(record == end_record, false);
	return true;
}


bool
AosConcurrentIILTester::mergeIIL()
{
return true;
	OmnNotImplementedYet;
	return false;
}

