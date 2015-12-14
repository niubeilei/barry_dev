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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActIILPattern2Opr.h"

#include "Alarm/Alarm.h"
//#include "API/AosApiG.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/VirtualFileObj.h" 
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "VirtualFile/VirtualFile.h"
#include "XmlUtil/XmlTag.h"
#include "NetFile/NetFile.h" 


AosActIILPattern2Opr::AosActIILPattern2Opr(const bool flag)
:
AosSdocAction(AOSACTTYPE_IILPATTERNOPR2, AosActionType::eIILPatternOpr2, flag),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize),
mValue(0)
{
}


AosActIILPattern2Opr::AosActIILPattern2Opr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_IILPATTERNOPR2, AosActionType::eIILPatternOpr2, false),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize),
mValue(0)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActIILPattern2Opr::~AosActIILPattern2Opr()
{
	//OmnScreen << "act iil batch opr deleted" << endl;
}


bool
AosActIILPattern2Opr::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	AosXmlTagPtr dataCol_tag = def->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(iiltag, false);

	//mIILName = iiltag->getAttrStr(AOSTAG_IILNAME);
	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);
	
	mLen = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	if (mLen < 0)
	{
		AosXmlTagPtr rcd_xml = iiltag->getFirstChild("datarecord");
		aos_assert_r(rcd_xml, false);

		OmnString rcd_objid = rcd_xml->getAttrStr(AOSTAG_OBJID, "");
	
		AosXmlTagPtr record;
		if (rcd_objid != "")
		{
			record = AosGetDocByObjid(rcd_objid, rdata);
		}
		else 
		{
			record = rcd_xml;
		}

		aos_assert_r(record, false);
		if (record->getAttrStr("type") == "ctnr")
		{
			record = record->getFirstChild("datarecord");
			aos_assert_r(record, false);
		}

		//mLen = record->getAttrInt("record_len", -1);
		mLen = record->getAttrInt("zky_length", -1);
		aos_assert_r(mLen > 0, false);
	}

	//mType = iiltag->getAttrStr(AOSTAG_TYPE);
	//aos_assert_r(mType!= "", false);

	//if (mType == "strinc")
	//{
	//	OmnString inctype = iiltag->getAttrStr(AOSTAG_INCTYPE);
	//	mIncType = AosIILUtil::IILIncType_toEnum(inctype);
	//	mInitdocid = iiltag->getAttrU64(AOSTAG_INITDOCID, 0);
	//}

	mType = "stradd";
	mMaxBuffSize = def->getAttrU64("max_buff_size", eMaxBuffSize);
	mBuildBitmap = def->getAttrBool("build_bitmap", false);

	mKeyOpr = def->getAttrStr("zky_key_opr", "0x01");       
	AosConvertAsciiBinary(mKeyOpr);

	mNumkeys = def->getAttrInt("zky_num_keys", 0);
	aos_assert_r(mNumkeys>0, false);
	OmnString s = def->getAttrStr("zky_value_idx", "");
	AosStrSplit split;
	split.splitStr(s.data(), ",");          
	vector<OmnString> &keys = split.entriesV();
	aos_assert_r(!keys.empty(), false);
	for (size_t i=0; i<keys.size(); i++) {
		mRsltIdx.push_back(atoi(keys[i].data()));
	}
	return true;
}


bool
AosActIILPattern2Opr::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!task && !mIsTest)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mTask = task;
	
	mFinished = false;

	//Jozhi 2014-03-11
	//bool rslt = snapShot(rdata);
	//aos_assert_r(rslt, false);
	
	//------------test
	//AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	//AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	//query_context->setOpr(eAosOpr_an);
	//query_context->setStrValue("");
	//query_context->setBlockSize(100);
	//rslt = AosQueryColumn(mIILName, query_rslt, 0, query_context,rdata);
	//aos_assert_r(query_rslt->getNumDocs() == 0, false);
	//-------------end

	AosXmlTagPtr files_tag = sdoc->getFirstChild("files");
	aos_assert_r(files_tag, false);

	AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
	aos_assert_r(file_tag, false);

	mCrtPos = file_tag->getAttrInt64(AOSTAG_START_POS, -1);
	aos_assert_r(mCrtPos != -1, false);

	mMaxLength = file_tag->getAttrInt64(AOSTAG_LENGTH, -1);
	// Chen Ding, 2013/10/16
	// aos_assert_r(mMaxLength != -1, false);
	aos_assert_r(mMaxLength > 0, false);
	aos_assert_r(mMaxLength % mLen == 0, false);

	mProcLength = 0;
	
	mFileId = file_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(mFileId != 0, false);
		
	mPhysicalId = file_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId != -1, false);
	
	AosNetFileObjPtr file = OmnNew AosIdNetFile(mPhysicalId, mFileId);
	aos_assert_r(file, false);

	file->seek(mCrtPos);

	vector<AosNetFileObjPtr> virFiles;
	virFiles.push_back(file);

	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	mScanner->setFiles(virFiles);
	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	int64_t buffsize = mMaxBuffSize / mLen;
	buffsize *= mLen;

	// ????
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}
	
	aos_assert_r(buffsize % mLen == 0, false);
	mScanner->addRequest(0, buffsize);
	return true;
}

