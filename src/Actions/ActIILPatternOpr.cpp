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
#include "Actions/ActIILPatternOpr.h"

#include "Alarm/Alarm.h"
//#include "API/AosApiG.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/NetFileObj.h" 
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/CompareFun.h"
#include "Util/Sort.h"
#include "NetFile/NetFile.h"
#include "XmlUtil/XmlTag.h"
#include "SEModules/SnapshotIdMgr.h"


AosActIILPatternOpr::AosActIILPatternOpr(const bool flag)
:
AosSdocAction(AOSACTTYPE_IILPATTERNOPR, AosActionType::eIILPatternOpr, flag),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize)
{
}


AosActIILPatternOpr::AosActIILPatternOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_IILPATTERNOPR, AosActionType::eIILPatternOpr, false),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActIILPatternOpr::~AosActIILPatternOpr()
{
	//OmnScreen << "act iil batch opr deleted" << endl;
}


bool
AosActIILPatternOpr::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	AosXmlTagPtr dataCol_tag = def->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(iiltag, false);

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

	//felicia, 2014/08/18
	mMapIILName = def->getAttrStr("zky_map_iilname", "");
	aos_assert_rr(mMapIILName!="", rdata, false);
	
	//mIILMap = AosIILEntryMapMgr::retrieveIILEntryMap(mMapIILName, rdata);
	//aos_assert_rr(mIILMap, rdata, false);

	OmnString sep = "0x02";
	AosConvertAsciiBinary(sep);
	mIILMap = AosIILEntryMapMgr::retrieveIILEntryMap(mMapIILName, true, sep, false, false, rdata);

	mIILMap->readlock();
	u32 size = mIILMap->size();
	mIILMap->unlock();

	if (size == 0) 
	{
		OmnAlarm << "map size is 0, iilname:" << mMapIILName << enderr;
		return false;
	}

	mSpeedIILName = def->getAttrStr("zky_speed_iilname", "");
	if (mSpeedIILName != "")
	{
		mSpeedMap = AosIILEntryMapMgr::retrieveIILEntryMap(mSpeedIILName, true, sep, false, false, rdata);

		mSpeedMap->readlock();
		size = mSpeedMap->size();
		mSpeedMap->unlock();

		if (size == 0) 
		{
			OmnAlarm << "map size is 0, iilname:" << mSpeedIILName << enderr;
			return false;
		}
	}

	mMergeIILName = def->getAttrStr("zky_merger_iilname", "");
	aos_assert_rr(mMergeIILName != "", rdata, false);

	mKeyOpr = def->getAttrStr("zky_key_opr", "0x01");
	AosConvertAsciiBinary(mKeyOpr);

	mOrigTimeFormat = AosTimeFormat::toEnum(def->getAttrStr(AOSTAG_ORIG_TIME_FORMAT, "yyyy-mm-dd hh:mm:ss"));	
	aos_assert_r(AosTimeFormat::isValid(mOrigTimeFormat), false);
	//mTargetTimeFormat = AosTimeFormat::toEnum(def->getAttrStr(AOSTAG_TARGET_TIME_FORMAT, "epochday"));	
	mTargetTimeFormat = AosTimeFormat::toEnum("epochday");	

	return true;
}


