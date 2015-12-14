////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/StorageEngineMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "DocTrans/BatchCreateDocTrans.h"
#include "StorageEngine/SengineDocInfoFixed.h"
#include "StorageEngine/SengineDocInfoCSV.h"
#include "StorageEngine/SengineDocInfoCommon.h"
#include "StorageEngine/Ptrs.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlDoc.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "StorageEngine/SengineImportDocRequest.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Util/File.h"
#include "Util/StrSplit.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/DocTypes.h"
#include "Debug/Debug.h"

//#include <map>

static bool				sgSanityCheck = true;

OmnSingletonImpl(AosStorageEngineMgrSingleton, 
				 AosStorageEngineMgr, 
				 AosStorageEngineMgrSelf, 
				"AosStorageEngineMgr");

AosStorageEngineMgr::AosStorageEngineMgr()
:
mNumThreads(1),
mTLock(OmnNew OmnMutex())
{
}


AosStorageEngineMgr::~AosStorageEngineMgr()
{
}


bool 
AosStorageEngineMgr::start()
{
	return true;
}


bool 
AosStorageEngineMgr::stop()
{
	return true;
}


bool 
AosStorageEngineMgr::config(const AosXmlTagPtr &config)
{
	AosXmlTagPtr conf = config->getFirstChild("storageengine");
	if (!conf) return true;

	mNumThreads = conf->getAttrU32("num_threads", 1);
	aos_assert_r(mNumThreads, false);

	for (u32 i = 0; i< mNumThreads; i++)
	{
		mThread[i] = 0;
	}
	return true;
}


void
AosStorageEngineMgr::startThread(const AosRundataPtr &rdata)
{
	OmnThreadedObjPtr thisptr(this, false);
	OmnString thrd_name;
	for (u32 i = 0; i< mNumThreads; i++)
	{
		thrd_name = "StorageEngineThrd_";
		thrd_name << i;
		mThread[i] = OmnNew OmnThread(thisptr, thrd_name, i, false, true, __FILE__, __LINE__);
		mLock[i] = OmnNew OmnMutex();
		mCondVar[i] = OmnNew OmnCondVar();
		mRundata[i] = rdata->clone(AosMemoryCheckerArgsBegin);
		mFlush[i] = true;
		mThread[i]->start(); 
	}
}


bool
AosStorageEngineMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		u32 logicid = thread->getLogicId();

		mLock[logicid]->lock();
		AosRundataPtr rdata = mRundata[logicid];
		aos_assert_rl(rdata, mLock[logicid], false);
		
		if (mProcData[logicid].size() <= 0)
		{
			aos_assert_r(mProcData[logicid].empty(), false);
			mCondVar[logicid]->wait(mLock[logicid]);
			mLock[logicid]->unlock();
			continue;
		}
		mFlush[logicid] = false;

		AosSengineImportDocRequestPtr ptr = mProcData[logicid].front();
		mProcData[logicid].pop();

		mLock[logicid]->unlock();

		AosSengineImportDocObjPtr caller = ptr->caller;
		aos_assert_r(caller, false);

		AosBuffPtr buff = caller->getNextBlock(ptr->reqId, ptr->expectedSize);
		aos_assert_r(buff, false);

		bool rslt = readyData(logicid, buff, ptr, rdata);
		if (!rslt)
		{
			rslt = caller->storageEngineError(rdata);
			state = OmnThrdStatus::eExit;
			aos_assert_r(rslt, false);
			return false;
		}

		caller->semPost();
		mLock[logicid]->lock();
		mRundata[logicid]->resetError();
		mFlush[logicid] = true;
		mLock[logicid]->unlock();
	}
	return true;
}


