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
// There are several types of fields:
// 1. Fields whose values are always unique
// 2. Fields whose values may not be unique
// 3. Fields whose values are made not unique
//
// How to Verify Results
// A group is made of several fields. One of them is elected as the sorting 
// field. The docid set for a group is determined by the join (AND) of all
// the fields' conditions. When a docid is queried, it checks all the fields.
// The docid must meet each field's condition. 
//
// This method can verify whether a queried docid is in a group or not, but
// it cannot verify all docids in a group have been queried. That is, if a 
// query missed some docids, this method won't be able to detect. 
//
// Each group has a sequence of seeds. These seeds are used to include 
// some docids that are in a group. Starting from the mStartDocid, these
// seeds define a 'Docid Cycle'. The cycle's size is the sum of all the
// seeds:
// 		mDocidCycleSize = sum(mSeeds)
// 		Number of Cycles N = (end_docid - start_docid) / mDocidCycleSize
// All the docids:
// 			Docid = N * mSeeds[i]
// must be in the group. Or in other word, if 'mCounts[i]' is used to keep
// the count of mSeeds[i] being hit, at the end, the following should be true:
//
// 		mCounts[i] == N. 
//
// In addition:
// 		Remainder = (end_docid - start_docid) % mDocidCycleSize
//
// Any seeds that are within 'Remainer' should add one more count.
//
// The docids that can be calculated through the above method are called 
// 'Known Docids'. We will use 'Known Docids' to check whether a query 
// missed something. This method won't be able to verify docids that are
// in the group but is not a Known Docid. 
//
// Modification History:
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/QueryStatistics.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BitmapMgr/BitmapUtil.h"
#include "QueryTorturer/SmartStatisticsTester.h"
#include "QueryTorturer/QueryField.h"
#include "Porting/Sleep.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryType.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/TaskObj.h"    
#include "SEInterfaces/TaskDataObj.h"
#include "SEUtil/IILIdx.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "UtilTime/TimeFormat.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/UtUtil.h"
#include "QueryTorturer/JQLQuery.h"
#include <cstdlib>

#define PERCENT(n)   (rand()%100 < n)

static OmnString sgDftIILName = "_zt44_test";
static bool sgShowLogLevel1 = false;
static bool sgShowLogLevel2 = false;
static u64 sgQueryGroup_QueryStart = 0;
static int sgNumGroupsCreated = 0;
static int sgNumCopies= 10;

u64 AosQueryStatistics::smSampleDocidSize = 1000000;
u32 AosQueryStatistics::smRangeFreq = 15;
u32 AosQueryStatistics::smSeed = 141;


AosQueryStatistics::AosQueryStatistics(
		AosSmartStatisticsTester *tester,
		const u64 group_id, 
		const OmnString &table_name)
:
mGroupStartTime(0),
mGroupEndTime(0),
mQueryStartTime(0),
mPageMaxTime(0),
mNumDocsQueried(0),
mTester(tester),
mNumMatchedDocs(0),
mGroupId(group_id),
mQueryTotalTime(0),
mNumPagesQueried(0),
mCrtQueryPos(-1),
mDocidCycleSize(0),
mNumDocsInGroup(0),
mSortingFieldIdx(0),
mStartDocid(0),
mEndDocid(0),
mNumEqualConds(0),
mDocidRangeSize(0),
mSampleDocidStart(0),
mTotalBlockLoadTime(0),
mTotalNumQueried(0), 
mTotalFullQuery(3000), 
mTotalBlocksTried(0),
mMaxBlockLoadTime(0), 
mMaxNumBlocksTried(0),
mCrtU64Value(0),
mIsMovingTo(false),
mNumAlpha(true),
mReverse(false),
mMoveToTimes(0),
mDataType(eDataFullCombination),
mTableName(table_name),
mLastSortValue(0)
{
	sgNumGroupsCreated++;
	// cout << "Num Groups Created: " << sgNumGroupsCreated << endl;
	mRundataPtr = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata = mRundataPtr.getPtr();
	mRundata->setSiteid(100);
//	mRundata->setSiteid(AosGetDftSiteId());
//	mActualBitmap2 = AosGetBitmap();
}


AosQueryStatistics::~AosQueryStatistics()
{
	sgNumGroupsCreated--;
	cout << "Num Groups Created: " << sgNumGroupsCreated << endl;
}


bool
AosQueryStatistics::runQuery()
{
	// This is the function called from SmartQueryTester, which 
	// creates a QueryGroup and calls QueryGroup::runQuery().
	// This function creates the query, 
	for (int i = 0; i < mTotalFullQuery; i++)
	{

		mSQLStmt = "";
		startQuery();

		bool rslt = createQuery();
		aos_assert_r(rslt, false);

		sgQueryGroup_QueryStart = OmnGetTimestamp();

		OmnString seExpr = createSelectExpr();

		//generate the sql statement
		//the first cmd is "use db ....."
		mSQLStmt << "use " << mDatabase << "; ";

		//second statement is select
		mSQLStmt << "select ";
		//mSQLStmt << createSelectExpr() << " from " << mTableName << " where ";
		if(mQuery_in[1]=="")
		{
			mSQLStmt << seExpr << " from " << mTableName << " group by "
					 << mQuery_in[0];
		}
		else
		{
			mSQLStmt << seExpr << " from " << mTableName << " group by "
				 << mQuery_in[0] << ",_day(" << mQuery_in[1] << ")";
		}
		/*
		for (u32 i=0; i<mConds.size(); i++)
		{
			cout << mConds[i].toString();
			if (i > 0) mSQLStmt << " && ";
			mSQLStmt << mConds[i].getCondClause();
		}

		mSQLStmt << " order by " << mSortFieldName;
		*/

		//start to run this query statement
		rslt = runQueryPriv();
		//if (!rslt) 
		//	exit(1);
	}

	exit(0);
}



bool
AosQueryStatistics::getTotalExpect()
{	
	//create count
	createCountKeyList();

	int key_value = 1;
	int key_value_total = 0;

	for(u32 j=0; j<mCountKeyList.size(); j++)
	{
		key_value = 1; 
		for(int i=0; i<mKey_n; i++)
		{	
			key_value = mCountKeyList[j][i] * key_value;
		}
		key_value_total += key_value;
	}
	key_value = key_value_total;

	if((mQuery_in[1]) == "")
		mTotalExpected = key_value;
	else
		mTotalExpected = key_value * mTimeNum;

	return true;
}


bool
AosQueryStatistics::getCountExpect()
{	
	//create select key list
	createSelectKeyField();

	int count_value = 1;
	int count_value_total = 0;

	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{	
		count_value = 1;
		for(u32 i=0; i<mQueryKeyList[j].size()-mKey_n; i++)
		{	
			//get the total count value 
			count_value = mRestCountKeyList[j][i] * count_value;
		}
		count_value_total += count_value;

	}
		
	if((mQuery_in[1]) == "")
		mCountExpected = count_value;
	else
		mCountExpected= count_value * mTimeNum;

	return true;
}



bool
AosQueryStatistics::runQueryPriv()
{
	OmnString stmt = "";
	OmnString results = "";

	// This function actually runs the query. It creates the 
	// SQL statement first, and then runs the SQL statement.
	//
	if (mCrtQueryPos < 0) mCrtQueryPos = createInitStartIdx();
	createInitPageSize();
	
	while (1)
	{
		mQueryStartTime = OmnGetTimestamp();
		mRundata->resetTimeTracker();

		stmt = mSQLStmt;
		//stmt << " limit " << mVerified << ", " << mPageSize << ";";
		stmt << " limit " << mVerified << ", " << 5000 << ";";

		cout << "SQL Statement: " << stmt << endl;

		bool rslt = AosJQLQuery::runQuery(mRundata, stmt);
		results = mRundata->getJqlData();
		aos_assert_r(rslt, false);
		aos_assert_r(results != "", false);

		//for testing only
//		results = AosSmartQueryTester::getResult();
		cout << "really data : " << results << endl;

		rslt = pageFinished(results);
		if (!rslt)
		{
			queryFailed(mRundata, mRundata->getErrmsg());
			return false;
		}

		if (mVerified == mTotalExpected)
		{
			cout << "Successfully verified all the data for a JQL statement." << endl;
			break;
		}
	}

	return true;
}