bool
AosActIILPatternOpr::run(
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
AosActIILPatternOpr::callBack(
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
AosActIILPatternOpr::createIIL(
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

			bool rslt;
			if(mSpeedIILName == "")
			{
				rslt = mergeIIL(buff, mRundata);
				aos_assert(rslt);
			}
			else
			{
				rslt = mergeIIL2(buff, mRundata);
				aos_assert(rslt);
			}

			rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
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
AosActIILPatternOpr::mergeIIL(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	aos_assert_r(buff->dataLen() % mLen == 0, false);

	char * entry = buff->data();
	aos_assert_r(entry, false);
	
	char * crt_entry = &entry[mLen];

	vector<OmnString> strs;
	vector<OmnString> crt_strs;

	AosStrSplit split;
	OmnString key; 
	map<OmnString, OmnString>::iterator itr;
	int64_t spend_time, map_time;

	int new_rcdlen = 2 * mLen + 40; 
	int pos = 0;
	bool rslt;

	u32 num_entries = buff->dataLen()/mLen;
	
	AosBuffPtr newBuff = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff2 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff3 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff4 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);

	for(size_t i=0; i<num_entries; i++)
	{
		if(i == num_entries-1) break;
		strs.clear();
		crt_strs.clear();

		//key format : carid0x01time0x01addr
		split.splitStr(entry, mKeyOpr.data());          
		strs= split.entriesV();

		split.splitStr(crt_entry, mKeyOpr.data());
		crt_strs = split.entriesV();

		aos_assert_r(strs.size() > 0, false);
		aos_assert_r(crt_strs.size() > 0, false);
		if(strs[0] != crt_strs[0])
		{
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];	
			continue;
		}

		if(strs[1].length() == 10)
		{
			strs[1] << " 00:00:00";
		}
		AosValueRslt value;
		rslt = AosTimeFormat::convert(strs[1].data(), strs[1].length(),
				    mOrigTimeFormat, AosTimeFormat::eEpochTime, value, rdata);
		aos_assert_r(rslt, false);

		int64_t start_time = value.getI64();
		
		if(crt_strs[1].length() == 10)
		{
			crt_strs[1] << " 00:00:00";
		}
		rslt = AosTimeFormat::convert(crt_strs[1].data(), crt_strs[1].length(),
				    mOrigTimeFormat, AosTimeFormat::eEpochTime, value, rdata);
		aos_assert_r(rslt, false);
		int64_t end_time = value.getI64();
		
		spend_time = end_time - start_time;

		key = "";
		key << strs[2] << mKeyOpr << crt_strs[2];

		mIILMap->readlock();
		itr = mIILMap->find(key);
		//aos_assert_r(itr != mIILMap->end(), false);
		
		if(itr == mIILMap->end())
		{
			mIILMap->unlock();
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];	
			continue;
		}
		
		map_time = atol((itr->second).data());
		mIILMap->unlock();
		/*
		if(itr != mIILMap->end())
		{
			//entry = crt_entry;
			//crt_entry = &crt_entry[mLen];	
			//continue;
			aos_assert_r(itr->second != "", false);
			map_time = atol((itr->second).data());
		}
		else
		{
			map_time = spend_time + 10;
		}
		*/

		if(spend_time < map_time)
		{
			/*
			rslt = AosTimeFormat::convert(strs[1].data(), strs[1].length(),
					    mOrigTimeFormat, mTargetTimeFormat, value, rdata);
			aos_assert_r(rslt, false);

			bool correct;
			OmnString time = value.getStr();
			OmnString new_key = time;
			new_key << mKeyOpr << entry << mKeyOpr << crt_entry;
			*/

			u64 docid1 =  *(u64 *)&entry[mLen - sizeof(u64)]; 
			u64 docid2 =  *(u64 *)&crt_entry[mLen - sizeof(u64)]; 
			OmnString new_key, new_key2, new_key3, new_key4;
			//new_key format : time1,KKID2,carid,docid1,time2,KKID1,docid2
			new_key << strs[1] << mKeyOpr 
				    << crt_strs[2] << mKeyOpr 
					<< strs[0] << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;
			
			//new_key2 format : KKID2,time1,carid,docid1,time2,KKID1,docid2
			new_key2 << crt_strs[2]<< mKeyOpr 
				    << strs[1]<< mKeyOpr 
					<< strs[0] << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;
			//new_key3 format : carid, KKID2, time1, docid1,time2,KKID1,docid2
			new_key3 << strs[0] << mKeyOpr 
				    << crt_strs[2] << mKeyOpr 
					<< strs[1] << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;

			//new_key4 format : carid,time1,KKID2,docid1,time2,KKID1,docid2
			new_key4 << strs[0] << mKeyOpr 
				    << strs[1] << mKeyOpr 
					<< crt_strs[2] << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;

			aos_assert_r(new_key.length() + sizeof(u64) <= (u32)new_rcdlen, false);
			char * crt = newBuff->data() + pos;
			memcpy(crt, new_key.data(), new_key.length());
			crt[new_key.length()] = 0;
			*(u64*)&crt[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt2 = newBuff2->data() + pos;
			memcpy(crt2, new_key2.data(), new_key2.length());
			crt2[new_key2.length()] = 0;
			*(u64*)&crt2[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt3 = newBuff3->data() + pos;
			memcpy(crt3, new_key3.data(), new_key3.length());
			crt3[new_key3.length()] = 0;
			*(u64*)&crt3[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt4 = newBuff4->data() + pos;
			memcpy(crt4, new_key4.data(), new_key4.length());
			crt4[new_key4.length()] = 0;
			*(u64*)&crt4[new_rcdlen-sizeof(u64)] = docid1;
			
			pos += new_rcdlen;
		}

		entry = crt_entry;
		crt_entry = &crt_entry[mLen];
	}

	newBuff->setDataLen(pos);
	newBuff2->setDataLen(pos);
	newBuff3->setDataLen(pos);
	newBuff4->setDataLen(pos);

	OmnString str;
	str << "<CompareFun cmpfun_type=\"custom\" cmpfun_size=\"" << new_rcdlen << "\">"
		<< 	"<cmp_fields>"
		<<  	"<field cmp_size=\"-1\" cmp_datatype=\"str\" cmp_pos=\"0\" />"
		<< 	"</cmp_fields>"
		<< "</CompareFun>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);
	AosCompareFunPtr cmp_fun = AosCompareFun::getCompareFunc(cmp_tag);
	AosCompareFun * cmp_fun_raw = cmp_fun.getPtr();
	
	char *tmpbuff = OmnNew char[cmp_fun_raw->size];
	char * buff_end = newBuff->data() + pos;
	AosSort::sort(newBuff->data(), buff_end, cmp_fun_raw, tmpbuff);
	
	char * buff_end2 = newBuff2->data() + pos;
	AosSort::sort(newBuff2->data(), buff_end2, cmp_fun_raw, tmpbuff);
	
	char * buff_end3 = newBuff3->data() + pos;
	AosSort::sort(newBuff3->data(), buff_end3, cmp_fun_raw, tmpbuff);
	
	char * buff_end4 = newBuff4->data() + pos;
	AosSort::sort(newBuff4->data(), buff_end4, cmp_fun_raw, tmpbuff);
	OmnDelete [] tmpbuff;

	u64 executor_id = 0;
	if(mBuildBitmap)
	{
		executor_id = 1;
	}

	OmnString iilname2 = mMergeIILName;
	iilname2 << "_2";
	
	OmnString iilname3 = mMergeIILName;
	iilname3 << "_3";
	
	OmnString iilname4 = mMergeIILName;
	iilname4 << "_4";
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			mMergeIILName, new_rcdlen, newBuff, executor_id, mMergeSnapId, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname2, new_rcdlen, newBuff2, executor_id, mMergeSnapId2, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname3, new_rcdlen, newBuff3, executor_id, mMergeSnapId3, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);

	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname4, new_rcdlen, newBuff4, executor_id, mMergeSnapId4, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosActIILPatternOpr::mergeIIL2(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	aos_assert_r(buff->dataLen() % mLen == 0, false);

	char * entry = buff->data();
	aos_assert_r(entry, false);
	
	char * crt_entry = &entry[mLen];

	vector<OmnString> strs;
	vector<OmnString> crt_strs;

	AosStrSplit split;
	OmnString key; 
	map<OmnString, OmnString>::iterator itr;
	int64_t spend_time, distance;
	int speed, min_speed;

	int new_rcdlen = 2 * mLen + 40; 
	int pos = 0;
	bool rslt;

	u32 num_entries = buff->dataLen()/mLen;
	
	AosBuffPtr newBuff = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff2 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff3 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);
	AosBuffPtr newBuff4 = OmnNew AosBuff(num_entries * new_rcdlen AosMemoryCheckerArgs);

	for(size_t i=0; i<num_entries; i++)
	{
		if(i == num_entries-1) break;
		strs.clear();
		crt_strs.clear();

		//key format : carid0x01time0x01addr
		split.splitStr(entry, mKeyOpr.data());          
		strs= split.entriesV();

		split.splitStr(crt_entry, mKeyOpr.data());
		crt_strs = split.entriesV();

		aos_assert_r(strs.size() > 0, false);
		aos_assert_r(crt_strs.size() > 0, false);
		if(strs[0] != crt_strs[0])
		{
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];	
			continue;
		}

		if(strs[1].length() == 10)
		{
			strs[1] << " 00:00:00";
		}
		AosValueRslt value;
		rslt = AosTimeFormat::convert(strs[1].data(), strs[1].length(),
				    mOrigTimeFormat, AosTimeFormat::eEpochTime, value, rdata);
		aos_assert_r(rslt, false);

		int64_t start_time = value.getI64();
		
		if(crt_strs[1].length() == 10)
		{
			crt_strs[1] << " 00:00:00";
		}
		rslt = AosTimeFormat::convert(crt_strs[1].data(), crt_strs[1].length(),
				    mOrigTimeFormat, AosTimeFormat::eEpochTime, value, rdata);
		aos_assert_r(rslt, false);
		int64_t end_time = value.getI64();
		
		spend_time = end_time - start_time;

		key = "";
		key << strs[2] << mKeyOpr << crt_strs[2];

		mIILMap->readlock();
		itr = mIILMap->find(key);
		//aos_assert_r(itr != mIILMap->end(), false);
		
		if(itr == mIILMap->end())
		{
			mIILMap->unlock();
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];	
			continue;
		}
	
		distance = atol((itr->second).data());	
		mIILMap->unlock();
		if (distance <= 0)
		{
			OmnAlarm << "invalid distance : " << distance << " kkid1 : " << strs[2] << " kkid2 : " << crt_strs[2] << enderr;
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];
			continue;
		}
		aos_assert_r(distance > 0, false);

		if (!spend_time)
		{
			spend_time = 1;
		}
		speed = distance / spend_time; //(m/s)

		speed = speed /3.6; // (km/s)
	
		mSpeedMap->readlock();
		itr = mSpeedMap->find(key);
		if (itr == mSpeedMap->end())
		{
			mSpeedMap->unlock();
			entry = crt_entry;
			crt_entry = &crt_entry[mLen];
			continue;
		}

		min_speed = atoi((itr->second).data());
		mSpeedMap->unlock();

		if(min_speed < speed)
		{
			/*
			rslt = AosTimeFormat::convert(strs[1].data(), strs[1].length(),
					    mOrigTimeFormat, mTargetTimeFormat, value, rdata);
			aos_assert_r(rslt, false);

			bool correct;
			OmnString time = value.getStr();
			OmnString new_key = time;
			new_key << mKeyOpr << entry << mKeyOpr << crt_entry;
			*/

			u64 docid1 =  *(u64 *)&entry[mLen - sizeof(u64)]; 
			u64 docid2 =  *(u64 *)&crt_entry[mLen - sizeof(u64)]; 
			OmnString new_key, new_key2, new_key3, new_key4;
			//new_key format : time1,KKID2,carid,speed,docid1,time2,KKID1,docid2
			new_key << strs[1] << mKeyOpr 
				    << crt_strs[2] << mKeyOpr 
					<< strs[0] << mKeyOpr
					<< speed << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;
			
			//new_key2 format : KKID2,time1,carid,speed,docid1,time2,KKID1,docid2
			new_key2 << crt_strs[2]<< mKeyOpr 
				    << strs[1]<< mKeyOpr 
					<< strs[0] << mKeyOpr
					<< speed << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;
			//new_key3 format : carid, KKID2, time1, speed, docid1,time2,KKID1,docid2
			new_key3 << strs[0] << mKeyOpr 
				    << crt_strs[2] << mKeyOpr 
					<< strs[1] << mKeyOpr
					<< speed << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;

			//new_key4 format : carid,time1,KKID2,speed,docid1,time2,KKID1,docid2
			new_key4 << strs[0] << mKeyOpr 
				    << strs[1] << mKeyOpr 
					<< crt_strs[2] << mKeyOpr
					<< speed << mKeyOpr
					<< docid1 << mKeyOpr
					<< crt_strs[1] << mKeyOpr
					<< strs[2] << mKeyOpr
					<< docid2;

			aos_assert_r(new_key.length() + sizeof(u64) <= (u32)new_rcdlen, false);
			char * crt = newBuff->data() + pos;
			memcpy(crt, new_key.data(), new_key.length());
			crt[new_key.length()] = 0;
			*(u64*)&crt[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt2 = newBuff2->data() + pos;
			memcpy(crt2, new_key2.data(), new_key2.length());
			crt2[new_key2.length()] = 0;
			*(u64*)&crt2[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt3 = newBuff3->data() + pos;
			memcpy(crt3, new_key3.data(), new_key3.length());
			crt3[new_key3.length()] = 0;
			*(u64*)&crt3[new_rcdlen-sizeof(u64)] = docid1;
			
			char * crt4 = newBuff4->data() + pos;
			memcpy(crt4, new_key4.data(), new_key4.length());
			crt4[new_key4.length()] = 0;
			*(u64*)&crt4[new_rcdlen-sizeof(u64)] = docid1;
			
			pos += new_rcdlen;
		}

		entry = crt_entry;
		crt_entry = &crt_entry[mLen];
	}

	newBuff->setDataLen(pos);
	newBuff2->setDataLen(pos);
	newBuff3->setDataLen(pos);
	newBuff4->setDataLen(pos);

	OmnString str;
	str << "<CompareFun cmpfun_type=\"custom\" cmpfun_size=\"" << new_rcdlen << "\">"
		<< 	"<cmp_fields>"
		<<  	"<field cmp_size=\"-1\" cmp_datatype=\"str\" cmp_pos=\"0\" />"
		<< 	"</cmp_fields>"
		<< "</CompareFun>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);
	AosCompareFunPtr cmp_fun = AosCompareFun::getCompareFunc(cmp_tag);
	AosCompareFun * cmp_fun_raw = cmp_fun.getPtr();
	
	char *tmpbuff = OmnNew char[cmp_fun_raw->size];
	char * buff_end = newBuff->data() + pos;
	AosSort::sort(newBuff->data(), buff_end, cmp_fun_raw, tmpbuff);
	
	char * buff_end2 = newBuff2->data() + pos;
	AosSort::sort(newBuff2->data(), buff_end2, cmp_fun_raw, tmpbuff);
	
	char * buff_end3 = newBuff3->data() + pos;
	AosSort::sort(newBuff3->data(), buff_end3, cmp_fun_raw, tmpbuff);
	
	char * buff_end4 = newBuff4->data() + pos;
	AosSort::sort(newBuff4->data(), buff_end4, cmp_fun_raw, tmpbuff);
	OmnDelete [] tmpbuff;

	u64 executor_id = 0;
	if(mBuildBitmap)
	{
		executor_id = 1;
	}

	OmnString iilname2 = mMergeIILName;
	iilname2 << "_2";
	
	OmnString iilname3 = mMergeIILName;
	iilname3 << "_3";
	
	OmnString iilname4 = mMergeIILName;
	iilname4 << "_4";
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			mMergeIILName, new_rcdlen, newBuff, executor_id, mMergeSnapId, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname2, new_rcdlen, newBuff2, executor_id, mMergeSnapId2, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);
	
	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname3, new_rcdlen, newBuff3, executor_id, mMergeSnapId3, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);

	rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			iilname4, new_rcdlen, newBuff4, executor_id, mMergeSnapId4, mTaskDocid, mRundata);
	aos_assert_r(rslt, false);

	return true;
}


