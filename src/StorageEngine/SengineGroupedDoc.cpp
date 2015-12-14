////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SengineGroupedDoc.h"

#include "API/AosApiG.h"
#include "DfmUtil/DfmGroupedDoc.h"
#include "DocServer/DocSvr.h"
#include "HadoopUtil/HadoopApi.h" 
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageEngine/SengineFixedDocReq.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util1/MemMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

// static AosDfmConfig sgSengDfmConf(AosDfmDocType::eGroupedDoc, AOS_DFM_BODYENGINE_SLAB,
// 		"__zkd_0001", AosModuleId::eInvalid, true, 1, "gzip", false);
static AosDfmConfig sgSengDfmConf(AosDfmDocType::eGroupedDoc, 
		"__zkd_0001", AosModuleId::eGroupDoc, true, 1, "gzip", false);


AosSengineGroupedDoc::AosSengineGroupedDoc(const bool regflag)
:
AosStorageEngineObj(AosDocType::eGroupedDoc, regflag),
mThread(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(0),
mFlush(true)
{
}


AosSengineGroupedDoc::~AosSengineGroupedDoc()
{
}


bool
AosSengineGroupedDoc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mQueue.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		AosSengineDocReqObjPtr req = mQueue.front();
		mQueue.pop();
		mFlush = false;
		mLock->unlock();

		aos_assert_r(mRundata, false);
		AosDocFileMgrObjPtr doc_mgr = AosDocSvrObj::getDocSvr()->getGroupedDocFileMgr(req->mVid, true, mRundata);
		if (!doc_mgr)
		{
			AosSetErrorU(mRundata, "no_file_mgr") << ": " << req->mVid;
			OmnAlarm << mRundata->getErrmsg() << enderr;
			continue;
		}

		bool rslt = true;
		switch(req->mType)
		{
		case AosSengineDocReqObj::eBatchInsertCommonDoc:
			 rslt = batchInsertCommonDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata);
			 break;
		case AosSengineDocReqObj::eFixedDoc:
			 rslt = saveBatchFixedDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata); 
			 break;

		case AosSengineDocReqObj::eCSVDoc:
			 rslt = batchInsertCSVDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata); 
			 break;

		case AosSengineDocReqObj::eDeleteCSVDoc:
			 rslt = batchDeleteCSVDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata);
			 break;

		case AosSengineDocReqObj::eUpdateCSVDoc:
			 rslt = batchUpdateCSVDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata);
			 break;
		case AosSengineDocReqObj::eBatchUpdateCommonDoc:
			 rslt = batchUpdateCommonDocPriv(req, doc_mgr, AosTransId::Invalid, mRundata);
			 break;

		default:
			 rslt = false;
			 break;
		}

		if (!rslt)
		{
			OmnAlarm << "save failed!!!" << enderr;
		}

		// send msg to client
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(rslt);
		(req->mTrans)->sendResp(resp_buff);

		mLock->lock();
		mFlush = true;
		mLock->unlock();
	}
	return true;
}


bool
AosSengineGroupedDoc::signal(const int threadLogicId)
{
	return true;
}


bool
AosSengineGroupedDoc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


AosDfmDocPtr
AosSengineGroupedDoc::readDocPriv(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const u64 &groupid,
		const bool read_body)
{
	if (snap_id == 0)
	{
		return docfilemgr->readDoc(rdata, groupid, read_body);
	}
	return docfilemgr->readDoc(rdata, snap_id, groupid, read_body);
}


bool
AosSengineGroupedDoc::saveDocPriv(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const u64 &snap_id,
		const AosDfmDocPtr &p_doc)
{
	if (snap_id == 0)
	{
		return docfilemgr->saveDoc(rdata, trans_id, p_doc);
	}

	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);
	return docfilemgr->saveDoc(rdata, snap_id, p_doc, trans_ids);
}