void
AosQueryStatistics::printStat(const AosQueryRsltObjPtr &results, const bool flag)
{
	// u64 end_time = OmnGetTimestamp();
	if (!flag) return;
	
	aos_assert(mTotalNumQueried > 0);
	char buff[200];
	sprintf(buff, "GID: %4u Pages: %5u AV: %5u Max: %8u"
			" BT: %5d NB: %5d BlockMax: %8d Blocks: %5d", 
			(unsigned int)mGroupId, 
			(unsigned int)mNumPagesQueried, 
			(unsigned int)mQueryTotalTime / mNumPagesQueried, 
			(unsigned int)mPageMaxTime, 
			mTotalBlockLoadTime/mTotalNumQueried, 
			mTotalBlocksTried/mTotalNumQueried,
			mMaxBlockLoadTime, 
			mMaxNumBlocksTried);
	cout << buff << endl;
	if (mNumPagesQueried % 1000 == 0)
	{
	    cout << "Query Contents for Page " << mNumPagesQueried << endl;
	    cout << "(not imp)" << endl;
	}

	if (sgShowLogLevel2)
	{
		if (results->getNumDocs() == 0)
		{
			cout << "Did not find anything" << endl;
		}
		else
		{
	    	cout << "Query Contents: (not imp)" << endl;
		}
	}
}


bool 
AosQueryStatistics::before(const u64 &a, const u64 &b)
{
	if(mReverse)
	{
		return (a > b);
	}
	else
	{
		return (a < b);
	}
	return true;
}


bool 
AosQueryStatistics::checkOrder(const u64 pre, const u64 next)
{
	if(mReverse)
	{
		return (pre >= next);
	}
	else
	{
		return (pre <= next);
	}

	return true;
}

void
AosQueryStatistics::next(u64 &v)
{
	if(mReverse)v--;
	else v++;	
}


bool
AosQueryStatistics::allDocsQueried()
{	
	// Starting from mCrtValue until the last value of the condition, 
	// all should be invalid.
	// currently we only get the min_docid & max_docid, if the docids 
	// are not continuious, we'll change to getting bitmaps of the docids
	u64 min_docid = mSortingField->getMinDocid();
	u64 max_docid = mSortingField->getMaxDocid();
	bool valid = true;
	for (u64 cur_docid = min_docid;cur_docid <= max_docid;cur_docid++)
	{
//		if(mActualBitmap2->checkDoc(cur_docid))
//		{
//			continue;
//		}
		valid = isValidDocid(cur_docid);
		aos_assert_r(!valid,false);
	}
	return true;
/*	OmnString ss;
	if(mReverse)
	{
		ss = mSortingField->getMinValue();
	}
	else
	{
		ss = mSortingField->getMaxValue();
	}
		
	u64 edge_value = AosStr2U64(ss);

	while(before(mCrtU64Value,edge_value) || mCrtU64Value == edge_value)
	{
	 	bool valid = isValid(mCrtU64Value);
	 	if(valid)
	 	{
	 		OmnAlarm << "The value:" << mCrtU64Value << " should not be valid." << enderr;
	 		valid = isValid(mCrtU64Value);
	 	}
	 	aos_assert_r(!valid, false);
		next(mCrtU64Value);
	}
*/	
	return true;
}


bool
AosQueryStatistics::checkMoveToRange(
		const u64 &real_distance,
		const u64 &crt_pos)
{
	i64 diff = (real_distance-crt_pos);
	if(diff < 0)diff *= -1;

	if(diff <= eErrorMargin) return true; 

	if(crt_pos == 0)
	{
 		// if the diff is bigger than eErrorMargin, it should be a later start position
 		OmnAlarm << "The move to query failed cause the result which "
			"should be the first is not there." << enderr;
		return false;
	}

	if(diff > (crt_pos *eErrorMarginPercent/100))
	{
 		OmnAlarm << "The move to query failed cause the result is not in range\n"
			<< "real:" << real_distance << "\n"
			<< "should be: " << crt_pos
			<< enderr;		
		return false;
	}
	return true;
}


bool
AosQueryStatistics::chooseMoveToPos()
{
	// srand(smSeed++);
	i64 max = mTester->getMaxDocid();
	i64 min = mTester->getMinDocid();
	aos_assert_r(max>=min,false);
	i64 area = max - min;
	if(mCrtQueryPos == 0)
	{
		// 2 choice
		// always from beginning for now
		if(PERCENT(50))
		{
			// start from beginning
			mCrtQueryPos = 0;
		}
		else
		{
			// start from a position
			mCrtQueryPos = rand()%area;
		}
		return true;
	}
	
	// not the first time
	// We have 3 choices, 
	// 		1: the position is before mCrtQueryPos.
	// 		2. the position is nearly after mCrtQueryPos.
	// 		3. the position is far away after mCrtQueryPos.
	u32 type = rand()%3;
	switch(type)
	{
		case 0:
			 // 		1: the position is before mCrtQueryPos.
			 mCrtQueryPos = rand() % mCrtQueryPos;
			 break;

		case 1:
			 // 		2. the position is nearly after mCrtQueryPos.
			 mCrtQueryPos = rand() % eCloseEnough + mCrtQueryPos;
			 if(mCrtQueryPos > area) mCrtQueryPos = area;
			 break;

		case 2:
			 // 		3. the position is far away after mCrtQueryPos.
			 mCrtQueryPos += eCloseEnough;
			 if(mCrtQueryPos >= area)
			 {
				mCrtQueryPos = area;
				break;
			 }	
			 mCrtQueryPos += rand()%(area - mCrtQueryPos);
			 break;

		default:
			 aos_assert_r(0,false);
	}

	return true;
}


bool
AosQueryStatistics::pageFinished(
		const OmnString &results)
{
	// One page finished. It marks all the docids.
	OmnString str = mRundata->getArg1(AOSARG_LOAD_BLOCK_TIME, true, "");
	u64 end_time = OmnGetTimestamp();
	if (str != "")
	{
		int nn = str.toInt();
		aos_assert_r(nn > 0, false);
		if (nn > mMaxBlockLoadTime) mMaxBlockLoadTime = nn;
		mTotalBlockLoadTime += nn;
		mTotalNumQueried++;

		nn = mRundata->getArg1(AOSARG_NUM_BLOCKS_TRIED, true, "").toInt();
		aos_assert_r(nn > 0, false);
		if (nn > mMaxNumBlocksTried) mMaxNumBlocksTried = nn;
		mTotalBlocksTried += nn;

		mRundata->setTotalTime(__FILE__, __LINE__, "TotalQueryTime");
	}

	u64 page_time = end_time - mQueryStartTime;
	mQueryTotalTime += page_time;
	if (page_time > mPageMaxTime) mPageMaxTime = page_time;
	++mNumPagesQueried;
	OmnString page_time_str;
	page_time_str << page_time;
	page_time_str.convertToScientific();
	// printStat(results, sgShowLogLevel1);

	//
	// Start to judge the query result
	//
	bool ret;
	switch (mDataType)
	{
		case eDataDocId:
			ret = verifyDataDocId(results);
			break;

		case eDataFullCombination:
			ret = verifyDataCombination(results);
			break;

		default:
			ret = false;
			break;
	}

	return ret;
}