AosActionObjPtr
AosActIILPatternOpr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActIILPatternOpr(def, rdata);
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
AosActIILPatternOpr::AosCreateIIL::run()
{
	mCaller->createIIL(mReqId, mExpectedSize, mFinished);
	return true;
}


bool
AosActIILPatternOpr::AosCreateIIL::procFinished()
{
	return true;
}

bool
AosActIILPatternOpr::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	vector<AosTaskDataObjPtr> snapshots;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	AosTaskDataObjPtr snapshot;
	vector<u32> virtualids = AosGetTotalCubeIds();

	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		//may be not rollback here
		OmnString iilname = "ActIILPatternOpr_iilname";
		iilname << virtualids[i];
		OmnString key;
		key << AOSTASKDATA_IIL_SNAPSHOT << "__"
			<< virtualids[i] << "__"
			<< iilname << "__"
			<< mTaskDocid;
		
		snapshot = task->getSnapShot(key, rdata);
		aos_assert_r(!snapshot, false);
		
		snapshot_id = iil_client->createSnapshot(virtualids[i], snapshot_id,  mTaskDocid, rdata);
		//snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtualids[i], snapid, iilname, mTaskDocid, false);
		//aos_assert_r(snapshot, false);
		mSnapMaps[virtualids[i]] = snapshot_id;
		//snapshots.push_back(snapshot);
	}

	map<u32, SnapInfo>   snap_iilnames;
	map<u32, SnapInfo>::iterator  snap_itr;

	u32 virtual_id = AosGetCubeId(mIILName);
	map<u32, u64>::iterator itr = mSnapMaps.find(virtual_id);
	aos_assert_r(itr != mSnapMaps.end(), false);
	mSnapId = itr->second;
	snap_itr = snap_iilnames.find(virtual_id);
	if(snap_itr == snap_iilnames.end())
	{
		set<OmnString> iilnames;
		iilnames.insert(mIILName);
		SnapInfo info;
		info.mSnapId = mSnapId;
		info.mIILNames = iilnames;
		snap_iilnames.insert(make_pair(virtual_id, info));
	}
	else
	{
		((snap_itr->second).mIILNames).insert(mIILName);
	}
