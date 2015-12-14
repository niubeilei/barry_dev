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
// 04/01/2014 Created by Linda 
// 2014/08/12 Modified by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcStatDoc.h"

#include "API/AosApi.h"
#include "DataProc/DataProc.h"
#include "StatUtil/Statistic.h"
#include "StatTrans/BatchSaveStatDocsTrans.h"
#include "TaskMgr/Task.h"

////////////////////////////////////////////
//	constructors/destructors/config
////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcStatDoc_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcStatDoc(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}

AosDataProcStatDoc::AosDataProcStatDoc(const int version)
:AosStreamDataProc(version,AosJimoType::eDataProcStatDoc)
{
	mCachedRecords = eMaxCachedRecords;
	mMdfInit = false;
	mIsDistCount = false;
	mNumRecords = 0;
	mRcdBuff = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	mRcdBuff->reset();

	mRecordBuff2Conf = NULL;

	mRcdBuff2 = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	mRcdBuff2->reset();
	initCounters();

	//mStatMdfInfo.mOpr = "";
}

AosDataProcStatDoc::AosDataProcStatDoc(const AosDataProcStatDoc &proc)
:
AosStreamDataProc(0, AosJimoType::eDataProcStatDoc)
{
	mBuffs = proc.mBuffs;
	//mStatistic = proc.mStatistic;
	//mStatCubeConfMap = proc.mStatCubeConfMap;
	//mStatMdfInfo = proc.mStatMdfInfo;
	//mNumRecords = proc.mNumRecords;
	//mCachedRecords = proc.mCachedRecords;
	//mRcdBuff = proc.mRcdBuff;
	//mRcdBuff2 = proc.mRcdBuff2;
	//mMdfInit = proc.mMdfInit;
	//mCubeId = proc.mCubeId;
	//mTimeValuePos = proc.mTimeValuePos;
	//mTimeUnit = proc.mTimeUnit;
	//mMeasureSize = proc.mMeasureSize;
	//mLock = proc.mLock;
	//mRecordBuff2Conf = proc.mRecordBuff2Conf;
	//mOpr = proc.mOpr;
	//mJson = proc.mJson;
	//mIsStream = true;



	mCachedRecords = eMaxCachedRecords;
	mMdfInit = false;
	mIsDistCount = false;
	mNumRecords = 0;
	mRcdBuff = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	mRcdBuff->reset();

	mRecordBuff2Conf = NULL;

	mRcdBuff2 = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	mRcdBuff2->reset();

	initCounters();
}

AosDataProcStatDoc::~AosDataProcStatDoc()
{
	//if I still have something;
}



AosJimoPtr
AosDataProcStatDoc::cloneJimo() const
{
	bool isStream = false;
	if(!mJson.isNull())
	{
		JSONValue stream = mJson["streaming"];
		isStream = stream.asBool();
	}
	if(isStream)
		return OmnNew AosDataProcStatDoc(*this);
	else
		return (AosJimo*)this;
}


AosDataProcObjPtr
AosDataProcStatDoc::cloneProc()
{
	return this;
}

void
AosDataProcStatDoc::initCounters()
{
	mTotalTime = 0;
	mTotalCounter = 0;
	mCollectInputDataTime = 0;
	mConfigStatMdfTime = 0;
	mConfigStatMdfCounter = 0;
	mSendDocTime = 0;
	mSendDocCounter = 0;
	mMergeBuffTime = 0;
	mRebuildBuffTime = 0;
	mFirstMergeTime = 0;
	mSecondMergeTime = 0;
}

void
AosDataProcStatDoc::outputCounters()
{
	OmnScreen << "StatDoc Finish : " 
		<< "\ntotal time : " << mTotalTime 
		<< ",\ntolal counter : " << mTotalCounter 
		<< ",\ncollect inputdata time: " << mCollectInputDataTime
		<< ",\nconfig statmdfinfo time: " << mConfigStatMdfTime
		<< ",\nconfig statmdfinfo counter : " << mConfigStatMdfCounter
		<< ",\nsend buff time : " << mSendDocTime
		<< ",\nsend buff counter : " << mSendDocCounter
		<< ",\nmerge buff total time : " << mMergeBuffTime
		<< ",\nmerge rebuild buff time : " << mRebuildBuffTime
		<< ",\ndocid time merge time : " << mFirstMergeTime
		<< ",\ndocid merge time :" << mSecondMergeTime
		<< endl;
}



