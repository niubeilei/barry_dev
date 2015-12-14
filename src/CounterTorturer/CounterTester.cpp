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
// 2011/06/15	Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "CounterTorturer/CounterTester.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "QueryClient/QueryClient.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "UtilTime/TimeInfo.h"
#include "Util1/Time.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterName.h"
#include "CounterUtil/CounterQueryType.h"
#include "DocClient/DocClient.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEModules/SeRundata.h"
#include "SearchEngine/DocServer.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Porting/Sleep.h"

int	 		AosCounterTester::mCounterId_Idx[eMaxThreadNum];
bool	 	AosCounterTester::mFlag[eMaxThreadNum];

const u32 sgRandSize = 1000;
static u32 sgRandNums[sgRandSize];
static OmnString sgRandStrs[sgRandSize];
static u32 sgRandIdx = 0;

AosCounterTester::AosCounterTester()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);

	for (u32 i=0; i<sgRandSize; i++)
	{
		sgRandNums[i] = rand();
		//sgRandStrs[i] = OmnRandom::lowcaseStr(2, 30);
		sgRandStrs[i] = OmnRandom::lowcaseStr(2, 10);
	}

	for (u32 i = 0; i< eMaxThreadNum; i++)
	{
		mCounterId_Idx[i] = 0;
		mFlag[i] = false;
	}
	mCacheNum = 1000;

}

AosCounterTester::~AosCounterTester()
{
}


bool 
AosCounterTester::start()
{
	OmnScreen << "    Start Counter Tester ..." << endl;
	return basicTest5(0);
	//start thread
	OmnThreadedObjPtr thisPtr(this, false);
	static OmnThreadPtr thread[eMaxThreadNum];
	for (int i = 0; i< eMaxThreadNum; i++)
	{
		thread[i] = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		thread[i]->start();
	}

	while(1)
	{
		OmnSleep(10000);
	}
	return true;
}

bool
AosCounterTester::threadFunc(OmnThrdStatus::E &state,const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		u32 thrdid = thread->getLogicId();//= thread->getThreadId();
		basicTest(thrdid);		
	}
	return true;
}

bool
AosCounterTester::signal(const int threadLogicId)
{
	return true;
}

bool
AosCounterTester::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool 
AosCounterTester::basicTest(const int thrdid)
{
	// This basic test simle creates counters. 
//	u64 starttime = OmnGetSecond();

//  int tries = mTestMgr->getTries();
//  OmnScreen << "Tries: " << tries << endl;
//  if (tries <= 0) tries = eDefaultTries;
	
//	tries = 0;
	AosRundataPtr rdata = OmnApp::getRundata();

//	for(int i = 0; i< tries; i++)
//	{
//		if (i % 100000 == 0) 
//		{
//			u32 delta = OmnGetSecond() - starttime;
//			if (delta > 0)
//			{
//				OmnScreen << "Trying: " << i << ". Speed: " << i / delta << endl;
//			}
//		}
		//mLock->lock();
		updateCounter(rdata, thrdid);
		//mLock->unlock();
//	}

	return true;
}


bool
AosCounterTester::updateCounter(const AosRundataPtr &rdata, const u32 &thrdid)
{
	OmnString cname;
	int64_t value;
	aos_assert_r(createCname(thrdid, cname, value), false);
	aos_assert_r(cname != "", false);
	aos_assert_r(value, false);
	aos_assert_r(addCounter(rdata, cname, value), false);
//if (mCounterId_Idx[thrdid]%10 == 0) OmnSleep(1);
OmnSleep(1);
	if (mCounterId_Idx[thrdid] > mCacheNum)
	{
//if (!mFlag[thrdid])
//{
//	mFlag[thrdid] = true;
//	OmnSleep(30);
//}
//else
//{
//if (!mCounterId_Idx[thrdid]%10) OmnSleep(1);
//OmnSleep(1);
//}
		OmnString counter_id;
		value = retrieveSingleCounter(rdata, thrdid, counter_id);
		aos_assert_r(counter_id != "", false);
		aos_assert_r(value > 0, false);
		aos_assert_r(verifyValue(counter_id, value), false);
	}
	return true;
}

bool
AosCounterTester::createCname(const u32 &thrdid, OmnString &cname, int64_t &value)
{
	OmnString lsCounterId = "counter";
	int r = rand() % 100;
	if (r < 60)
	{
		lsCounterId << "_" << thrdid<< "_" << mCounterId_Idx[thrdid];
		mCounterId_Idx[thrdid] ++;
	}
	else
	{
		if (mCounterId_Idx[thrdid] == 0)
		{
			lsCounterId << "_" << thrdid<< "_" << mCounterId_Idx[thrdid];
			mCounterId_Idx[thrdid] ++;
		}
		else
		{
			int old_idx = rand() % mCounterId_Idx[thrdid]; 
			lsCounterId <<"_" << thrdid <<"_"<< old_idx; 
		}
	}

	if (sgRandIdx >= sgRandSize - 10) sgRandIdx = 0;
	int num_segs = (sgRandNums[sgRandIdx++] % 6) + 1;
	cname = lsCounterId;
	num_segs = 2;
	for (int i=0; i<num_segs; i++)
	{
		if (cname != "") cname << AOS_COUNTER_SEGMENT_SEP;
		cname << sgRandStrs[sgRandIdx++];
	}
	value = rand()%10000 +1;
	//value = 1;
	mLock->lock();
	if (mCnames.count(lsCounterId) == 0)
	{
		vector<OmnString> counter_name; 
		counter_name.push_back(cname);
		mCnames[lsCounterId] = counter_name; 
		mValues[lsCounterId] = value;
	}
	else
	{
		mCnames[lsCounterId].push_back(cname);
		int64_t vv = mValues[lsCounterId];
		mValues[lsCounterId] = vv + value;
	}
	mLock->unlock();
	return true;
}


