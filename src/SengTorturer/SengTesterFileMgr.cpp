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
// 09/07/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/SengTesterFileMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "DfmUtil/DfmDocNorm.h"
#include "Random/RandomUtil.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DocFileMgr.h"
#include "Util/MemoryChecker.h"
#include "XmlUtil/SeXmlParser.h"



const OmnString AosTestFileMgr::scDir = "./TestData";
const OmnString AosTestFileMgr::scFilename = "thread";


AosTestFileMgr::AosTestFileMgr()
:
mDfmConf(AosDfmDocType::eNormal, "Data", AosModuleId::eDoc)
{
}


AosTestFileMgr::~AosTestFileMgr()
{
}

bool 
AosTestFileMgr::createFileMgr( 
		const AosRundataPtr &rdata,
		const int tid)
{
	// This function saves the doc 'doc' into the corresponding
	// files.
	//1.save the body
	OmnString filename;
	filename<<scFilename<<tid;

	mFileMgr[tid]= AosRetrieveDocFileMgr(rdata, tid, mDfmConf);
	if(!mFileMgr[tid])
	{
		mFileMgr[tid] = AosCreateDocFileMgr(rdata, tid, mDfmConf);
	}
	return true;
}


bool
AosTestFileMgr::createDoc(
		const AosRundataPtr &rdata,
		const int tid,
		const u64 &docid,
		const char *data,
		const int data_len)
{
	if (!mFileMgr[tid]) aos_assert_r(createFileMgr(rdata, tid), false);

	//OmnScreen << "create doc  : " << docid << endl;
	AosBuffPtr doc_buff = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs); 
	doc_buff->setBuff(data, data_len);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(docid); 
	dfm_doc->setBodyBuff(doc_buff);

	vector<AosTransId> v;
	bool result = mFileMgr[tid]->saveDoc(rdata, v, dfm_doc);
	aos_assert_r(result, false);
	return true;
}


AosXmlTagPtr
AosTestFileMgr::readDoc(
		const AosRundataPtr &rdata,
		const int tid,
		const u64 &docid AosMemoryCheckDecl)
{
	if (!mFileMgr[tid]) aos_assert_r(createFileMgr(rdata, tid), 0);

	AosDfmDocPtr dfm_doc = mFileMgr[tid]->readDoc(rdata, docid);
	aos_assert_r(dfm_doc, 0);
	
	AosBuffPtr body_buff = dfm_doc->getBodyBuff();
	aos_assert_r(body_buff, 0);

	AosXmlParser parser;
	AosXmlTagPtr xml_doc = parser.parse(body_buff->data(), body_buff->dataLen(),"" AosMemoryCheckerFileLine);
	return xml_doc;
}


bool 
AosTestFileMgr::modifyDoc(
		const AosRundataPtr &rdata,
		const int tid,
		const u64 &docid,
		const char *data,
		const int data_len)
{
	// This function modifies the doc. It retrieves the
	// doc file manager based on 'tester' and then asks the
	// doc file manager to modify the doc.
	//OmnScreen << " modify doc  : " << docid << endl;
	AosBuffPtr doc_buff = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs); 
	doc_buff->setBuff(data, data_len);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(docid); 
	dfm_doc->setBodyBuff(doc_buff);

	vector<AosTransId> v;
	bool result = mFileMgr[tid]->saveDoc(rdata, v, dfm_doc);
	aos_assert_r(result, false);
	return true;
}


bool 
AosTestFileMgr::removeDoc(
		const AosRundataPtr &rdata,
		const int tid, 
		const u64 &docid)
{
	//OmnScreen << " remove doc  : " << docid << endl;
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(docid); 
	vector<AosTransId> v;
	bool rslt = mFileMgr[tid]->deleteDoc(rdata, v, dfm_doc);    
	aos_assert_r(rslt, false);
	return true;

}


bool
AosTestFileMgr::isDocDeleted(
		const AosRundataPtr &rdata,
		const int tid, 
		const u64 &docid)
{
	//OmnScreen << "is doc delete : " << docid << endl;
	if (!mFileMgr[tid]) aos_assert_r(createFileMgr(rdata, tid),false);
	AosDfmDocPtr dfm_doc = mFileMgr[tid]->readDoc(rdata, docid, false);
	if(!dfm_doc) return true;
	return false;

}


