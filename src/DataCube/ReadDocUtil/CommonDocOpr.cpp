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
// 2014/05/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataCube/ReadDocUtil/CommonDocOpr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "DocTrans/BatchGetCommonDocAsyncTrans.h"

AosCommonDocOpr::AosCommonDocOpr()
:
mRecordLen(-1)
{
}


AosCommonDocOpr::~AosCommonDocOpr()
{
}


bool
AosCommonDocOpr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	return true;
}


bool
AosCommonDocOpr::shufferDocids(
		const AosRundataPtr &rdata,
		const AosBuffPtr &docids_buff,
		const u32 docid_num,
		map<u32, IdVector> &docid_grp)
{
	u64 docid;
	u32 remain = docid_num;
	while(remain--)
	{
		docid = docids_buff->getU64(0);	
		aos_assert_r(docid, false);
		
		pushDocidToDocidGrp(docid, docid_grp);
	}
	
	return true;
}

bool
AosCommonDocOpr::shufferDocids(
		const AosRundataPtr &rdata,
		IdVector &docids, 
		map<u32, IdVector> &docid_grp)
{
	u64 docid;
	for(u32 i=0; i<docids.size(); i++)
	{
		docid = docids[i];
		aos_assert_r(docid, false);
		
		pushDocidToDocidGrp(docid, docid_grp);
	}
	return true;
}


bool
AosCommonDocOpr::pushDocidToDocidGrp(
		const u64 docid,
		map<u32, IdVector> &docid_grp)
{
	u32 vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));

	map<u32, IdVector>::iterator itr = docid_grp.find(vid);
	if(itr == docid_grp.end())
	{
		IdVector id_v;
		pair<map<u32, IdVector>::iterator, bool> itr_rslt;
		itr_rslt = docid_grp.insert(make_pair(vid, id_v));
		aos_assert_r(itr_rslt.second, false);
		itr = itr_rslt.first;
	}
	
	aos_assert_r(itr != docid_grp.end(), false);
	(itr->second).push_back(docid);
	return true;
}


bool
AosCommonDocOpr::sendReadDocTrans(
		const AosRundataPtr &rdata,
		const u64 reqid,
		const u32 vid,
		IdVector &docids,
		const AosAsyncRespCallerPtr &resp_caller)
{
	aos_assert_r(docids.size() > 0, false);
	if (mRecordLen == -1)
	{
		//AosDataRecordObjPtr record = AosGetDataRecordByDocid(rdata->getSiteid(), docids[0], rdata);
		//aos_assert_r(record, false);

		//mRecordLen = record->getEstimateRecordLen();
		mRecordLen = AosGetDataRecordLenByDocid(rdata->getSiteid(), 0, rdata);
		aos_assert_r(mRecordLen > 0, false);
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBatchGetCommonDocAsyncTrans(reqid, vid, docids, mRecordLen, snap_id, resp_caller);
	AosSendTransAsyncResp(rdata, trans);
	return true;
}


bool
AosCommonDocOpr::getNextDocidResp(
		const AosBuffPtr &big_buff,
		bool &finished,
		u64 &docid,
		AosBuffPtr &docid_resp)
{
	//buff->setU64(docids[i]);
	//buff->setU32(len);
	//buff->setU64(schema_docid);
	//buff->setBuff(doc, len);
	aos_assert_r(big_buff, false);
	if(big_buff->getCrtIdx() >= big_buff->dataLen())
	{
		finished = true;
		return true;
	}

	finished = false;

	docid = big_buff->getU64(0);
	aos_assert_r(docid != 0, false);
	
	u32 doc_len = big_buff->getU32(0);
	aos_assert_r(doc_len != 0, false);

	u64 schema_docid = big_buff->getU64(0);
	aos_assert_r(schema_docid != 0, false);
/*
{
big_buff->getCrtIdx();
char* doc = (big_buff->data())+ big_buff->getCrtIdx();
OmnString tmp(doc, doc_len);
OmnScreen << "debug docid: " << docid << " len: " << doc_len << " schema_docid: " << schema_docid << " doc: " << tmp << endl;
}
*/

	big_buff->backU64();
	big_buff->backU32();
	docid_resp = big_buff->getBuff(doc_len + sizeof(u32) + sizeof(u64), true AosMemoryCheckerArgs);
	return true;
}