int64_t
AosCounterTester::retrieveSingleCounter(const AosRundataPtr &rdata, const u32 &thrdid, OmnString &counter_id)
{
	counter_id = "counter"; 
	int old_idx = rand() % (mCounterId_Idx[thrdid] - 50); 
	counter_id << "_" << thrdid <<"_"<< old_idx; 
	//OmnString cname = mCnames[counter_id];
	vector<OmnString> counter_name = mCnames[counter_id];
	aos_assert_r(counter_name.size()!=0, -1);
	int idx = rand()%counter_name.size();
	OmnString cname = counter_name[idx];
	aos_assert_r(cname != "", -1);

	vector<AosStatType::E>  statTypes;
	statTypes.push_back(AosStatType::toEnum("sum"));
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	OmnString ct_id = AosIILName::composeCounterIILName(counter_id);
OmnString counter_nameStr = counter_id;
/*
	OmnString counter_nameStr;
	bool finished = false;
	vector<OmnString> str;
	AosStrSplit::splitStrByChar(cname.data(), AOS_COUNTER_SEGMENT_SEP, str, 100, finished);
	u32 size = str.size();
	aos_assert_r(size != 0, false);
	for (u32 i = 0; i< size-1; i++)
	{
		if (counter_nameStr != "") counter_nameStr << AOS_COUNTER_SEGMENT_SEP;
		counter_nameStr << str[i]; 
	}
	if (counter_nameStr != "") counter_nameStr << AOS_COUNTER_PART_SEP;
	counter_nameStr << str[size-1]; 
*/
	bool rslt = AosRetrieveSingleCounter(ct_id, counter_nameStr, statTypes, 1, 0, 0, buff, rdata);
	buff->reset();
	aos_assert_r(rslt, -1);
	int64_t vv = buff->getInt64(0);
	return vv;
}

bool
AosCounterTester::verifyValue(const OmnString &counter_id, const int64_t &value)
{
	int64_t vv = mValues[counter_id]; 
//vv = mCnames[counter_id].size();
	aos_assert_r(vv == value, false);
	return true;
}

bool
AosCounterTester::addCounter(const AosRundataPtr &rdata, const OmnString &cname, const int64_t &value)
{
	OmnString timeStr = "2012/06/08 10:55:22";
	bool rslt = AosCounterClt::getSelf()->procCounter(
			cname, value, AosTimeGran::eDaily,
			timeStr, "%Y/%m/%d %H:%M:%S", "sum", "2fmt", "set", rdata);
	aos_assert_r(rslt, false);
	return true;
}

/*
bool
AosCounterTester::parseCname(const OmnString &cname)
{
	vector<OmnString> containers, members;
	AosStrSplit::splitStrBySubstr(name, AOS_COUNTER_SEGMENT_SEP,
			        containers, members, 1000);
}
*/

bool 
AosCounterTester::basicTest1(const int thrdid)
{
	// This basic test simle creates counters. 
	u64 starttime = OmnGetSecond();

    int tries = mTestMgr->getTries();
    OmnScreen << "Tries: " << tries << endl;
    if (tries <= 0) tries = eDefaultTries;
	
	tries = 1;
	AosRundataPtr rdata = OmnApp::getRundata();

	for(int i = 0; i< tries; i++)
	{
		if (i % 100000 == 0) 
		{
			u32 delta = OmnGetSecond() - starttime;
			if (delta > 0)
			{
				OmnScreen << "Trying: " << i << ". Speed: " << i / delta << endl;
			}
		}
		addCounter(thrdid);
	}

	// run Query
	OmnSleep(10);
	createDoc(rdata);
	for (int i = 0; i<1; i++)
	{
		runQuery(rdata);
	}
	return true;
}


bool
AosCounterTester::addCounter(const int thrdid)
{
	static OmnString lsCounterId = "counter123";

	if (sgRandIdx >= sgRandSize - 10) sgRandIdx = 0;
	int num_segs = (sgRandNums[sgRandIdx++] % 6) + 1;
	OmnString cname;
	for (int i=0; i<num_segs; i++)
	{
		if (cname != "") cname << AOS_COUNTER_SEGMENT_SEP;
		cname << sgRandStrs[sgRandIdx++];
	}
	//cname = "co.cool.cn.js.sz";
	cname = "cool.cn.js.sz";
//int r = rand()%5; 
//int r = rand()%3;
//if (r == 0) cname = "cool|$$|cn|$$|js|$$|sz";
//if (r == 1) cname = "cool|$$|cn|$$|js|$$|wx";
//if (r == 2) cname = "cool|$$|cn|$$|hl|$$|cs";
//if (r == 3) cname = "cool|$$|cn|$$|hl|$$|zz";
//if (r == 4) cname = "cool1";
	bool rslt = procCounter(cname, mRundata);
	aos_assert_rr(rslt, mRundata, false);
	return true;
}