bool
AosQueryStatistics::verifyDataDocId(OmnString results)
{
	//AosXmlTagPtr rslts_xml = AosStr2Xml(mRundata, results AosMemoryCheckerArgs);
	AosXmlTagPtr rslts_xml = AosStr2Xml(mRundata, results AosMemoryCheckerArgs);
	aos_assert_r(rslts_xml, false);
	AosXmlTagPtr tag = rslts_xml->getFirstChild();
	if (!tag)
	{
		// The query guarantees that if it returns an empty string, the 
		// query finishes.
		bool rslt = allDocsQueried();
		aos_assert_r(rslt, false);
		return groupQueryFinished();
	}

	// Process the results
	int entries_found = 0;
	u64 cur_docid = 0;
	OmnString cur_value;
	while (tag)
	{
		cur_docid = tag->getAttrU64("docid", 0);
		aos_assert_r(cur_docid, false);
		cur_value = tag->getAttrStr(mSortFieldName);
		aos_assert_r(cur_value != "", false);
//		mActualBitmap2->appendDocid(cur_docid);
		if (!docQueried(cur_value, cur_docid)) return false;
		entries_found++;
		tag = rslts_xml->getNextChild();
	}

	return true;
}

//
//Full combination means all the possible values of
//all the fields will have a record in the table
//
bool
AosQueryStatistics::verifyDataCombination(OmnString results)
{
	int start;
	int end;
	int recordInPage = 0;
	int realTotal;
	AosQueryTesterUtil::CondDef cond;
	AosXmlTagPtr rslts_xml;
	AosXmlTagPtr rec_xml;
	AosQueryFieldPtr field;

	//verify record one by one
	// we assume the query results start from XML format:
	// 	<content total="5" start_idx="0" crt_idx="0" queryid="75" num="5" time="201.700000ms">
	// 		<record f1="10" f2="tom"></record>
	// 		<record f2="14" f2="jerry"></record>
	// 		<record f3="15" f2="mary"></record>
	// 	</content>
	//
	
	//remove non-xml format part firstly
 	start = results.indexOf("<content", 0);
 	end = results.indexOf("/content>", 0);
	if (start >=  0 && end > start)
	{
		results = results.substr(start, end + 8);
		//rslts_xml = AosStr2Xml(mRundata, results AosMemoryCheckerArgs);
		rslts_xml = AosStr2Xml(mRundata, results AosMemoryCheckerArgs);
		aos_assert_r(rslts_xml, false);

		realTotal = rslts_xml->getAttrU64("total", 0);
	}
	
	//get the expect total;
	getTotalExpect();
	if (mTotalExpected != realTotal)
	{
		cout << "Query Failed --- total record to hit:"
			<< mTotalExpected << " real record hit: " 
			<< realTotal << endl; 

		return false;
	}

	aos_assert_r(rslts_xml, false);
	rec_xml = rslts_xml->getFirstChild(true); 

	while (rec_xml) 
	{
		recordInPage++;
		mVerified++;
		if (mVerified > mTotalExpected)
		{
			cout << "Query Failed --- exceed total record to hit:"
				<< mTotalExpected  << endl; 

			return false;
		}
		//verify record one by one
		if (!verifyRecordCombination(rec_xml))
			return false;

		//get next child
		rec_xml = rslts_xml->getNextChild(); 
	}

	if (recordInPage > 0)
	{
		cout << "Successfully verified one page of a JQL statement  with " << recordInPage << " records." << endl;
	}
	else
	{
		if (mVerified != mTotalExpected*sgNumCopies)
		{
			cout << "Query Failed --- total record to hit:"
				<< mTotalExpected << " real record verified: " 
				<< mVerified << endl; 

			return false;
		}
	}

	return true;
}

bool
AosQueryStatistics::verifyRecordCombination(
		const AosXmlTagPtr &rec_xml)
{
	OmnString key = "";
	OmnString real_value = "";
	map<OmnString, int> keyValueMap;

	keyValueMap.clear();
	//currently, we may only check the field values
	//which is selected. 
	for (u32 i = 0; i < mSelectKeyList_k.size(); i++)
	{
		key = mSelectKeyList_k[i];

		//real_value = mSelectMeasureList[i];
		
		//get "1" from "k1_1"  
		OmnString k_val = rec_xml->getNodeText(key);
		int idx = k_val.indexOf(0, '_', true);
		int len = k_val.length();
		OmnString k_value = k_val.substr(idx+1, len-1);
		int k_int = k_value.toInt64(0);
		keyValueMap[mSelectKeyList_k[i]] = k_int;
	}

	OmnString measure = "";
	for (u32 i = 0; i < mSelectMeasureList.size(); i++)
	{
		measure = mSelectMeasureList[i];

		//get aggr name
		//sum, count, max, .....
		int idx = measure.indexOf(0, '(', true);
		OmnString aggrName = measure.substr(0, idx-1);

		//////////////////////////////////////////////////////////////
		//  get measure value
		/////////////////////////////////////////////////////////////
		//get "sum" and "v1" from "sum(v1)"	
		//get measure field
		//v1, v2, v3
		int length = measure.length();
		OmnString real_v = measure.substr(idx+1, length-1);	
		//get "v1" from "v1)"
		int indx = real_v.indexOf(0, ')', true);
		OmnString real_vl = real_v.substr(0, indx-1);

		OmnString real_sum = measure.substr(0, idx-1);
		OmnString real_sum_v;
			real_sum_v << "" << aggrName
				<< "0x28" << real_vl << "0x29";
		OmnString k_val_sum = rec_xml->getNodeText(real_sum_v);
		int query_total = k_val_sum.toInt64(0);

		//get expected measure value
		i64 expected_value = 0;
		if (aggrName == "sum")
		{
			expected_value = getSumValue(keyValueMap, real_vl);
		}
		else if (aggrName == "count")
		{
			expected_value = getCountValue(keyValueMap, real_vl);
		}	
		else if (aggrName == "max")
		{
			expected_value = getMaxValue(keyValueMap, real_vl);
		}
		else if (aggrName == "min")
		{
			expected_value = getMinValue(keyValueMap, real_vl);
		}
		else
		{
			expected_value = getDistinctValue(keyValueMap, real_vl);
		}
	
		if(expected_value != query_total)
		{
			cout << "Query Failed --- Field value not returned: "
				    << key << " --- " << expected_value << endl;
			return false;
		}
	}

	return true;
}

bool
AosQueryStatistics::createSelectKeyField()
{
	mSelectKeyList.clear();
	OmnString k_name;

	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		mSelectKeyList_k.clear();
		for(int i=0; i<mKey_n; i++)
		{
			k_name = mQueryKeyList[j][i]->getFieldName();
			mSelectKeyList_k.push_back(k_name);
		}
		mSelectKeyList.push_back(mSelectKeyList_k);
	}

	return true;
}



