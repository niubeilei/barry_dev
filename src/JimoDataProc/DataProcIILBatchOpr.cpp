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
// 2014/12/24 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcIILBatchOpr.h"

#include "API/AosApi.h"
#include "DataProc/DataProc.h"
#include "SEUtil/IILName.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "SEInterfaces/TaskDataObj.h"
#include "TaskMgr/Task.h"
#include "JSON/JSON.h"
#include "SEModules/SnapshotIdMgr.h"

#include <boost/make_shared.hpp>

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcIILBatchOpr_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcIILBatchOpr(version);
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

#define U64MAX_VALUE	((u64)0xffffffffffffffff)
#define I64MAX_VALUE	((i64)0x7fffffffffffffff)
#define D64MAX_VALUE	((d64)0x7fffffffffffffff)


AosDataProcIILBatchOpr::AosDataProcIILBatchOpr(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcIILBatchOpr),
mRecordType(AosDataRecordType::eInvalid),
mKeyStr(0),
mKeyStrLen(-1)
{
}


AosDataProcIILBatchOpr::AosDataProcIILBatchOpr(const AosDataProcIILBatchOpr &proc)
:
//AosJimoDataProc(proc)
//AosStreamDataProc(1, AosJimoType::eDataProcIILBatchOpr)
AosStreamDataProc(proc),
mKeyStr(0)
{
	mIILName = proc.mIILName;
	mShuffleType = proc.mShuffleType;
	mOpr = proc.mOpr;
	mIILType = proc.mIILType;
	mIncType = proc.mIncType;
	mInitdocid = proc.mInitdocid;
	mBuildBitmap = proc.mBuildBitmap;
	mSnapId = proc.mSnapId;
	mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtr();
	mRecordLen = proc.mRecordLen;
	mRecordType = proc.mRecordType;
	mMaxKeyLen = proc.mMaxKeyLen;
	mTaskDocid = proc.mTaskDocid;
	mKeyStr = OmnNew char [eMaxKeySize];
	mKeyStrLen = eMaxKeySize;
}


AosDataProcIILBatchOpr::~AosDataProcIILBatchOpr()
{
	OmnDelete [] mKeyStr;
	mKeyStr = 0;
}


bool
AosDataProcIILBatchOpr::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name="dp2" jimo_objid="dataprociilbatchopr_jimodoc_v0">
	//<![CDATA[
	//{
	//	"iilname": "_zt44_myds_f2",
	//	"opr": "stradd"
	//	inc_type: "xxxx"
	//	init_docid: "xxxx"
	//	build_bitmap: "xxxx"
	//}
	//]]></dataproc>
	OmnString dp_json = def->getNodeText();
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);
	mJson = json;

	try
	{
		aos_assert_r(def, false);

		if(!mIsStreaming)
		{
			mName = def->getAttrStr(AOSTAG_NAME, "");
			aos_assert_r(mName != "", false);
		}
	
		OmnScreen << "dataproc config json is: " << dp_json << endl;

		mIILName = json["iilname"].asString();
		aos_assert_r(mIILName != "", false);

		mShuffleType = json["shuffle_type"].asString();
		OmnString str = json["opr"].asString();
		mOpr = AosIILOpr::toEnum(str);
		aos_assert_r(AosIILOpr::isValid(mOpr), false);

		str = json["iil_type"].asString("BigStr");
		mIILType = AosIILType_toCode(str);
		aos_assert_r(AosIsValidIILType(mIILType), false);

		if (mOpr == AosIILOpr::eInc && (mIILType == eAosIILType_Str || eAosIILType_BigStr))
		{
			OmnString inctype = json["inc_type"].asString();
			mIncType = AosIILUtil::IILIncType_toEnum(inctype);
			mInitdocid = json["init_docid"].asInt(0);

		}
		if(mIsStreaming)
		{	
			mLenType = json["len_type"].asString();
			if (mLenType == "")
			{
				mLenType = "fixed";
			}
			else
			{
				aos_assert_r(mLenType == "var", false);
				int max_len = json["key_max_len"].asInt();
				if (max_len <= 0)
				{
					mRecordLen = eMaxKeySize + sizeof(u64);
				}
				else
				{
					mRecordLen = max_len + sizeof(u64);
				}
			}
		}

		mMaxKeyLen = json["key_max_len"].asInt();
		mBuildBitmap = json["build_bitmap"].asBool();
		mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
		mBuffRaw = mBuff.getPtr();
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}

}

