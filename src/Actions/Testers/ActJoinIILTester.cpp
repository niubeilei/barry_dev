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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActJoinIILTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosActJoinIILTester::AosActJoinIILTester()
:
mNumColumns(-1),
mDocid(10000),
mIILIDs(0),
mNumIILs(-1),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActJoinIILTester";
	for (int i=0; i<eMaxRecordSize; i++)
	{
		mRecords[i] = 0;
	}
	mRundata->setSiteid(100);
}


bool 
AosActJoinIILTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	//torturer();
	return true;
}


int sgCompFunc(const void *left, const void *right)
{
	const char *a = (const char *)left;
	const char *b = (const char *)right;
	return strcmp(a, b);
}


bool 
AosActJoinIILTester::basicTest()
{
	// This function creates an instance of AosActJoinIIL.
	// It joins:
	// 		IIL1: [phone_num, call_duration]
	// 		IIL2: [phone_num, num_shortmsg]
	//
	// 	<sdoc zky_type="joiniil">
	// 		<zky_iilscanners>
	// 			<iilscanner zky_iilname="???"
	// 				zky_matchtype="key"
	// 				zky_fieldsep="$"
	// 				zky_ignorematcherr="false"
	// 				pagesize="100">
	// 				<zky_valuecond 
	// 					zky_value1="*"
	// 					zky_opr1="an"/>
	// 				<zky_selectors>
	// 					<value_selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="key"
	// 						zky_fieldidx="0"
	// 						zky_createflag="true"
	// 						zky_ignore="true">
	// 						<zky_default_value type="string">xxx</zky_default_value>
	// 					</selector>
	// 					<selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="value"
	// 						zky_fieldidx="1"
	// 						zky_createflag="true">
	// 						<zky_default_value type="u64">0</zky_default_value>
	// 					</selector>
	// 				</zky_selectors>
	// 			</iilscanner>
	// 			<iilscanner zky_iilname="???"
	// 				zky_matchtype="key"
	// 				zky_fieldsep="$"
	// 				zky_ignorematcherr="false"
	// 				pagesize="100">
	// 				<zky_valuecond 
	// 					zky_value1="*"
	// 					zky_opr1="an"/>
	// 				<zky_selectors>
	// 					<value_selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="key"
	// 						zky_fieldidx="0"
	// 						zky_createflag="true"
	// 						zky_ignore="true">
	// 						<zky_default_value type="string">xxx</zky_default_value>
	// 					</selector>
	// 					<selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="value"
	// 						zky_fieldidx="2"
	// 						zky_createflag="true">
	// 						<zky_default_value type="u64">0</zky_default_value>
	// 					</selector>
	// 				</zky_selectors>
	// 			</iilscanner>
	// 		</zky_iilscanners>
	// 	</sdoc>
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("testers");
	aos_assert_r(tag, false);
	tag = tag->getFirstChild("sdoc");
	aos_assert_r(tag, false);

	return actionRun(tag, mRundata);
}


bool
AosActJoinIILTester::actionRun(const AosXmlTagPtr &xml, const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);
	aos_assert_r(rdata->getSiteid() > 0, false);

	AosActionObjPtr action = AosActionObj::createAction(xml, rdata);
	aos_assert_r(action, false);
	
	AosTaskObjPtr task = OmnNew AosTaskAction(false AosMemoryCheckerArgs);
	action->setTask(task);
	
	return action->run(xml, rdata);
}