bool
AosStorageEngineMgr::readyData(
		const u32 logicid,
		const AosBuffPtr &buff,
		const AosSengineImportDocRequestPtr &ptr,
		const AosRundataPtr &rdata)
{
	switch(ptr->opr)
	{
	case AosGroupDocOpr::eBatchDelete:
	{
		OmnNotImplementedYet;
		return true;
	}
	case AosGroupDocOpr::eUpdate:
	{
		OmnNotImplementedYet;
		return true;
	}
	case AosGroupDocOpr::eBatchInsert:
	{
		AosDataAssemblerType::E doc_ass_type = ptr->docAssType; 
		switch (doc_ass_type)
		{
			case AosDataAssemblerType::eDocVar : 
				return	saveNormalDoc(logicid, buff, ptr->fileid, ptr->snapIds, ptr->taskDocid, rdata);

			//case AosDataAssemblerType::eDocNorm :
			//	return saveFixedLengthDoc(ptr->sizeid, ptr->record_len, buff, ptr->snapIds, ptr->taskDocid, rdata);

			//case AosDataAssemblerType::eDocCSV :
			//	return saveCSVDoc(ptr->sizeid, ptr->record_len, buff, ptr->snapIds, ptr->taskDocid, rdata);
			case AosDataAssemblerType::eDocNorm :
			case AosDataAssemblerType::eDocCSV :
				return batchInsertDoc(buff, ptr->snapIds, ptr->taskDocid, rdata);

			default :
				break;
		}
		OmnAlarm << "error doc asm type" << doc_ass_type << enderr;
		break;
	}
	default:
		break;
	}
	return false;
}


//Jozhi 2015/01/23
/*
bool
AosStorageEngineMgr::saveFixedLengthDoc(
		const u32 sizeid,
		const int record_len,
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record_len > 0, false);
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;

	buff->reset();
	if (buff->dataLen() <= 0) return true;

	u64 docid = 0;
	int record_size = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		aos_assert_r(sizeid == AosGetSizeIdByDocid(docid), false);
		
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			aos_assert_r(snap_ids.count(vid) != 0, false);
			doc_info = OmnNew AosSengineDocInfoFixed(AosGroupDocOpr::eBatchInsert, vid, snap_ids[vid], task_docid);
			docInfos[vid] = doc_info;
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);

		record_size = buff->getInt(0);
		aos_assert_r(record_size == record_len, false);

		int64_t offset = buff->getCrtIdx();

		bool rslt = doc_info->addDoc(buff->data() + offset, record_size, docid, rdata);
		aos_assert_r(rslt, false);
		buff->setCrtIdx(offset + record_size);
	}

	bool rslt = false;
	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		rslt = doc_info->sendRequestPublic(rdata);
		if (!rslt) return false;

		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}
*/

bool
AosStorageEngineMgr::saveNormalDoc(
		const u32 logicid,
		const AosBuffPtr &buff,
		const u64 &fileid,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	buff->reset();
	if (buff->dataLen() <= 0) return true;

	bool rslt = procCacheBuff(logicid, fileid, buff, snap_ids, task_docid, rdata);
	aos_assert_r(rslt, false);

	u64 docid = 0;
	int record_size = 0;
	int64_t crt_idx = buff->getCrtIdx();
	while (buff->dataLen()>0 && crt_idx < buff->dataLen())
	{
		int64_t copy_len = buff->dataLen() - crt_idx;
		if (copy_len > eDocidLength)
		{
			docid = buff->getU64(0);
			aos_assert_r(docid, false);

			record_size = buff->getInt(0);
			aos_assert_r(record_size, false);

			if (copy_len - eDocidLength < record_size)
			{
				AosBuffPtr cache_buff = OmnNew AosBuff(copy_len AosMemoryCheckerArgs);
				cache_buff->setBuff(buff->data()+crt_idx, copy_len);
				mCacheData[logicid].insert(make_pair(fileid, cache_buff));
				break;
			}
		}
		else
		{
			AosBuffPtr cache_buff = OmnNew AosBuff(copy_len AosMemoryCheckerArgs);
			cache_buff->setBuff(buff->data() + crt_idx, copy_len);	
			mCacheData[logicid].insert(make_pair(fileid, cache_buff));
			break;
		}

		crt_idx = buff->getCrtIdx();
		AosDocType::E type = AosDocType::getDocType(docid);
		aos_assert_r(type == AosDocType::eNormalDoc, false);

		char *doc = buff->data() + crt_idx;
		sanityCheck2(docid, record_size, doc);

		int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		aos_assert_r(snap_ids.count(vid) != 0, false);
		AosTransPtr trans = OmnNew AosBatchCreateDocTrans(
				docid, doc, record_size, snap_ids[vid], 
				task_docid, true, false);
		AosSendTrans(rdata, trans);

		buff->setCrtIdx(crt_idx + record_size);

		crt_idx = buff->getCrtIdx();
	}

	return true;
}