void
AosActIILPattern2Opr::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return;
	}
	OmnThrdShellProcPtr runner = OmnNew AosCreateIIL(this, reqId, expected_size, finished);
	bool rslt = addThreadShellProc(runner, mRundata);
	if (!rslt)
	{
		actionFailed(mRundata);
	}
}	

	
void
AosActIILPattern2Opr::createIIL(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return;
	}
	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);

	aos_assert(buff);
	int64_t bufflen = buff->dataLen();

	if (bufflen > 0)
	{
		aos_assert(bufflen % mLen == 0);
		AosIILExecutorObjPtr executor;
		u64 executor_id = 0;
		if(mBuildBitmap)
		{
			executor_id = 1;
		}
		if (mType == "strinc")
		{
			bool rslt = AosIILClientObj::getIILClient()->StrBatchInc(
					mIILName, mLen, buff, mInitdocid, mIncType, mSnapId, 
					mTaskDocid, mRundata);
			aos_assert(rslt);
		}
		else if(mType == "stradd")
		{
//			AosBitmapEngineObjPtr engine = AosBitmapEngineObj::getBitmapEngine();
//			if (engine)
//			{
//				executor = engine->getIILExecutor(mRundata);
//				if(executor)
//				{
//					executor_id = executor->getExecutorID();
//				}
//				else
//				{
//					executor_id = 0;
//				}
//			}
//
			OmnString iilname = mIILName;
			iilname << "_distmap";

			bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
					iilname, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
			aos_assert(rslt);
		}
		else if(mType == "u64add")
		{
//			AosBitmapEngineObjPtr engine = AosBitmapEngineObj::getBitmapEngine();
//			if (engine)
//			{
//				executor = engine->getIILExecutor(mRundata);
//				if(executor)
//				{
//					executor_id = executor->getExecutorID();
//				}
//				else
//				{
//					executor_id = 0;
//				}
//			}

			bool rslt = AosIILClientObj::getIILClient()->U64BatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
			aos_assert(rslt);
		}
		else
		{
			OmnAlarm << "Invalid type : " << mType << enderr;
		}

		mCrtPos += bufflen;
		mProcLength += bufflen;
		aos_assert(mProcLength <= mMaxLength);

		int progress = mProcLength * 100 / mMaxLength;
		if (progress - mProcgress >= 3 || mProcgress == 100)
		{
		    mProcgress = progress;                                                                                           
			if (mTask) mTask->actionProgressed(mProcgress, mRundata);
		}	
	}

	if (finished || mProcLength == mMaxLength)
	{
		distinctCountInit(mRundata);
		AosActionObjPtr thisptr(this, true);
		//Jozhi 2013-11-19 Testing
		if (mTask) mTask->actionFinished(thisptr, mRundata);
		//if (mTask) mTask->actionFailed(thisptr, mRundata);
		mFinished = true;
		aos_assert(mScanner);
		mScanner->destroyedMember();
		mScanner = 0;
		//Jozhi 2014-04-28 is delete file
		if (!mTask || !mTask->getIsDeleteFile())
		{
			return;
		}
		mTask = 0;
		aos_assert(mFileId != 0);
		aos_assert(mPhysicalId != -1);

		bool svr_death = false;
		bool rslt =  AosNetFileCltObj::deleteFileStatic(
				    mFileId, mPhysicalId, svr_death, mRundata.getPtr());
		aos_assert(rslt);
		return;
	}

	int64_t buffsize = mMaxBuffSize / mLen;
	buffsize *= mLen;
	
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}

	aos_assert(buffsize % mLen == 0);
	mScanner->addRequest(reqId, buffsize); 
}