AosXmlTagPtr 
AosSengineGroupedDoc::serializeToXmlDoc(
		const AosXmlTagPtr &xml,
		const AosDataRecordObjPtr rcd,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, 0);

	u64 docid = xml->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, 0);

	u32 siteid = rdata->getSiteid();

	AosDataRecordObjPtr record = rcd;
	if (!record)
	{
		record = AosGetDataRecordByDocid(siteid, docid, rdata);
	}
	if (!record)
	{
		// Did not find the data record. This is an error.
		AosSetErrorU(rdata, "no_data_record_001") << ": " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnString str = xml->getNodeText();
	aos_assert_r(str.length() > 0, 0);

	// u64 start_time = OmnGetTimestamp();

	// JACKIE-HADOOP
	/*
	AosXmlTagPtr record_doc = record->getRecordDoc();
	if (record_doc && record_doc->getAttrStr("data_source") == "hadoop")
	{
		OmnString ip = record_doc->getAttrStr("ip", ""); 
		int port = record_doc->getAttrInt("port", 0); 
		OmnString filename = record_doc->getAttrStr("hadoopfile", ""); 
		int buffsize = record_doc->getAttrInt("buffsize", 0); 
		const char *data = str.data();
		// u16 file_id = *(u16*)data;
		int64_t offset = *(int64_t*)&data[2];
		u32 length = *(u32*)&data[10];
		AosBuffPtr buff = OmnNew AosBuff(length AosMemoryCheckerArgs);
		hdfsFS fs = hdfsConnect(ip.data(), port); 
		if (!fs) {                                                            
			AosSetErrorU(rdata, "Failed to connect to hdfs!\n : ") << enderr;
			return 0;
		}
		hdfsFile file = hdfsOpenFile(fs, filename.data(), O_RDONLY, buffsize, 0, 0);
		if (!file) {                                                          
			AosSetErrorU(rdata, "Failed to open this file : ") << filename << enderr;
			return 0;
		}
		char * block = OmnNew char[length];
		int num = AosReadHadoopFile(rdata, block, fs, file, offset, buffsize);
		aos_assert_r(num>0, 0);
		buff->setData1(block, num, true);
		OmnString tmp(buff->data(), buff->dataLen());
		str = tmp;
	}
	*/

	AosXmlTagPtr doc = record->serializeToXmlDoc(str.data(), str.length(), rdata.getPtr());
	// u64 cost = OmnGetTimestamp() - start_time;
	aos_assert_r(doc, 0);
		
	//OmnScreen << "parse FixedLenDoc To Xml, time cost:" << AosTimestampToHumanRead(cost) << endl;
	
	record->clear();
	
	doc->setAttr(AOSTAG_DOCID, docid);
	doc->setAttr(AOSTAG_SITEID, siteid);

	return doc;
}		


bool
AosSengineGroupedDoc::deleteGroupDocs(
		const u32 &groupid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	// version		u32
	// siteid		u32
	// groupid		u32
	// seqno		u32
	// offset		u64
	// blocksize	u32	
	// compsize		u32

	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, groupid, false);
	if(!p_doc)	return false;

	bool rslt = docfilemgr->deleteDoc(rdata, trans_id, p_doc);
	aos_assert_r(rslt, false);
	return true;
}


AosDocFileMgrObjPtr
AosSengineGroupedDoc::retrieveDocFileMgrByKey(
		const u32   virtual_id,
		const OmnString dfm_key,
		const bool create_flag,
		const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	AosDocFileMgrObjPtr doc_mgr = vfsMgr->retrieveDocFileMgrByKey(
		rdata.getPtr(), virtual_id, dfm_key, create_flag, sgSengDfmConf);
	return doc_mgr;
}


bool
AosSengineGroupedDoc::addReq(
		const AosSengineDocReqObjPtr &req,
		const AosRundataPtr &rdata)
{
	aos_assert_r(req, false);

	mLock->lock();
	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false);
		mThread= OmnNew OmnThread(thisptr, "SengineGroupedDoc", 0, false, true, __FILE__, __LINE__);
		mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		mThread->start();
	}

	mQueue.push(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


void
AosSengineGroupedDoc::flushContents()
{
	if (!mThread)  return;

	//OmnScreen << " wait wait wait clean StorageEngine Cache ;" << endl;
	while(1)
	{
		OmnSleep(1);

		mLock->lock();
		if (mQueue.empty() && mFlush)
		{
			mLock->unlock();
			return ;
		}
		mLock->unlock();
	}
}


bool
AosSengineGroupedDoc::sanityCheck(
		const u32 &offset, 
		const char *data, 
		const int &len, 
		const int &record_size)
{
	aos_assert_r(len > 0, false);
	char *cmp_data;
	AosBuffPtr doc_buff = OmnNew AosBuff(record_size AosMemoryCheckerArgs);
	memset(doc_buff->data(), 0, record_size);
	cmp_data = (char*)doc_buff->data();

	u64 check_offset = offset;
	while(check_offset < (u32)len)
	{
		aos_assert_r(memcmp(&data[check_offset], cmp_data, record_size) == 0, false);
		check_offset += record_size;
	}	
	return true;
}

//eSizeIdHeaderSize
// Each sizeid has: 2^28 = 268 435 456 docs. 
// Each group has 4000 docs
// There are 67108 groups
// There are 140 virtuals
// Each virtual has 480 groups
// Head size is 29
// The size of headers for one sizeid is 480*29 = 13920
//
//maxdocsize             4M          group size
//1k                    4000        numdocs per group
//268 435 456/4000= 67108*29 = 1946132 / 50 =  39K ==> 600K
//