bool
AosDataProcStatDoc::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name="dp2" jimo_objid="dataprociilbatchopr_jimodoc_v0">
	//<![CDATA[
	//{
	//	"type": "statdoc",
	//	"stat_name": "db_t1_stat_ST_t1_internal_0"
	//}
	//]]></dataproc>
	
	mLock = OmnNew OmnMutex();

	aos_assert_r(def, false);
	mName = def->getAttrStr("zky_name");
	aos_assert_r(mName != "", false);

	OmnString dp_json = def->getNodeText();

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);

	//when use JqlSerive ,we should use this
	mJson = json;

	OmnScreen << "dataproc config json is: " << dp_json << endl;

	aos_assert_r(json.isMember("stat_objid"), false);
	OmnString stat_name = json["stat_objid"].asString();
	OmnString time_field_str = json["time_unit"].asString();
	aos_assert_r(stat_name!= "", false);

	mStatistic = AosStatistic::retrieveStat(rdata, stat_name);
	aos_assert_r(mStatistic, false);
	mTimeUnit = time_field_str;

	mOpr = json["opr"].asString();

	AosXmlTagPtr stat_cubes_conf = mStatistic->getStatCubesConf();
	aos_assert_r(stat_cubes_conf, false);

	u32 cube_id;
	AosXmlTagPtr each_conf = stat_cubes_conf->getFirstChild();
	mStatCubeConfMap.clear();
	while(each_conf)
	{
		cube_id = each_conf->getAttrU32("cube_id", 0);	

		mStatCubeConfMap.insert(make_pair(cube_id, each_conf));
		each_conf = stat_cubes_conf->getNextChild();
	}

	mIsStream = mIsStreaming;
	if(!mIsStream)
	{
		mMeasureSize = mStatistic->getMeasureSize();

		//set cubeid
		//get cube id which is shuffle Id for statjoin right now
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		OmnString value = task->getTaskENV(AOSTAG_SHUFFLE_ID, rdata);
		mCubeId = value.toInt(); 
	}
	return true;
}



bool AosDataProcStatDoc::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{

	u64 tStart,tEnd,tMiddle;
	tStart = OmnGetTimestamp();
	mLock->lock();
	if (mNumRecords > 0)
	{
		if(mIsDistCount)
		{
			tMiddle =  OmnGetTimestamp();
			AosBuffPtr buf = mergeRecord(mBuffs,mMeasureSize,mStatMdfInfo.mRecordType);
			
			tEnd = OmnGetTimestamp();
			mMergeBuffTime += tEnd - tMiddle;
			
			tMiddle =  OmnGetTimestamp();
			bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, buf);
			
			tEnd = OmnGetTimestamp();
			mSendDocTime += tEnd - tMiddle;
			mSendDocCounter++;
			
			if(!rslt)
			{
				mLock->unlock();
				return AosDataProcStatus::eError;
			}
		}
		else
		{
			if(mStatMdfInfo.mRecordType == AosDataRecordType::eBuff)
			{
				tMiddle =  OmnGetTimestamp();
				bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, mRcdBuff2);
				
				tEnd = OmnGetTimestamp();
				mSendDocTime += tEnd - tMiddle;
				mSendDocCounter++;
				
				if(!rslt)
				{
					mLock->unlock();
					return AosDataProcStatus::eError;
			
				}
			}
			else
			{
				tMiddle =  OmnGetTimestamp();
				bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, mRcdBuff);
				
				tEnd = OmnGetTimestamp();
				mSendDocTime += tEnd - tMiddle;
				mSendDocCounter++;
				
				if(!rslt)
				{
					mLock->unlock();
					return AosDataProcStatus::eError;
				}
			}
		}
		mMdfInit=false;
		mStatMdfInfo.clear();
	}
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcStatDoc");
	mLock->unlock();
	
	tEnd = OmnGetTimestamp();
	mTotalTime += tEnd-tStart;
	outputCounters();
	return true;
}