i64
AosQueryStatistics::getSumValue(map<OmnString, int> keyValueMap, OmnString measureField)
{
	//create select key list
	createSelectKeyField();

	i64 total;
	int key_rest = 1;
	int key_rest_ano = 0;
	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		key_rest = 1;
		for(u32 i=0; i<mQueryKeyList[j].size(); i++)
		{
			//get the group by key
			if(i<mSelectKeyList[j].size())
			{
				OmnString group_key = mSelectKeyList[j][i];

				if(group_key == "")
					return false;
				else
					key_rest *= 1;
			}
			else
				key_rest = mSumKeyList[j][i] * key_rest;    
		}
		if(key_rest == 1)
			key_rest_ano = 0;
		else
			key_rest_ano += key_rest; 
	}
	//key_rest = key_rest_ano;

	/*
	OmnString key = mSelectKeyList[0][0];
	int key_index = keyValueMap[key];

	//group by having time field or not
	if(mQuery_in[1] == "")
		total = key_rest * key_index * mTimeNum;
	else
		total = key_rest * key_index;
	*/

	int key_index = 0;

	if (measureField == "v1")
	{	
		int min = 0;
		int max = 0;
		int less = 0;

		if (keyValueMap.find("k1") == keyValueMap.end())
		{
			//k2 not in group by list
			for(u32 j=0; j<mQueryKeyList.size(); j++)
			{
				for(u32 i=0; i<mQueryKeyList[j].size(); i++)
				{
					if(mQueryKeyList[j][i]->getFieldName()=="k1")
					{
						min = mQueryKeyList[j][i]->getMinU64Value();
						max = mQueryKeyList[j][i]->getMaxU64Value();
						less = max + min;
					}
				}
			}
			total = key_rest / 2 * less;
		}
		else
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{
				 if(mSelectKeyList[0][j] == "k1")
					 key_index = keyValueMap[mSelectKeyList[0][j]];
			}

			//k1 in group by list
			total = key_rest * key_index;
		}
		if(mQuery_in[1] == "")
		{
			total *= mTimeNum;
		}
		else
			total = total;
	}

	else if (measureField == "v2")
	{
		int min = 0;
		int max = 0;
		int less = 0;

		if (keyValueMap.find("k2") == keyValueMap.end())
		{
			//k2 not in group by list
			for(u32 j=0; j<mQueryKeyList.size(); j++)
			{
				for(u32 i=0; i<mQueryKeyList[j].size(); i++)
				{
					if(mQueryKeyList[j][i]->getFieldName()=="k2")
					{
						min = mQueryKeyList[j][i]->getMinU64Value();
						max = mQueryKeyList[j][i]->getMaxU64Value();
						less = max + min;
					}
				}
			}
			total = key_rest / 2 * less;
		}
		else
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{
				 if(mSelectKeyList[0][j] == "k2")
					 key_index = keyValueMap[mSelectKeyList[0][j]];
			}
			//k1 in group by list
			total = key_rest * key_index;
		}
		if(mQuery_in[1] == "")
		{
			total *= mTimeNum;
		}
		else
			total = total;
	}
	else
	{
		//for d1
	}

	return total;
}


i64
AosQueryStatistics::getMaxValue(map<OmnString, int> keyValueMap, OmnString measureField)
{
	//create select key list
	createSelectKeyField();

	i64 max = 0;
	if (measureField == "v1")
	{
		//get key index value
		int key_index = keyValueMap[mSelectKeyList[0][0]];
		
		if(mSelectKeyList.size() == 1)
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{
				 if(mSelectKeyList[0][j] == "k1")
					  key_index = keyValueMap[mSelectKeyList[0][j]];

				//if(j+1<mSelectKeyList[0].size())
				//{   
				//	 if(keyValueMap[mSelectKeyList[0][j+1]] >= keyValueMap[mSelectKeyList[0][j]])
				//	 key_index = keyValueMap[mSelectKeyList[0][j+1]];
				//}
			}
		}

		for(u32 i=0; i<mSelectKeyList.size(); i++)
		{
			if(i+1<mSelectKeyList.size())
			{
				if(keyValueMap[mSelectKeyList[i][0]] <= keyValueMap[mSelectKeyList[i+1][0]])
					key_index = keyValueMap[mSelectKeyList[i+1][0]];
			}
		}

		if (keyValueMap.find("k1") == keyValueMap.end())
		{
			//k1 not in group by list
			createMinMaxValue(); 
			max = mExpectMaxValue;
		}
		else
			max = key_index;
	}
	else if(measureField == "v2")
	{
		int key_index;
		//get key index value
		if(mSelectKeyList.size() == 1)
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{	
				if(mSelectKeyList[0][j] == "k2")
					  key_index = keyValueMap[mSelectKeyList[0][j]];


				// if(j+1<mSelectKeyList[0].size())
				// {   
				//	 if(keyValueMap[mSelectKeyList[0][j+1]] >= keyValueMap[mSelectKeyList[0][j]])
				//	 key_index = keyValueMap[mSelectKeyList[0][j+1]];
				// }
			}
		}

		for(u32 i=0; i<mSelectKeyList.size(); i++)
		{
			if(i+1<mSelectKeyList.size())
			{
				if(keyValueMap[mSelectKeyList[i][0]] <= keyValueMap[mSelectKeyList[i+1][0]])
					key_index = keyValueMap[mSelectKeyList[i+1][0]];
			}
		}

		if (keyValueMap.find("k2") == keyValueMap.end())
		{
			//k1 not in group by list
			createMinMaxValue();
			max = mExpectMaxValue;
		}
		else
			max = key_index;
	}
	else
	{
		//d1	
	}

	return max;
}


bool
AosQueryStatistics::createMinMaxValue()
{
	int min = 0;
	int max = 0;
	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		for(u32 i=0; i<mQueryKeyList[j].size(); i++)
		{
			if(mQueryKeyList[j][i]->getFieldName() == "k2")
			{   
				min = mQueryKeyList[j][i]->getMinU64Value();
			}
		}
	}
	mExpectMinValue = min;

	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		for(u32 i=0; i<mQueryKeyList[j].size(); i++)
		{
			if(mQueryKeyList[j][i]->getFieldName() == "k2")
			{
				max = mQueryKeyList[j][i]->getMaxU64Value();
			}
		}
	}
	mExpectMaxValue = max;

	return true;
}


i64
AosQueryStatistics::getMinValue(map<OmnString, int> keyValueMap, OmnString measureField)
{	
	//create select key list
	createSelectKeyField();
	

	i64 min;
	if (measureField == "v1")
	{	
		//OmnString index_v = measureField.substr(1, 1);
		//int idx = index_v.toInt64(0);
		int key_index = keyValueMap[mSelectKeyList[0][0]];
		if(mSelectKeyList.size() == 1)
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{
				 if(mSelectKeyList[0][j] == "k1")
					 key_index = keyValueMap[mSelectKeyList[0][j]];
			}
		}
		//get key index value
		for(u32 i=0; i<mSelectKeyList.size(); i++)
		{
			if(i+1<mSelectKeyList.size())
			{
				if(keyValueMap[mSelectKeyList[i][0]] >= keyValueMap[mSelectKeyList[i+1][0]])
					key_index = keyValueMap[mSelectKeyList[i+1][0]];
			}
		}
	
		if (keyValueMap.find("k1") == keyValueMap.end())
		{
			//k1 not in group by list	
			createMinMaxValue();
			min = mExpectMinValue;
		}
		else
			min = key_index;
	}
	else if(measureField == "v2")
	{	
		//OmnString index_v = measureField.substr(1, 1);
		//int idx = index_v.toInt64(0);
		int key_index = keyValueMap[mSelectKeyList[0][0]];	
		if(mSelectKeyList.size() == 1)
		{
			for(u32 j=0; j<mSelectKeyList[0].size(); j++)
			{	 
				if(mSelectKeyList[0][j] == "k2")
					 key_index = keyValueMap[mSelectKeyList[0][j]];

				// if(j+1<mSelectKeyList[0].size())
				 //{   
				//	 if(keyValueMap[mSelectKeyList[0][j+1]] >= keyValueMap[mSelectKeyList[0][j]])
				//	 key_index = keyValueMap[mSelectKeyList[0][j]];
				// }
			}
		}
	
		//get key index value
		for(u32 i=0; i<mSelectKeyList.size(); i++)
		{
			if(i+1<mSelectKeyList.size())
			{
				if(keyValueMap[mSelectKeyList[i][0]] >= keyValueMap[mSelectKeyList[i+1][0]])
					key_index = keyValueMap[mSelectKeyList[i+1][0]];
			}
		}
	
		if (keyValueMap.find("k2") == keyValueMap.end())
		{
			//k2 not in group by list	
			createMinMaxValue();
			min = mExpectMinValue;
		}
		else
			min = key_index;
	}
	else
	{
		//d1	
	}

	return min;
}


