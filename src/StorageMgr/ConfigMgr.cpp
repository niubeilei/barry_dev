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
// 09/17/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/ConfigMgr.h"

#include "API/AosApiI.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DfmUtil/Ptrs.h"
#include "FmtMgr/Ptrs.h"
#include "FmtMgr/Fmt.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/SystemId.h"
#include "StorageMgr/VirtualFileSys.h"
#include "Util/TransId.h"
#include "XmlUtil/SeXmlParser.h"

//AosDfmDocNormPtr AosConfigMgr::smDfmDoc = OmnNew AosDfmDocNorm(0);

AosConfigMgr::AosConfigMgr(const AosVirtualFileSysPtr &vfs)
:
mVfs(vfs)
{
}


AosConfigMgr::~AosConfigMgr()
{
}


bool
AosConfigMgr::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);
	// AosDevLocationPtr location = OmnNew AosDevLocation1();

	//AosDfmProcPtr thisptr(this, false);
	//mDocFileMgr = mVfs->retrieveDocFileMgrById(AOS_DFMID_CONFIGMGR, thisptr, rdata);
	//AosDfmConfig conf(AosDfmDocType::eNormal, AOS_DFM_BODYENGINE_SLAB, "Config",
	//		AosModuleId::eInvalid, false);
	AosDfmConfig conf(AosDfmDocType::eNormal, "Config", AosModuleId::eConfigMgr, false);
	
	// config mgr not need snapshotMgr. (snapshot_mgr will init files).
	conf.mNeedSnapshotMgr = false;

	mDocFileMgr = mVfs->retrieveDocFileMgrById(rdata.getPtr(), AOS_DFMID_CONFIGMGR, conf);
	if(!mDocFileMgr)
	{
		//mDocFileMgr = mVfs->createDocFileMgrById(AOS_DFMID_CONFIGMGR, thisptr, rdata);
		mDocFileMgr = mVfs->createDocFileMgrById(rdata.getPtr(), AOS_DFMID_CONFIGMGR, conf);
	}

	return true;	
}


bool
AosConfigMgr::stop()
{
	return true;
}


AosXmlTagPtr
AosConfigMgr::getDoc(const u64 &docid, AosRundata *rdata)
{
	//AosDfmDocPtr dfm_doc = smDfmDoc->clone(docid);
	//bool rslt = mDocFileMgr->readDoc(rdata, dfm_doc);
	//aos_assert_rr(rslt, rdata, 0);
	
	AosDfmDocPtr dfm_doc = mDocFileMgr->readDoc(rdata, docid); 
	if(!dfm_doc)
	{
		OmnScreen << "can't find the doc. docid:" << docid << endl;
		return 0;
	}

	AosBuffPtr body_buff = dfm_doc->getBodyBuff();
	aos_assert_r(body_buff, 0);
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(body_buff->data(),
			body_buff->dataLen(), "" AosMemoryCheckerArgs);
	return doc;
}


bool
AosConfigMgr::saveDoc(
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		AosRundata *rdata)
{
	aos_assert_r(docid, false);

	//OmnScreen << "ketty config, saveDoc:::"
	//		<<  doc->toString()
	//		<< endl;

	u32 docsize = doc->getDocsize();
	char * doc_data = (char *)doc->getData();
	AosBuffPtr new_body_buff = OmnNew AosBuff(doc_data, docsize, docsize, true AosMemoryCheckerArgs); 

	//AosDfmDocPtr dfm_doc = smDfmDoc->clone(docid);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(docid);
	dfm_doc->setBodyBuff(new_body_buff);
	dfm_doc->setOrigLen(new_body_buff->dataLen());
	
	//Linda, 2013/08/22 LINDAFMT
	AosFmtPtr fmt = OmnNew AosFmt();
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	if(AosIsSelfCubeSvr())
	{
		aos_assert_r(fmt_mgr, 0);
		fmt_mgr->setFmt(fmt);
	}

	bool rslt = mDocFileMgr->saveDoc(rdata, AosTransId::Invalid, dfm_doc);
	aos_assert_r(rslt, false);

	if(AosIsSelfCubeSvr())
	{
		fmt_mgr->addToGlobalFmt(rdata, fmt);
		fmt_mgr->setFmt(0);
	}
	return rslt;
}