bool
AosDataProcIILBatchOpr::flush(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_r(buff->dataLen() % mRecordLen == 0, false);
	u64 docid;
	docid = mTaskDocid;
	if(mIsStreaming)
	{
		docid = getServiceDocid();
	}
	aos_assert_r(docid > 0, false);
//OmnScreen << "====andy=== DataProcIILBatchOpr IILName is : " << mIILName << endl;
//OmnScreen << "====levi=== DataProcIILBatchOpr buff is : " << buff->data() << endl;

	bool rslt = false;
	u64 executor_id = 0;

	OmnString iilname = mIILName;
	if(mIsStreaming)
	{
		/*
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		OmnString value = task->getTaskENV(AOSTAG_SHUFFLE_ID, rdata);
		int shuffle_id = value.toInt(); 
		if (shuffle_id >= 0)
		{
			const char *pos = strstr((const char*)iilname.data(), "__$group"); 
			if (pos)
			{
				iilname = OmnString(iilname.data(), pos-iilname.data()); 
				iilname = AosIILName::composeCubeIILName(shuffle_id, iilname);
			}
		}
		*/

	}
	else
	{
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		OmnString value = task->getTaskENV(AOSTAG_SHUFFLE_ID, rdata);
		int shuffle_id = value.toInt(); 
		if (shuffle_id >= 0)
		{
			const char *pos = strstr((const char*)iilname.data(), "__$group"); 
			if (pos)
			{
				iilname = OmnString(iilname.data(), pos-iilname.data()); 
				iilname = AosIILName::composeCubeIILName(shuffle_id, iilname);
			}
		}
	}

	if (mOpr == AosIILOpr::eInc)
	{
		if (mIILType == eAosIILType_Str || mIILType == eAosIILType_BigStr)
		{
			rslt = AosStrBatchInc(iilname, mRecordLen, 
					buff, mInitdocid, mIncType, mSnapId, docid, rdata);
		}
	}
	else if (mOpr == AosIILOpr::eAdd)
	{
		//Jozhi for streaming to rebuild bitmap
		if (mIsStreaming)
		{
			executor_id = eStreamingExecutorId;
		}
		//print by levi 2015/11/12
		//u64 tStart = OmnGetTimestamp();
		rslt = AosBatchAdd(iilname, mIILType, mRecordLen, 
				buff, executor_id, mSnapId, docid, rdata);
		//u64 tEnd = OmnGetTimestamp();
		//OmnScreen << "IILBatchOpr batchadd take : " << (tEnd-tStart) << ";  IILBatchOprName : " << mName << endl;
	}
	else if (mOpr == AosIILOpr::eDel)
	{
		rslt = AosBatchDel(iilname, mIILType, mRecordLen, 
				buff, executor_id, mSnapId, docid, rdata);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataProcIILBatchOpr::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	return true;
}


AosDataProcStatus::E
AosDataProcIILBatchOpr::procData(
		AosRundata* rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;
	if (mProcDataCount % eShowProcNum == 0)
	{
		updateCounter(mName, eShowProcNum);
	}
	bool rslt = false;
	AosDataRecordObj * input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);

	if (mRecordType == AosDataRecordType::eInvalid)
	{
		mRecordType = input_record->getType();
		vector<AosDataFieldObjPtr> fields = input_record->getFields();
		aos_assert_r(fields.size() == 2, AosDataProcStatus::eError);
		mKeyType = fields[0]->getType();
		mValueType = fields[1]->getType();
		if (mKeyType == AosDataFieldType::eStr)
		{
			if (mMaxKeyLen <= 0)
			{
				mRecordLen = eMaxKeySize + sizeof(u64);
			}
			else
			{
				mRecordLen = mMaxKeyLen + sizeof(u64);
			}
		}
		else
		{
			mRecordLen = sizeof(u64)*2;
		}
	}

	if (mRecordType == AosDataRecordType::eBuff)
	{
		AosValueRslt key_rslt, value_rslt;
		rslt = input_record->getFieldValue(0, key_rslt, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		rslt = input_record->getFieldValue(1, value_rslt, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		int rcd_len = input_record->getRecordLen();

		switch (mKeyType)
		{
		case AosDataFieldType::eStr:
			{
				if (rcd_len > mRecordLen)
				{
					AosBuffPtr buff = mBuff;
					mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
					mBuffRaw = mBuff.getPtr();
					rslt = flush(buff, rdata);
					aos_assert_r(rslt, AosDataProcStatus::eError);
					while(rcd_len > mRecordLen)
					{
						mRecordLen += eMaxKeySize;
					}
				}
				int offset = mBuffRaw->getCrtIdx();
				if (offset + mRecordLen > mBuffRaw->buffLen())
				{
					if (mIsStreaming && mBuffRaw->dataLen() == 0)
					{
						mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
						mBuffRaw = mBuff.getPtr();
					}
					else
					{
						AosBuffPtr buff = mBuff;
						mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
						mBuffRaw = mBuff.getPtr();
						rslt = flush(buff, rdata);
						aos_assert_r(rslt, AosDataProcStatus::eError);
					}
					offset = mBuffRaw->getCrtIdx();
				}
				if (key_rslt.isNull())
				{
					mBuffRaw->setChar('\b');
					mBuffRaw->setChar('\0');
				}
				else
				{
					int key_len = mRecordLen - sizeof(u64);
					if (mKeyStrLen < key_len)
					{
						if(mKeyStr)
							OmnDelete [] mKeyStr;
						mKeyStr = OmnNew char[key_len];
						mKeyStrLen = key_len;
					}
					int len = 0;
					key_rslt.getCStr(mKeyStr, key_len, len);
					aos_assert_r(len+1 <= key_len , AosDataProcStatus::eError);
					if (len == 0)
						return AosDataProcStatus::eContinue;
					mBuffRaw->setBuff(mKeyStr, len+1);
				}
				mBuffRaw->setCrtIdx(mRecordLen - sizeof(u64) + offset);
				break;
			}
		case AosDataFieldType::eBinU64:
			{
				aos_assert_r(rcd_len - 2 == mRecordLen, AosDataProcStatus::eError); 
				if (key_rslt.isNull())
					return AosDataProcStatus::eContinue;

				mBuffRaw->setU64(key_rslt.getU64());
				break;
			}
		case AosDataFieldType::eBinDouble:
			{
				aos_assert_r(rcd_len - 2 == mRecordLen, AosDataProcStatus::eError); 
				if (key_rslt.isNull())
					return AosDataProcStatus::eContinue;

				double dd = key_rslt.getDouble();
				u64 uu = *(u64*)&dd;
				mBuffRaw->setU64(uu);
				break;
			}
		case AosDataFieldType::eBinInt64:
		case AosDataFieldType::eBinDateTime:
			{
				aos_assert_r(rcd_len - 2 == mRecordLen, AosDataProcStatus::eError); 
				if (key_rslt.isNull())
					return AosDataProcStatus::eContinue;

				i64 ii = key_rslt.getI64();
				u64 uu = *(u64*)&ii;
				mBuffRaw->setU64(uu);
				break;
			}
		default:
			OmnNotImplementedYet;
			return AosDataProcStatus::eError;
		}
		switch(mValueType)
		{
		case AosDataFieldType::eBinU64:
			mBuffRaw->setU64(value_rslt.getU64());
			break;
		case AosDataFieldType::eBinDouble:
			{
				double dd = value_rslt.getDouble();
				u64 uu = *(u64*)&dd;
				mBuffRaw->setU64(uu);
				break;
			}
		case AosDataFieldType::eBinInt64:
		case AosDataFieldType::eBinDateTime:
			{
				i64 ii = value_rslt.getI64();
				u64 uu = *(u64*)&ii;
				mBuffRaw->setU64(uu);
				break;
			}
		default:
		OmnNotImplementedYet;
		return AosDataProcStatus::eError;
		}

	}
	//else if (mRecordType ==  AosDataRecordType::eFixedBinary)
	//{
	//	char * data = input_record->getData(rdata);
	//	int rcd_len = input_record->getRecordLen();
	//	mBuffRaw->setBuff(data, rcd_len);
	//}
	else
	{
		OmnAlarm << "not handle this record type: " << AosDataRecordType::toStr(mRecordType) << enderr;
		OmnNotImplementedYet;
	}

	if ( !checkBuff(rdata, mBuff) )
	{
		aos_assert_r(mBuff->dataLen() > 0, AosDataProcStatus::eError);
		int64_t start_pos = mBuff->dataLen() - mRecordLen;

		// 1. get current record data
		AosBuffPtr buff = OmnNew AosBuff(mRecordLen+10 AosMemoryCheckerArgs);
		aos_assert_r(buff, AosDataProcStatus::eError);
		buff->setBuff(mBuff->data()+start_pos, mRecordLen);

		// 2. reset mBuff length 
		mBuff->setDataLen(start_pos);

		// 3. flush buff
		rslt = flush(mBuff, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		// 4. replace mBuff
		mBuff = buff;	
		mBuffRaw = mBuff.getPtr();
	}

	if (mBuffRaw->dataLen() > eMaxBuffSize)
	{
		AosBuffPtr buff = mBuffRaw;
		if(!mIsStreaming)
			mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		else
			mBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);

		mBuffRaw = mBuff.getPtr();
		rslt = flush(buff, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		return AosDataProcStatus::eContinue;
	}


	mOutputCount++;
	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcIILBatchOpr::cloneJimo() const
{
	return OmnNew AosDataProcIILBatchOpr(*this);
}

AosDataProcObjPtr 
AosDataProcIILBatchOpr::cloneProc()
{
	//return this;
	return OmnNew AosDataProcIILBatchOpr(*this);
}


bool
AosDataProcIILBatchOpr::createSnapshots(const AosRundataPtr &rdata)
{
	//mSnapId = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	mSnapId = 0;
	vector<AosTaskDataObjPtr> snapshots;
	u32 virtual_id = AosGetCubeId(mIILName);
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	AosTaskDataObjPtr snapshot;
	set<OmnString> iilnames;
	iilnames.insert(mIILName);
	snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtual_id, mSnapId, iilnames, mTaskDocid, false);
	aos_assert_r(snapshot, false);
	snapshots.push_back(snapshot);
	aos_assert_r(snapshots.size() == 1, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}

bool
AosDataProcIILBatchOpr::createSnapshotsStreaming(const AosRundataPtr &rdata)
{
	mSnapId = 0;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);

	u32 virtual_id = AosGetCubeId(mIILName);
	mSnapId = iil_client->createSnapshot(virtual_id, mSnapId, getServiceDocid(), rdata);
	return true;
}



bool
AosDataProcIILBatchOpr::start(const AosRundataPtr &rdata)
{	
	showDataProcInfo(__FILE__, __LINE__, "start", "DataProcIILBatchOpr");
	if(mIsStreaming)
	{
		return createSnapshotsStreaming(rdata);
	}
	else
	{
		return createSnapshots(rdata);
	}
	return true;
}


bool
AosDataProcIILBatchOpr::finish(const AosRundataPtr &rdata)
{
	if (mBuffRaw->dataLen() > 0)
	{
		AosBuffPtr buff = mBuffRaw;	
		mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

		mBuffRaw = mBuff.getPtr();
		bool rslt = flush(buff, rdata);
		return rslt;
	}
	return true;
}


bool
AosDataProcIILBatchOpr::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	i64 counter = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
		counter += procs[i]->getProcDataCount() % eShowProcNum;
	}
	OmnScreen << "DataProcIILBatchOpr" << "(" << mName << ")" << "finished:" 
		<< ", procDataCount:" << procDataCount
		<< ", OutputCount:" << procOutputCount << endl;

	updateCounter(mName, counter);
	for (u32 i=0; i<procs.size(); i++)
	{
		bool rslt = procs[i]->finish(rdata);
		aos_assert_r(rslt, false);
	}

	if(mBuildBitmap) AosIILRebuildBitmap(mIILName, rdata);

	showDataProcInfo(__FILE__, __LINE__, procDataCount, procOutputCount);
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcIILBatchOpr");
	return true;
}


/*
bool
AosDataProcIILBatchOpr::setShuffleId(const OmnString &key)
{
	if (mShuffleId > -1) return true;

	mShuffleType == "cube" 
		?  mShuffleId = AosGetCubeId(key)
		: mShuffleId = AosGetPhysicalId(key);
	return true;
}
*/

bool 
AosDataProcIILBatchOpr::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	try
	{
		string cdata = jsonstr;
		JSONValue jsonObj;
		JSONReader reader;
		bool rslt = reader.parse(jsonstr, jsonObj);
		aos_assert_r(rslt, false);

		mJson = jsonObj;
		string iilname = jsonObj["iilname"].asString();
		string opr = jsonObj["opr"].asString();

		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, obj_name);
		OmnString dpconf = "";	
		dpconf 
			<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
			<< 	AOSTAG_OBJID << "=\"" << objid << "\">"
			<< 		"<dataproc zky_name=\"" << obj_name << "\" jimo_objid=\"dataprociilbatchopr_jimodoc_v0\">"
			<< 			"<![CDATA[" << cdata << "]]>"
			<< 		"</dataproc>"
			<< "</jimodataproc>";

		//return AosCreateDoc(dpconf, true, rdata);
		prog->saveLogicDoc(rdata, objid, dpconf);
		return true;
	}
	catch (std::exception &e)
	{
		OmnScreen << "JSONException: " << e.what() << "\n" << jsonstr << endl;
		return false;
	}

	return false;
}


