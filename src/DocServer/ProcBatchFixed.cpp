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
// 03/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcBatchFixed.h"

#include "API/AosApiG.h"
#include "DocServer/DocInfo2.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
//#include "TransUtil/XmlTrans.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/DocTypes.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "StorageEngine/StorageEngineMgr.h"



AosProcBatchFixed::AosProcBatchFixed(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHFIXED, AosDocSvrProcId::eBatchFixed, regflag)
{

}


AosProcBatchFixed::~AosProcBatchFixed()
{
}


bool
AosProcBatchFixed::proc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &request, 
		const AosBigTransPtr &trans)
{
	// The request is in the form:
	// 	<trans AOSTAG_NUM_DOCS="xxx"
	// 		AOSTAG_VIRTUAL_ID="xxx"
	// 		AOSTAG_RECORD_SIZE="xxx"
	// 		<docids><![BDATA[xxx]]></docids>
	// 		<buffs><![BDATA[xxx]]></buffs>
	// 	</trans>
	int num_docs = request->getAttrInt(AOSTAG_NUM_DOCS, -1);
	if (num_docs <= 0)
	{
		AosSetErrorU(rdata, "invalid_num_docs") << ": " << num_docs;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int record_size = request->getAttrInt(AOSTAG_RECORD_SIZE, -1);
	if (record_size < 0)
	{
		AosSetErrorU(rdata, "invalid_record_size") << ": ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 sizeid = request->getAttrU64(AOSTAG_SIZEID, 0);
	if (sizeid <= 0)
	{
		AosSetErrorU(rdata, "invalid_sizeid") << ": " << sizeid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve docids
	AosXmlTagPtr tag = request->getFirstChild("docids");
	aos_assert_rr(tag, rdata, false);	
	AosBuffPtr docid_buff = tag->getNodeTextBinaryUnCopy(AosMemoryCheckerArgsBegin);
	aos_assert_r(docid_buff, false);
	//u64 *docids = (u64*)docid_buff->data();
	
	// Retrieve buffs
	tag = request->getFirstChild("buffs");
	aos_assert_rr(tag, rdata, false);	
	AosBuffPtr buff = tag->getNodeTextBinaryUnCopy(AosMemoryCheckerArgsBegin);
	aos_assert_rr(buff, rdata, false);
	
	u64 buffsize = request->getAttrU64(AOSTAG_BUFF_SIZE, 10000000);
	aos_assert_r(buffsize > 0, false);

	// FixedLength compress
	// Linda 2013/01/14
	u64 job_id = request->getAttrU64(AOSTAG_JOB_DOCID, 0);
	aos_assert_r(job_id, false);
	bool rslt = AosStorageEngineMgr::getSelf()->saveCache(job_id, sizeid, num_docs, docid_buff, buff, record_size, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
	// Linda, 2013/01/26
	/*
	u32 physicalNum = AosGetNumPhysicals();
	u32 virtualNum = AosGetNumCubes();
	u32 vid;
	map<u32, AosDocInfo2Ptr> docInfos;
	AosDocInfo2Ptr doc_info;
	for (vid = AosGetSelfServerId(); vid<virtualNum; vid+=physicalNum)
	{
		doc_info = OmnNew AosDocInfo2(vid);
		docInfos[vid] = doc_info;
		doc_info->createMemory(buffsize, record_size);
	}
	
	bool rslt;
	for(int i=0; i<num_docs; i++)
	{
		vid = AosGetVirtualIDByDocid(docids[i]);
		doc_info = docInfos[vid];
		aos_assert_r(doc_info, false); 
		rslt = doc_info->addDoc(buff->data()+i*record_size, record_size, docids[i], rdata);
		aos_assert_r(rslt, false);
	}
	
	for (vid = AosGetSelfServerId(); vid<virtualNum; vid+=physicalNum)
	{
		doc_info = docInfos[vid];
		aos_assert_r(doc_info, false); 
		doc_info->sendRequestPublic(rdata, 0);
	}
	*/
	
	/*
	AosDocFileMgrObjPtr doc_mgr = 
		AosDocSvr::getSelf()->getDocFileMgrBySizeid(vid, sizeid, true, rdata);
	if (!doc_mgr)
	{
		AosSetErrorU(rdata, "no_file_mgr") << ": " << vid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Need to save the batch docs.
	//bool rslt = doc_mgr->saveBatchFixedDocs(num_docs, docids, buff, record_size, rdata);
	AosDocType::E type  = AosDocType::eFixedLength;
	AosStorageEngineObjPtr engine;
	engine = AosStorageEngineObj::getStorageEngine(type);
	aos_assert_r(engine, false);
	
	//bool rslt = engine->saveBatchFixedDocs(num_docs, docids, buff, record_size, doc_mgr, rdata);
	bool rslt = engine->saveBatchFixedDocs(sizeid, num_docs, docids, buff, record_size, doc_mgr, rdata);
	aos_assert_rr(rslt, rdata, false);
	rdata->setOk();
	return true;
	*/
}


AosDocSvrProcPtr
AosProcBatchFixed::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchFixed(false);
	return proc;
}
# endif