bool
AosCounterTester::procCounter(
			const OmnString &cname,
			const AosRundataPtr &rdata)
{
	static int v0 = 0;
	static int v1 = 0;
	static int v2 = 0;
	static int v3 = 0;
	static int v4 = 0;
	static int v5 = 0;
	int64_t  value = 100;
	v0 += value;
	int vv = rand()%6;
	//int vv = rand()%2;
	OmnString timeStr = "2012/05/19 00:00:00";
	if (vv == 1)
	{
		value = 100;
		//timeStr = "2012/04/26 00:00:00";
		timeStr = "04-26-2012 00:00:00";
		v1 += value;
	}
	if (vv == 2)
	{
		value = 100;
		//timeStr = "2012/03/26 00:00:00";
		timeStr = "03-26-2012 00:00:00";
		v2 += value;
	}
	if (vv == 3)
	{
		value = 100;
		//timeStr = "2012/05/02 00:00:00";
		timeStr = "05-02-2012 00:00:00";
		v3 += value;
	}
	if (vv == 4)
	{
		value = 100;
		//timeStr = "2012/05/01 00:00:00";
		timeStr = "05-01-2012 00:00:00";
		v4 += value;
	}

	if (vv == 5)
	{
		value = 100;
		//timeStr = "2012/05/16 00:00:00";
		timeStr = "05-16-2012 00:00:00";
		v5 += value;
	}

	bool rslt = AosCounterClt::getSelf()->procCounter(
			cname, value, AosTimeGran::eDaily,
			timeStr, "mm-dd-yyyy", "sum", "2fmt", "add", rdata);

//	value = 1;
//	rslt = AosCounterClt::getSelf()->procCounter(
//			cname, value, AosTimeGran::eDaily,
//			timeStr, "%Y/%m/%d %H:%M:%S", "avg", "2fmt", rdata);
//OmnScreen << v0 << ":" << v1 <<":" << v2 <<":" << v3 <<":" << v4 <<":" << v5 << endl;
	return rslt;
}


bool
AosCounterTester::runQuery(const AosRundataPtr &rdata)
{
	OmnString reqStr = composeSingleReq();
	aos_assert_r(reqStr != "", false);
    AosXmlParser parser;
    AosXmlTagPtr childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	AosQueryReqPtr query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	OmnString contents;
	bool rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents<< endl;

	reqStr = composeMultiReq();
    childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	contents = "";
	rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents<< endl;

	reqStr = composeSubReq();
    childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	contents = "";
	rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents<< endl;
	return true;
}

OmnString
AosCounterTester::composeSingleReq()
{
	OmnString reqStr = "<cmd psize='30' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' "
		<< " zky_counter_id = 'cool' avg='true' factor1 = '0.01' "
		<< "sum='true' type= 'counterSingle'>"
			<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-03-01 00:00:00' "
			<< "zky_endtime='2012-05-30 00:00:00' zky_timetype='d' zky_timegran='day' >"
			<< "</zky_time_condition>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cool]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[js]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";

	return  reqStr;
}

OmnString
AosCounterTester::composeMultiReq()
{
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'>"
		<< "<term type='arith' zky_ctobjid='giccreators_querynew_term_h' order='false' reverse='false'>"
		<< "<selector type='attr' aname='zky_pctrs'/>"
		<< "<cond type='arith' ctype='const' zky_opr='eq'><![CDATA[datagentest_dep1]]></cond>"
		<< "</term>"
		<< "</cond>"
		<< "<stat>"
		<< "<term zky_ctobjid='giccreators_query_cond_countermulti_h' type='counterMulti'"
		<< " zky_counter_id = 'cool' avg='true' factor1 = '0.01'"
		<< "sum='true'>"
		<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime1='2012-05-01 00:00:00' "
		<< "zky_endtime1='2012-05-09 00:00:00' zky_timegran='day' zky_timetype='d' >"
		<< "</zky_time_condition>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cool]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[js]]></termname>"
		<< "<termname zky_value_type='attr' zky_ctobjid='vpd_countername_varterm_h' zky_xpath='zky_a'>"
		<< "<zky_docselector zky_type='retrieveddoc'/>"
		<< "</termname>"
		<< "</counter_names>"
		<< "</term>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<<"<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_sum</oname>"
		<< "<cname>zky_sum</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";
	return reqStr;
}


OmnString
AosCounterTester::composeSubReq()
{	
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' "
		<< " zky_counter_id = 'cool' factor1 = '0.01'"
		<< "sum='true' arg = 'true' type= 'counterSub'>"
		 	<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-05-10 00:00:00' "
			<< "zky_endtime='2012-05-19 00:00:00' zky_timegran='day' zky_timetype='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cool]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<<"<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_sum</oname>"
		<< "<cname>zky_sum</cname>"
		<< "</fname>"
		<<"<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_avg</oname>"
		<< "<cname>zky_avg</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";

	return reqStr;
}

OmnString
AosCounterTester::composeSub2Req()
{	
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h'  "
		<< " zky_level = '3' zky_x1='2' zky_y1 = '1'"
		<< " zky_counter_id = 'test' use_epochtime = 'true' use_iilname='true'"
		<< " factor1 = '0.01'"
		<< "sum='true' max = 'true' avg1 = 'true' type= 'counterSub2'>"
			<< "<aggregate  zky_aggregate_type='mergeEntries' zky_mergefield_index = '0' "
			<< " />"
		 	<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-06-07 00:00:00' "
			<< "zky_endtime='2012-06-15 00:00:00' zky_timegran='day' zky_timetype='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[cool]]></termname>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[cn]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>xml</oname>"
		<< "<cname>xml</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";

	return reqStr;
}