OmnScreen << "####### ActIILPatternOpr virtualid : " << virtual_id << " snapid : " << mSnapId << " iilname : " << mIILName << endl;

	virtual_id = AosGetCubeId(mMergeIILName);
	itr = mSnapMaps.find(virtual_id);
	aos_assert_r(itr != mSnapMaps.end(), false);
	mMergeSnapId = itr->second;
	snap_itr = snap_iilnames.find(virtual_id);
	if(snap_itr == snap_iilnames.end())
	{
		set<OmnString> iilnames;
		iilnames.insert(mMergeIILName);
		SnapInfo info;
		info.mSnapId = mMergeSnapId;
		info.mIILNames = iilnames;
		snap_iilnames.insert(make_pair(virtual_id, info));
	}
	else
	{
		((snap_itr->second).mIILNames).insert(mMergeIILName);
	}
OmnScreen << "####### ActIILPatternOpr virtualid : " << virtual_id << " snapid : " << mMergeSnapId<< " iilname : " << mMergeIILName<< endl;
	
	OmnString iilname2 = mMergeIILName;
	iilname2 << "_2";
	virtual_id = AosGetCubeId(iilname2);
	itr = mSnapMaps.find(virtual_id);
	aos_assert_r(itr != mSnapMaps.end(), false);
	mMergeSnapId2 = itr->second;
	snap_itr = snap_iilnames.find(virtual_id);
	if(snap_itr == snap_iilnames.end())
	{
		set<OmnString> iilnames;
		iilnames.insert(iilname2);
		SnapInfo info;
		info.mSnapId = mMergeSnapId2;
		info.mIILNames = iilnames;
		snap_iilnames.insert(make_pair(virtual_id, info));
	}
	else
	{
		((snap_itr->second).mIILNames).insert(iilname2);
	}