bool 
AosDataProcStatDoc::finish(
		const AosRundataPtr &rdata)
{

	mLock->lock();
	if (mNumRecords > 0)
	{
		if(mIsDistCount)
		{
			AosBuffPtr buf = mergeRecord(mBuffs,
					mMeasureSize,
					mStatMdfInfo.mRecordType);
			bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, buf);
			if(!rslt)
			{
				mLock->unlock();
				return AosDataProcStatus::eError;
			}
		}
		else
		{
			if(mStatMdfInfo.mRecordType == AosDataRecordType::eBuff)
			{
				bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, mRcdBuff2);
				if(!rslt)
				{
					mLock->unlock();
					return AosDataProcStatus::eError;
				}
			}
			else
			{
				bool rslt = sendSaveStatDocTrans(rdata.getPtr(), mCubeId, mRcdBuff);
				if(!rslt)
				{
					mLock->unlock();
					return AosDataProcStatus::eError;
				}
			}
		}
		mMdfInit=false;
		mStatMdfInfo.clear();
	}
	mLock->unlock();
	return true;
}


/////////////////////////////////////////////////////
// trans helper
////////////////////////////////////////////////////
bool
AosDataProcStatDoc::sendSaveStatDocTrans(
		AosRundata *rdata_raw,
		const u32 cube_id,
		const AosBuffPtr &input_data)
{
	//input_data->dumpData(true,"statdoc for distcount : ");
	map<u32, AosXmlTagPtr>::iterator itr = mStatCubeConfMap.find(cube_id);
	aos_assert_r(itr != mStatCubeConfMap.end(), false);
	AosXmlTagPtr cube_conf = itr->second;
	aos_assert_r(cube_conf, false);

	bool svr_death;
	AosBuffPtr resp;
	AosTransPtr trans = OmnNew AosBatchSaveStatDocsTrans(
			cube_id, cube_conf, input_data, mStatMdfInfo);


	AosSendTrans(rdata_raw, trans, resp, svr_death);


	if(svr_death)
	{
		OmnScreen << "batch save stat docs error ! svr death!" << svr_death;
		return false;
	}
	aos_assert_r(resp, 0);
	bool rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	mNumRecords=0;
	mRcdBuff->reset();
	mRcdBuff->setDataLen(0);//yang,fix bug
	mBuffs.clear();

	mRcdBuff2->reset();
	mRcdBuff2->setDataLen(0);

	return true;
}