i64
AosQueryStatistics::getDistinctValue(map<OmnString, int> keyValueMap, OmnString measureField)
{	
	int value = 0;

	if (measureField == "v1")
	{	
		OmnString index_v = measureField.substr(1, 1);
		int idx = index_v.toInt64(0);

		if (keyValueMap.find("k1") == keyValueMap.end())
		{
			//k1 not in group by list
			value = mSumKeyList[0][idx-1];
		}
		else
			value = 1;
	}
	else if(measureField == "v2")
	{	
		OmnString index_v = measureField.substr(1, 1);
		int idx = index_v.toInt64(0);

		if (keyValueMap.find("k2") == keyValueMap.end())
		{
			//k1 not in group by list
			value = mSumKeyList[0][idx-1];
		}
		else
			value = 1;
	}
	else
	{
		//d1	
	}

	return value;

}
	

i64
AosQueryStatistics::getCountValue(map<OmnString, int> keyValueMap, OmnString measureField)
{	
	getCountExpect();
	int count = 0;
	//int count_total = 0;
	//int s = 1;
	if (measureField == "v1")
	{	
		for(u32 j=0; j<mSelectKeyList.size(); j++)
		{
			/*for(u32 i=0; i<mSelectKeyList[j].size(); i++)
			{
				s = s*2;
			}
			if (keyValueMap.find("k1") == keyValueMap.end())
			{
				//k1 not in group by list
				if(mSelectKeyList.size() == 1)
					count = mCountExpected/s/mSelectKeyList.size();
				else
					count = mCountExpected/s/mSelectKeyList.size() * mTimeNum;
			}
			else
			{
				if(mQuery_in[1] == "")
					count = mCountExpected/s * mTimeNum;
				else
					count = mCountExpected/s;

				count_total += count;
			}
			*/
			if (keyValueMap.find("k1") == keyValueMap.end())
			{
				//k1 not in group by list
				count = mCountExpected * mTimeNum;
			}
			else
			{
				count = mCountExpected * mTimeNum;
			}
		}
	}
	else if(measureField == "v2")
	{	
		for(u32 j=0; j<mSelectKeyList.size(); j++)
		{
			/*for(u32 i=0; i<mSelectKeyList[j].size(); i++)
			{
				s = s*2;
			}
			if (keyValueMap.find("k2") == keyValueMap.end())
			{
				//k2 not in group by list
				//if(mSelectKeyList.size() == 1)
				//	count = mCountExpected/s/mSelectKeyList.size();
				//else
					count = mCountExpected/s/mSelectKeyList.size() * mTimeNum;
			}
			else
			{
				if(mQuery_in[1] == "")
					count = mCountExpected/s * mTimeNum;
				else
					count = mCountExpected/s;

				count_total += count;
			}
			*/
			if (keyValueMap.find("k2") == keyValueMap.end())
			{
				//k2 not in group by list
				count = mCountExpected * mTimeNum;
			}
			else
			{
				count = mCountExpected * mTimeNum;
			}

		}
	}

	return count;
}

/*
bool
AosQueryStatistics::verifyRecordCombination(
		const AosXmlTagPtr &rec_xml)
{
	AosQueryFieldPtr field;
	AosQueryTesterUtil::CondDef cond;
	u64 val;
	OmnString name = "";

	//currently, we may only check the field values
	//which is selected. 
	for (u32 i = 0; i < mFields.size(); i++)
	{
		field = mFields[i];
		if (field->isSelectField())
		{
			name = field->getFieldname();
			if (!rec_xml->hasAttribute(name, false))
			{
				cout << "Query Failed --- Field value not returned: "
					<< name << " --- " << rec_xml->toString() << endl; 
				return false;
			}

			//get the value from record xml
			val = rec_xml->getAttrU64(name, 0);
			if (!field->isInRange(val))
			{
				cout << "Query Failed --- Field value out of range: "
					<< name << " --- " << rec_xml->toString() << endl; 
				
				return false;
			}
		}
	}

	//verify condition fields meet requirements
	for (u32 i = 0; i < mConds.size(); i++)
	{
		cond = mConds[i];
		name = cond.getFieldname();
		if (!rec_xml->hasAttribute(name, false))
		{
			cout << "Query Failed --- Condition Field value not returned: "
				<< name << " --- " << rec_xml->toString() << endl; 
			return false;
		}

		//get the value from record xml
		val = rec_xml->getAttrU64(name, 0);
		if (!cond.isInRange(val))
		{
			cout << "Query Failed --- Field value doesn't meet condition: "
				<< name << " --- " << rec_xml->toString() 
				<< cond.getCondClause() << endl; 
			
			return false;
		}

		if (!verifySorting(rec_xml))
			return false;
	}
	
	return true;
}
*/

int
AosQueryStatistics::determineSortingIIL()
{
	// This function picks a field as the sorting field.
	aos_assert_r(mFields.size() > 0, -1);
	int field_idx = rand() % mFields.size();
	mSortingField = mFields[field_idx];
	mNumAlpha = mSortingField->isNumAlpha();
	return mFields[field_idx]->getFieldIdx();
}


bool
AosQueryStatistics::createQuery()
{
	// This function creates the conditions and determines the 
	// sorting field name. 
	/*aos_assert_r(mFields.size() > 0, false);

	vector<AosQueryFieldPtr> fields = mFields;
	mSortingFieldIdx = determineSortingIIL();

	AosQueryTesterUtil::CondDef cond;
	mNumEqualConds = 0;
	mSortFieldName = "";
	while (fields.size() > 0)
	{
		// 1. Pick a field
		int kk = rand() % fields.size();
		AosQueryFieldPtr field = fields[kk];
		fields[kk] = fields[fields.size()-1];
		fields.pop_back();

		// 2. Check whether it is the sorting IIL
		cond.order = false;
		if (field->getFieldIdx() == mSortingFieldIdx)
		{
			field->setSortingField(true);
			cond.order = true;	
			mSortFieldName = field->getFieldname();
		}
		else
		{
			field->setSortingField(false);
		}

		bool rslt = field->createCond(cond);

		if (field->getFieldIdx() == mSortingFieldIdx)
		{
			mReverse = cond.reverse;
		}

		if (cond.opr == eAosOpr_eq) mNumEqualConds++;

		aos_assert_r(rslt, false);
		mConds.push_back(cond);
	}

	aos_assert_r(mConds.size() > 0, false);
	aos_assert_r(mSortFieldName != "", false);

	//determine the count based on the condition
	mTotalExpected = 1;
	for (u32 i = 0; i < mConds.size(); i++)
	{
		cond = mConds[i];
		mTotalExpected = mTotalExpected * cond.getTotal(); 
		if (mTotalExpected == 0)
			break;
	}
	
	//for fields not in the condition, total value 
	//counts all the possible values
	if (mTotalExpected > 0)
	{
		AosQueryFieldPtr field;

		for (u32 i = 0; i < mFields.size(); i++)
		{
			field = mFields[i];
			if (!field->isCondField())
				mTotalExpected *= field->getMaxU64Value()
					- field->getMinU64Value() + 1;
		}
	}
	
	cout << "Total record to hit: " << mTotalExpected << endl;

	OmnString key = createQueryKeyList();
	OmnString time = createQueryTimeField();
	OmnString measure = createQueryMeasureField();

	vector<OmnString> vect;
	vect[0] = key;
	vect[1] = time;
	vect[2] = measure;

	*/
	return true;
}


bool
AosQueryStatistics::createCountKeyList()
{
	u64 max,min;
	int str_num = 0;
	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		mCountKeyList_k.clear();
		for(int i=0; i<mKey_n; i++)
		{
			max = mQueryKeyList[j][i]->getMaxU64Value();
			min = mQueryKeyList[j][i]->getMinU64Value();
			str_num = max - min + 1;

			mCountKeyList_k.push_back(str_num);
		}
		mCountKeyList.push_back(mCountKeyList_k);
	}

	return true;
}