bool
AosActJoinIILTester::torturer()
{
	// This function:
	// 1. Randomly creates a number of IILs,
	// 2. Randomly determines how to join the IILs,
	// 3. Do the joins, 
	// 4. Check the results.
	bool rslt;
	while (1)
	{
		// Reset
		OmnScreen << "Reset memory ..." << endl;
		rslt = reset();
		aos_assert_r(rslt, false);

		// Create Data
		OmnScreen << "To generate data ..." << endl;
		rslt = createTable();
		aos_assert_r(rslt, false);
		
		// Create IILs
		OmnScreen << "Creating IILs ..." << endl;
		rslt = createIILs();
		aos_assert_r(rslt, false);

		// Greate Config
		//OmnString confs = createConfs();
		//AosXmlTagPtr sdoc = AosXmlParser::parse(confs AosMemoryCheckerArgs);
		//aos_assert_r(sdoc, false);
		//OmnScreen << "Configurations: " << confs << endl;

		// Run Action
		//rslt = actionRun(sdoc, mRundata);
		//aos_assert_r(rslt, false);
	}
	return true;
}

	
bool
AosActJoinIILTester::reset()
{
	for (int i=0; i<eMaxRecordSize; i++)
	{
		delete [] mRecords[i];
		mRecords[i] = 0;
	}
	return true;
}


bool
AosActJoinIILTester::createTable()
{
	mMatchType = rand() % eMaxMatchTypes;
	mJoinType = rand() % eMaxJoinTypes;
	mNumColumns = rand() % (eMaxColumns - eMinColumns + 1) + eMinColumns;
	mNumIILs = mNumColumns - 1;
	mNumRecords = rand() % (eMaxRecordSize - eMinRecordSize + 1) + eMinRecordSize;
	mRecordLen = 0;
	
	OmnString iilname;
	for (int i=0; i<mNumColumns; i++)
	{
		if(mMatchType == eKey)
		{
			if(i == 0) mColumnTypes[0] = rand() % eMaxColumnsTypes;
			else mColumnTypes[i] = eU64;
		}
		else if(mMatchType == eValue)
		{
			if(i == 0) mColumnTypes[0] = eU64;
			else mColumnTypes[i] = rand() % eMaxColumnsTypes;
		}
		else
		{
			return false;
		}

		if(mColumnTypes[i] == eStr)
		{
			mColumnLens[i] = rand() % (eMaxStrLen - eMinStrLen + 1) + eMinStrLen;
		}
		else if(mColumnTypes[i] == eU64)
		{
			mColumnLens[i] = 8;
		}
		else
		{
			return false;
		}
		mRecordLen += mColumnLens[i];

		if(i == 0) continue;
		
		iilname = "_zt44_ken_iil_";
		iilname << mIILIDs++;
		
		IILInfo iil_info;
		iil_info.mIILName = iilname;
		mIILInfo[i] = iil_info;
	}
	
	OmnString str, ss;
	u64 vv = 0;

	OmnString key_str;
	u64 key_u64 = 0, value = 0;
	for (int i=0; i<mNumRecords; i++)
	{
		mRecords[i] = OmnNew char[mRecordLen + 1];
	
		str = "";
		ss = "";
		vv = 0;
		
		if(mMatchType == eKey)
		{
			if(mColumnTypes[0] == eStr)
			{
				key_str = OmnRandom::letterDigitStr(mColumnLens[0], mColumnLens[0], true);
				str << key_str;
			}
			else if(mColumnTypes[0] == eU64)
			{
				key_u64 = rand();
				str << key_u64;
			}
			else
			{
				return false;
			}
		}
		else if(mMatchType == eValue)
		{
			value = rand();
			str << value;
		}
		else
		{
			return false;
		}

		for (int col=1; col<mNumColumns; col++)
		{
			if (mColumnTypes[col] == eU64)
			{
				vv = rand();
				str << vv;

				// Chen Ding, 06/01/2012
				// addU64Doc(mIILInfo[i].mIILName, vv, );
			}
			else if(mColumnTypes[col] == eStr)
			{
				ss = OmnRandom::letterDigitStr(mColumnLens[i], mColumnLens[i], true);
				str << ss;
			}
		}
		strncpy(&mRecords[i][0], ss.data(), ss.length());
		mRecords[i][mRecordLen] = 0;
	}
	return true;
}