bool
AosStorageEngineMgr::procCacheBuff(
		const u32 logicid,
		const u64 &fileid, 
		const AosBuffPtr &doc_buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{	
	map<u64, AosBuffPtr>::iterator iter = mCacheData[logicid].find(fileid); 
	if (iter == mCacheData[logicid].end())
	{
		return true;
	}

	AosBuffPtr pre_buff = iter->second; 
	aos_assert_r(pre_buff, false);

	mCacheData[logicid].erase(fileid);

	char *doc;
	int64_t record_size = 0;
	u64 docid = 0;

	pre_buff->reset();
	int64_t pre_buff_datalen = pre_buff->dataLen();
	if (pre_buff_datalen < eDocidLength) 
	{
		int64_t copy_size = eDocidLength - pre_buff_datalen;
		pre_buff->setCrtIdx(pre_buff_datalen);

		aos_assert_r(doc_buff->dataLen() > copy_size, false);
		pre_buff->setBuff(doc_buff->data(), copy_size);

		pre_buff->reset();
		docid = pre_buff->getU64(0);
		aos_assert_r(docid, false);

		record_size = pre_buff->getInt(0);
		aos_assert_r(record_size, false);

		aos_assert_r(doc_buff->dataLen() >= record_size + copy_size, false);
		doc = doc_buff->data() + copy_size;

		sanityCheck2(docid, record_size, doc);
		doc_buff->setCrtIdx(copy_size + record_size);
	}
	else
	{
		docid = pre_buff->getU64(0);
		aos_assert_r(docid, false);

		record_size = pre_buff->getInt(0);
		aos_assert_r(record_size, false);

		int64_t crt_idx = pre_buff->getCrtIdx(); 

		int64_t partial_record_size = pre_buff_datalen - eDocidLength;
		aos_assert_r(partial_record_size < record_size, false);

		int64_t copy_size = record_size - partial_record_size; 
		aos_assert_r(doc_buff->dataLen() >= copy_size, false);

		pre_buff->setCrtIdx(pre_buff_datalen);
		pre_buff->setBuff(doc_buff->data(), copy_size);

		doc = pre_buff->data() + crt_idx;
		sanityCheck2(docid, record_size, doc);

		doc_buff->setCrtIdx(copy_size);
	}

	aos_assert_r(docid, false);
	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type == AosDocType::eNormalDoc, false);
	
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	aos_assert_r(snap_ids.count(vid) != 0, false);
	AosTransPtr trans = OmnNew AosBatchCreateDocTrans(
			docid, doc, record_size, snap_ids[vid], 
			task_docid,true, false);
	AosSendTrans(rdata, trans);
	return true;
}


bool
AosStorageEngineMgr::signal(const int threadLogicId)
{
	return true;
}


bool
AosStorageEngineMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosStorageEngineMgr::saveDoc(
		const AosDataAssemblerType::E type,
		const u32 sizeid, 
		const u64 &fileid,
		const int record_len,
		const AosSengineImportDocObjPtr &caller, 
		const u64 &reqId,
		const int64_t &size,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	mTLock->lock();
	if (!mThread[0]) startThread(rdata); 
	mTLock->unlock();
	aos_assert_r(sizeid, false);
	int idx = sizeid % mNumThreads;

	OmnTagFuncInfo << "save doc: sizeid is: " << sizeid <<
		" size is: " << size << " fileid is: " << fileid <<
		" record_len is: " << record_len << " task_docid is: " <<
		task_docid << endl;

	AosSengineImportDocRequestPtr ptr = OmnNew AosSengineImportDocRequest(
		sizeid, caller, reqId, size, type, fileid, record_len, snap_ids, task_docid);
	aos_assert_r(ptr, false);
	ptr->opr = AosGroupDocOpr::eBatchInsert;

	mLock[idx]->lock();
	mProcData[idx].push(ptr);
	mCondVar[idx]->signal(); 
	mLock[idx]->unlock();
	return true;
}


bool
AosStorageEngineMgr::sanityCheck(const u64 &fileid)
{
	if (!sgSanityCheck) return true;
	u32 sizeid = (u32)fileid;
	u32 logicid = sizeid % mNumThreads;
	map<u64, AosBuffPtr>::iterator iter = mCacheData[logicid].find(fileid); 
	aos_assert_r(iter == mCacheData[logicid].end(), false);
	return true;
}