bool
AosQueryStatistics::createQueryKeyField()
{

	mQueryKeyList = getKeyList();

	//the second time data
	mKey_n = rand()%(mQueryKeyList[0].size()) + 1;
	OmnString str_name;
	u64 max, min;
	int num = 0;
	for(u32 j=0; j<mQueryKeyList.size(); j++)
	{
		mSumKeyList_k.clear();
		for(u32 i=0; i<mQueryKeyList[j].size(); i++)
		{
			max = mQueryKeyList[j][i]->getMaxU64Value();
			//str_min = mQueryKeyList[j][i]->getMinValue();
			min = mQueryKeyList[j][i]->getMinU64Value();
			num = max - min + 1;
			
			mSumKeyList_k.push_back(num);
		}
		mSumKeyList.push_back(mSumKeyList_k);

		for(u32 i=mKey_n; i<mQueryKeyList[j].size(); i++)
		{
			max = mQueryKeyList[j][i]->getMaxU64Value();
			min = mQueryKeyList[j][i]->getMinU64Value();
			num = max - min + 1;

			mRestCountKeyList_k.push_back(num);
		}
		mRestCountKeyList.push_back(mRestCountKeyList_k);
	}

	for(int i=0; i<mKey_n; i++)
	{
		max = mQueryKeyList[0][i]->getMaxU64Value();
		min = mQueryKeyList[0][i]->getMinU64Value();
		num = max - min + 1;

		//if(i == (n-1))
			str_name = mQueryKeyList[0][i]->getFieldName();
		//else
		//	str_name << mQueryKeyList[i]->getFieldName() << ",";

		//mCountKeyList.push_back(str_num);
		mSelectKeyList_k.push_back(str_name);	
		
		max = mQueryKeyList[0][i]->getMaxU64Value();
		min = mQueryKeyList[0][i]->getMinU64Value();
		num = max - min + 1;

		//if(i == (n-1))
			str_name = mQueryKeyList[0][i]->getFieldName();
		//else
		//	str_name << mQueryKeyList[i]->getFieldName() << ",";
	}

	return true;
}


OmnString
AosQueryStatistics::createQueryTimeField()
{
	mQueryTimeField = getTimeField();

	OmnString time_field = mQueryTimeField[0]->getFieldName();

	u64 max = mQueryTimeField[0]->getMaxU64Value();  
	u64 min = mQueryTimeField[0]->getMinU64Value();
	int num = max - min + 1;
	mTimeNum = num;

	OmnString str = time_field;

	return str;
}


bool
AosQueryStatistics::createQueryMeasureField()
{
	OmnString str, aggr_func;
	mQueryMeasureList = getMeasureList();
	
	//get "v1, v2, v3" from measure
	for(u32 i=0; i<mQueryMeasureList[0].size(); i++)
	{
		OmnString queryValue = mQueryMeasureList[0][i]->getFieldName();
		mQueryValueList.push_back(queryValue);
	}

	//get "aggr" from measure
	for(u32 i=0; i<mAggrList.size(); i++)
	{
		OmnString queryAggr = mAggrList[i]->getAggrFunc();
		mQueryAggrList.push_back(queryAggr);
	}

	int n = rand()%mQueryValueList.size()+1;

	u32 rd = rand()%mQueryAggrList.size();
	aggr_func = mQueryAggrList[rd];
	
	for(int i=0; i<n; i++)
	{
		str << aggr_func << "(" << mQueryValueList[i] << ")";
		mSelectMeasureList.push_back(str);
		str = "";
	}

	return true;
}


OmnString
AosQueryStatistics::createSelectExpr()
{
	mQuery_in.clear();
	mSumKeyList.clear();
	mRestCountKeyList_k.clear();
	mRestCountKeyList.clear();
	mMaxKeyList.clear();
	mMinKeyList.clear();
	mCountKeyList_k.clear();
	mCountKeyList.clear();
	mSelectKeyList.clear();	 
	mSelectKeyList_k.clear();	 
	mQueryValueList.clear();
	mQueryAggrList.clear();
	mSelectMeasureList.clear();

	OmnString selExpr = "";
	mTimeNum = 1;
	OmnString time = createQueryTimeField();
	
	createQueryKeyField();
	createQueryMeasureField();

	OmnString key;
	for(u32 i=0; i<mSelectKeyList_k.size(); i++)
	{
		if(i == (mSelectKeyList_k.size()-1))
			key <<  mSelectKeyList_k[i];
		else
			key <<  mSelectKeyList_k[i] << ",";
	}

	OmnString measure;
	for(u32 i=0; i<mSelectMeasureList.size(); i++)
	{
		if(i == (mSelectMeasureList.size()-1))
			measure <<  mSelectMeasureList[i];
		else
			measure <<  mSelectMeasureList[i] << ",";
	}

	mQuery_in.push_back(key);
	mQuery_in.resize(3);
	mQuery_in[1] = time;
	mQuery_in[2] = measure;

	//random time field
	int n = rand()%1;
	if(n == 0)
	{
		time = "";
		mQuery_in[1] = time;
	}

	if(time == "" && measure !="")
		selExpr << key << "," << measure;
	else if(time == "" && measure == "")
		selExpr << key;
	else if(time != "" && measure == "")
		selExpr << key << "," << time;
	else
		selExpr << key << "," << measure << "," << time;
	return selExpr;
}


int
AosQueryStatistics::createInitStartIdx()
{
	// This function determines the initial query start position. 
	int nn = rand()%100;
	if (nn <= 90) return 0;

	return OmnRandom::intByRange(
			1000, 1000000, 20, 
			1000001, 100000000, 30, 
			100000001, 1000000000, 5);
}

int
AosQueryStatistics::createInitPageSize()
{
	int pSize = 0;

	if (rand()%100 < 90) 
	{
		pSize = mTester->getPagesize();
	}
	else
	{
		pSize = OmnRandom::intByRange(
			100, 1000, 40, 
			1001, 100000, 30,
			1000001, 1000000, 10);
	}

	cout << pSize << endl;

	/*
	if (pSize > 0)
		setPageSize(pSize);
	*/

	return mPageSize;
}