OmnScreen << "####### ActIILPatternOpr virtualid : " << virtual_id << " snapid : " << mMergeSnapId2<< " iilname : " << iilname2<< endl;
	
	OmnString iilname3 = mMergeIILName;
	iilname3 << "_3";
	virtual_id = AosGetCubeId(iilname3);
	itr = mSnapMaps.find(virtual_id);
	aos_assert_r(itr != mSnapMaps.end(), false);
	mMergeSnapId3 = itr->second;
	snap_itr = snap_iilnames.find(virtual_id);
	if(snap_itr == snap_iilnames.end())
	{
		set<OmnString> iilnames;
		iilnames.insert(iilname3);
		SnapInfo info;
		info.mSnapId = mMergeSnapId3;
		info.mIILNames = iilnames;
		snap_iilnames.insert(make_pair(virtual_id, info));
	}
	else
	{
		((snap_itr->second).mIILNames).insert(iilname3);
	}
OmnScreen << "####### ActIILPatternOpr virtualid : " << virtual_id << " snapid : " << mMergeSnapId3<< " iilname : " << iilname3<< endl;
	
	OmnString iilname4 = mMergeIILName;
	iilname4 << "_4";
	virtual_id = AosGetCubeId(iilname4);
	itr = mSnapMaps.find(virtual_id);
	aos_assert_r(itr != mSnapMaps.end(), false);
	mMergeSnapId4 = itr->second;
	snap_itr = snap_iilnames.find(virtual_id);
	if(snap_itr == snap_iilnames.end())
	{
		set<OmnString> iilnames;
		iilnames.insert(iilname4);
		SnapInfo info;
		info.mSnapId = mMergeSnapId4;
		info.mIILNames = iilnames;
		snap_iilnames.insert(make_pair(virtual_id, info));
	}
	else
	{
		((snap_itr->second).mIILNames).insert(iilname4);
	}