//
// initialize statistics modify info for statistics data update
//
bool
AosDataProcStatDoc::configStatMdfInfo(AosRundata *rdata_raw,
									  AosDataRecordObj *input_record)
{
	mStatMdfInfo.mRecordLen = input_record->getRecordLen();
	mStatMdfInfo.mRecordType = input_record->getType();
	mStatMdfInfo.mOpr = mOpr;

	u32 measure_idx = 0;
	if(mStatMdfInfo.mRecordType == AosDataRecordType::eBuff)
	{
		int dfcnt = input_record->getNumFields();
		for(int i = 0; i < dfcnt; i++)
		{
			AosDataFieldObj* df = input_record->getFieldByIdx1(i);
			aos_assert_r(df,0);
			OmnString dfname = df->getName();
			//AosStrValueInfo dfinfo = df->getFieldInfo();

			if(dfname!="sdocid" && dfname!="isnew" && dfname!="statkey" && dfname!="time")
			{
				OmnString measure_name = df->getName();
				aos_assert_r(measure_name != "", NULL);
				measure_name.escape();
				//if (measure_name.hasPrefix("count0x28"))
				//	measure_name = "count0x280x2a0x29";
				if(measure_name.hasPrefix("dist_count"))
				{
					mStatMdfInfo.mMeasureType = AosStatModifyInfo::DIST_COUNT;
					mIsDistCount = true;
				}
				else
				{
					mStatMdfInfo.mMeasureType = AosStatModifyInfo::NORMAL;
					 mIsDistCount = false;
				}

				bool rslt;
				//int time_value_pos;
				int vt2d_idx;
				AosVt2dInfo vt2d_info;
				AosStatTimeUnit::E grpby_time_unit;

				if (mTimeUnit != "")
				{
					grpby_time_unit = AosStatTimeUnit::getTimeUnit(mTimeUnit);
				}
				else
				{
					grpby_time_unit = AosStatTimeUnit::eInvalid;
				}

				if(grpby_time_unit != AosStatTimeUnit::eInvalid)
					vt2d_idx = mStatistic->getVt2dIdxByMeasureIdx(measure_idx, grpby_time_unit);
				else
					vt2d_idx = mStatistic->getVt2dIdxByMeasureIdx(measure_idx);

				aos_assert_r(vt2d_idx != -1, NULL);

				rslt = mStatistic->getVt2dInfo(vt2d_idx, vt2d_info);
				aos_assert_r(rslt, NULL);

				mStatMdfInfo.addVt2dMdfInfo(rdata_raw,vt2d_idx,vt2d_info);
				mStatMdfInfo.addBuff2Measure(rdata_raw,measure_idx,vt2d_idx,vt2d_info);

				measure_idx++;
				//break;
			}
		}
		mMdfInit = true;
	}
	else
	{
		int dfcnt = input_record->getNumFields();
		for(int i = 0; i < dfcnt; i++)
		{
			AosDataFieldObj* df = input_record->getFieldByIdx1(i);
			aos_assert_r(df,0);
			OmnString dfname = df->getName();
			AosStrValueInfo dfinfo = df->mFieldInfo;

			if(dfname == "sdocid")
			{
				mStatMdfInfo.mSdocidPos = dfinfo.field_offset;
			}
			else if(dfname=="isnew")
			{
				mStatMdfInfo.mIsNewFlagPos = dfinfo.field_offset;
			}
			else if(dfname=="statkey")
			{
				mStatMdfInfo.mKeyPos = dfinfo.field_offset;
			}
			else if(dfname == "time")
			{
				//mStatMdfInfo.mVt2dMdfInfos[0].mTimeFieldPos
				mTimeValuePos = dfinfo.field_offset;
			}
			else
			{

				OmnString measure_name = df->getName();
				aos_assert_r(measure_name != "", NULL);
				measure_name.escape();
				//if (measure_name.hasPrefix("count0x28"))
				//	measure_name = "count0x280x2a0x29";

				bool rslt;
				//int time_value_pos;
				int vt2d_idx;
				AosVt2dInfo vt2d_info;
				AosStatTimeUnit::E grpby_time_unit;

				if (mTimeUnit != "")
				{
					grpby_time_unit = AosStatTimeUnit::getTimeUnit(mTimeUnit);
				}
				else
				{
					grpby_time_unit = AosStatTimeUnit::eInvalid;
				}

				if(grpby_time_unit != AosStatTimeUnit::eInvalid)
					vt2d_idx = mStatistic->getVt2dIdxByMeasureIdx(measure_idx, grpby_time_unit);
				else
					vt2d_idx = mStatistic->getVt2dIdxByMeasureIdx(measure_idx);

				aos_assert_r(vt2d_idx != -1, NULL);

				rslt = mStatistic->getVt2dInfo(vt2d_idx, vt2d_info);
				aos_assert_r(rslt, NULL);

				mStatMdfInfo.addMeasure(rdata_raw, measure_idx, dfinfo.field_offset,
						dfinfo.field_len, mTimeValuePos, vt2d_idx, vt2d_info);

				measure_idx++;
			}
		}
		mMdfInit = true;
	}

	return true;
}