bool
AosQueryStatistics::docQueried(const OmnString &value, const u64 docid)
{
	// A doc was queried. It checks whether the doc is in the group.
	// 1. It uses the sorting field to convert 'docid' to a value. 
	// 2. The value shall no less (ascending order) than mCrtValue,
	//    otherwise it is an error.
	// 3. If the value is the same as mCrtValue, it is an error
	//    (note that in the current implementations, no duplicated
	//    values are allowed. We will expand this to support 
	//    duplicated values in the future).
	// 4. The values 
	// 		[next(mCrtValue), prev(docid)]
	// 	  shall not be in the condition. Otherwise, it is an error.
	// 5. If it is the first page, mCrtValue should not be set. 
	//    The first value should >= sorting field's first value.
	//    Otherwise, it is an error. Anything in the following
	//    range is invalid:
	//    	[first_value, prev(docid)]
	// 6. If it is move to, 
	mNumDocsQueried++;

	bool rslt = verifyNormal(value, docid);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosQueryStatistics::isValidDocid(const u64 &docid)
{
	//1. if the docid is not in the docid area, invalid
	//2. if in any of the fields, the docid is not fit for the condition, invalid.
	bool rslt = true;
	for(u32 i = 0;i < mFields.size();i++)
	{
		rslt = mFields[i]->isDocValid(docid);
		if(!rslt) return false;
	}
	return true;
}

bool 
AosQueryStatistics::isValid(const u64 &value)
{
	//1. if the docid is not in the docid area, invalid
	//2. if in any of the fields, the docid is not fit for the condition, invalid.
	u64 docid = mSortingField->value2Docid(value);
	if(!docid) return false;
		
	if(docid > mTester->getMaxDocid() || docid < mTester->getMinDocid())
	{
		return false;
	}
	bool rslt = false;
	for(u32 i = 0;i < mFields.size();i++)
	{
		rslt = mFields[i]->isDocValid(docid);
		if(!rslt) 
		{
			return false;
		}
	}
	
	return true;
}


bool 
AosQueryStatistics::verifyNormal(const OmnString &value, const u64 docid)
{
	// 'value' should be a non-zero digit string.
	u64 u64_value = atoll(value.data());
	aos_assert_r(u64_value > 0, false);

	aos_assert_r(before(mCrtU64Value,u64_value) || u64_value == mCrtU64Value, false);
	aos_assert_r(docid == mSortingField->value2Docid(u64_value), false);

	while (before(mCrtU64Value, u64_value))
	{
	 	// every value before current should be invalid
	 	bool valid = isValid(mCrtU64Value);
	 	if(valid)
	 	{
	 		OmnAlarm << "The value:" << mCrtU64Value << " should not be valid." << enderr;
	 		valid = isValid(mCrtU64Value);
	 	}
	 	aos_assert_r(!valid, false);
	 	next(mCrtU64Value);
	}

	aos_assert_r(mCrtU64Value == u64_value,false);
	bool valid = isValid(u64_value);
	aos_assert_r(valid, false);
	u64 docid2 = mSortingField->value2Docid(u64_value);
	aos_assert_r(docid == docid2,false);
	return true;	
}

bool
AosQueryStatistics::startQuery()
{
	mRundata->resetTimeTracker();
	mGroupStartTime = OmnGetTimestamp();
	mQueryStartTime = mGroupStartTime;
	mNumDocsQueried = 0;
	mNumMatchedDocs = 0;
	mCrtQueryPos = 0;
	mTotalExpected = 0;
	mVerified = 0;
	mLastSortValue = 0;
	mSortingField = 0;
	mSQLStmt = "";
	mConds.clear();

	//init all keys
	//AosQueryStatisyics::queryStatistics;
	/*
	mKeyList.clear();
	AosQueryFieldkey key;
	OmnString name;

	for (u32 i = 1; i <= 6; i++)
	{
		name = "";
		name << "k" << i;
		key.setFieldName(name);
	}
	*/
	
	return true;
}


OmnString
AosQueryStatistics::getQueryStat()
{
	OmnString stat = "<entry ";
	stat << "total_time=\"" << (mGroupEndTime - mGroupStartTime)/1000 << "\" "
		<< "num_matched_docs=\"" << mNumMatchedDocs << "\" "
		<< "num_conds=\"" << mConds.size() << "\" "
		<< "total_docs=\"" << mNumDocsQueried << "\">";

	for (u32 i=0; i<mConds.size(); i++)
	{
		stat << "<cond opr=\"" << AosOpr_toStr(mConds[i].opr) << "\" "
			<< "order=\"" << mConds[i].order << "\" "
			<< "reverse=\"" << mConds[i].reverse << "\" "
			<< "value1=\"" << mConds[i].value1 << "\" "
			<< "value2=\"" << mConds[i].value2 << "\" "
			<< "iilname=\"" << mConds[i].iilname<< "\"/>";
	}
	stat << "</entry>";
	return stat;
}


bool
AosQueryStatistics::groupQueryFinished()
{
	// if (sgPrintCount++ == 100) 
	// {
	// 	sgPrintCount = 0;
		printStat(0, true);
	// }

	if (sgShowLogLevel1)
	{
		cout << "Group finished successfully!!!!!!!!!!!!!!. Group ID: " << mGroupId << endl;
	}
	mGroupEndTime = OmnGetTimestamp();
	saveQueryStat();

	aos_assert_r(mTester, false);
	mTester->groupFinished(mGroupId);
	return true;
}


bool 
AosQueryStatistics::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &msg)
{
	OmnAlarm << "Query failed: " << rdata->getErrmsg() << ". GroupID: " << mGroupId << enderr;
	dumpQueryGroup();

	OmnFile file("failed_query", OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file.isGood(), false);
	bool rslt = file.put(0, mSQLStmt.data(), mSQLStmt.length(), true);
	aos_assert_r(rslt, false);
	mTester->groupFailed(mGroupId);
	return true;
}


bool
AosQueryStatistics::createGroup()
{
	// This function creates a query group. It randomly 
	// picks some fields for this query group.
	int total_fields = mTester->getNumFields();
	aos_assert_r(total_fields > 4, false);

	// int num_fields = OmnRandom::intByRange(2, 5, 100);
	// int num_fields = OmnRandom::nextInt(2, 5);
	int num_fields = 2;
	vector<AosQueryFieldPtr> fields = mTester->getFields();
	mFields.clear();
	for (int i=0; i<num_fields; i++)
	{
		int idx = rand() % fields.size();
		AosQueryFieldPtr field = fields[idx]->clone();
		fields[idx] = fields[fields.size()-1];
		fields.pop_back();
		mFields.push_back(field);
	}
	return true;
}


