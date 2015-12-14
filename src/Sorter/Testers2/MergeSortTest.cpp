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
// 2013/02/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Sorter/Testers2/MergeSortTest.h"

#include "Sorter/MergeFileSorter.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "DataSort/DataSort.h"
#include "DataSort/Ptrs.h"
#include "Sorter/MergeFileSorter.h"
#include "DataTypes/DataColOpr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include "Util/LocalFile.h"
#include "NetFile/NetFile.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
#include <hash_map>
using namespace std;
using namespace __gnu_cxx;

extern i64 sgNumber;
extern i64 sgTime;
extern i64 sgMaxRoundTime;
extern i64 sgMaxMemory;
extern i64 sgAlarmFlag;

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("MergeSortTest", __FILE__, __LINE__);   
AosMergeSortTest::AosMergeSortTest()
{
	mRound = 0;
	mNumber = sgNumber;
	mTime = sgTime;
	mMaxRoundTime = sgMaxRoundTime;
	mMaxMemory = sgMaxMemory;
	mAlarmFlag = sgAlarmFlag;
	
	mSeed = 1005;
	mPrintFlag = false;
}

bool 
AosMergeSortTest::start()
{
	system("mkdir /tmp/torturer/");
	if(mAlarmFlag) system("touch /tmp/torturer/alarm_flag_file");
	OmnString state_file_str = "/tmp/torturer/sorttorturer_states_file";
	mStateFile = OmnNew OmnFile(state_file_str, OmnFile::eCreate AosMemoryCheckerArgs);
	
	time_t total_time_dur = 0;
	time_t round_time_dur = 0;
	time_t round_start_time = 0;
	time_t start_time = time(NULL);
	time_t end_time = start_time;
	int k = 0;
	
	if(mNumber>0)
	{
		for(u64 k=0; k<mNumber; k++)
		{
			mStateFile->openFile(OmnFile::eAppend);
			round_start_time = time(NULL);
			
			sorttorturer();
		
			end_time = time(NULL);
			total_time_dur = (end_time-start_time);
			round_time_dur = (end_time-round_start_time);
			OmnString time_str = "total time : ";
			time_str << total_time_dur << "s\n";
			time_str << "current round take time : " << round_time_dur << "s\n";
			mStateFile->append(time_str);
			mStateFile->closeFile();
		}
	}
	else
	{	
		while(total_time_dur/60 < mTime)
		{
			mStateFile->openFile(OmnFile::eAppend);
			round_start_time = time(NULL);
			
			sorttorturer();
			
			end_time = time(NULL);
			total_time_dur = (end_time-start_time);
			round_time_dur = (end_time-round_start_time);
			OmnString time_str = "total time : ";
			time_str << total_time_dur << "s\n";
			time_str << "current round take time : " << round_time_dur << "s\n";
			mStateFile->append(time_str);
			mStateFile->closeFile();
		}    
	}
}

bool 
AosMergeSortTest::sorttorturer()
{
/*
 * Create
 	1. config record_num/check_num/field_num/aggr_flag
	2. config key_fields(with_order)
	3. create data and hash
	4. put into files
 
 * Sort

 * Check
 	1. get buff from sort_rslt
	2. update total num
	3. if it in hash
		count current key until current key finish
		compare aggr info
			if aggr
				update aggr info
			if not aggr
				check if it is the only record in rslt
				check aggr info
*/

//Create
	//data_type
	//	fixBin
	//	varLen
	//aggr_flag
	//field_num
//	fieldType* mFieldType = OmnNew fieldType[mNumFields];

	OmnString round_str = "\nRound : ";
	round_str << mRound << "\n";
	mStateFile->append(round_str);
	
	srand(mSeed++);
	OmnScreen << "==================================" << endl;
	OmnScreen << "         Round:<< " << mRound++ << endl;
	OmnScreen << "==================================" << endl;

	init();
	configFieldType();
	//key_fields(with order)
	configKeyAggrField();

	mHash.clear();
	mHashKeys.clear();
	//create files (HASH)

	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	mRundata = rdata;

	createFiles(rdata);

	OmnScreen << mConf << endl;
	//Sort
	mSort = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mVirFiles, rdata);
	//	sgThreadPool->proc(OmnNew AosGetOutputFile(mSort, mCmpFun, false, mRundata));
	//	bool rslt = false;

	mSort->sort();

	//Check
	if(mIfFixBin)
	{
		checkForFixBin();
	}
	else
	{
		checkForBuff();
	}
	deleteFile();

	return true;
}

	void