bool
AosActIILPattern2Opr::distinctCountInit(
		const AosRundataPtr &rdata)
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();            
	aos_assert_r(query_context, false);

	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	query_context->setStrValue("*");
	query_context->setBlockSize(1000000);
	AosQueryRsltObjPtr query_rslt;

	AosIILClientObj::getIILClient()->deleteIIL(mIILName, true, rdata);

	int max_len = 1000000;
	AosBuffPtr newBuff = OmnNew AosBuff(max_len*mLen AosMemoryCheckerArgs);
	char* data = newBuff->data();

	bool rslt, finished;
	OmnString key;
	u64 value;

	int i = 0;
	while(1)
	{
		query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt->setWithValues(true);

		OmnString iilname = mIILName;
		iilname << "_distmap";
		rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, mRundata);
		aos_assert_r(rslt, false);

		while(1)
		{
			rslt = query_rslt->nextDocidValue(value, key, finished, mRundata);
			aos_assert_r(rslt, 0);

			if (finished)
			{
				break;
			}
			
			strcpy(data, key.data());
			*(u64*)(data+mLen) = value;
			data+=mLen;
			i++;
			if (i>max_len-1)
			{
				newBuff->setDataLen(data-newBuff->data());
				AosBuffPtr buff = distinctCount(newBuff, rdata);
				AosIILClientObj::getIILClient()->StrBatchAdd(
					mIILName, mLen, buff, 0, mSnapId, mTaskDocid,
					mRundata);
				i=0;
				newBuff->reset();
				data = newBuff->data();
			}
			
		};

		if (query_context->finished())
		{
			    break;
		}
	}

	newBuff->setDataLen(data-newBuff->data());
	if (newBuff->dataLen()) {
		AosBuffPtr buff = distinctCount(newBuff, rdata);
		AosIILClientObj::getIILClient()->StrBatchAdd(
			mIILName, mLen, buff, 0, mSnapId, mTaskDocid,
			mRundata);
	}

	return true;
}

AosBuffPtr
AosActIILPattern2Opr::distinctCount(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, NULL);
	aos_assert_r(buff->dataLen() % mLen == 0, NULL);

	char * entry = buff->data();
	aos_assert_r(entry, NULL);

	AosBuffPtr newBuff = OmnNew AosBuff(buff->dataLen() AosMemoryCheckerArgs);
	char * newentry = newBuff->data();
	
	while(entry < buff->data()+buff->dataLen())
	{
		OmnString keystr(entry);
		AosStrSplit split;
		split.splitStr(entry, mKeyOpr.data());          
		vector<OmnString> &keys = split.entriesV();
		aos_assert_r(keys.size() == mNumkeys, NULL);

		if (mKeys.empty())
		{
			mKeys.swap(keys);
			mValue = 1;
			entry += mLen;
			continue;
		}
		else {
			bool matched = true;
			for (size_t i=0; i<mNumkeys-1; i++)
			{
				if (mKeys[i] != keys[i])
				{
					matched = false;
					break;
				}
			}	
			if (matched)
			{
				for (size_t i=0; i<mNumkeys; i++)
				{
					if (mKeys[i] != keys[i])
					{
						matched = false;
						break;
					}
				}	
				if (!matched)
				{
					mValue++;	
				}
			}
			else 
			{
				OmnString key;
				for (size_t i=0; i<mRsltIdx.size(); i++)
				{
					if (i == mRsltIdx.size()-1) {
						key << mKeys[mRsltIdx[i]];
					}
					else {
						key << mKeys[mRsltIdx[i]] << mKeyOpr;
					}
				}	

				memcpy(newentry, key.data(), key.length());
				newentry[key.length()] = 0;
				*(u64*)(newentry+mLen-sizeof(u64)) = mValue;
				newentry += mLen;

				mKeys.swap(keys);
				mValue = 1;
			}
			entry += mLen;
		}
	}

	OmnString key;
	for (size_t i=0; i<mRsltIdx.size(); i++)
	{
		if (i == mRsltIdx.size()-1) {
			key << mKeys[mRsltIdx[i]];
		}
		else {
			key << mKeys[mRsltIdx[i]] << mKeyOpr;
		}
	}	
	memcpy(newentry, key.data(), key.length());
	newentry[key.length()] = 0;
	*(u64*)(newentry+mLen-sizeof(u64)) = mValue;
	newentry += mLen;


	newBuff->setDataLen(newentry-newBuff->data());
	
	return newBuff;
}


AosActionObjPtr
AosActIILPattern2Opr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActIILPattern2Opr(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActIILPattern2Opr::AosCreateIIL::run()
{
	mCaller->createIIL(mReqId, mExpectedSize, mFinished);
	return true;
}


bool
AosActIILPattern2Opr::AosCreateIIL::procFinished()
{
	return true;
}

bool
AosActIILPattern2Opr::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	//1. if task is not finish check snapshot is exist, 
	//2. if snapshot exist, rollback it
	//3. otherwise to create snapshot
	vector<AosTaskDataObjPtr> snapshots;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	u32 virtual_id = AosGetCubeId(mIILName);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	//may be not rollback here
	OmnString key;
	key << AOSTASKDATA_IIL_SNAPSHOT << "__"
		<< virtual_id << "__"
		<< mIILName << "__"
		<< mTaskDocid;
	AosTaskDataObjPtr snapshot = task->getSnapShot(key, rdata);
	aos_assert_r(!snapshot, false);
	//mSnapId = iil_client->createSnapshot(virtual_id, mTaskDocid, rdata);
	mSnapId = 0;
	set<OmnString> iilnames;
	iilnames.insert(mIILName);
	snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtual_id, mSnapId, iilnames, mTaskDocid, false);
	aos_assert_r(snapshot, false);
	snapshots.push_back(snapshot);
	aos_assert_r(snapshots.size() == 1, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}
#endif