OmnString
AosCounterTester::composeSub2Req2()
{	
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h'  "
		<< " zky_level = '2' zky_x = '0' zky_y = '1' zky_ypsize1='5' "
		<< " zky_counter_id = 'test' use_epochtime = 'true' use_iilname='true'  factor1 = '0.01'"
		<< "sum='true' max = 'true' avg1 = 'true' type= 'counterSub2'>"
			<< "<aggregate  zky_mergefield_type ='u64' zky_aggregate_type1='mergeEntries' "
			<< " zky_mergefield_index = '0' zky_mergefield_index21 = '2' >"
//			<< "<child zky_value ='3' zky_key='call_0' />"
//			<< "<child zky_value ='9' zky_key='call_1' />"
//			<< "<child zky_value ='19' zky_key='call_2' />"
//			<< "<child zky_value ='29' zky_key='call_4' />"
//			<< "<child zky_value ='39' zky_key='call_5' />"
//			<< "<child zky_key='call_6' />"
			<< "</aggregate>"
		 	<< "<zky_time_condition zky_timeformat='2fmt' "
			<< " zky_starttime1='2012-06-10 00:00:00' zky_timeopr1='or' "
			<< "zky_endtime1='2012-06-12 00:00:00' zky_timegran1='day' zky_timetype1='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[cool0]]></termname>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[cn]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>xml</oname>"
		<< "<cname>xml</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";

	return reqStr;
}



bool
AosCounterTester::createDoc(const AosRundataPtr &rdata)
{
	OmnString objid = "datagentest_dep1";
	if (AosDocClient::getSelf()->getDocByObjid(objid, rdata)) return  true;
	OmnString docstr = "<container zky_objid=\"datagentest_dep1\" "; 
	docstr << "zky_otype=\"zky_ctnr\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" />";
	AosDocClient::getSelf()->createDocSafe1(
			    rdata, docstr, "", "", true, false,
				false, false, true, true);
	aos_assert_r(AosDocClient::getSelf()->getDocByObjid("datagentest_dep1", rdata), false);

	OmnString docstr1 =  "<vpd zky_pctrs=\"datagentest_dep1\" zky_otype=\"zky_ctnr\" ";
	docstr1 << "zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_objid=\"datagentest1\" zky_a=\"sz\" />";
	AosDocClient::getSelf()->createDocSafe1(
			    rdata, docstr1, "", "", true, false,
				false, false, true, true);
	
	aos_assert_r(AosDocClient::getSelf()->getDocByObjid("datagentest1", rdata), false);

	OmnString docstr2 = "<vpd zky_pctrs=\"datagentest_dep1\" zky_otype=\"zky_ctnr\" ";
	docstr2 << "zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_objid=\"datagentest2\" zky_a=\"aaa\" />";
	AosDocClient::getSelf()->createDocSafe1(
			    rdata, docstr2, "", "", true, false,
				false, false, true, true);

	return true;
}


////////////////////////////// test Sub2 /////////////////////////

bool 
AosCounterTester::basicTest2(const int thrdid)
{
	// This basic test simle creates counters. 
	u64 starttime = OmnGetSecond();

    int tries = mTestMgr->getTries();
    OmnScreen << "Tries: " << tries << endl;
    if (tries <= 0) tries = eDefaultTries;
	
	tries = 100;
	AosRundataPtr rdata = OmnApp::getRundata();

	for(int i = 0; i< tries; i++)
	{
		if (i % 100000 == 0) 
		{
			u32 delta = OmnGetSecond() - starttime;
			if (delta > 0)
			{
				OmnScreen << "Trying: " << i << ". Speed: " << i / delta << endl;
			}
		}
		addCounter2(rdata);
	}

	// run Query
	OmnSleep(15);
	createDoc(rdata);
	for (int i = 0; i<1; i++)
	{
		runQuery2(rdata);
	}
	return true;
}


bool
AosCounterTester::addCounter2(const AosRundataPtr &rdata)
{
	OmnString cname = createCname2();
	OmnString iilname = "test";
	u64 value = rand() %100;
	u64 docid = 0;
	bool rslt = AosIncrementKeyedValue(iilname, cname, docid, true, value, 0, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}


u16
AosCounterTester::createEntryType2(const int level)
{
	// EntryTypecool.cn.time.1
	int append_bit = 0; 
	AosStatType::E stat_type;
	//if (rand()%2)
	//{
		stat_type = AosStatType::toEnum("sum"); 
	//}
	//else
	//{
	//	stat_type =  AosStatType::toEnum("max");
	//}
	//AosTimeGran::E time_gran = AosTimeGran::toEnum("day"); 
	AosTimeGran::E time_gran = AosTimeGran::toEnum("ntm"); 
	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, stat_type, time_gran);
	OmnScreen << entry_type << endl;
	return entry_type;
}