bool
AosStorageEngineMgr::sanityCheck2(
		const u64 &docid, 
		const int64_t record_size,
		const char *doc)
{
	if (!sgSanityCheck) return true;
	aos_assert_r(doc, false);
	aos_assert_r(record_size > 0, false);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(doc, record_size, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	aos_assert_r(xml->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
	return true;
}

bool
AosStorageEngineMgr::updateDoc(
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosStorageEngineMgr::updateCSVDoc(
		const u32 sizeid,
		const int record_len,
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	aos_assert_r(record_len > 0, false);
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;

	buff->reset();
	if (buff->dataLen() <= 0) return true;

	//for debugging purpose
	//print out the buffer content char by char
	OmnTagFuncInfo << "Start to dump buff content" << endl;
	printChars(buff);

	u64 docid = 0;
	int record_size = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		aos_assert_r(sizeid == AosGetSizeIdByDocid(docid), false);

		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			aos_assert_r(snap_ids.count(vid) != 0, false);
			doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eUpdate, vid, snap_ids[vid], task_docid);
			docInfos[vid] = doc_info;
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);
		
		record_size = buff->getInt(0);
		aos_assert_r(record_size, false);

		int64_t offset = buff->getCrtIdx();
		
		bool rslt = doc_info->updateDoc(buff->data() + offset, record_size, docid, rdata);
		aos_assert_r(rslt, false);
		buff->setCrtIdx(offset + record_size);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		doc_info->sendRequestPublic(rdata);
		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}

bool
AosStorageEngineMgr::batchDeleteDoc(
		const AosBuffPtr &buff,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;

	buff->reset();
	if (buff->dataLen() <= 0) return true;

	u64 docid = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			AosDataRecordObjPtr record = AosGetDataRecordByDocid(rdata->getSiteid(), docid, rdata);
			AosDataRecordType::E type = record->getType();
			if (type == AosDataRecordType::eCSV)
			{
				doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eBatchDelete, vid, 0, task_docid);
			}
			else if (type == AosDataRecordType::eFixedBinary)
			{
				doc_info = OmnNew AosSengineDocInfoFixed(AosGroupDocOpr::eBatchDelete, vid, 0, task_docid);
			}
			docInfos[vid] = doc_info;
			int record_len = AosGetDataRecordLenByDocid(rdata->getSiteid(), docid, rdata);
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);
		bool rslt = doc_info->deleteDoc(docid, rdata);
		aos_assert_r(rslt, false);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		doc_info->sendRequestPublic(rdata);
		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}


//Jozhi 2015/01/23
/*
bool
AosStorageEngineMgr::deleteCSVDoc(
		const AosBuffPtr &buff,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;

	buff->reset();
	if (buff->dataLen() <= 0) return true;

	u64 docid = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);

		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eDelete, vid, 0, task_docid);
			docInfos[vid] = doc_info;
			int record_len = AosGetDataRecordLenByDocid(rdata->getSiteid(), docid, rdata);
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);
		bool rslt = doc_info->deleteDoc(docid, rdata);
		aos_assert_r(rslt, false);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		doc_info->sendRequestPublic(rdata);
		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}
*/

bool
AosStorageEngineMgr::insertDoc(
		const AosBuffPtr &buff,
		const u64 snap_id,
		const AosRundataPtr &rdata)
{
	OmnAlarm << "NotImplementedYet" << enderr;
	return true;
}

bool
AosStorageEngineMgr::batchInsertDocCommon(
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	//buff format
	//docid, doc_length, schema_docid, doc
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;
	buff->reset();
	if (buff->dataLen() <= 0) return true;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{

		u64 docid = buff->getU64(0);
		aos_assert_r(docid, false);
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			aos_assert_r(snap_ids.count(vid) != 0, false);
			doc_info = OmnNew AosSengineDocInfoCommon(AosGroupDocOpr::eBatchInsert, vid, snap_ids[vid], task_docid);
			docInfos[vid] = doc_info;
			int record_len = AosGetDataRecordLenByDocid(rdata->getSiteid(), docid, rdata);
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);

		int doc_len = buff->getInt(-1);
		u64 schema_docid = buff->getU64(0);
		bool rslt = doc_info->addDoc(docid, doc_len, schema_docid, buff->data() + buff->getCrtIdx(), rdata);
		aos_assert_r(rslt, false);
		buff->setCrtIdx(buff->getCrtIdx() + doc_len);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		bool rslt = doc_info->sendRequestPublic(rdata);
		if (!rslt) return false;
	}
	return true;

}