AosBuffPtr
AosDataProcStatDoc::collectInputData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records)
{
	mTotalCounter++;
	bool rslt;
	AosValueRslt input_rslt;
	AosDataRecordObj *input_record = input_records[0];

	aos_assert_r(input_record, 0);
	DumpRecord(input_record, "StatDoc input is: ");

	//mCubeId = getCubeIdFromInputData(input_record->getData(rdata_raw));
	if(input_record->getType() == AosDataRecordType::eBuff)
	{
		// init modify info only once
		if(!mMdfInit)
		{
			u64 tStart = OmnGetTimestamp();
			rslt = configStatMdfInfo(rdata_raw, input_record);
			mConfigStatMdfTime += OmnGetTimestamp()-tStart;
			mConfigStatMdfCounter++;
			aos_assert_r(rslt, NULL);
		}
		int rcdbuflen = input_record->getRecordLen();
		AosBuffPtr rcdbuf =  OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		rcdbuf->setBuff(input_record->getData(rdata_raw),rcdbuflen);
		rcdbuf->reset();	
		if(mIsDistCount)
		{
			mBuffs.push_back(rcdbuf);
			mNumRecords++;
			return mRcdBuff2; 
		}
		AosBuffPtr encodedRcdBuf = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		
		//encode the record
		//record format
		// recordlen + f1len + f1 + f2len + f2 + ... +fnlen + fn
		char* data = input_record->getData(rdata_raw);
		int record_len = 0;
		int dfcnt = input_record->getNumFields();
		int crt_pos = 0, str_len = 0;
		for(int i = 0; i < dfcnt; i++)
		{
			AosDataFieldObj* df = input_record->getFieldByIdx1(i);

			aos_assert_r(df,0);
			OmnString dfname = df->getName();
			//AosStrValueInfo dfinfo = df->getFieldInfo();

			AosDataFieldType::E dftype = df->getType();
			switch(dftype)
			{
				case AosDataFieldType::eStr:
					crt_pos += 1;
					rcdbuf->setCrtIdx(crt_pos);
					str_len = rcdbuf->getInt(-1);
					encodedRcdBuf->setU64(str_len);	// set field len
					crt_pos += sizeof(int);
					encodedRcdBuf->setBuff(data+crt_pos,str_len);	// set field content
					crt_pos += str_len;
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += str_len; // add field
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+str_len);
					break;
				case AosDataFieldType::eBinU64:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(u64));
					encodedRcdBuf->setU64(*(u64*)(data+crt_pos));
					if(dfname == "sdocid")
						aos_assert_r(*(u64*)(data+crt_pos) != 0,0)
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(u64); // add field
					crt_pos += sizeof(u64);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(u64) + 1);
					break;
				case AosDataFieldType::eBinInt64:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(u64));
					encodedRcdBuf->setI64(*(i64*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(i64); // add field
					crt_pos += sizeof(int64_t);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(int64_t) + 1);
					break;
				case AosDataFieldType::eBinU32:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(u32));
					encodedRcdBuf->setU32(*(u32*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(u32); // add field
					crt_pos += sizeof(u32);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(u32) + 1);
					break;
				case AosDataFieldType::eBinInt:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(int));
					encodedRcdBuf->setInt(*(int*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(int); // add field
					crt_pos += sizeof(int);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(int) + 1);
					break;
				case AosDataFieldType::eDouble:
				case AosDataFieldType::eBinDouble:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(double));
					encodedRcdBuf->setDouble(*(double*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(double); // add field
					crt_pos += sizeof(double);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(double) + 1);
					break;
				case AosDataFieldType::eBinChar:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(char));
					encodedRcdBuf->setChar(*(char*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(char); // add field
					crt_pos += sizeof(char);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(char) + 1);
					break;
				case AosDataFieldType::eBinBool:
					crt_pos += 1;
					encodedRcdBuf->setU64(sizeof(bool));
					encodedRcdBuf->setBool(*(bool*)(data+crt_pos));
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += sizeof(bool); // add field
					crt_pos += sizeof(bool);
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+sizeof(bool) + 1);
					break;
				case AosDataFieldType::eBuff:
					crt_pos += 1;
					str_len = rcdbuf->getInt(-1);
					encodedRcdBuf->setU64(str_len);
					crt_pos += sizeof(int);
					encodedRcdBuf->setBuff(data+crt_pos,str_len);
					crt_pos += str_len;
					record_len += sizeof(u64);	// add 8 byte field len
					record_len += str_len; // add field
					rcdbuf->setCrtIdx(rcdbuf->getCrtIdx()+str_len + 1);
					break;
				default:
					break;
			}
		}
		encodedRcdBuf->setU64(0);//reserve 8 bytes
		char* bufdata = encodedRcdBuf->data();
		int datalen = encodedRcdBuf->dataLen();
		memmove(bufdata+8,bufdata,datalen - sizeof(u64));
		*(u64*)bufdata = record_len + sizeof(u64);//set record total len
		mRcdBuff2->setBuff(encodedRcdBuf);
		mNumRecords++;
		return mRcdBuff2;
	}
	else
	{
		int len = input_record->getRecordLen();
		aos_assert_r(len, 0);

		// init modify info only once
		if(!mMdfInit)
		{
			rslt = configStatMdfInfo(rdata_raw, input_record);
			aos_assert_r(rslt, NULL);
		}
		char *data = input_record->getData(rdata_raw);
		//mRcdBuff->addBuff(data, len);

		mRcdBuff->setBuff(data, len);

		//AosBuffPtr buf;
		//buf = OmnNew AosBuff(data,len,len);
		//AosBuffPtr tmpbuf = buf->clone();
		//mBuffs.push_back(tmpbuf);

		mNumRecords++;

		return mRcdBuff;
	}
}