bool
AosActJoinIILTester::createIILs()
{
	for(int i=0; i<mNumRecords; i++)
	{
			
	}
	return true;
}











/*

bool
AosActJoinIILTester::splitTable()
{
	// 1. Determine the primary key
	//    A joined table has a primary key. The values of the primary key
	//    can be contributed by one or more IILs. For inner joins, the 
	//    primary key must appear in all the IILs. For outer joins, it may
	//    or may not appear in all the IILs.
	//
	//    Will randomly pick a column as the primary key. In the current
	//    implementations, primary keys must be string values.
	mPrimaryKeyIdx = rand() % mNumColumns;
	for (int i=0; i<mNumColumns; i++)
	{
		if (mColumnTypes[mPrimaryKeyIdx] == eString)
		{
			// Found the primary key. Need to sort the column.
			qsort(mColumnValues[mPrimaryKeyIdx], mTableSize, eMaxValueLen, sgCompFunc);
			break;
		}
		else
		{
			mPrimaryKeyIdx++;
			if (mPrimaryKeyIdx == mNumColumns) mPrimaryKeyIdx = 0;
		}
	}

	mColumnStatus[mPrimaryKeyIdx] = 1;

	// 2. Generate IILs.
	//    Each non-primary key column in a joined table is created in one of 
	//    the following ways:
	//    a. eKey: Direct copy from IIL key
	//    b. eValue: Direct copy from IIL value
	//    b. eField: A substring from the IIL key column
	mNumIILs = 0;
	for (int col_idx=0; col_idx<mNumColumns; col_idx++)
	{
		if (col_idx == mPrimaryKeyIdx) 
		{
			continue;
		}
		
		// It is not the primary key column. Determine the match type
		// mIILInfo[mNumIILs].match_type = 
		// 	(AosIILMatchType::E)(rand() % (AosIILMatchType::eMax-1) + 1);
		mIILInfo[mNumIILs].match_type = AosIILMatchType::eKey;

		mIILInfo[mNumIILs].field_idx = pickColumn();

		switch (mIILInfo[mNumIILs].match_type)
		{
		case AosIILMatchType::eKey:
			 createNewIILAsKey(mNumIILs, col_idx);
			 break;

		case AosIILMatchType::eValue:
			 // Currently, we do not support eValue. 
			 mIILInfo[mNumIILs].match_type = AosIILMatchType::eKey;
			 createNewIILAsKey(mNumIILs, col_idx);
			 break;

		// case AosIILMatchType::eFamily:
		// 	 createNewIILAsFamily(mNumIILs, col_idx);
		// 	 break;

		case AosIILMatchType::eMapper:
			 createNewIILAsMapper(mNumIILs, col_idx);
			 break;

		default:
			 OmnAlarm << "invalid match type: " << mIILInfo[mNumIILs].match_type << enderr;
			 return false;
		}

		// Note that in the current immplementations, each IIL contibutes to 
		// only one record value. We will enhance it later on.
		mNumIILs++;
	} 

	return true;
}


bool
AosActJoinIILTester::createNewIILAsKey(const int iilidx, const int col_idx)
{
	// The IIL's key is the same as mPrimaryKeyIdx, its value determines 
	// the 'col_idx' column. 
	// If it is inner join, for each primary key value, this IIL must 
	// have the value. If it is outer join, the value may or may not 
	// appear in this IIL. If it does not, it needs to set the default
	// value. 
	mIILInfo[iilidx].is_inner_join = rand() % 2;

	mIILInfo[iilidx].iilname = createIILName();
	mIILInfo[iilidx].value_type = generateValueType();
	if (mIILInfo[iilidx].is_inner_join)
	{
		// This is an inner join. For each primary key, the primary key
		// must appear in this IIL. But the IIL may contain values that 
		// are not primary keys. 
		//
		// For each matched value, the value of 'col_idx' is determined
		// by AosIILValueType. 
		int key_idx = 0;
		OmnString prev_key;
		OmnString crt_key;
		OmnString ss;
		for (int i=0; i<mTableSize; i++)
		{
			crt_key= &mColumnValues[mPrimaryKeyIdx][key_idx*eMaxValueLen];

			// Generate some values that are a primary key
			int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

			for (int kk=0; kk<nn; kk++)
			{
				ss = OmnRandom::generateValue(prev_key, crt_key);
				mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
			}

			// Generate the primary key value
			u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_key, col_idx, i);
			mIILInfo[iilidx].values.addValue(crt_key, docid, mRundata);

			prev_key = crt_key;
			key_idx++;
		}

		// Add some more values after the last primary key
		crt_key = "";
		int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

		for (int kk=0; kk<nn; kk++)
		{
			ss = OmnRandom::generateValue(prev_key, crt_key);
			mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
		}
	}
	else
	{
		// This is an outer join. For each primary key, the primary key
		// may or may not appear in this IIL. For primary keys that are not 
		// in this IIL, the corresponding value need to be set by this IIL's
		// default value. Below is what it needs to do:
		// 1. For each primary key, randomly determines whether it should 
		//    match or not. 
		// 2. If match, generate the key and docid accordingly. 
		// 3. If not, set the corresponding value to the default value
		//    of this IIL.
		mIILInfo[iilidx].default_value = OmnRandom::letterDigitStr(5, 10, true);
		for (int i=0; i<mTableSize; i++)
		{
			if (rand() % mOuterJoinWeight == 0)
			{
				// Need to generate a match.
				OmnString crt_key = &mColumnValues[col_idx][i*eMaxValueLen];
				u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_key, col_idx, i);
				mIILInfo[iilidx].values.addValue(crt_key, docid, mRundata);
			}
			else
			{
				// It is not a match. Need to set the default value.
				strcpy(&mColumnValues[col_idx][i*eMaxValueLen], mIILInfo[iilidx].default_value.data());
			}
		}
	}
	return true;
}


bool
AosActJoinIILTester::createNewIILAsFamily(const int iilidx, const int col_idx)
{
	// The primary key matches the prefix of the IIL. 
	//    A joined table has a primary key. The values of the primary key
	//    can be contributed by one or more IILs. For inner joins, the 
	//    primary key must appear in all the IILs. For outer joins, it may
	//    or may not appear in all the IILs.
	//
	//    Will randomly pick a column as the primary key. In the current
	//    implementations, primary keys must be string values.
	mIILInfo[iilidx].is_inner_join = rand() % 2;

	mIILInfo[iilidx].iilname = createIILName();
	mIILInfo[iilidx].value_type = generateValueType();
	if (mIILInfo[iilidx].is_inner_join)
	{
		// This is an inner join. For each primary key, the primary key
		// must appear in this IIL. But the IIL may contain values that 
		// are not primary keys. 
		//
		// For each matched value, the value of 'col_idx' is determined
		// by AosIILValueType. 
		int key_idx = 0;
		OmnString prev_value;
		OmnString crt_value;
		OmnString ss;
		for (int i=0; i<mTableSize; i++)
		{
			crt_value = &mColumnValues[mPrimaryKeyIdx][key_idx*eMaxValueLen];

			// Generate some values that are a primary key
			int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

			for (int kk=0; kk<nn; kk++)
			{
				ss = OmnRandom::generateValue(prev_value, crt_value);
				mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
			}

			// Generate the primary key value. Since this is to match the prefix,
			// the value should be:
			// 		crt_value + "|$$|" + string
			ss = OmnRandom::letterDigitStr(3, 10, true);
			crt_value << "|$$|" << ss;
			u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_value, col_idx, i);
			mIILInfo[iilidx].values.addValue(crt_value, docid, mRundata);

			prev_value = crt_value;
			key_idx++;
		}

		// Add some more values after the last primary key
		crt_value = "";
		int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

		for (int kk=0; kk<nn; kk++)
		{
			ss = OmnRandom::generateValue(prev_value, crt_value);
			mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
		}
	}
	else
	{
		// This is an outer join. For each primary key, the primary key
		// may or may not appear in this IIL. For primary keys that are not 
		// in this IIL, the corresponding value need to be set by this IIL's
		// default value. Below is what it needs to do:
		// 1. For each primary key, randomly determines whether it should 
		//    match or not. 
		// 2. If match, generate the key and docid accordingly. 
		// 3. If not, set the corresponding value to the default value
		//    of this IIL.
		mIILInfo[iilidx].default_value = OmnRandom::letterDigitStr(5, 10, true);
		for (int i=0; i<mTableSize; i++)
		{
			if (rand() % mOuterJoinWeight == 0)
			{
				// Need to generate a match.
				OmnString crt_value = &mColumnValues[col_idx][i*eMaxValueLen];
				u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_value, col_idx, i);
				mIILInfo[iilidx].values.addValue(crt_value, docid, mRundata);
			}
			else
			{
				// It is not a match. Need to set the default value.
				strcpy(&mColumnValues[col_idx][i*eMaxValueLen], mIILInfo[iilidx].default_value.data());
			}
		}
	}
	return true;
}


bool
AosActJoinIILTester::createNewIILAsMapper(const int iilidx, const int col_idx)
{
	// A 'mapper' is used to provide a prefix matching during joins. 
	// For instance, we may use a mapper to map phone numbers to geographic
	// locations. In this case, there may be two IILs, one with phone numbers
	// and the other (the mapper IIL) with phone number prefix. 
	//
	// During joins, when a match is found, the mapping-from IIL moves but
	// the mapper IIL do not move until the only IIL with the smallest value
	// is the mapper IIL. In that case, the mapper IIL moves to the next one.
	//
	// Mapper IILs may contribute to record field values. It is normally 
	// inner joins, but it is possible to be outer joins. 
	mIILInfo[iilidx].is_inner_join = rand() % 10;

	mIILInfo[iilidx].iilname = createIILName();
	mIILInfo[iilidx].value_type = generateValueType();
	if (mIILInfo[iilidx].is_inner_join)
	{
		// This is an inner join. For each primary key, the primary key
		// must appear in this IIL. But the IIL may contain values that 
		// are not primary keys. 
		//
		// For each matched value, the value of 'col_idx' is determined
		// by AosIILValueType. 
		int key_idx = 0;
		OmnString prev_key;
		OmnString crt_key;
		OmnString ss;
		for (int i=0; i<mTableSize; i++)
		{
			crt_key= &mColumnValues[mPrimaryKeyIdx][key_idx*eMaxValueLen];

			// Generate some values that are a primary key
			int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

			for (int kk=0; kk<nn; kk++)
			{
				ss = OmnRandom::generateValue(prev_key, crt_key);
				mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
			}

			// Generate the primary key value
			u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_key, col_idx, i);
			mIILInfo[iilidx].values.addValue(crt_key, docid, mRundata);

			prev_key = crt_key;
			key_idx++;
		}

		// Add some more values after the last primary key
		crt_key = "";
		int nn = OmnRandom::intByRange(
					0, 1, 50,
					2, 10, 50,
					11, 100, 20,
					101, 1000, 5,
					1001, 10000, 5);

		for (int kk=0; kk<nn; kk++)
		{
			ss = OmnRandom::generateValue(prev_key, crt_key);
			mIILInfo[iilidx].values.addValue(ss, rand(), mRundata);
		}
	}
	else
	{
		// This is an outer join. For each primary key, the primary key
		// may or may not appear in this IIL. For primary keys that are not 
		// in this IIL, the corresponding value need to be set by this IIL's
		// default value. Below is what it needs to do:
		// 1. For each primary key, randomly determines whether it should 
		//    match or not. 
		// 2. If match, generate the key and docid accordingly. 
		// 3. If not, set the corresponding value to the default value
		//    of this IIL.
		mIILInfo[iilidx].default_value = OmnRandom::letterDigitStr(5, 10, true);
		for (int i=0; i<mTableSize; i++)
		{
			if (rand() % mOuterJoinWeight == 0)
			{
				// Need to generate a match.
				OmnString crt_key = &mColumnValues[col_idx][i*eMaxValueLen];
				u64 docid = generateDocid(mIILInfo[iilidx].value_type, crt_key, col_idx, i);
				mIILInfo[iilidx].values.addValue(crt_key, docid, mRundata);
			}
			else
			{
				// It is not a match. Need to set the default value.
				strcpy(&mColumnValues[col_idx][i*eMaxValueLen], mIILInfo[iilidx].default_value.data());
			}
		}
	}
	return true;
}


u64
AosActJoinIILTester::generateDocid(
		const AosIILValueType::E value_type, 
		OmnString &key, 
		const int col_idx, 
		const int record_idx)
{
	// This function is called to create an IIL entry:
	// 		[key, docid]
	// where 'key' is passed in. The rules of governing the generation of 
	// docid, and possibly modifying 'key', are determined by 'value_type'.
	// This function may modify the column values since they are determined
	// by IIL entry values and value selection type.
	u64 vv;
	OmnString ss;
	switch (value_type)
	{
	case AosIILValueType::eKey:
		 // 1. IIL key is the same as 'key'
		 // 2. IIL docid can be randomly determined
		 // 3. The cell value is the same as 'key'. 
		 return rand();

	case AosIILValueType::eValue:
		 // 1. IIL key is the same as 'key'
		 // 2. IIL docid can be randomly determined.
		 // 3. The cell value is the same as 'docid'.
		 vv = rand();
		 ss << vv;
		 strcpy(&mColumnValues[col_idx][record_idx*eMaxValueLen], ss.data());
		 return vv;

	// case AosIILValueType::eFamily:
	// 	 // 1. IIL key is in the form:
	// 	 // 	'key' + "|$$|" + random string
	// 	 // 2. IIL docid can be randomly determined.
	// 	 // 3. The cell value is 'key'.
	// 	 strcpy(&mColumnValues[col_idx][record_idx*eMaxValueLen], key.data());
	// 	 key << "|$$|" << OmnRandom::digitStr(5, 8);
	// 	 return rand();

	// case AosIILValueType::eMember:
	// 	 // 1. IIL key is in the form:
	// 	 // 	'key' + "|$$|" + random string
	// 	 // 2. IIL docid can be randomly determined.
	// 	 // 3. The call value is the newly generated random string.
	// 	 ss = OmnRandom::digitStr(5, 8);
	// 	 strcpy(&mColumnValues[col_idx][record_idx*eMaxValueLen], ss.data());
	// 	 key << "|$$|" << ss;
	// 	 return rand();

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return AosIILValueType::eInvalid;
}


bool
AosActJoinIILTester::createConfForOneIIL(const int iilidx)
{
	// Each scanner requires the following:
	// 	iilname
	// 	matchtype
	// 	fieldsep
	// 	ignore_match_error
	// 	pagesize
	// 	value1
	// 	opr1
	// 	value2
	// 	opr2
	// 	selectors:
	// 		entry_selector	
	// 		value_type
	// 		field_idx
	// 		create_flag
	// 		dft_value
	
	// Determine iilname
	mScanners[iilidx].iilname 				= mIILInfo[iilidx].iilname;
	mScanners[iilidx].matchtype 			= mIILInfo[iilidx].match_type;
	mScanners[iilidx].fieldsep 				= getFieldSep();
	mScanners[iilidx].ignore_match_error 	= getIgnoreMatchError();
	mScanners[iilidx].pagesize 				= getPageSize();
	mScanners[iilidx].value1 				= getValue1();
	mScanners[iilidx].opr1 					= getOpr1();
	mScanners[iilidx].value2 				= getValue2();
	mScanners[iilidx].opr2 					= getOpr2();

	mScanners[iilidx].selectors.clear();

	// The first selector should be the key selector. 
	Selector selector;
	selector.entry_selector = getEntrySelectorType();
	selector.value_type = getPrimaryKeyValueType(iilidx);
	selector.field_idx = mPrimaryKeyIdx;
	selector.create_flag = (mIILInfo[iilidx].is_inner_join)?"false":"true";
	selector.dft_value = "";
	mScanners[iilidx].selectors.push_back(selector);
	
	if (mIILInfo[iilidx].field_idx >= 0)
	{
		// The second one will set a value column
		selector.entry_selector = getEntrySelectorType();
		selector.value_type = AosIILValueType::toString(mIILInfo[iilidx].value_type);
		selector.field_idx = mIILInfo[iilidx].field_idx;
		selector.create_flag = (mIILInfo[iilidx].is_inner_join)?"false":"true";
		selector.dft_value = mIILInfo[iilidx].default_value;
		mScanners[iilidx].selectors.push_back(selector);
	}

	return true;
}
	

OmnString
AosActJoinIILTester::getPrimaryKeyValueType(const int iilidx)
{
	switch (mIILInfo[iilidx].match_type)
	{
	case AosIILMatchType::eKey:
		 return AOSIILVALUETYPE_KEY;

	case AosIILMatchType::eValue:
		 return AOSIILVALUETYPE_VALUE;

	// case AosIILMatchType::eFamily:
	// 	 return AOSIILVALUETYPE_FAMILY;

	default:
		 break;
	}

	OmnAlarm << "Invalid match type: " << mIILInfo[iilidx].match_type << enderr;
	return "";
}


OmnString
AosActJoinIILTester::createOneScanner(const int idx)
{
	OmnString docstr;
	docstr
		<<	"<iilscanner zky_iilname=\"" << mScanners[idx].iilname << "\" "
		<<		"zky_matchtype=\"" << AosIILMatchType::toStr(mScanners[idx].matchtype) << "\" "
		<<		"zky_fieldsep=\"" << mScanners[idx].fieldsep << "\" "
		<<		"zky_ignorematcherr=\"" << mScanners[idx].ignore_match_error << "\" "
		<<		"pagesize=\"" << mScanners[idx].pagesize << "\">"
		<<		"<zky_valuecond "
		<<			"zky_value1=\"" << mScanners[idx].value1 << "\" "
		<<			"zky_opr1=\"" << mScanners[idx].opr1 << "\" "
		<<			"zky_value2=\"" << mScanners[idx].value2 << "\" "
		<<			"zky_opr2=\"" << mScanners[idx].opr2 << "\"/>"
		<<		"<zky_selectors>";

	for (u32 i=0; i<mScanners[idx].selectors.size(); i++)
	{
		docstr << "<selector "
			<< "zky_entryselector=\"" << mScanners[idx].selectors[i].entry_selector << "\" "
			<< "zky_value_type=\"" << mScanners[idx].selectors[i].value_type << "\" "
			<< "zky_fieldidx=\"" << mScanners[idx].selectors[i].field_idx << "\" "
			<< "zky_createflag=\"" << mScanners[idx].selectors[i].create_flag << "\">"
			<<		"<zky_default_value type=\"string\">" 
			<<			mScanners[idx].selectors[i].dft_value 
			<< 		"</zky_default_value>"
			<< "</selector>";
	}	

	docstr << "</zky_selectors></iilscanner>";

	return docstr;
}


OmnString
AosActJoinIILTester::getFieldSep()
{
	return "|$$|";
}


OmnString
AosActJoinIILTester::getIgnoreMatchError()
{
	return (rand() % 2)?"true":"false";
}


int
AosActJoinIILTester::getPageSize()
{
	return mPagesize;
}


OmnString
AosActJoinIILTester::getValue1()
{
	return "*";
}


OmnString
AosActJoinIILTester::getOpr1()
{
	return "an";
}


OmnString
AosActJoinIILTester::getValue2()
{
	return "*";
}


OmnString
AosActJoinIILTester::getOpr2()
{
	return "an";
}


OmnString
AosActJoinIILTester::getEntrySelectorType()
{
	return AOSENTRYSELTYPE_FIRST;
}


OmnString
AosActJoinIILTester::createConfs()
{
	// This function determines the joins. Joins are defined by
	// a smart doc:
	// 	<sdoc zky_type="joiniil">
	// 		<zky_iilscanners>
	// 			<iilscanner zky_iilname="???"
	// 				zky_matchtype="key"
	// 				zky_fieldsep="$"
	// 				zky_ignorematcherr="false"
	// 				pagesize="100">
	// 				<zky_valuecond 
	// 					zky_value1="*"
	// 					zky_opr1="an"/>
	// 				<zky_selectors>
	// 					<value_selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="key"
	// 						zky_fieldidx="0"
	// 						zky_createflag="true"
	// 						zky_ignore="true">
	// 						<zky_default_value type="string">xxx</zky_default_value>
	// 					</selector>
	// 					<selector 
	// 						zky_entryselector="first"
	// 						zky_value_type="value"
	// 						zky_fieldidx="1"
	// 						zky_createflag="true">
	// 						<zky_default_value type="u64">0</zky_default_value>
	// 					</selector>
	// 				</zky_selectors>
	// 			</iilscanner>
	// 			...
	// 		</zky_iilscanners>
	// 	</sdoc>
	for (int i=0; i<mNumIILs; i++)
	{
		createConfForOneIIL(i);
	}

	mNumScanners = mNumIILs;

	OmnString docstr = "<sdoc zky_type=\"joiniil\"><zky_iilscanners>";
	for (int i=0; i<mNumScanners; i++)
	{
		docstr << createOneScanner(i);
	}

	docstr << "</zky_iilscanners></sdoc>";
	return docstr;
}


OmnString
AosActJoinIILTester::createIILName()
{
	OmnString iilname = "_zt44_";
	iilname << rand();
	return iilname;
}


AosIILValueType::E
AosActJoinIILTester::generateValueType()
{
	return (AosIILValueType::E)(rand() % (AosIILValueType::eMax -1) + 1);
}


int 
AosActJoinIILTester::pickColumn()
{
	int nn = rand() % mNumColumns;
	for (int i=0; i<mNumColumns; i++)
	{
		if (mColumnStatus[nn] == -1)
		{
			mColumnStatus[nn] = 1;
			return nn;
		}
		nn++;
		if (nn >= mNumColumns) nn = 0;
	}

	return -1;
}


bool
AosActJoinIILTester::createIILs()
{
	int num_physicals = AosGetNumPhysicals();
	vector<AosBuffPtr> buffs;
	for (int i=0; i<num_physicals; i++)
	{
		AosBuffPtr buff = OmnNew AosBuff(100, 0 AosMemoryCheckerArgs);
		buffs.push_back(buff);
	}
	
	OmnString name;
	u64 value;
	int physical_id;
	for (int i=0; i<mNumIILs; i++)
	{
		OmnString iilname = mIILInfo[i].iilname;
		AosStrU64Array &values = mIILInfo[i].values;
		bool finished = false;
		while (!finished)
		{
			values.nextValue(name, value, finished);
			if (!finished)
			{
				bool rslt = AosAddStrValueDoc(buffs, iilname, name, value, false, false, 
						physical_id, mRundata);
				aos_assert_r(rslt, false);
			}
		}
	}
	return true;
}

*/
