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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/ReadDocUtil/MultiFixedDocOpr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "DocTrans/BatchGetMultiFixedDocAsyncTrans.h"

AosMultiFixedDocOpr::AosMultiFixedDocOpr()
{
}


AosMultiFixedDocOpr::~AosMultiFixedDocOpr()
{
}


bool
AosMultiFixedDocOpr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	return true;
}


bool
AosMultiFixedDocOpr::shufferDocids(
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
AosMultiFixedDocOpr::shufferDocids(
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
AosMultiFixedDocOpr::pushDocidToDocidGrp(
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
AosMultiFixedDocOpr::sendReadDocTrans(
		const AosRundataPtr &rdata,
		const u64 reqid,
		const u32 vid,
		IdVector &docids,
		const AosAsyncRespCallerPtr &resp_caller)
{
	aos_assert_r(docids.size() > 0, false);
	vector<int> record_lens;
	map<u32, int> sizeid_lens;
	u32 size_id;
	int record_len;
	for (u32 i = 0; i < docids.size(); i++)
	{
		size_id = AosGetSizeIdByDocid(docids[i]);
		if (sizeid_lens.find(size_id) == sizeid_lens.end())
		{
			record_len = AosGetDataRecordLenBySizeId(rdata->getSiteid(), size_id, rdata.getPtr());
			sizeid_lens[size_id] = record_len;
			record_lens.push_back(record_len);
		}
		else
		{
			record_lens.push_back(sizeid_lens[size_id]);
		}
	}
	aos_assert_r(record_lens.size() == docids.size(), false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBatchGetMultiFixedDocAsyncTrans(reqid, vid, docids, record_lens, snap_id, resp_caller);
	AosSendTransAsyncResp(rdata, trans);
	return true;
}


bool
AosMultiFixedDocOpr::getNextDocidResp(
		const AosBuffPtr &big_buff,
		bool &finished,
		u64 &docid,
		AosBuffPtr &docid_resp)
{
	// big_buff format.
	// 0.	resp_len				u32
	// 1.	docid					u64
	// 2.	key_str_len				int
	// 3.	key_str					key_str_len
	// 4.	time_cond_value_len		int
	// 5.	time_cond_value			time_cond_value_len
	// 6.	.... (repeat 4 and 5)
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