///////////////////////////
// proc data
//////////////////////////
AosDataProcStatus::E
AosDataProcStatDoc::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// The record format is:
	// 	[local_sdocid, key, et, measure, measure, ...]
	// If it is distinct count, there shall be only one measure.
	u64 tStart,tMiddle,tEnd;
	tStart = OmnGetTimestamp();
	
	mLock->lock();

	tMiddle = OmnGetTimestamp();
	AosBuffPtr input_data = collectInputData(rdata_raw, input_records);
	aos_assert_r(input_data, AosDataProcStatus::eError);

	tEnd = OmnGetTimestamp();
	mCollectInputDataTime += tEnd-tMiddle;
	
	if(mIsStreaming)
	{
		//char * input_d = input_data->data();
		//mCubeId = getCubeIdFromInputData(input_d);
		mCubeId = 0;
		aos_assert_r(mCubeId >=0, AosDataProcStatus::eError);
	}
	if (mNumRecords >= mCachedRecords)
	{
		//mCubeId = getCubeIdFromInputData(input_data);
		aos_assert_r(mCubeId >=0, AosDataProcStatus::eError);

		if(mIsDistCount)
		{
			tMiddle = OmnGetTimestamp();
			AosBuffPtr buf = mergeRecord(mBuffs,
					mMeasureSize,
					mStatMdfInfo.mRecordType);
			
			tEnd = OmnGetTimestamp();
			mMergeBuffTime += tEnd-tMiddle;

			tMiddle = OmnGetTimestamp();
			bool rslt = sendSaveStatDocTrans(rdata_raw, mCubeId, buf);
			
			tEnd = OmnGetTimestamp();
			mSendDocTime += tEnd - tMiddle;
			mSendDocCounter++;
		
			if(!rslt)
			{
				mLock->unlock();
				return AosDataProcStatus::eError;
			}
		}
		else
		{
			tMiddle = OmnGetTimestamp();
			bool rslt = sendSaveStatDocTrans(rdata_raw, mCubeId, input_data);
			
			tEnd = OmnGetTimestamp();
			mSendDocTime += tEnd - tMiddle;
			mSendDocCounter++;
			if(!rslt)
			{
				mLock->unlock();
				return AosDataProcStatus::eError;
			}
		}
	}

	mLock->unlock();
	tEnd = OmnGetTimestamp();
	mTotalTime += tEnd-tStart;
	return AosDataProcStatus::eContinue;
}


bool
AosDataProcStatDoc::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="dataprocstatjoin_jimodoc_v0">
	//    <![CDATA[{key_fields:["k1", "k2", "k3"], time_field:"xxx", measure_fields:["m1", "m2"], cube_id:"xxx"}]]>
	//  </dataproc>
	//</jimodataproc >
	//
	try
	{
		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
		JSONReader reader;
		JSONValue jsonObj;
		reader.parse(jsonstr, jsonObj);
		mJson = jsonObj;

		OmnString dp_str = "";
		dp_str
			<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<< 		AOSTAG_PUBLIC_DOC << "=\"true\" " 
			<< 		AOSTAG_OBJID << "=\"" << objid << "\">"
			<< 		"<dataproc zky_name=\"" << dpname << "\" jimo_objid=\"dataprocstatdoc_jimodoc_v0\">"
			<< 			"<![CDATA[" << jsonstr << "]]>"
			<< 		"</dataproc>"
			<< "</jimodataproc>";

		//return AosCreateDoc(dp_str, true, rdata);
		prog->saveLogicDoc(rdata, objid, dp_str);
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException... " << endl;
	}

	return false;
}

int
//AosDataProcStatDoc::getCubeIdFromInputData(const AosBuffPtr &input_data)
AosDataProcStatDoc::getCubeIdFromInputData(char *input_data)
{
	u64 first_stat_docid = *(u64 *)(input_data + 1);
	aos_assert_r(first_stat_docid >0, false);
	return (first_stat_docid >> 32);
}