bool
AosDataProcIILBatchOpr::checkBuff(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	int64_t bufflen = buff->dataLen();
	if (bufflen < mRecordLen) 
	{
		OmnAlarm << "buff data error" << enderr;
		return false;
	}
	else if (bufflen == mRecordLen)
	{
		return true;
	}	
	aos_assert_r(bufflen >= (2 * mRecordLen), false);

	char *cur_pos = buff->data() + (bufflen - 2 * mRecordLen);
	if (mKeyType == AosDataFieldType::eStr && 
			(mOpr == AosIILOpr::eInc || mOpr == AosIILOpr::eAdd || mOpr == AosIILOpr::eDel))
	{
		int rslt1 = -1;
		int cmp_len = mRecordLen - sizeof(u64);
		if (*cur_pos == '\b')
		{
			if (*(cur_pos + mRecordLen) == '\b')
				rslt1 = 0;
			else
				rslt1 = -1;
		}
		else
		{
			if (*(cur_pos + mRecordLen) == '\b')
			{
				aos_assert_r(false, false);
			}
			else
			{
				rslt1 = strncmp(cur_pos, cur_pos + mRecordLen, cmp_len);
			}
		}
		aos_assert_r(rslt1 <= 0, false);
		if(rslt1 == 0)
		{
			// compare docid
			if (mValueType == AosDataFieldType::eBinU64)
			{
				aos_assert_r( (*((u64*)(cur_pos+cmp_len)))  <= (*((u64*)(cur_pos + mRecordLen + cmp_len))) , false);
			}
			else if (mValueType == AosDataFieldType::eBinDouble)
			{
				aos_assert_r( (*((double*)(cur_pos+cmp_len)))  <= (*((double*)(cur_pos + mRecordLen + cmp_len))) , false);
			}
			else if (mValueType == AosDataFieldType::eBinInt64 || mValueType == AosDataFieldType::eBinDateTime)
			{
				aos_assert_r( (*((int64_t*)(cur_pos+cmp_len)))  <= (*((int64_t*)(cur_pos + mRecordLen + cmp_len))) , false);
			}
			else
			{
				OmnAlarm << "value type error: " << enderr;
			}
		}
	}
	else
	{
		aos_assert_r(mRecordLen == (2 * sizeof(u64)), false);
		if (mKeyType == AosDataFieldType::eBinInt64 
				|| mKeyType == AosDataFieldType::eInt64 
				|| mKeyType == AosDataFieldType::eBinDateTime
				|| mKeyType == AosDataFieldType::eDateTime)
		{
			aos_assert_r( (*((int64_t*)cur_pos))  <= (*((int64_t*)(cur_pos + mRecordLen))) , false);
		}
		else if (mKeyType == AosDataFieldType::eBinU64 
				|| mKeyType == AosDataFieldType::eU64)
		{
			aos_assert_r( (*((u64*)cur_pos))  <= (*((u64*)(cur_pos + mRecordLen))) , false);
		}
		else if (mKeyType == AosDataFieldType::eBinDouble 
				|| mKeyType == AosDataFieldType::eDouble)
		{
			aos_assert_r( (*((double*)cur_pos))  <= (*((double*)(cur_pos + mRecordLen))) , false);
		}
		else
		{
			OmnAlarm << "daaaaa" << enderr;
		}
	}

	return true;
}