bool
AosQueryStatistics::saveQueryStat()
{
	static int lsMaxStatLength = 10000000;

	if (mStat == "") mStat = "<QueryStat>";
	mStat << getQueryStat();
	if (mStat.length() > lsMaxStatLength)
	{
		if (mStatFilename == "") mStatFilename = "query_statistics.xml";

		mStat << "</QueryStat>";
		OmnFile file(mStatFilename, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(file.isGood(), false);
		file.append(mStat, true);
		mStat = "<QueryStat>";
	}
	return true;
}


void
AosQueryStatistics::setShowLogLevel1(const bool b)
{
	sgShowLogLevel1 = b;
}


void
AosQueryStatistics::setShowLogLevel2(const bool b)
{
	sgShowLogLevel2 = b;
}


void
AosQueryStatistics::dumpQueryGroup()
{
	OmnString ss;
	ss << mRundata->getErrmsg() << "\n";

	ss << "Query Group: " << mGroupId << "\n";
	ss << "    Group Start Time: " << mGroupStartTime << "\n";
	ss << "    Group End Time: " << mGroupEndTime << "\n";
	ss << "    Query Start Time: " << mQueryStartTime << "\n";
	ss << "    Num Docs Queried: " << mNumDocsQueried << "\n";
	ss << "    Num Matched Docs: " << mNumMatchedDocs << "\n";
	ss << "    NumPagesQueried: " << mNumPagesQueried << "\n";
	ss << "    Docid Cycle Size: " << mDocidCycleSize << "\n";
	ss << "    Num Docs in Group: " << mNumDocsInGroup << "\n";
	ss << "    Sorting IIL Index: " << mSortingFieldIdx << "\n";
	ss << "    Start Docid: " << mStartDocid << "\n";
	ss << "    End Docid: " << mEndDocid << "\n";
	ss << "    Num Equal Conds: " << mNumEqualConds << "\n";
	ss << "    Docid Range Size: " << mDocidRangeSize << "\n";
	ss << "    Sample Docid Start: " << mSampleDocidStart<< "\n";
	ss << "    Sample Docid Size: " << smSampleDocidSize << "\n";
	ss << "    Range Freq: " << smRangeFreq << "\n";
	ss << "    Num Queried Sample Docids: " << mSampleDocids.size() << "\n";

	if (mConds.size() == 0)
	{
		ss << "    No conds" << "\n";
	}
	else
	{
		ss << "    Conditions: " << mConds.size() << "\n";
		for (u32 i=0; i<mConds.size(); i++)
		{
			ss << "        " << mConds[i].toString() << "\n";
		}
	}

	ss << "    Fields: " << mFields.size() << "\n";
	for (u32 i=0; i<mFields.size(); i++)
	{
		ss << "        Field: " << mFields[i]->toString() << "\n";
	}

	//ss << "    Seeds: \n" << mSeeds->dumpToStr(10) << "\n";

	// smapitr_t itr = mSampleDocids.begin();
	// int idx = 0;
	// int line = 0;
	// while (itr != mSampleDocids.end())
	// {
	// 	if (line++ > 20)
	// 	{
	// 		ss << "\n";
	// 		line = 0;
	// 	}
	// 	if (idx++ > 0) ss << ",";
	// 	ss << itr->first;
	// 	itr++;
	// }
	// ss << "\n";
	
	ss << "    Query: " << mSQLStmt << "\n\n";

	cout << ss << endl;

	OmnFile ff("query_group_dump.txt", OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert(ff.isGood());
	ff.put(0, ss.data(), ss.length(), true);

	//dumpMissingDocids();
}


OmnString
AosQueryStatistics::getFieldValues(const u64 docid)
{
	OmnString ss;
	for (u32 i=0; i<mFields.size(); i++)
	{
		if (i > 0) ss << ", ";
		ss << mFields[i]->docid2U64Value(docid);
	}

	return ss;
}


bool
AosQueryStatistics::verifyMultipleFields()
{
	// All fields have queried their bitmaps. This function 
	// ANDs all their bitmaps. That bitmap should contain
	// all and only the docids the query is supposed to have.
	AosBitmapObjPtr bitmap = mFields[0]->getQueriedBitmap();
	for (u32 i=1; i<mFields.size(); i++)
	{
		bitmap->orBitmap(mFields[i]->getQueriedBitmap());
	}

	return verifyDocidsForQuery(bitmap);
}


void
AosQueryStatistics::dumpMissingDocids()
{
	aos_assert(mSeeds->getNumBits() > 0);
	u64 docid = mTester->getStartDocid();
	
	cout << "Dumping missing docids" << endl;
	mMissedDocids.clear();
	u32 idx = 0;
	while (docid < mEndDocid && mMissedDocids.size() < 100)
	{
		docid += mSeedVec[idx++];
		if (idx >= mSeedVec.size()) idx = 0;
//if (!isSampleDocid(docid))continue;
if (!isDocInRange(docid)) continue;

//		if (!isDocInRange(docid)) break;
/*		smapitr_t itr = mSampleDocids.find(docid);
		if (itr == mSampleDocids.end())
		{
			if (line++ > 0) cout << ",";
			cout << docid;
			if (line > 10) line = 0;
			mMissedDocids.push_back(docid);
		}
		else
		{
			mSampleDocids.erase(itr);
		}
*/
	}
	cout << "\n";

	// Now, mSampleDocids contain only the docids that were
	// queried but not expected (considered as 'Extra Docids'

	if (mSampleDocids.size() > 0)
	{
		smapitr_t itr = mSampleDocids.begin();
		cout << "Number of Extra docids: " << mSampleDocids.size() << endl;
		int nn = 0;
		int line = 0;
		while (nn++ < 100 && itr != mSampleDocids.end())
		{
			if (line++ > 0) cout << ",";
			cout << itr->first;
			itr++;
		}
		cout << "\n";
	}
}


bool
AosQueryStatistics::verifyDocidsForOneField(
		const AosBitmapObjPtr &bitmap, 
		const AosQueryFieldPtr &field)
{
	// 'bitmap' contains all the bits the field is supposed to fetch.
	// It scans all the docids. Any docid that is in the field's range
	// should be in 'bitmap'. Otherwise, it is an error.
	vector<u64> extra_docids;
	vector<u64> missed_docids;
	cout << "Number of bits: " << bitmap->getNumBits() << endl;
	for (u64 docid=mStartDocid; docid<mEndDocid; docid++)
	{
		u64 value;
		if (!field->isInRange(docid, value))
		{
			// The docid should not be in the query
			if (bitmap->checkDoc(docid))
			{
				if (extra_docids.size() < 20)
				{
					extra_docids.push_back(docid);
				}
			}
		}
		else
		{
			// The docid should be in the query, so should it be in 'bitmap'.
			if (!bitmap->checkDoc(docid))
			{
				if (missed_docids.size() < 20)
				{
					missed_docids.push_back(docid);
				}
			}
		}
	}

	if (extra_docids.size() > 0)
	{
		cout << "********** Field contains extra docids!" << endl;
		for (u32 i=0; i<extra_docids.size(); i++)
		{
			if (i > 0) cout << ",";
			cout << extra_docids[i];
		}
		cout << endl;
	}

	if (missed_docids.size() > 0)
	{
		cout << "********** Field missing docids!" << endl;
		for (u32 i=0; i<missed_docids.size(); i++)
		{
			if (i > 0) cout << ",";
			cout << missed_docids[i];
		}
		cout << endl;
	}

	return true;
}


bool
AosQueryStatistics::verifyDocidsForQuery(
		const AosBitmapObjPtr &bitmap) 
{
	// 'bitmap' contains all the bits the query is supposed to fetch.
	// Any sample docid that satisifies 'field' condition should 
	// be in 'bitmap'. Otherwise, it is an error.
	cout << "Verify Sample Docids for the entire query ..." << endl;
	vector<u64> extra_docids;
	vector<u64> missed_docids;
	for (u64 docid=mStartDocid; docid<mEndDocid; docid++)
	{
		bool passed = true;
		for (u32 i=0; i<mFields.size(); i++)
		{
			u64 value;
			if (!mFields[i]->isInRange(docid, value))
			{
				passed = false;
				break;
			}
		}

		// The docid should not be in the query
		if (!passed && bitmap->checkDoc(docid))
		{
			if (extra_docids.size() < 20)
			{
				extra_docids.push_back(docid);
			}
		}
		else if (passed && !bitmap->checkDoc(docid))
		{
			if (missed_docids.size() < 20)
			{
				missed_docids.push_back(docid);
			}
		}
	}

	if (extra_docids.size() == 0 && missed_docids.size() == 0)
	{
		cout << "Verify Sample Docids for the entire query ... Passed!" << endl;
		return false;
	}

	cout << "Verify Sample Docids for the entire query ... Failed!" << endl;

	if (extra_docids.size() > 0)
	{
		cout << "Error: Field contains extra docids!" << endl;
		for (u32 i=0; i<extra_docids.size(); i++)
		{
			if (i > 0) cout << ",";
			cout << extra_docids[i];
		}
		cout << endl;
	}

	if (missed_docids.size() > 0)
	{
		cout << "Error: Field missing docids!" << endl;
		for (u32 i=0; i<missed_docids.size(); i++)
		{
			if (i > 0) cout << ",";
			cout << missed_docids[i];
		}
		cout << endl;
	}

	return false;
}


bool 
AosQueryStatistics::verifySorting(AosXmlTagPtr rec_xml)
{
	u64 val;
	OmnString name;
	OmnString orderStr = "normal";

	if (mReverse)
		orderStr = "reverse";

	if (mSortingField)
	{
		//get the sorting field value
		name = mSortingField->getFieldname();
		if (!rec_xml->hasAttribute(name, false))
		{
			cout << "Query Failed --- Sorting Field value not returned: "
				<< name << " --- " << rec_xml->toString() << endl; 
			return false;
		}

		//get the value from record xml
		val = rec_xml->getAttrU64(name, 0);
		//the first record need no check
		if (mVerified != 1)
		{
			if (!checkOrder(mLastSortValue, val))
			{
				cout << "Query Failed --- Sorting field not in " << orderStr << " order: "
					<< name << " --- " << rec_xml->toString() <<  " Previous value: " << mLastSortValue << endl; 
				
				return false;
			}
		}

		mLastSortValue = val;
	}

	return true;
}
	
bool 
AosQueryStatistics::queryFinished(
					const AosRundataPtr &rdata,  
					const AosQueryRsltObjPtr &results,
					const AosQueryProcObjPtr &proc)
{
	OmnNotImplementedYet;
	return false;
}

void
AosQueryStatistics::setFields(vector<AosQueryFieldPtr> fields)
{
	mFields = fields;
}

void
AosQueryStatistics::setPageSize(int pageSize)
{
	mPageSize = pageSize;
}