///////////////////////////////////////////////////////////////////////////////
// 								dist count helper
///////////////////////////////////////////////////////////////////////////////
//  the offset is based on the AosDataProcStatJoin::fillRecordVt2d ouput
//  the formar : 
//  	isNew | Docid | statKeyLen | statKey | timeUnit | mea1 | ..|mea3
// 	every field has one byte flag ,'0' means valid ,non-'0' means invalid
// 	isNew is 1 byte 
//  Docid is 8 byte
//	statKeyLen is 4 byte
//	timeUnit is 8 byte
//	every measure is 8 byte
/////////////////////////////////////////////////////////////////////////////
bool
AosDataProcStatDoc::rebuildBuff(
					const vector<AosBuffPtr>& input_record,
					vector<AosBuffPtr>& newArray,
					const u32 mea_num,
					const AosDataRecordType::E &recordType)
{
	int size = input_record.size();
	u64 timeValueLen = sizeof(u64)*(mea_num+1)*2;
	u64 newLen = sizeof(u64)*2 + sizeof(u8) + 1 + timeValueLen;
	for(int i = 0;i < size;i++)
	{
		vector<u64> value;
		char *data = input_record[i]->data();
		int dataLen = input_record[i]->dataLen();
		u8 isNew = *(u8*)&data[ISNEW_POS];
		u64 docid = *(u64*)&data[DOCID_POS];
		aos_assert_r(docid >0, false);
		u64 keyLen = *(u32*)&data[STATKEYLEN_POS];
		OmnString keyStr((char*)&data[STATKEY_POS],keyLen);
		
		u64 time  = *(u64*)&data[STATKEY_POS+keyLen+1];
		for(size_t i = mea_num; i > 0; i--)
		{
			value.push_back(*(u64*)&data[dataLen-i*9 +1]);
		}
		AosBuffPtr newBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		newBuff->appendU64(keyStr.length()+newLen);
		newBuff->appendU8(isNew);
		newBuff->appendU64(docid);
		newBuff->addBuff(keyStr.data(),keyStr.length());
		newBuff->setCrtIdx(newBuff->dataLen());
		newBuff->setChar('\0');
		newBuff->appendU64(timeValueLen);
		newBuff->appendU64(time);
		newBuff->appendU64(sizeof(u64)*(mea_num+1));
		for(size_t i = 0; i < mea_num;i++ )
		{
			newBuff->appendU64(value[i]);
		}
		newArray.push_back(newBuff);
//		OmnScreen << "statdoc for distcount IsNew : " << isNew  <<" docid : " << docid << " StatKey : "<< keyStr << 
//			" time : " << time << " measure : " << value[0] << endl;
	}

	return true;
}