AosMergeSortTest::init()
{
//	mIfFixBin = true;
	mIfFixBin = rand()%2;
//	mIfAggr = false;
	mIfAggr = rand()%2;
//	mIfAggrSet = false;
	mIfAggrSet = rand()%2;
	mHashPos = -1;
//	mNumFields = 3;
	mNumFields = rand()%20+1;
//	mNumKeyFields = 1;
	mNumKeyFields = rand()%mNumFields+1;
//	mNumRecords = 3;
	mNumRecords = rand()%1000+1;
	mFixStrLen = 8;
	mCharElem<<"_1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

void
AosMergeSortTest::configKeyAggrField()
{
	bool* key_field_flag = OmnNew bool[mNumFields];
	memset(key_field_flag,0,sizeof(bool)*mNumFields);
	mKeyFieldPos = OmnNew i64[mNumKeyFields];
	memset(mKeyFieldPos,-1,sizeof(i64)*mNumKeyFields);
	u64 pos=0;
	u64 tmp_numkeyfields = 0;
	for(u64 i=0,j=0; i<mNumKeyFields; i++)
	{
		srand(mSeed++);
		pos = rand()%mNumFields;
		if(key_field_flag[pos]==0)
		{
			key_field_flag[pos] = 1;
			mKeyFieldPos[j++] = pos;
			tmp_numkeyfields++;
		}
	}
	mNumKeyFields = tmp_numkeyfields;

	bool* aggr_field_flag = OmnNew bool[mNumFields];
	memset(aggr_field_flag, 0, sizeof(bool)*mNumFields);
	if(mIfAggr)
	{
		if(mNumFields==mNumKeyFields)
		{
			mIfAggr = false;
			return;
		}
		if(mIfAggrSet)
		{
			mNumAggrFields = 1;
		}
		else
		{
			srand(mSeed++);
			mNumAggrFields = rand()%(mNumFields-mNumKeyFields);
		}
		mAggrFieldPos = OmnNew u64[mNumAggrFields];
		u64	tmp_numaggrfields = 0;
		for(u64 i=0,j=0; i<mNumAggrFields; i++ )
		{
			srand(mSeed++);
			pos = rand()%mNumFields;
			if(key_field_flag[pos]==0)
			{
				if(aggr_field_flag[pos]==0)
				{
					if(mFieldInfo[pos].type==eStr && !mIfAggrSet)
					{
						continue;
					}
					aggr_field_flag[pos] = 1;
					mAggrFieldPos[j++] = pos;
					tmp_numaggrfields++;
				}
			}
		}

		mNumAggrFields = tmp_numaggrfields;
		if(mNumAggrFields==0)
		{
			mIfAggr = false;
		}
		for(u64 i=0; i<mNumFields; i++)
		{
			if(aggr_field_flag[i]==1)
			{
				if(mFieldInfo[i].type == eStr || mIfAggrSet)
				{
					mFieldInfo[i].aggrtype = eSet;
				}
				else
				{
mFieldInfo[i].aggrtype = (aggrType)1;
//					mFieldInfo[i].aggrtype = (aggrType)(rand()%3);
				}
			}
		}
	}
}

void
AosMergeSortTest::configFieldType()
{
//	fieldType type;
	fieldInfo field_info;
	mFieldLen = 0;
	mFieldInfo.clear();
	for(u64 i=0; i<mNumFields; i++)
	{
		if(i==2&&percent(80))
		{
			field_info.type = eU64;
		}
		else
		{
			srand(mSeed++);
			field_info.type = (fieldType)(rand()%4);
		}
		mFieldInfo.push_back(field_info);
	}
}

bool
AosMergeSortTest::createFiles(const AosRundataPtr &rdata)
{
	if(mIfFixBin)
	{
		OmnScreen << "==============create Fixbin file===============" << endl;	
		return createFixBinFiles(rdata);
	}
	else
	{
		OmnScreen << "==============create buff file===============" << endl;	
		return createBuffFiles(rdata);
	}
}

bool
AosMergeSortTest::createFixBinFiles(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 file_id;
	OmnString file_name, tmp_file_name;
	AosNetFileObjPtr file;
	OmnLocalFilePtr tmp_file;

	tmp_file_name = "";
	tmp_file_name << "tmp_file";
	
	mFieldLen = 0;
	mFileLen = 10;
	int vv = mFileLen;

	OmnString tmp_str;
	i64	tmp_i64;
	u64	tmp_u64;
	d64	tmp_d64;
	int len;
	int record_len;
	mIfStrFromHash = OmnNew bool[mFieldInfo.size()];
	while(vv)
	{
		tmp_file = AosNetFileCltObj::createRaidFileStatic(
				mRundata.getPtr(), file_id, tmp_file_name, 0, false);
		aos_assert_r(tmp_file && tmp_file->isGood(), false);
		AosBuffPtr buff = OmnNew AosBuff(1000000*4  AosMemoryCheckerArgs);
		OmnScreen << "======files: " << vv << "======" << endl; 
		for(int k=0; k<mNumRecords; k++)
		{
			//rand()
			//1.2.3.4.5... fields get from existing data
/*
			memset(mIfStrFromHash, 0, sizeof(bool)*mNumFields);
			aggrInfo aggr_info;

			len = 0;
			record_len = 0;
			vector<int> lens;
			for (u64 j = 0; j<mFieldInfo.size(); j++)
			{
				len = (mFieldInfo[j].type==eStr? mFixStrLen : 8); 
				record_len += len;
				lens.push_back(len);
			}
			//
			// set record len
			AosBuff::encodeRecordBuffLength(record_len);
			buff->setInt(record_len);
*/
			mHashKey = "";
			mHashPos = -1;
			aggrInfo aggr_info;
			if(mHash.size())
			{
				srand(mSeed++);
				mHashPos = rand()%mHash.size();
			}
			mFieldValues.clear();
			mFieldLen = 0;
			for(u64 i=0; i<mNumFields; i++)
			{
				tmp_str = "";
				tmp_i64 = 0;
				tmp_u64 = 0;
				tmp_d64 = 0;
				
				fieldValue tmp_fieldvalue;
				i64_struct tmp_i64struct;
				u64_struct tmp_u64struct;
				d64_struct tmp_d64struct;
				str_struct tmp_strstruct;
				
				switch(mFieldInfo[i].type)
				{
					case eI64:
						tmp_i64 = getI64(mHashPos, i);
if(mPrintFlag)
cout << "I64:" << tmp_i64 << endl;
						buff->setI64(tmp_i64);
						mFieldInfo[i].fieldpos = mFieldLen;
						mFieldLen += 8;
						tmp_fieldvalue.i64_value = tmp_i64;
						tmp_fieldvalue.type = eI64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_i64struct.current.push_back(tmp_i64);
						tmp_i64struct.sum = tmp_i64;
						tmp_i64struct.max = tmp_i64;
						tmp_i64struct.min = tmp_i64;
						aggr_info.i64_aggr.push_back(tmp_i64struct);
						break;
					case eD64:
						tmp_d64 = getD64(mHashPos, i);
if(mPrintFlag)
cout << "D64:" << tmp_d64 << endl;
						buff->setDouble(tmp_d64);
						mFieldInfo[i].fieldpos = mFieldLen;
						mFieldLen += 8;
						tmp_fieldvalue.d64_value = tmp_d64;
						tmp_fieldvalue.type = eD64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_d64struct.current.push_back(tmp_d64);
						tmp_d64struct.sum = tmp_d64;
						tmp_d64struct.max = tmp_d64;
						tmp_d64struct.min = tmp_d64;
						aggr_info.d64_aggr.push_back(tmp_d64struct);
						break;
					case eU64:
						tmp_u64 = getU64(mHashPos, i);
if(mPrintFlag)
cout << "U64:" << tmp_u64 << endl;
						buff->setU64(tmp_u64);
						mFieldInfo[i].fieldpos = mFieldLen;
						mFieldLen += 8;
						tmp_fieldvalue.u64_value = tmp_u64;
						tmp_fieldvalue.type = eU64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_u64struct.current.push_back(tmp_u64);
						tmp_u64struct.sum = tmp_u64;
						tmp_u64struct.max = tmp_u64;
						tmp_u64struct.min = tmp_u64;
						aggr_info.u64_aggr.push_back(tmp_u64struct);
						break;
					case eStr:
						tmp_str = getStr(mHashPos, i);
if(mPrintFlag)
cout << "Str:" << tmp_str << endl;
						buff->setBuff(tmp_str.data(), tmp_str.length()+1);
						mFieldInfo[i].fieldpos = mFieldLen;
						mFieldLen += (8+1);
						mFieldInfo[i].strlen = (8+1);
						tmp_fieldvalue.str_value = tmp_str;
						tmp_fieldvalue.type = eStr;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_strstruct.current.push_back(tmp_str);
						tmp_strstruct.max = tmp_str;
						tmp_strstruct.min = tmp_str;
						aggr_info.str_aggr.push_back(tmp_strstruct);
						break;
					default:
						break;
				}
			}
//cout << "\n" << endl;	
			mHashKey = createHashKey(mFieldValues);
//cout << "HashKey:"<<mHashKey<<endl;
			//create a hashkey by key_field_values				
			if(mHash.size()<1000)
			{
				//if key in hash
				//	updateRecord();
				//if not in
				//	createRecord();
				//	updateRecord();
				if(!mHash.count(mHashKey))
				{
					aggr_info.num = 1;
					mHash.insert(pair<OmnString, aggrInfo>(mHashKey, aggr_info));
					mHashKeys.push_back(mHashKey);
				}
				else
				{
					updateRecord(aggr_info);	
				}
			}
			else
			{
				//if key in hush
				//	updateRecord();
				if(mHash.count(mHashKey))
				{
					updateRecord(aggr_info);
				}
			}
		}

		createCmpFunc(rdata);

		AosBuffArrayPtr array = OmnNew AosBuffArray(buff, mCmpFun, false);
		array->sort();
		AosBuffPtr new_buff = array->getBuff();
		aos_assert_r(buff->dataLen() == new_buff->dataLen(), false);

//		AosMergeFileSorter::sanitycheck(mCmpFun.getPtr(), new_buff->data(), new_buff->dataLen());
		rslt = tmp_file->append(new_buff->data(), new_buff->dataLen(), true); 
		aos_assert_r(rslt, false);

		//mWriteTotal += new_buff->dataLen();
		file = OmnNew AosIdNetFile(0, file_id);
		mVirFiles.push_back(file);
		//mFinished = true;
		vv--;
	}
//cout << mConf << endl;
	return true;
}

bool
AosMergeSortTest::createBuffFiles(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 file_id;
	OmnString file_name, tmp_file_name;
	AosNetFileObjPtr file;
	OmnLocalFilePtr tmp_file;

	int reamin_size = 0;
	tmp_file_name = "";
	tmp_file_name << "tmp_file";

	mIfStrFromHash = OmnNew bool[mFieldInfo.size()];

	u64 num_records = 0;
	mFileLen = 10;
	int vv = mFileLen;
	
	OmnString tmp_str;
	i64	tmp_i64;
	u64	tmp_u64;
	d64	tmp_d64;
	
	int len;
	int record_len;

	while(vv)
	{
		tmp_file = AosNetFileCltObj::createRaidFileStatic(
				mRundata.getPtr(), file_id, tmp_file_name, 0, false);
		aos_assert_r(tmp_file && tmp_file->isGood(), false);
		AosBuffPtr buff = OmnNew AosBuff(1000000*4  AosMemoryCheckerArgs);
		OmnScreen << "======files: " << vv << "======" << endl; 
		
		
		for(int k=0; k<mNumRecords; k++)
		{
//			OmnScreen << "=======Record:" << k << "=========" << endl;
			memset(mIfStrFromHash, 0, sizeof(bool)*mNumFields);
			aggrInfo aggr_info;

			len = 0;
			record_len = 0;
			vector<int> lens;
			for (u64 j = 0; j<mFieldInfo.size(); j++)
			{
				len = getLen(mFieldInfo[j].type, j);
				record_len += len;
				lens.push_back(len);
				record_len++;
			}
			//
			// set record len
			AosBuff::encodeRecordBuffLength(record_len);
			buff->setInt(record_len);
			
			mHashKey = "";
			mHashPos = -1;
			if(mHash.size())
			{
				srand(mSeed++);
				mHashPos = rand()%mHash.size();
			}

			mFieldValues.clear();
			mFieldLen = 0;
//OmnScreen << "\n" << endl;
			for (u64 j = 0; j < mFieldInfo.size(); j++)
			{
				tmp_str = "";
				tmp_i64 = 0;
				tmp_u64 = 0;
				tmp_d64 = 0;
				fieldValue tmp_fieldvalue;
				i64_struct tmp_i64struct;
				u64_struct tmp_u64struct;
				d64_struct tmp_d64struct;
				str_struct tmp_strstruct;
				switch(mFieldInfo[j].type)
				{
					case eI64:
						tmp_i64 = getI64(mHashPos, j);
						buff->setU8(0);
						buff->setI64(tmp_i64);
if(mPrintFlag)
cout << "I64: " << num_records++ << ":" <<tmp_i64 << endl;
						mFieldInfo[j].fieldpos = mFieldLen;
						mFieldLen++;
						tmp_fieldvalue.i64_value = tmp_i64;
						tmp_fieldvalue.type = eI64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_i64struct.current.push_back(tmp_i64);
						tmp_i64struct.sum = tmp_i64;
						tmp_i64struct.max = tmp_i64;
						tmp_i64struct.min = tmp_i64;
						aggr_info.i64_aggr.push_back(tmp_i64struct);
						break;
					case eD64:
						tmp_d64 = getD64(mHashPos, j);
						buff->setU8(0);
						buff->setDouble(tmp_d64);
if(mPrintFlag)
cout << "D64: " << num_records++ << ":" << tmp_d64 << endl;
						mFieldInfo[j].fieldpos = mFieldLen;
						mFieldLen++;
						tmp_fieldvalue.d64_value = tmp_d64;
						tmp_fieldvalue.type = eD64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_d64struct.current.push_back(tmp_d64);
						tmp_d64struct.sum = tmp_d64;
						tmp_d64struct.max = tmp_d64;
						tmp_d64struct.min = tmp_d64;
						aggr_info.d64_aggr.push_back(tmp_d64struct);
						break;
					case eU64:
						tmp_u64 = getU64(mHashPos, j);
						buff->setU8(0);
						buff->setU64(tmp_u64);
if(mPrintFlag)
cout << "U64: " << num_records++ << ":" << tmp_u64 << endl;
						mFieldInfo[j].fieldpos = mFieldLen;
						mFieldLen++;
						tmp_fieldvalue.u64_value = tmp_u64;
						tmp_fieldvalue.type = eU64;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_u64struct.current.push_back(tmp_u64);
						tmp_u64struct.sum = tmp_u64;
						tmp_u64struct.max = tmp_u64;
						tmp_u64struct.min = tmp_u64;
						aggr_info.u64_aggr.push_back(tmp_u64struct);
						break;
					case eStr:
						tmp_str = getStr(mHashPos, j, lens[j]);
if(mPrintFlag)
cout << "Str: " << num_records++ << ":" << tmp_str << endl;
						buff->setU8(0);
						buff->setCharStr(tmp_str.data(), lens[j]-4);
						mFieldInfo[j].fieldpos = mFieldLen;
						mFieldInfo[j].strlen = tmp_str.length();
						mFieldLen++;
						tmp_fieldvalue.str_value = tmp_str;
						tmp_fieldvalue.type = eStr;
						mFieldValues.push_back(tmp_fieldvalue);
						tmp_strstruct.current.push_back(tmp_str);
						tmp_strstruct.max = tmp_str;
						tmp_strstruct.min = tmp_str;
						aggr_info.str_aggr.push_back(tmp_strstruct);
						break;
					default:
						break;
				}
			}
		
			//buff->setU8(0);
			mHashKey = createHashKey(mFieldValues);
//cout << "HashKey:"<<mHashKey<<endl;
			//create a hashkey by key_field_values				
			if(mHash.size()<1000)
			{
				//if key in hash
				//	updateRecord();
				//if not in
				//	createRecord();
				//	updateRecord();
				if(!mHash.count(mHashKey))
				{
					aggr_info.num = 1;
					mHash.insert(pair<OmnString, aggrInfo>(mHashKey, aggr_info));
					mHashKeys.push_back(mHashKey);
				}
				else
				{
					updateRecord(aggr_info);	
				}
			}
			else
			{
				//if key in hush
				//	updateRecord();
				if(mHash.count(mHashKey))
				{
					updateRecord(aggr_info);
				}
			}
		}

		createCmpFunc(rdata);

		buff->reset();
		AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(true);
		array->setCompareFunc(mCmpFun.getPtr());
		rslt = array->setBodyBuff(buff, reamin_size);
		aos_assert_r(rslt, false);
		array->sort();
		AosBuffPtr head_buff = array->getHeadBuff();
		rslt = AosMergeFileSorter::sanitycheck(mCmpFun.getPtr(), head_buff->data(), head_buff->dataLen());
		aos_assert_r(rslt, false);
		AosBuffPtr body_buff;
		set<i64> bodyAddrSet;
		rslt = AosBuffArrayVar::procHeaderBuff(mCmpFun.getPtr(), head_buff, body_buff, bodyAddrSet);
		aos_assert_r(rslt, false);
		aos_assert_r(buff->dataLen() == body_buff->dataLen(), false);
		rslt = tmp_file->append(body_buff->data(), body_buff->dataLen(), true); 
		aos_assert_r(rslt, false);

		file = OmnNew AosIdNetFile(0, file_id);
		mVirFiles.push_back(file);
		vv--;
	}
	return true;
}

void
AosMergeSortTest::createCmpFunc(const AosRundataPtr &rdata)
{
	mConf = "";
	if(mIfFixBin)
	{
		mConf << "<compareFun"
			<< " cmpfun_reserve=\"" << false << "\""
			<< " record_type=\"fixbin\""
			<< " cmpfun_size=\"" << mFieldLen << "\""
			<< " cmpfun_type=\"custom\" >"
			<< 	"<cmp_fields>";
		for (u32 i = 0; i < mNumKeyFields; i++)
		{
			mConf <<  "<field ";
			if(mFieldInfo[mKeyFieldPos[i]].type==eStr)
			{
				mConf <<   " cmp_size=\"" << mFieldInfo[mKeyFieldPos[i]].strlen << "\"";
			}
			else
			{
				mConf <<  " cmp_size=\"8\"";
			}
			mConf <<  " cmp_datatype=\"" << toString(mFieldInfo[mKeyFieldPos[i]].type) << "\""
				  <<  " field_type=\"" << toString(mFieldInfo[mKeyFieldPos[i]].type) << "\""
				  <<  " cmp_pos=\"" << mFieldInfo[mKeyFieldPos[i]].fieldpos << "\"/>";
		}
		mConf <<  	"</cmp_fields>";
		if(mIfAggr)
		{
			mConf << "<aggregations>";
			for(u64 i=0; i<mNumAggrFields; i++)
			{
				mConf << "<aggregation agr_pos=\""<< mFieldInfo[mAggrFieldPos[i]].fieldpos << "\""
					  << " agr_type=\"" << toString(mFieldInfo[mAggrFieldPos[i]].type) 
					  << "\"  field_type=\"" 
				 	  << toString(mFieldInfo[mAggrFieldPos[i]].type) << "\" agr_fun=\""<<toString(mFieldInfo[mAggrFieldPos[i]].aggrtype)<<"\"/>";
			}
			mConf << "</aggregations>";
		}
		mConf << "</compareFun>";
	}
	else
	{
		mConf << "<compareFun"
			<< " cmpfun_reserve=\"" << false << "\""
			<< " record_type=\"buff\""
			<< " cmpfun_size=\"" << 4 + 8 + mNumFields * 2 << "\""
			<< " cmpfun_type=\"custom\" >"
			<< 	"<datafields>";

		for (u32 i = 0; i < mNumFields; i++)
		{
			mConf << "<field type=\"" << toString2(mFieldInfo[i].type) << "\"/>";
		}

		mConf << "</datafields>"
			<< "<cmp_fields>";
		for (u32 i = 0; i < mNumKeyFields; i++)
		{
			mConf <<  "<field cmp_size=\"-1\""
				 <<  " field_type=\"" << toString(mFieldInfo[mKeyFieldPos[i]].type) << "\""
				 <<	 " cmp_pos=\"" << mKeyFieldPos[i] << "\""
				 <<  " cmp_datatype=\"record\"/>";
		}
		mConf <<  	"</cmp_fields>";
		if(mIfAggr)
		{
			mConf << "<aggregations>";
			for(u64 i=0; i<mNumAggrFields; i++)
			{
				//需添加norm以外的类型
				mConf << "<aggregation agr_pos=\""<<mAggrFieldPos[i]<<"\" agr_type=\"record\"  field_type=\"" 
				 << toString(mFieldInfo[mAggrFieldPos[i]].type) << "\" agr_fun=\""<<toString(mFieldInfo[mAggrFieldPos[i]].aggrtype)<<"\"/>";
			}
			mConf << "</aggregations>";
		}
		mConf << "</compareFun>";
	}
	AosXmlTagPtr cmp_tag =  AosXmlParser::parse(mConf AosMemoryCheckerArgs);
	mCmpFun = AosCompareFun::getCompareFunc(cmp_tag);
}

bool
AosMergeSortTest::deleteFile()
{
	bool rslt = false;
	int server_id = -1;
	u64 file_id = 0;
	bool svr_death = false;
	for (u32 i = 0; i < mVirFiles.size(); i++)
	{
		server_id = mVirFiles[i]->getServerId();
		file_id = mVirFiles[i]->getFileId();
		rslt = AosNetFileCltObj::deleteFileStatic(file_id, server_id, svr_death, mRundata.getPtr());
		aos_assert_r(rslt, false);
	}
	mVirFiles.clear();
	return true;
}

bool
AosMergeSortTest::percent(const u64 &cen)
{
	srand(mSeed++);
	return (rand()%100)<cen;
}

i64
AosMergeSortTest::getI64(const i64 &hash_pos, const u64 &i)
{
	if(percent(80))
	{
		i64 rslt;
		srand(mSeed++);
		rslt = (i64)(((u64)rand())<<33 | ((u64)rand())<<2 | ((u64)rand())>>29);
		return rslt;
	}
	else
	{
		if(hash_pos == -1)
		{
			srand(mSeed++);
			return (i64)(((u64)rand())<<33 | ((u64)rand())<<2 | ((u64)rand())>>29);
		}

		mHashItr = mHash.find(mHashKeys[hash_pos]);

		u64 j=0;
		u64 k=0;
		while(j+k<i)
		{
			if(mFieldInfo[j+k].type==eI64)
			{
				j++;
			}
			else
			{
				k++;
			}
		}
		u64 size = mHashItr->second.i64_aggr[j].current.size();
		srand(mSeed++);
		return mHashItr->second.i64_aggr[j].current[rand()%size];
	}
}

u64
AosMergeSortTest::getU64(const i64 &hash_pos, const u64 &i)
{
	if(percent(80))
	{
		u64 rslt;
		srand(mSeed++);
		rslt = ((u64)rand())<<32 | (u64)rand();
		return rslt;
	}
	else
	{
		if(hash_pos == -1)
		{
			srand(mSeed++);
			return ((u64)rand())<<32 | (u64)rand();
		}

		mHashItr = mHash.find(mHashKeys[hash_pos]);

		u64 j=0;
		u64 k=0;
		while(j+k<i)
		{
			if(mFieldInfo[j+k].type==eU64)
			{
				j++;
			}
			else
			{
				k++;
			}
		}
		u64	size = mHashItr->second.u64_aggr[j].current.size();
		srand(mSeed++);
		return (u64)(mHashItr->second.u64_aggr[j].current[rand()%size]);
	}
}

d64
AosMergeSortTest::getD64(const i64 &hash_pos, const u64 &i)
{
	if(percent(80))
	{
		d64 rslt;
		srand(mSeed++);
		rslt = (d64)(((u64)rand())<<33 | ((u64)rand())<<2 | ((u64)rand())>>29);
		return rslt;
	}
	else
	{
		if(hash_pos == -1)
		{
			srand(mSeed++);
			return (d64)(((u64)rand())<<33 | ((u64)rand())<<2 | ((u64)rand())>>29);
		}

		mHashItr = mHash.find(mHashKeys[hash_pos]);

		u64 j=0;
		u64 k=0;
		while(j+k<i)
		{
			if(mFieldInfo[j+k].type==eD64)
			{
				j++;
			}
			else
			{
				k++;
			}
		}
		u64 size = mHashItr->second.d64_aggr[j].current.size();
		srand(mSeed++);
		return (d64)(mHashItr->second.d64_aggr[j].current[rand()%size]);
	}
}

OmnString
AosMergeSortTest::getStr(const i64 &hash_pos, const u64 &i)
{
	if(percent(80))
	{
		return randStr(8);
	}
	else
	{
		if(hash_pos == -1)
		{
			return randStr(8);
		}

		mHashItr = mHash.find(mHashKeys[hash_pos]);

		u64 j=0;
		u64 k=0;
		while(j+k<i)
		{
			if(mFieldInfo[j+k].type==eStr)
			{
				j++;
			}
			else
			{
				k++;
			}
		}
		u64 size = mHashItr->second.str_aggr[j].current.size();
		srand(mSeed++);
		return mHashItr->second.str_aggr[j].current[rand()%size];
	}
}

OmnString
AosMergeSortTest::getStr(const i64 &hash_pos, const u64 &j, const u64& len)
{
	if(mIfStrFromHash[j])
	{
		if(hash_pos == -1)
		{
			return randStr(len-4);
		}
		//return mHash[hash_pos].str_aggr[j];
	}
	else
	{
		return randStr(len-4);
	}
}

OmnString
AosMergeSortTest::randStr(const u64& len)
{
	OmnString rslt;
	for(u64 i=0; i<len; i++)
	{
		srand(mSeed++);
		rslt << mCharElem[rand()%63];
	}
	return rslt;
}

int
AosMergeSortTest::getLen(const fieldType& type, const u64& j)
{
	switch(type)
	{
		case eStr:
/*			if(percent(10))
			{
				mIfStrFromHash[j] = 1;
				//return mHash[mHashPos].str_aggr[j].length();
			}
*/
			return random()%50 + 5;
		case eD64:
			return sizeof(double);
		case eU64:
			return sizeof(u64);
		case eI64:
			return sizeof(i64);
	}
	OmnAlarm << enderr;
	return 0;
}

OmnString
AosMergeSortTest::createHashKey(const vector<fieldValue>& field_values)
{
	OmnString key1 = "";
	OmnString key2 = "";
	OmnString key3 = "";
	OmnString key = "";
	OmnString tmp_str;
	bool ifstr = false;
	u64 flag = 0;
	for(u64 i=0; i<mNumKeyFields; i++)
	{
		switch(field_values[mKeyFieldPos[i]].type)
		{
			case eI64:
				//key1+=field_values[mKeyFieldPos[i]].i64_value & 0xffffffff;
key<<field_values[mKeyFieldPos[i]].i64_value;
break;
				if(flag%3==0)
				{
					key1<<field_values[mKeyFieldPos[i]].i64_value;
				}
				if(flag%3==1)
				{
					key2<<field_values[mKeyFieldPos[i]].i64_value;
				}
				if(flag%3==2)
				{
					key3<<field_values[mKeyFieldPos[i]].i64_value;
				}
				flag++;
				break;
			case eU64:
				//key1+=field_values[mKeyFieldPos[i]].u64_value & 0xffffffff;
key<<field_values[mKeyFieldPos[i]].u64_value;
break;
				if(flag%3==0)
				{
					key1<<field_values[mKeyFieldPos[i]].u64_value;
				}
				if(flag%3==1)
				{
					key2<<field_values[mKeyFieldPos[i]].u64_value;
				}
				if(flag%3==2)
				{
					key3<<field_values[mKeyFieldPos[i]].u64_value;
				}
				flag++;
				break;
			case eD64:
				//tmp_str<<field_values[mKeyFieldPos[i]].d64_value;
				//key1+=tmp_str.getHashKey();
key<<field_values[mKeyFieldPos[i]].d64_value;
break;
				if(flag%3==0)
				{
					key1<<field_values[mKeyFieldPos[i]].d64_value;
				}
				if(flag%3==1)
				{
					key2<<field_values[mKeyFieldPos[i]].d64_value;
				}
				if(flag%3==2)
				{
					key3<<field_values[mKeyFieldPos[i]].d64_value;
				}
				flag++;
				//tmp_str="";
				break;
			case eStr:
				//ifstr = true;
				//key1+=field_values[mKeyFieldPos[i]].str_value.getHashKey();
key<<field_values[mKeyFieldPos[i]].str_value;
break;
				if(flag%3==0)
				{
					key1<<field_values[mKeyFieldPos[i]].str_value;
				}
				if(flag%3==1)
				{
					key2<<field_values[mKeyFieldPos[i]].str_value;
				}
				if(flag%3==2)
				{
					key3<<field_values[mKeyFieldPos[i]].str_value;
				}
				flag++;
				break;
		}
	}
	
	tmp_str << key.getHashKey();
	return tmp_str;
	tmp_str << key1.getHashKey() << key2.getHashKey() << key3.getHashKey();

	if(!ifstr)
	{
		switch(field_values[mKeyFieldPos[0]].type)
		{
			case eI64:
				tmp_str<<field_values[mKeyFieldPos[0]].i64_value;
				break;
			case eU64:
				tmp_str<<field_values[mKeyFieldPos[0]].u64_value;
				break;
			case eD64:
				tmp_str<<field_values[mKeyFieldPos[0]].d64_value;
				break;
		}
		key2<<tmp_str.getHashKey();
	}
	return key2<<key1;
}

i64
AosMergeSortTest::strToNum(const OmnString& str)
{
	i64	rslt = 0;
	for(int i=0; i<str.length(); i++)
	{
		rslt+=(i64)(str.data()[i]);
	}
	return rslt;
}

void
AosMergeSortTest::updateRecord(const aggrInfo aggr_info)
{
	mHashItr = mHash.find(mHashKey);
	
	vector<i64>::iterator i64_itr;
	vector<d64>::iterator d64_itr;
	vector<u64>::iterator u64_itr;
	vector<OmnString>::iterator str_itr;

	mHashItr->second.num = mHashItr->second.num+1;
	for(u64 i=0,j=0,m=0,n=0; i+j+m+n<mNumFields; )
	{
		if(mFieldInfo[i+j+m+n].type == eI64)
		{
			i64_itr = mHashItr->second.i64_aggr[i].current.begin();
			for(u64 ii=0; ii<mHashItr->second.i64_aggr[i].current.size(); ii++)
			{
				if(*i64_itr == aggr_info.i64_aggr[i].current[0])
				{
					break;
				}
				i64_itr++;
			}
			if(i64_itr>mHashItr->second.i64_aggr[i].current.end())
			{
				mHashItr->second.i64_aggr[i].current.push_back(aggr_info.i64_aggr[i].current[0]);
			}

			mHashItr->second.i64_aggr[i].sum += aggr_info.i64_aggr[i].sum;
			if(mHashItr->second.i64_aggr[i].max<aggr_info.i64_aggr[i].max)
			{
				mHashItr->second.i64_aggr[i].max = aggr_info.i64_aggr[i].max;
			}
			if(mHashItr->second.i64_aggr[i].min>aggr_info.i64_aggr[i].min)
			{
				mHashItr->second.i64_aggr[i].min = aggr_info.i64_aggr[i].min;
			}
			i++;
		}	
		else if(mFieldInfo[i+j+m+n].type == eU64)
		{
			u64_itr = mHashItr->second.u64_aggr[m].current.begin();
			for(u64 ii=0; ii<mHashItr->second.u64_aggr[m].current.size(); ii++)
			{
				if(*u64_itr == aggr_info.u64_aggr[m].current[0])
				{
					break;
				}
				u64_itr++;
			}
			if(u64_itr>mHashItr->second.u64_aggr[m].current.end())
			{
				mHashItr->second.u64_aggr[m].current.push_back(aggr_info.u64_aggr[m].current[0]);
			}

			mHashItr->second.u64_aggr[m].sum += aggr_info.u64_aggr[m].sum;
			if(mHashItr->second.u64_aggr[m].max<aggr_info.u64_aggr[m].max)
			{
				mHashItr->second.u64_aggr[m].max = aggr_info.u64_aggr[m].max;
			}
			if(mHashItr->second.u64_aggr[m].min>aggr_info.u64_aggr[m].min)
			{
				mHashItr->second.u64_aggr[m].min = aggr_info.u64_aggr[m].min;
			}
			m++;
		}
		else if(mFieldInfo[i+j+m+n].type == eD64)
		{
			d64_itr = mHashItr->second.d64_aggr[n].current.begin();
			for(u64 ii=0; ii<mHashItr->second.d64_aggr[n].current.size(); ii++)
			{
				if(*d64_itr == aggr_info.d64_aggr[n].current[0])
				{
					break;
				}
				d64_itr++;
			}
			if(d64_itr>mHashItr->second.d64_aggr[n].current.end())
			{
				mHashItr->second.d64_aggr[n].current.push_back(aggr_info.d64_aggr[n].current[0]);
			}

			mHashItr->second.d64_aggr[n].sum += aggr_info.d64_aggr[n].sum;
			if(mHashItr->second.d64_aggr[n].max<aggr_info.d64_aggr[n].max)
			{
				mHashItr->second.d64_aggr[n].max = aggr_info.d64_aggr[n].max;
			}
			if(mHashItr->second.d64_aggr[n].min>aggr_info.d64_aggr[n].min)
			{
				mHashItr->second.d64_aggr[n].min = aggr_info.d64_aggr[n].min;
			}
			n++;
		}
		else if(mFieldInfo[i+j+m+n].type == eStr)
		{
			str_itr = mHashItr->second.str_aggr[j].current.begin();
			for(u64 ii=0; ii<mHashItr->second.str_aggr[j].current.size(); ii++)
			{
				if(*str_itr == aggr_info.str_aggr[j].current[0])
				{
					break;
				}
				str_itr++;
			}
			if(str_itr>mHashItr->second.str_aggr[j].current.end())
			{
				mHashItr->second.str_aggr[j].current.push_back(aggr_info.str_aggr[j].current[0]);
			}

			if(mHashItr->second.str_aggr[j].max<aggr_info.str_aggr[j].max)
			{
				mHashItr->second.str_aggr[j].max = aggr_info.str_aggr[j].max;
			}
			if(mHashItr->second.str_aggr[j].min>aggr_info.str_aggr[j].min)
			{
				mHashItr->second.str_aggr[j].min = aggr_info.str_aggr[j].min;
			}
			j++;
		}
	}
}

bool
AosMergeSortTest::checkForBuff()
{
	AosBuffPtr buff = mSort->nextBuff();
	int record_len=mCmpFun->size;
	char* crt;
	char* tmp_crt = "";
	int num_buff = 1;

	u64	tmp_u64;
	i64 tmp_i64;
	d64 tmp_d64;
	OmnString tmp_str;
	fieldValue tmp_fieldvalue;
	i64_struct tmp_i64struct;
	str_struct tmp_strstruct;

	AosBuffArrayVarPtr buff_arry1;
	AosBuffPtr head_buff1;
	vector<fieldValue> tmp_field_value;
	vector<fieldValue> tmp_field_value1;
	u64 rslt_record_num = 0;
	OmnString tmp_hash_key = "";
	u64 same_record_num = 0;
	mHashKey = "";
	while(buff)
	{

		//check sort
		//check aggr
		OmnScreen << "number buff:" << num_buff++ << endl;
		u64 num;
		AosBuffPtr new_buff = OmnNew AosBuff(buff->dataLen() AosMemoryCheckerArgs);
		int reamin_size;
		char * crt;
		int str_len;
		int pos=0;
		crt=buff->data();
		OmnScreen << mConf << endl;
		while(pos<buff->dataLen())
		{
			pos+=4;
			mFieldValues.clear();
			//OmnScreen << "number record:" << rslt_record_num << endl;
			//cout << "\n" << endl;
			for(int i=0;i<mFieldInfo.size();i++)
			{
				switch(mFieldInfo[i].type)
				{
					case eU64:
						pos++;
						tmp_u64 = *(u64*)(crt+pos);
						tmp_fieldvalue.u64_value = tmp_u64;
						tmp_fieldvalue.type = eU64;
						tmp_fieldvalue.current = (i64)tmp_u64;
						mFieldValues.push_back(tmp_fieldvalue);
if(mPrintFlag)
cout << "U64:" << rslt_record_num << ":" <<tmp_u64<<endl;;
						pos+=8;
						break;
					case eI64:
						pos++;
						tmp_i64 = *(i64*)(crt+pos);
						tmp_fieldvalue.i64_value = tmp_i64;
						tmp_fieldvalue.type = eI64;
						tmp_fieldvalue.current = (i64)tmp_i64;
						mFieldValues.push_back(tmp_fieldvalue);
if(mPrintFlag)
cout << "I64:" << rslt_record_num << ":" <<tmp_i64<<endl;
						pos+=8;
						break;
					case eD64:
						pos++;
						tmp_d64 = *(d64*)(crt+pos);
						tmp_fieldvalue.d64_value = tmp_d64;
						tmp_fieldvalue.type = eD64;
						tmp_fieldvalue.current = (i64)tmp_d64;
						mFieldValues.push_back(tmp_fieldvalue);
if(mPrintFlag)
cout << "D64:" << rslt_record_num << ":" << tmp_d64 << endl;
						//OmnScreen << "D64:" <<tmp_d64<<endl;
						pos+=8;
						break;
					case eStr:
						pos++;
						str_len = *(int*)(crt+pos);
						pos+=4;
						tmp_str = OmnString(crt+pos,str_len);
						tmp_fieldvalue.str_value = tmp_str;
						tmp_fieldvalue.type = eStr;
						mFieldValues.push_back(tmp_fieldvalue);
if(mPrintFlag)
cout << "Str:" << rslt_record_num << ":" <<tmp_str<<endl;
						pos+=str_len;
						break;
				}
			}

			if(!checkSortByFieldValue(tmp_field_value, mFieldValues))
			{
				cout << "==========================================" << endl;
				cout << "==========================================" << endl;
				cout << mConf << endl;
				cout << "==========================================" << endl;
				cout << "Sort error records:" << endl;
				cout << "the record before last record:" << endl;
				printFieldValue(tmp_field_value1);
				cout << "last record:" << endl;
				printFieldValue(tmp_field_value);
				cout << "current record:" << endl;
				printFieldValue(mFieldValues);
				OmnAlarm << "Sort error" << enderr;
			}

			tmp_field_value1.clear();
			tmp_field_value1.assign(tmp_field_value.begin(), tmp_field_value.end());
			tmp_field_value.clear();
			tmp_field_value.assign(mFieldValues.begin(), mFieldValues.end());

			rslt_record_num++;
			
			tmp_hash_key = createHashKey(mFieldValues);
			if(tmp_hash_key == mHashKey || mHashKey == "")
			{
				same_record_num++;	
			}
			else
			{
				if(!mIfAggr && mHash.count(mHashKey))
				{
					checkAggrNum(mHashKey, same_record_num);
				}
				same_record_num = 1;
			}
			
			mHashKey = tmp_hash_key;
			//cout << "HashKey:"<<mHashKey<<endl;
			if(mHash.count(mHashKey))
			{
				if(mIfAggr)
				{
					bool rslt = checkAggr(mFieldValues);
					aos_assert_r(rslt,false);
				}
			}
		}

/*
		new_buff->setBuff(buff->data(), buff->dataLen());
		reamin_size = 0;
		AosBuffArrayVarPtr buff_arry = OmnNew AosBuffArrayVar(true);
		buff_arry->setCompareFunc(mCmpFun);
		bool rslt = buff_arry->setBodyBuff(new_buff, reamin_size, true);
		aos_assert_r(rslt, false);
		AosBuffPtr head_buff = buff_arry->getHeadBuff();

		crt = head_buff->data();
		num = head_buff->dataLen()/record_len;
//check sort between block

		if(tmp_crt!="")
		{
			if(mCmpFun->cmp(tmp_crt,crt)>0)
			{
				OmnScreen << mConf << endl;
				OmnScreen << "============print record==============" << endl;
				OmnScreen << "first record:" << endl;
				printRecord(tmp_crt);
				OmnScreen << "second record:" << endl;
				printRecord(crt);

				OmnAlarm << "sort error between buffs" << enderr;
			}
		}

//check sort in the same block
		for(i64 i=0; i<num-1; i++)
		{
			if(mCmpFun->cmp(crt,crt+record_len)>0)	
			{
				OmnScreen << "============print record==============" << endl;
				OmnScreen << "first record:" << endl;
				printRecord(crt);
				OmnScreen << "second record:" << endl;
				printRecord(crt+record_len);
				OmnAlarm << "sort error in the same buff" << enderr;
			}
			crt+=record_len;
		}

		buff_arry1 = OmnNew AosBuffArrayVar(true);
		buff_arry1->setCompareFunc(mCmpFun);
		rslt = buff_arry1->setBodyBuff(new_buff, reamin_size, true);
		aos_assert_r(rslt, false);
		head_buff1 = buff_arry1->getHeadBuff();
		tmp_crt = head_buff1->data();
		tmp_crt+=(num-1)*record_len;
*/
		buff = mSort->nextBuff();
	}

	if(!mIfAggr)
	{
		aos_assert_r(rslt_record_num==mNumRecords*mFileLen, false);
	}
	
	return true;
}

bool
AosMergeSortTest::checkForFixBin()
{
	char* crt;
	char* tmp_crt="";
	u64	tmp_u64;
	i64 tmp_i64;
	d64 tmp_d64;
	OmnString tmp_str;
	fieldValue tmp_fieldvalue;
	i64_struct tmp_i64struct;
	str_struct tmp_strstruct;

	vector<fieldValue> tmp_field_value;
	vector<fieldValue> tmp_field_value1;
	
	int record_len=mCmpFun->size;
	AosBuffPtr buff = mSort->nextBuff();
	AosBuffPtr tmp_buff;
	int num_buff = 1;
	u64 rslt_record_num = 0;
	OmnString tmp_hash_key = "";
	u64 same_record_num = 0;
	mHashKey = "";
	while(buff)
	{
		OmnScreen << "number buff:" << num_buff++ << endl;
		crt = buff->data();
/*
		if(tmp_crt!="")
		{
			if(mCmpFun->cmp(tmp_crt, crt)<0)
			{
				OmnScreen << mConf << endl;
				OmnScreen << "============print record==============" << endl;
				OmnScreen << "first record:" << endl;
				printRecord(tmp_crt);
				OmnScreen << "second record:" << endl;
				printRecord(crt);
				OmnAlarm << "sort error between buffs" << enderr;		
			}
		}
*/
		int pos=0;
		u64 num;
		OmnScreen << mConf << endl;
		while(pos<buff->dataLen())
		{
			mFieldValues.clear();
			OmnScreen << "number record:" << rslt_record_num << endl;
			for(int i=0;i<mFieldInfo.size();i++)
			{
				switch(mFieldInfo[i].type)
				{
					case eI64:
						tmp_i64 = *(i64*)(crt+pos);
//cout << "I64: " << tmp_i64 << endl;
						tmp_fieldvalue.i64_value = tmp_i64;
						tmp_fieldvalue.type = eI64;
						tmp_fieldvalue.current = tmp_i64;
						mFieldValues.push_back(tmp_fieldvalue);
						pos+=8;
						break;
					case eU64:
						tmp_u64 = *(u64*)(crt+pos);
//cout << "U64: " << tmp_u64 << endl;
						tmp_fieldvalue.u64_value = tmp_u64;
						tmp_fieldvalue.type = eU64;
						tmp_fieldvalue.current = (i64)tmp_u64;
						mFieldValues.push_back(tmp_fieldvalue);
						pos+=8;
						break;
					case eD64:
						tmp_d64 = *(d64*)(crt+pos);
//cout << "D64: " << tmp_d64 << endl;
						tmp_fieldvalue.d64_value = tmp_d64;
						tmp_fieldvalue.type = eD64;
						tmp_fieldvalue.current = (i64)tmp_d64;
						mFieldValues.push_back(tmp_fieldvalue);
						pos+=8;
						break;
					case eStr:
						tmp_str = OmnString(crt+pos,mFixStrLen+1);
//cout << "Str: " << tmp_str << endl;
						tmp_fieldvalue.str_value = tmp_str;
						tmp_fieldvalue.type = eStr;
						mFieldValues.push_back(tmp_fieldvalue);
						pos+=9;
						break;
					default:
						break;
				}
			}
//cout << '\n' << endl;
			if(!checkSortByFieldValue(tmp_field_value, mFieldValues))
			{
				cout << "==========================================" << endl;
				cout << "==========================================" << endl;
				cout << "Sort error records:" << endl;
				cout << mConf << endl;
				cout << "==========================================" << endl;
				cout << "the record before last record:" << endl;
				printFieldValue(tmp_field_value1);
				cout << "last record:" << endl;
				printFieldValue(tmp_field_value);
				cout << "current record:" << endl;
				printFieldValue(mFieldValues);
				OmnAlarm << "Sort error" << enderr;
			}

			tmp_field_value1.clear();
			tmp_field_value1.assign(tmp_field_value.begin(), tmp_field_value.end());
			tmp_field_value.clear();
			tmp_field_value.assign(mFieldValues.begin(), mFieldValues.end());

//cout << "\n" << endl;	
			rslt_record_num++;
			tmp_hash_key = createHashKey(mFieldValues);
			if(tmp_hash_key == mHashKey || mHashKey == "")
			{
				same_record_num++;	
			}
			else
			{
				if(!mIfAggr && mHash.count(mHashKey))
				{
					checkAggrNum(mHashKey, same_record_num);
				}
				same_record_num = 1;
			}

			mHashKey = tmp_hash_key;
			//cout << "HashKey:"<<mHashKey<<endl;
			if(mHash.count(mHashKey))
			{
				if(mIfAggr)
				{
					bool rslt = checkAggr(mFieldValues);
					aos_assert_r(rslt,false);
				}
			}
		}
/*
		u64 tmp_pos = 0;
		for(u64 i=0; i<buff->dataLen()/(mNumFields*8)-1; i++)
		{
//			int rslt=mCmpFun->cmp(crt, crt+mNumFields*8);
			if(!checkRecordSort(crt,crt+mNumFields*8))
			{
				OmnScreen << mConf << endl;
				OmnScreen << "============print record==============" << endl;
				OmnScreen << "first record:" << endl;
				printRecord(crt);
				OmnScreen << "second record:" << endl;
				printRecord(crt+mNumFields*8);
				OmnAlarm << "sort error in the same buff" << enderr;		
				checkRecordSort(crt,crt+mNumFields*8);
			}
			crt+=mNumFields*8;
		}
		
		tmp_crt=crt;
*/
		buff = mSort->nextBuff();
	}

	if(!mIfAggr)
	{
		aos_assert_r(rslt_record_num==mNumRecords*mFileLen, false);
	}

}

void
AosMergeSortTest::printRecord(char *crt)
{
	u64 pos = 4;
	if(mIfFixBin)
	{
		pos = 0;
	}
	u64 str_len = 0;
	OmnString str = "";
	for(u64 jj=0; jj<mNumFields; jj++)
	{
		switch(mFieldInfo[jj].type)
		{
			case eU64:
		//		OmnScreen << *(u64*)(crt+pos) << endl;
cout << "U64: " << *(u64*)(crt+pos) << endl;
				pos+=8;
				break;
			case eI64:
		//		OmnScreen << *(i64*)(crt+pos) << endl;
cout << "I64: " << *(i64*)(crt+pos) << endl;
				pos+=8;
				break;
			case eD64:
//				OmnScreen << *(d64*)(crt+pos) << endl;
cout << "D64: " << *(d64*)(crt+pos) << endl;
				pos+=8;
				break;
			case eStr:
				str_len = mFixStrLen;
				if(!mIfFixBin)
				{
					str_len = *(int*)(crt+pos);
					pos+=4;
				}
				str = OmnString(crt+pos,str_len);
//				OmnScreen << str << endl;
cout << "Str: " << str << endl;
				str = "";
				pos+=str_len;
				break;
			default:
				break;
		}
	}
}

bool
AosMergeSortTest::checkAggr(vector<fieldValue>& field_value)
{
	mHashItr = mHash.find(mHashKey);
	vector<i64>::iterator  i64_itr;
	vector<u64>::iterator  u64_itr;
	vector<d64>::iterator  d64_itr;
	vector<OmnString>::iterator  str_itr;
	u64* tmp_aggrfieldpos = OmnNew u64[mNumAggrFields];
	memcpy(tmp_aggrfieldpos, mAggrFieldPos, sizeof(u64)*mNumAggrFields);
	bool rslt = aggrPosSort(tmp_aggrfieldpos, mNumAggrFields);
	aos_assert_r(rslt, false);
	i64 tmp_i64 = 0;
	u64 tmp_u64 = 0;
	d64 tmp_d64 = 0;
	OmnString tmp_str = "";
	
	for(u64 i=0,j=0,m=0,n=0,k=0; (i+j+m+n<mNumFields)&&k<mNumAggrFields;)
	{
		if(mFieldInfo[i+j+m+n].type==eI64)
		{
			if(tmp_aggrfieldpos[k]==i+j+m+n)
			{
				switch(mFieldInfo[tmp_aggrfieldpos[k]].aggrtype)
				{
					case eSet:
						tmp_i64 = field_value[tmp_aggrfieldpos[k]].i64_value;
						i64_itr = mHashItr->second.i64_aggr[i].current.begin();
						for(u64 ii=0; ii<mHashItr->second.i64_aggr[i].current.size(); ii++)
						{
							if(*i64_itr==tmp_i64)
							{
								break;
							}
							i64_itr++;
						}
						aos_assert_r(i64_itr<=mHashItr->second.i64_aggr[i].current.end(),false);
						break;
					case eMax:
						aos_assert_r(mHashItr->second.i64_aggr[i].max==field_value[tmp_aggrfieldpos[k]].i64_value,false);
						break;
					case eMin:
						aos_assert_r(mHashItr->second.i64_aggr[i].min==field_value[tmp_aggrfieldpos[k]].i64_value,false);
						break;
					case eNorm:
						/////////sum???????
						aos_assert_r(mHashItr->second.i64_aggr[i].sum==field_value[tmp_aggrfieldpos[k]].i64_value,false);
						break;
					default:
						break;
				}
				k++;
			}
			i++;
		}
		else if(mFieldInfo[i+j+m+n].type==eU64)
		{
			if(tmp_aggrfieldpos[k]==i+j+m+n)
			{
				switch(mFieldInfo[tmp_aggrfieldpos[k]].aggrtype)
				{
					case eSet:
						tmp_u64 = field_value[tmp_aggrfieldpos[k]].u64_value;
						u64_itr = mHashItr->second.u64_aggr[m].current.begin();
						for(u64 ii=0; ii<mHashItr->second.u64_aggr[m].current.size(); ii++)
						{
							if(*u64_itr==tmp_u64)
							{
								break;
							}
							u64_itr++;
						}
						aos_assert_r(u64_itr<=mHashItr->second.u64_aggr[m].current.end(),false);
						break;
					case eMax:
						aos_assert_r(mHashItr->second.u64_aggr[m].max==field_value[tmp_aggrfieldpos[k]].u64_value,false);
						break;
					case eMin:
						aos_assert_r(mHashItr->second.u64_aggr[m].min==field_value[tmp_aggrfieldpos[k]].u64_value,false);
						break;
					case eNorm:
						/////////sum???????
						aos_assert_r(mHashItr->second.u64_aggr[m].sum==field_value[tmp_aggrfieldpos[k]].u64_value,false);
						break;
					default:
						break;
				}
				k++;
			}
			m++;
		}
		else if(mFieldInfo[i+j+m+n].type==eD64)
		{
			if(tmp_aggrfieldpos[k]==i+j+m+n)
			{
				switch(mFieldInfo[tmp_aggrfieldpos[k]].aggrtype)
				{
					case eSet:
						tmp_d64 = field_value[tmp_aggrfieldpos[k]].d64_value;
						d64_itr = mHashItr->second.d64_aggr[n].current.begin();
						for(u64 ii=0; ii<mHashItr->second.d64_aggr[n].current.size(); ii++)
						{
							if(*d64_itr==tmp_d64)
							{
								break;
							}
							d64_itr++;
						}
						aos_assert_r(d64_itr<=mHashItr->second.d64_aggr[n].current.end(),false);
						break;
					case eMax:
						aos_assert_r(mHashItr->second.d64_aggr[n].max==field_value[tmp_aggrfieldpos[k]].d64_value,false);
						break;
					case eMin:
						aos_assert_r(mHashItr->second.d64_aggr[n].min==field_value[tmp_aggrfieldpos[k]].d64_value,false);
						break;
					case eNorm:
						/////////sum???????
						aos_assert_r(mHashItr->second.d64_aggr[n].sum==field_value[tmp_aggrfieldpos[k]].d64_value,false);
						break;
					default:
						break;
				}
				k++;
			}
			n++;
		}
		else if(mFieldInfo[i+j+m+n].type==eStr)
		{
			if(tmp_aggrfieldpos[k]==i+j+m+n)
			{
				switch(mFieldInfo[tmp_aggrfieldpos[k]].aggrtype)
				{
					case eSet:
						tmp_str = field_value[tmp_aggrfieldpos[k]].str_value;
						str_itr = mHashItr->second.str_aggr[j].current.begin();
						for(u64 ii=0; ii<mHashItr->second.str_aggr[j].current.size(); ii++)
						{
							if(*str_itr == tmp_str)
							{
								break;
							}
							str_itr++;
						}
						aos_assert_r(str_itr<=mHashItr->second.str_aggr[j].current.end(),false);
						break;
					case eMax:
						aos_assert_r(mHashItr->second.str_aggr[j].max==field_value[tmp_aggrfieldpos[k]].str_value,false);
						break;
					case eMin:
						aos_assert_r(mHashItr->second.str_aggr[j].min==field_value[tmp_aggrfieldpos[k]].str_value,false);
						break;
					default:break;
				}
				k++;
			}
			j++;
		}
	}
	return true;	
}

bool
AosMergeSortTest::checkAggrNum(const OmnString& key, const u64& num)
{
	mHashItr = mHash.find(key);
	aos_assert_r(mHashItr->second.num==num, false);
}

bool
AosMergeSortTest::aggrPosSort(u64* aggr_pos, const u64 num)
{
	for(u64 i=0; i<num; i++)
	{
		for(u64 j=1; j<num-i; j++)
		{
			if(aggr_pos[j]<aggr_pos[j-1])
			{
				u64 tmp = aggr_pos[j];
				aggr_pos[j] = aggr_pos[j-1];
				aggr_pos[j-1] = tmp;
			}
		}
	}
	return true;
}

bool
AosMergeSortTest::checkRecordSort(const char* l, const char * r)
{
	int rslt=1;
	for(int i=0; i<mNumKeyFields; i++)
	{
		switch(mFieldInfo[mKeyFieldPos[i]].type)
		{
			case eStr:
				if(strncmp(l+mFieldInfo[mKeyFieldPos[i]].fieldpos,r+mFieldInfo[mKeyFieldPos[i]].fieldpos,mFixStrLen)>0)
				{
					return false;
				}
				break;
			case eI64:
				{
					i64 l_val = *(i64*)(l+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					i64 r_val = *(i64*)(r+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					if(l_val>r_val)
					{
						return false;
					}
				}
				break;
			case eD64:
				{
					d64 l_val = *(d64*)(l+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					d64 r_val = *(d64*)(r+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					if(l_val>r_val)
					{
						return false;
					}
				}
				break;
			case eU64:
				{
					u64 l_val = *(u64*)(l+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					u64 r_val = *(u64*)(r+mFieldInfo[mKeyFieldPos[i]].fieldpos);
					if(l_val>r_val)
					{
						return false;
					}
				}
				break;
		}
	}
	return rslt;
}

bool
AosMergeSortTest::checkSortByFieldValue(vector<fieldValue>& l, vector<fieldValue>& r)
{
	int jj;
	if(l.size()==0)
	{
		return true;
	}
	for(i64 i=0; i<mNumKeyFields; i++)
	{
		switch(mFieldInfo[mKeyFieldPos[i]].type)
		{
			case eStr:
				/*
				if((r[mKeyFieldPos[i]].str_value.getChar(0) >= 'A' && 
						r[mKeyFieldPos[i]].str_value.getChar(0) <= 'Z' &&
						l[mKeyFieldPos[i]].str_value.getChar(0) >= 'a' &&
						l[mKeyFieldPos[i]].str_value.getChar(0) <= 'z') 
						||
					(r[mKeyFieldPos[i]].str_value.getChar(0) >= '0' &&
					 	r[mKeyFieldPos[i]].str_value.getChar(0) <= '9' &&
						l[mKeyFieldPos[i]].str_value.getChar(0) 
						))
				{
					break;
				}
				*/
				if(strcmp(l[mKeyFieldPos[i]].str_value.data(), r[mKeyFieldPos[i]].str_value.data()) > 0)
				{
					OmnAlarm << enderr;
					return false;
				}
				else if(strcmp(l[mKeyFieldPos[i]].str_value.data(), r[mKeyFieldPos[i]].str_value.data()) != 0)
				{
					return true;
				}
				break;
			case eI64:
				if(l[mKeyFieldPos[i]].i64_value > r[mKeyFieldPos[i]].i64_value)
				{
					OmnAlarm << enderr;
					return false;
				}
				else if(l[mKeyFieldPos[i]].i64_value != r[mKeyFieldPos[i]].i64_value)
				{
					return true;
				}
				break;
			case eD64:
				if(l[mKeyFieldPos[i]].d64_value > r[mKeyFieldPos[i]].d64_value)
				{
					OmnAlarm << enderr;
					return false;
				}
				else if(l[mKeyFieldPos[i]].d64_value != r[mKeyFieldPos[i]].d64_value)
				{
					return true;
				}
				break;
			case eU64:
				if(l[mKeyFieldPos[i]].u64_value > r[mKeyFieldPos[i]].u64_value)
				{
					OmnAlarm << enderr;
					return false;
				}
				else if(l[mKeyFieldPos[i]].u64_value != r[mKeyFieldPos[i]].u64_value)
				{
					return true;
				}
				break;
		}
	}
	return true;	
}

void
AosMergeSortTest::printFieldValue(const vector<fieldValue>& field_value)
{
	for(i64 i=0; i<field_value.size(); i++)
	{
		switch(field_value[i].type)
		{
			case eStr:
				cout << "Str: " << field_value[i].str_value << endl;
				break;
			case eI64:
				cout << "I64: " << field_value[i].i64_value << endl;
				break;
			case eD64:
				cout << "D64: " << field_value[i].d64_value << endl;
				break;
			case eU64:
				cout << "U64: " << field_value[i].u64_value << endl;
				break;
		}
	}
	return;
}