OmnString
AosCounterTester::createCname2()
{
	AosTimeFormat::E format = AosTimeFormat::toEnum("yyyymmddhhmmss");
	OmnString timeStr;
	switch(rand()%4)
	{
	case 0:
		 timeStr = "20120609101011";
		 break;
	case 1:
		 timeStr = "20120610101011";
		 break;
	case 2:
		 timeStr = "20120611101011";
		 break;
	case 3:
		 timeStr = "20120612000000";
		 break;
	}

	int num = rand()%50;
	int level;
	OmnString cname;
	int r = rand() % 2 + 1;
	r = 2;
	//u16 entry_type1  = *(u16 *)name.data();	
	switch(r)
	{
	case 4:
		{
			level = 3;
			u16 entry_type = createEntryType2(level); 
			OmnString name((char *) &entry_type, sizeof(u16));
			//name << "cool" << "\1" << "cn" << "\1";
			//name << AosTimeFormat::strToEpochDay(timeStr, format) << ".";
			//name << "星期" << rand()%7 + 1 << ".";
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "星期" << rand()%7 + 1 << "\1";
			name << num;
			cname = name;
			break;
		}
	case 3:
		{
			level = 3;
			u16 entry_type = createEntryType2(level);
			OmnString name((char *) &entry_type, sizeof(u16));
			name << "cool" << "\1";
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "cn" << num;
			cname = name;
			break;
		}
	case 2:
		{
			level = 2;
			u16 entry_type = createEntryType2(level);
			OmnString name((char *) &entry_type, sizeof(u16));
//			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
//			int rr = rand() %3;
//			if (rr == 0)
//			{
//				name << "语音" << "\1";
//			}
//			else
//			{
//				if (rr == 1)
//				{
//					name << "短信" << "\1";
//				}
//				else
//				{
//					name << "VPN" << "\1";
//				}
//			}
			name << "cool" << rand() % 3 << "\1";
			name << "合肥" << rand()%50 + 1;
			cname = name;
			break;
		}
	case 1:
		{

			level = 2;
			u16 entry_type = createEntryType2(level);
			OmnString name((char *) &entry_type, sizeof(u16));
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "call_" << num;
			cname = name;
			break;
		}
	}


//entry_type1  = *(u16 *)cname.data();	
OmnScreen << "cname : " <<cname << endl;

	return cname;
}

bool
AosCounterTester::runQuery2(const AosRundataPtr &rdata)
{
	OmnString reqStr = composeSub2Req2();
	aos_assert_r(reqStr != "", false);
    AosXmlParser parser;
    AosXmlTagPtr childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	AosQueryReqPtr query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	OmnString contents;
	bool rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents<< endl;

	reqStr = composeSub2Req();
	aos_assert_r(reqStr != "", false);
    childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents<< endl;
	return true;
}


////////////////////////////// test  Sub2 /////////////////////////


////////////////////////////// test Mutil Sub /////////////////////////

bool
AosCounterTester::basicTest3(const int thrdid)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	int tries = 1000;
	for(int i = 0; i< tries; i++)
	{
		addCounter3(rdata);
	}

	OmnSleep(10);
	for (int i = 0; i < 10; i++)
	{
		runQuery3(rdata);
	}
	return true;
}


bool
AosCounterTester::addCounter3(const AosRundataPtr &rdata)
{
	OmnString counter_id;
	int r = rand()%5;
	if (r == 0) counter_id = "lin";
	if (r == 1) counter_id = "xiao";  
	if (r == 2) counter_id = "yu";
	if (r == 3) counter_id = "cool";
	if (r == 4) counter_id = "linda";

	OmnString cname = createKey3();	
	counter_id = AosIILName::composeCounterIILName(counter_id);

	u64 iilid = AosGetKeyedStrValue(1607, counter_id, 0, rdata);
	if (iilid == 0) 
	{
		// It is not created yet. Create it now.
		iilid = AosCreateIIL(counter_id, eAosIILType_Str, true, false, rdata);
		aos_assert_r(iilid,  0);
		u64 tmp_iilid = 1607;
		AosSetStrValueDocUnique(tmp_iilid, false, counter_id, iilid, false, rdata);
		aos_assert_r(tmp_iilid == 1607,  0);
	}
	u64 value = rand() %100;
	u64 docid = 0;
OmnScreen << "counter_id : " << counter_id  <<" , cname : " << cname << " , value : " << value << endl;
	bool rslt = AosIncrementKeyedValue(iilid, cname, docid, value, 0xfffffffffffffffLL, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}

OmnString
AosCounterTester::createKey3()
{
	OmnString timeStr;
	switch(rand()%4)
	{
		case 0:
			timeStr = "20120609101011";
			break;
		case 1:
			timeStr = "20120610101011";
			break;
		case 2:
			timeStr = "20120611101011";
			break;
		case 3:
			timeStr = "20120612101011";
			break;
	}

	AosTimeGran::E time_gran = AosTimeGran::toEnum("day"); 
	AosTime::TimeFormat format = AosTime::convertTimeFormatWithDft("2fmt");
	u64 unitime = AosTime::getUniTime(format, time_gran, timeStr, "%Y%m%d%H:%M:%S");

	//OmnString key = "Tf|$_|";
	OmnString key = "Nf|$_|";

	OmnString cname;
	int r = rand()%4;
	if (r == 0) 
	{
		//cname = "cn|$$|js|$$|sz";
		cname = "cn|$_|js";
		//cname = "cn|$$|";
		//cname << unitime << "|$_|js";
	}
	if (r == 1) 
	{
		//cname = "cn|$$|bj|$$|wx";
		cname = "cn|$_|bj";
		//cname = "cn|$$|";
		//cname << unitime << "|$_|bj";
	}
	if (r == 2) 
	{
		//cname = "cn|$$|hh|$$|cs";
		cname = "cn|$_|hh";
		//cname = "cn|$$|";
		//cname << unitime << "|$_|hh";
	}
	if (r == 3) 
	{
		//cname = "cn|$$|hl|$$|zz";
		cname = "cn|$_|hl";
		//cname = "cn|$$|";
		//cname << unitime << "|$_|hl";
	}
	key << cname;
	return key;
}


bool
AosCounterTester::runQuery3(const AosRundataPtr &rdata)                                 
{
	OmnString reqStr = composeMultiSubReq();
	aos_assert_r(reqStr != "", false);
	AosXmlParser parser;
	AosXmlTagPtr childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	AosQueryReqPtr query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	OmnString contents;
	bool rslt = query->procPublic(rdata, childelem, contents);
	aos_assert_r(rslt, false);
	OmnScreen << contents<< endl;
	return true;
}


OmnString
AosCounterTester::composeMultiSubReq()
{   
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' "
		<< " zky_counter_id = 'cool,yu1,linda, xiao' factor1 = '0.01'"
		<< "sum='true'  type= 'counterMultiSub' zky_member1='hh'>"
		<< "<zky_time_condition1 zky_timeformat='2fmt' zky_starttime='2012-06-08 00:00:00' "
		<< "zky_endtime='2012-06-15 00:00:00' zky_timegran='day' zky_timetype='d'>"
		<< "</zky_time_condition1>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_' >"
		<< "<![CDATA[cn]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";
	return reqStr;
}




///////////////////////////////test Mutil Sub End//////////////////////////

////////////////////////////// test Mutil Sub2 /////////////////////////

bool
AosCounterTester::basicTest4(const int thrdid)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	int tries = 1000;
	for(int i = 0; i< tries; i++)
	{
		addCounter4(rdata);
	}
	OmnSleep(10);
	for (int i = 0; i< 10; i++)
	{
		runQuery4(rdata);
	}
	return true;
}


