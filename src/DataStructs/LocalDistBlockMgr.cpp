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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/LocalDistBlockMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "DataStructs/StatIdMgr.h"

AosLocalDistBlockMgr::AosLocalDistBlockMgr(
		const AosXmlTagPtr &control_doc, 
		const AosBuffPtr &buff)
{
	u64 docs_per_distblock = control_doc->getAttrU64("zky_doc_per_distblock", 0);
	aos_assert(docs_per_distblock);
	AosBuffPtr ctrl_buff = buff;
	if (!buff)
	{
		ctrl_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		ctrl_buff->setInt64(0);
	}
	mStatIdMgr = OmnNew AosStatIdMgr(ctrl_buff, docs_per_distblock);
}


AosLocalDistBlockMgr::~AosLocalDistBlockMgr()
{
}

bool
AosLocalDistBlockMgr::getStatDocByDocid(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &dist_block_docid,
		const u64 &stat_docid,
		char *&stat_doc,
		int64_t &stat_doc_len)
{
	bool rslt = getDistBlockByDocid(rdata, dist_block, dist_block_docid, stat_docid);
	aos_assert_r(rslt, false);

	stat_doc = dist_block.getStatDoc(stat_docid);
	stat_doc_len = dist_block.getStatDocSize();
	aos_assert_r(stat_doc_len > 0, false);
	return true;
}


bool
AosLocalDistBlockMgr::getDistBlockByDocid(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &dist_block_docid,
		const u64 &stat_docid)
{
	u32 idx = stat_docid / dist_block.getDocsPerDistBlock();
	if (dist_block.getGroupId() == idx && dist_block.getDocid() == dist_block_docid) return true;

	AosBuffPtr buff = retrieveBinaryDoc(dist_block_docid, rdata);
	aos_assert_r(buff && buff->dataLen() > 0, false);

	dist_block.set(buff, dist_block_docid, idx, false);
	return true;
}


u64
AosLocalDistBlockMgr::getEntryByStatId(const u64 &stat_id)
{
	return mStatIdMgr->getEntryByStatId(stat_id);
}

u64 gKettyTime1 = 0;
u64 gKettyNumLocals = 0;

AosBuffPtr
AosLocalDistBlockMgr::retrieveBinaryDoc(
		const u64 &did,
		const AosRundataPtr &rdata)
{
u64 tt1 = OmnGetTimestamp();
	AosBuffPtr buff;
	AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(did, rdata);	
	aos_assert_r(doc, 0);
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r(buff->dataLen() > 0, 0);
u64 tt2 = OmnGetTimestamp();
gKettyTime1 += tt2-tt1;
gKettyNumLocals++;
	return buff;
}

void
AosLocalDistBlockMgr::KettyInitPrintTime()
{
	gKettyTime1 = 0;
	gKettyNumLocals = 0;
}

void
AosLocalDistBlockMgr::KettyPrintTime()
{
	OmnScreen << " DistBlockMgr Times: "
		<< "gKettyTime1= " << gKettyTime1 << "; " 
		<< "gKettyNumLocals= " << gKettyNumLocals << "; " 
		<< endl;
}