OmnScreen << "####### ActIILPatternOpr virtualid : " << virtual_id << " snapid : " << mMergeSnapId4<< " iilname : " << iilname4<< endl;

	snap_itr = snap_iilnames.begin();
	for(; snap_itr != snap_iilnames.end(); ++snap_itr)
	{
		snapshot = AosTaskDataObj::createTaskDataIILSnapShot(snap_itr->first, (snap_itr->second).mSnapId, (snap_itr->second).mIILNames, mTaskDocid, false);
		aos_assert_r(snapshot, false);
		snapshots.push_back(snapshot);
	}

	aos_assert_r(snapshots.size() > 0, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}


/*
bool
AosActIILPatternOpr::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	//1. if task is not finish check snapshot is exist, 
	//2. if snapshot exist, rollback it
	//3. otherwise to create snapshot
	vector<AosTaskDataObjPtr> snapshots;
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();

	bool rslt = getSnapShotId(mIILName, mSnapId, snapshots, task, rdata);
	aos_assert_r(rslt, false);

	rslt = getSnapShotId(mMergeIILName, mMergeSnapId, snapshots, task, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname2 = mMergeIILName;
	iilname2 << "_2";
	rslt = getSnapShotId(iilname2, mMergeSnapId2, snapshots, task, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname3 = mMergeIILName;
	iilname3 << "_3";
	rslt = getSnapShotId(iilname3, mMergeSnapId3, snapshots, task, rdata);
	aos_assert_r(rslt, false);
	
	OmnString iilname4 = mMergeIILName;
	iilname4 << "_4";
	rslt = getSnapShotId(iilname4, mMergeSnapId4, snapshots, task, rdata);
	aos_assert_r(rslt, false);

	aos_assert_r(snapshots.size() > 0, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}

bool
AosActIILPatternOpr::getSnapShotId(
		const OmnString &iilname, 
		u64 &snapid,
		vector<AosTaskDataObjPtr> &snapshots,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	
	u32 virtual_id = AosGetCubeId(iilname);
	aos_assert_r(task, false);
	//may be not rollback here
	OmnString key;
	key << AOSTASKDATA_IIL_SNAPSHOT << "__"
		<< virtual_id << "__"
		<< iilname << "__"
		<< mTaskDocid;
	AosTaskDataObjPtr snapshot = task->getSnapShot(key, rdata);
	aos_assert_r(!snapshot, false);
	mSnapId = iil_client->createSnapshot(virtual_id, mTaskDocid, rdata);
	set<OmnString> iilnames;
	iilnames.insert(mIILName);
	snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtual_id, mSnapId, iilnames, mTaskDocid, false);
	aos_assert_r(snapshot, false);
	snapshots.push_back(snapshot);
	return true;
}
*/

#endif