bool
AosCounterTester::addCounter4(const AosRundataPtr &rdata)
{
	OmnString cname = createCname4();
	OmnString counter_id;
	int r = rand()%5;
	if (r == 0) counter_id = "lin";
	if (r == 1) counter_id = "xiao";  
	if (r == 2) counter_id = "yu";
	if (r == 3) counter_id = "cool";
	if (r == 4) counter_id = "linda";

	u64 value = rand() %100;
	u64 docid = 0;
OmnScreen << "counterid : " << counter_id <<" , cname : " << cname << " , value : " << value << endl;
	bool rslt = AosIncrementKeyedValue(counter_id, cname, docid, true, value, 0, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}

OmnString
AosCounterTester::createCname4()
{
	AosTimeFormat::E format = AosTimeFormat::toEnum("yyyymmddhhmmss");
	OmnString timeStr;
	switch(rand()%4)
	{
	case 0:
		 timeStr = "20120609101011";
		 break;
	case 1:
		 timeStr = "20120610101011";
		 break;
	case 2:
		 timeStr = "20120611101011";
		 break;
	case 3:
		 timeStr = "20120612101011";
		 break;
	}

	int num = rand()%50;
	int level;
	OmnString cname;
	int r = rand() % 2 + 1;
	r = 2;
	switch(r)
	{
	case 4:
		{
			level = 3;
			u16 entry_type = createEntryType4(level); 
			OmnString name((char *) &entry_type, sizeof(u16));
			//name << "cool" << "\1" << "cn" << "\1";
			//name << AosTimeFormat::strToEpochDay(timeStr, format) << ".";
			//name << "星期" << rand()%7 + 1 << ".";
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "星期" << rand()%7 + 1 << "\1";
			name << num;
			cname = name;
			break;
		}
	case 3:
		{
			level = 3;
			u16 entry_type = createEntryType4(level);
			OmnString name((char *) &entry_type, sizeof(u16));
			name << "cool" << "\1";
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "cn" << num;
			cname = name;
			break;
		}
	case 2:
		{
			level = 2;
			u16 entry_type = createEntryType4(level);
			OmnString name((char *) &entry_type, sizeof(u16));
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			//name << "cn" << rand() %4 << "\1";
			name << "合肥" << rand()%26 + 1;
			cname = name;
			break;
		}
	case 1:
		{

			level = 2;
			u16 entry_type = createEntryType4(level);
			OmnString name((char *) &entry_type, sizeof(u16));
			name << AosTimeFormat::strToEpochDay(timeStr, format) << "\1";
			name << "call_" << num;
			cname = name;
			break;
		}
	}
OmnScreen << "cname : " <<cname << endl;
	return cname;
}

bool
AosCounterTester::runQuery4(const AosRundataPtr &rdata)                                 
{
	OmnString reqStr = composeMultiSub2Req();
	aos_assert_r(reqStr != "", false);
	AosXmlParser parser;
	AosXmlTagPtr childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	AosQueryReqPtr query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	OmnString contents;
	bool rslt = query->procPublic(rdata, childelem, contents);
	aos_assert_r(rslt, false);
	OmnScreen << contents<< endl;
	return true;
}


OmnString
AosCounterTester::composeMultiSub2Req()
{   
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' "
		<< " zky_counter_id = 'xiao,yu,linda' factor1 = '0.01'"
		<< " zky_level = '2' zky_x='0' zky_y = '1' "
		<< " sum='true'  max='true' use_epochtime = 'true' use_iilname='true' "
		<< " type= 'counterMultiSub2' zky_member1='hh'>"
			<< "<aggregate  zky_aggregate_type1='mergeEntries' zky_mergefield_index = '0' "
			<< " />"
			<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-06-08 00:00:00' "
			<< "zky_endtime='2012-06-11 00:00:00' zky_timegran='day' zky_timetype='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
		//<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_' >"
		//<< "<![CDATA[cn1]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";
	return reqStr;
}

u16
AosCounterTester::createEntryType4(const int level)
{
	// EntryTypecool.cn.time.1
	int append_bit = 0; 
	AosStatType::E stat_type;
	if (rand()%2)
	{
		stat_type = AosStatType::toEnum("sum"); 
	}
	else
	{
		stat_type =  AosStatType::toEnum("max");
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum("day"); 
	//AosTimeGran::E time_gran = AosTimeGran::toEnum("ntm"); 
	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, stat_type, time_gran);
	OmnScreen << entry_type << endl;
	return entry_type;
}


////////////////////////////////test Multi Sub2//////////////////////////////////



/////////////////////////////////// StatAddCounter2 ////////////////////////////////
bool 
AosCounterTester::basicTest5(const int thrdid)
{
	// This basic test simle creates counters. 
	u64 starttime = OmnGetSecond();

    int tries = mTestMgr->getTries();
    OmnScreen << "Tries: " << tries << endl;
    if (tries <= 0) tries = eDefaultTries;
	
	tries = 10000;
	AosRundataPtr rdata = OmnApp::getRundata();

	for(int i = 0; i< tries; i++)
	{
		if (i % 100000 == 0) 
		{
			u32 delta = OmnGetSecond() - starttime;
			if (delta > 0)
			{
				OmnScreen << "Trying: " << i << ". Speed: " << i / delta << endl;
			}
		}
		addCounter5(thrdid);
	}

	// run Query
	OmnSleep(10);
	for (int i = 0; i<4; i++)
	{
		runQuery5(rdata);
	}
	return true;
}


bool
AosCounterTester::addCounter5(const int thrdid)
{
	static OmnString lsCounterId = "counter123";

	if (sgRandIdx >= sgRandSize - 10) sgRandIdx = 0;
	int num_segs = (sgRandNums[sgRandIdx++] % 6) + 1;
	OmnString cname;
	for (int i=0; i<num_segs; i++)
	{
		if (cname != "") cname << AOS_COUNTER_SEGMENT_SEP;
		cname << sgRandStrs[sgRandIdx++];
	}
	if (rand()%2 == 0)
	{
		cname = "test.cn.js.sz";
	}
	else
	{
		cname = "test.cn.js.wx";
	}
int r = rand()%7;
//if (r == 0) cname = "test.cn.js.sz";
//if (r == 1) cname = "yu.cn.js.sz";
//if (r == 2) cname = "linda.cn.js.lj";
//if (r == 3) cname = "yu.cn.js.cz";
//if (r == 4) cname = "test.cn.js.wx";
//if (r == 5) cname = "linda.cn.hl.sz";
//if (r == 6) cname = "test.cn.hl.cz";

if (r == 0) 
{
	cname = "";
	cname << "test" << "\1" << "cn"<< "\1" << "js" << "\1" <<"sz";
}
if (r == 1) 
{
	cname = "";
	cname << "yu" << "\1" << "cn"<< "\1" << "js" << "\1" <<"sz";
}

if (r == 2) 
{
	cname = "";
	cname << "linda" << "\1" << "cn"<< "\1" << "js" << "\1" <<"lj";
}
if (r == 3) 
{
	cname = "";
	cname << "yu" << "\1" << "cn"<< "\1" << "js" << "\1" <<"cz";
}
if (r == 4) 
{
	cname = "";
	cname << "test" << "\1" << "cn"<< "\1" << "js" << "\1" <<"wx";
}

if (r == 5) 
{
	cname = "";
	cname << "linda" << "\1" << "cn"<< "\1" << "hl" << "\1" <<"sz";
}
if (r == 6) 
{
	cname = "";
	cname << "test" << "\1" << "cn"<< "\1" << "hl" << "\1" <<"cz";
}


	bool rslt = procCounter5(cname, mRundata);
	aos_assert_rr(rslt, mRundata, false);
	return true;
}

bool
AosCounterTester::procCounter5(
			const OmnString &cname,
			const AosRundataPtr &rdata)
{
	int vv = rand()%6;
	u64 value = 102;
	OmnString timeStr = "06-07-2012 00:00:00";
	if (vv == 1)
	{
		timeStr = "06-08-2012 00:00:00";
	}
	if (vv == 2)
	{
		timeStr = "06-09-2012 00:00:00";
	}
	if (vv == 3)
	{
		timeStr = "06-02-2012 00:00:00";
	}
	if (vv == 4)
	{
		timeStr = "06-01-2012 00:00:00";
	}
	if (vv == 5)
	{
		timeStr = "06-12-2012 00:00:00";
	}

	bool rslt = AosCounterClt::getSelf()->procCounter2(
			cname, value, AosTimeGran::eDaily,
			timeStr, "mm-dd-yyyy", "sum", "2fmt", "add2", rdata);

	return rslt;
}


bool
AosCounterTester::runQuery5(const AosRundataPtr &rdata)
{
	OmnString reqStr = composeAll2Req2();
	aos_assert_r(reqStr != "", false);
    AosXmlParser parser;
    AosXmlTagPtr childelem = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	AosQueryReqPtr query = AosQueryClient::getSelf()->createQuery(rdata, childelem);
	if (!query || !query->isGood())
	{
		return false;
	}

	OmnString contents;
	bool rslt = query->procPublic(rdata, childelem, contents);
	OmnScreen << contents << endl;
	return true;
}



OmnString
AosCounterTester::composeSub2Req5()
{	
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h'  "
		<< " zky_level = '4' zky_x1='2' zky_y1 = '1' append_bit ='true' "
		<< " zky_counter_id = 'test' use_epochtime = 'true' use_iilname='false'"
		<< " factor1 = '0.01' zky_cname_format='term0.term2.time.time3'"
		<< "sum='true' max1 = 'true' avg1 = 'true' type= 'counterSub2'>"
		 	<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-06-01 00:00:00' "
			<< "zky_endtime='2012-06-15 00:00:00' zky_timegran='day' zky_timetype='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[js]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[sz]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>xml</oname>"
		<< "<cname>xml</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";

	return reqStr;
}

OmnString
AosCounterTester::composeMultiSub2Req2()
{   
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' "
		<< " zky_counter_id = 'test' factor1 = '0.01'"
		<< " zky_level = '4' zky_x='0' zky_cname_format='term0.term2.time.time3' "
		<< " sum='true'  max='true' use_epochtime = 'true' use_iilname='false' "
		<< " type= 'counterMultiSub2' append_bit ='true' >"
			<< "<zky_time_condition zky_timeformat='2fmt' zky_starttime='2012-06-01 00:00:00' "
			<< "zky_endtime='2012-06-15 00:00:00' zky_timegran='day' zky_timetype='d'>"
			<< "</zky_time_condition>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[js]]></termname>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[sz]]></termname>"
		<< "</counter_names>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>zky_cname</oname>"
		<< "<cname>zky_cname</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>";
	return reqStr;
}


OmnString
AosCounterTester::composeAll2Req2()
{
	OmnString reqStr = "<cmd psize='20' opr='retlist' start_idx='0'>";
	reqStr << "<conds>"
		<< "<cond type='AND'/>"
		<< "<stat>"
		<< "<counter zky_ctobjid='giccreators_query_cond_countersimple_h' type= 'counterAll'>"
		<< "<cond sum ='true'>"
		<< "<zky_counter_id>test, linda, yu</zky_counter_id>"
		<< "<use_iilname>false</use_iilname>"
		<< "<append_bit>true</append_bit>"
		<< "<zky_level>3</zky_level>"
//		<< "<zky_cname_format>term0.term2.time.time3</zky_cname_format>"
		<< "<counter_names>"
		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
		<< "<![CDATA[cn]]></termname>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[js]]></termname>"
//		<< "<termname zky_value_type='const' zky_ctobjid='vpd_countername_normalterm_h'>"
//		<< "<![CDATA[sz]]></termname>"
		<< "</counter_names>"
		<< "<zky_time_condition zky_timeformat='2fmt'>"
		<< "<use_epochtime>true</use_epochtime>"
		<< "<zky_starttime>2012-06-01 00:00:00</zky_starttime>"
		<< "<zky_endtime>2012-06-15 00:00:00</zky_endtime>"
		<< "<zky_timegran>day</zky_timegran>"
		<< "</zky_time_condition>"
		<< "</cond>"
		<< "<middle_result>"
		<< "<aggregate zky_aggregate_type='mergeEntries' zky_mergefield_index = '0'>"
		<< "</aggregate>"
		<< "<filling zky_type='time'>"
		<< "<zky_fill_field></zky_fill_field>"
		<< "<zky_fill_value>0</zky_fill_value>"
		<< "</filling>"
		<< "</middle_result>"
		<< "<result>"
		<< "<format zky_type= 'twodime'>"
		<< "<zky_x>1</zky_x>"
		<< "<zky_ypsize></zky_ypsize>"
		<< "</format>"
		<< "<datatype zky_type=''>"
		<< "<factor></factor>"
		<< "</datatype>"
		<< "</result>"
		<< "</counter>"
		<< "</stat>"
		<< "</conds>"
		<< "<fnames>"
//		<< "<fname type='7' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky_cname</oname>"
//		<< "<cname>zky_cname</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky_cname0</oname>"
//		<< "<cname>zky_cname0</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky_cname1</oname>"
//		<< "<cname>zky_cname1</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky_cname2</oname>"
//		<< "<cname>zky_cname2</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky_cname3</oname>"
//		<< "<cname>zky_cname3</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky__zt1i_linda0_sum</oname>"
//		<< "<cname>zky__zt1i_linda0_sum</cname>"
//		<< "</fname>"
//		<< "<fname type='1' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
//		<< "<oname>zky__zt1i_test1_sum</oname>"
//		<< "<cname>zky__zt1i_test1_sum</cname>"
//		<< "</fname>"
		<< "<fname type='x' zky_ctobjid='giccreators_query_fname_h' join_idx='0'>"
		<< "<oname>xml</oname>"
		<< "<cname>xml</cname>"
		<< "</fname>"
		<< "</fnames>"
		<< "</cmd>"; 

	return reqStr;
}