//2015.1.29 arvin  merge the same record
AosBuffPtr
AosDataProcStatDoc::mergeRecord(
		const vector<AosBuffPtr>& recordArray,
		const u32 mea_num,
		const AosDataRecordType::E &recordType)
{
	u64 tStart,tMiddle,tEnd;
	tStart = OmnGetTimestamp();
	vector<AosBuffPtr> buffArray;
	
	rebuildBuff(recordArray,buffArray,mea_num,recordType);
	tEnd = OmnGetTimestamp();
	
	mRebuildBuffTime += tEnd - tStart;
	////////////////////////////////////////////////////////////////////////////////////////
	//  after rebuild ,every buff format:
	//  totalLen(u64) |isNew(u8) |Docid(u64) |statKey |timeMeasureLen(u64) |timeUnit(u64) | 
	//  		measureLen(u64) | measure(u64) | measureLen(u64) | measure(u64).... 
	//  every len contain self 
	//  statKey is variable-length and has '\000' in the end;
	//////////////////////////////////////////////////////////////////////////////////////
	i64 docidPos = 9;
	i64 keyPos = 17;
	
	map<pair<u64,u64>,AosBuffPtr> mapDocidTimeRecord;
	map<pair<u64,u64>,AosBuffPtr>::iterator it;
	
	map<u64,AosBuffPtr> mapDocidRecord;
	map<u64,AosBuffPtr>::iterator itRcd;
	AosBuffPtr newBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	tMiddle = OmnGetTimestamp();
	//merge the same sdocid and time
	// len0(u64)|isNew(u8)| id(u64) |k1+..Kn | len2(u64) | t1(u64) | V1..Vn
	for(size_t i = 0;i < buffArray.size() ;i++)
	{
		AosBuffPtr& rebuildBuff = buffArray[i];
		rebuildBuff->reset();
		rebuildBuff->setCrtIdx(docidPos);
		u64 sDocId = rebuildBuff->getU64(0);

		aos_assert_r(sDocId >0, NULL);

		rebuildBuff->setCrtIdx(rebuildBuff->dataLen()-(16 * mea_num)-8);
		u64 time = rebuildBuff->getU64(0);
		it = mapDocidTimeRecord.find(pair<u64,u64>(sDocId,time));
		if( mapDocidTimeRecord.end() == it )
		{
			mapDocidTimeRecord[pair<u64,u64>(sDocId,time)] = rebuildBuff;
		}
		else
		{
			AosBuffPtr& inMapBuff = it->second;
			u64 increaseSize = (mea_num+1)*sizeof(u64);
			char *data = inMapBuff->data();
			rebuildBuff->setCrtIdx(8);
			u8 isNew = rebuildBuff->getU8(0);
			rebuildBuff->setCrtIdx(rebuildBuff->dataLen()-mea_num*8);
			inMapBuff->reset();
			u64 totalLen = inMapBuff->getU64(0);
			inMapBuff->reset();
			inMapBuff->setU64(totalLen + mea_num*16);
			if(isNew)
				inMapBuff->setU8(isNew);
			int len = strlen(&data[keyPos]);
			inMapBuff->setCrtIdx(keyPos+len+1);
			int timeValueLenPos = inMapBuff->getCrtIdx();
			u64 timeValueLen = inMapBuff->getU64(0);
			inMapBuff->setCrtIdx(timeValueLenPos);
			inMapBuff->setU64(timeValueLen + (mea_num+1)*sizeof(u64));
			inMapBuff->setCrtIdx(inMapBuff->dataLen());
			
			inMapBuff->setU64((mea_num+1)*sizeof(u64));
			for(size_t j = 0 ; j < mea_num ;j++)
			{
				inMapBuff->setU64(rebuildBuff->getU64(0));
			}
		}
	}
 	tEnd = OmnGetTimestamp();
	mFirstMergeTime += tEnd - tMiddle;
	
	tMiddle = OmnGetTimestamp();
	//merge the same docid record
	it = mapDocidTimeRecord.begin();
	itRcd = mapDocidRecord.begin();
	mapDocidRecord[it->first.first] = it->second;
	++it;
	for(; it != mapDocidTimeRecord.end();it++)
	{
		u64 docid = it->first.first;
		itRcd = mapDocidRecord.find(docid);
		if(itRcd !=  mapDocidRecord.end() )
		{
			AosBuffPtr &docidTimeBuff = it->second;
			docidTimeBuff->reset();
			docidTimeBuff->setCrtIdx(keyPos);

			char *data = docidTimeBuff->data();
			int len = strlen(&data[docidTimeBuff->getCrtIdx()]);
			docidTimeBuff->setCrtIdx(it->second->getCrtIdx() + len +1);
			u64 timeValueLen = docidTimeBuff->getU64(0);
			docidTimeBuff->setCrtIdx(docidTimeBuff->getCrtIdx()-8);

			AosBuffPtr &docidBuff = itRcd->second;
			docidBuff->reset();
			u64 totalLen = docidBuff->getU64(0)+timeValueLen;
			docidBuff->reset();
			docidBuff->setU64(totalLen);
			for(size_t i = 0;i < timeValueLen/8;i++)
			{
				docidBuff->appendU64(docidTimeBuff->getU64(0));
			}
		}
		else
		{
			mapDocidRecord[it->first.first] = it->second;
		}
	}
 	
	tEnd = OmnGetTimestamp();
	mSecondMergeTime += tEnd - tMiddle;
	//merge all the records into a AosBuff
	for(itRcd = mapDocidRecord.begin();itRcd != mapDocidRecord.end(); itRcd++)
	{
		newBuff->addBuff(itRcd->second->data(),itRcd->second->dataLen());
	}
	return newBuff;
}