bool
AosStorageEngineMgr::batchInsertDoc(
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;
	buff->reset();
	if (buff->dataLen() <= 0) return true;
	u64 docid = 0;
	int record_size = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			AosDataRecordObjPtr record = AosGetDataRecordByDocid(rdata->getSiteid(), docid, rdata);
			AosDataRecordType::E type = record->getType();
			aos_assert_r(snap_ids.count(vid) != 0, false);
			if (type == AosDataRecordType::eCSV)
			{
				doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eBatchInsert, vid, snap_ids[vid], task_docid);
			}
			else if (type == AosDataRecordType::eFixedBinary)
			{
				doc_info = OmnNew AosSengineDocInfoFixed(AosGroupDocOpr::eBatchInsert, vid, snap_ids[vid], task_docid);
			}
			docInfos[vid] = doc_info;
			int record_len = AosGetDataRecordLenByDocid(rdata->getSiteid(), docid, rdata);
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);
		
		record_size = buff->getInt(0);
		aos_assert_r(record_size, false);

		int64_t offset = buff->getCrtIdx();
		
		bool rslt = doc_info->addDoc(buff->data() + offset, record_size, docid, rdata);
		aos_assert_r(rslt, false);
		buff->setCrtIdx(offset + record_size);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		bool rslt = doc_info->sendRequestPublic(rdata);
		if (!rslt) return false;
		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}


//Jozhi 2015/01/23
//change to call the batchInsertDoc()
/*
bool
AosStorageEngineMgr::saveCSVDoc(
		const u32 sizeid,
		const int record_len,
		const AosBuffPtr &buff,
		map<int, u64> &snap_ids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record_len > 0, false);
	u32 vid;
	map<u32, AosSengineDocInfoPtr> docInfos;
	AosSengineDocInfoPtr doc_info;

	buff->reset();
	if (buff->dataLen() <= 0) return true;

	//for debugging purpose
	//print out the buffer content char by char
	OmnTagFuncInfo << "Start to dump buff content" << endl;
	printChars(buff);

	u64 docid = 0;
	int record_size = 0;
	while (buff->dataLen() > 0 && buff->getCrtIdx() < buff->dataLen())
	{
		docid = buff->getU64(0);
		aos_assert_r(docid, false);
		aos_assert_r(sizeid == AosGetSizeIdByDocid(docid), false);

		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		doc_info = docInfos[vid];
		if (!doc_info)
		{
			aos_assert_r(snap_ids.count(vid) != 0, false);
			doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eBatchInsert, vid, snap_ids[vid], task_docid);
			docInfos[vid] = doc_info;
			doc_info->createMemory(eMaxBuffSize, record_len);
		}
		aos_assert_r(doc_info, false);
		
		record_size = buff->getInt(0);
		aos_assert_r(record_size, false);

		int64_t offset = buff->getCrtIdx();
		
		bool rslt = doc_info->addDoc(buff->data() + offset, record_size, docid, rdata);
		aos_assert_r(rslt, false);
		buff->setCrtIdx(offset + record_size);
	}

	vector<u32> total_vids = AosGetTotalCubeIds();		
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		vid = total_vids[i];
		doc_info = docInfos[vid];
		if(!doc_info) continue;

		bool rslt = doc_info->sendRequestPublic(rdata);
		if (!rslt) return false;
		aos_assert_r(doc_info->getNumDocs() == 0, false);
	}
	return true;
}
*/


void
AosStorageEngineMgr::flushContents()
{
	mTLock->lock();
	if (!mThread[0]) 
	{
		mTLock->unlock();
		return;
	}
	for (u32 idx = 0; idx < mNumThreads; idx++)
	{
		//OmnScreen << " wait wait wait clean StorageEngine Cache logicid:" << idx << ";" << endl;
		while(1)
		{
			OmnSleep(1);
			
			mLock[idx]->lock();
			if (mProcData[idx].empty() &&  mFlush[idx])
			{
				mLock[idx]->unlock();
				break;
			}
			mLock[idx]->unlock();
		}
	}
	mTLock->unlock();
}

void
AosStorageEngineMgr::printChars(const AosBuffPtr &buff)
{
	char ch;

	//only print out 500 chars for now
	for (u32 i = 0; i < 500; i++)
	{
		ch = buff->getChar('\0');
		cout << ch;
	}
	
	cout << endl;
	cout.flush();
	buff->reset();
}
