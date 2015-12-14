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
#include "DataStructs/ShuffleStatId.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
//#include "DocClient/DocidMgr.h"
#include "DataStructs/DataStructsUtil.h"
#include "DataStructs/StatIdMgr.h"
#include "DataStructs/DataStructsUtil.h"

AosShuffleStatId::AosShuffleStatId(
		const AosXmlTagPtr &control_doc,
		const AosBuffPtr &buff)
:
mCtrlBuff(buff),
mCtrlDoc(0),
mDocsPerDistBlock(0)
{
	u64 docs_per_dist_block = control_doc->getAttrU64("zky_doc_per_distblock", 0);
	aos_assert(docs_per_dist_block);

	mCtrlDoc = control_doc->clone(AosMemoryCheckerArgsBegin);
	mStatIdMgr = OmnNew AosStatIdMgr(mCtrlBuff, docs_per_dist_block);
	mDocsPerDistBlock = docs_per_dist_block;
}


AosShuffleStatId::~AosShuffleStatId()
{
}

int
AosShuffleStatId::getPhysicalIdByStatId(
		const AosRundataPtr &rdata, 
		const u64 &stat_id)
{
	int vid;
	u64 docid = getDocid(rdata, stat_id, vid);
	aos_assert_r(docid, -1);

	int pid = AosGetPhysicalId(docid);
	return pid;
}

u32
AosShuffleStatId::getVirtualIdByStatId(
		const AosRundataPtr &rdata, 
		const u64 &stat_id)
{
	int vid;
	u64 docid = getDocid(rdata, stat_id, vid);
	aos_assert_r(docid, 0);
	return vid;
}


u64 
AosShuffleStatId::getDocid(
		const AosRundataPtr &rdata,
		const u64 &stat_id,
		int &vid)
{
	vid = AosDataStructsUtil::getVirtualIdByStatid(stat_id, mDocsPerDistBlock);
	u64 docid = mStatIdMgr->getEntryByStatId(stat_id);
	if (docid == 0)
	{
		//OmnString objid;
		//docid = AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
		docid = AosDataStructsUtil::nextSysDocid(rdata, vid);
		bool rslt = mStatIdMgr->addEntryByStatId(stat_id, docid); 
		aos_assert_r(rslt, 0);
	}
	
	int vvid = AosGetCubeId(docid);
	aos_assert_r(vvid == vid, 0);
	return docid;
}


u64
AosShuffleStatId::getBinaryDocid(const u64 &stat_id)
{
	return mStatIdMgr->getEntryByStatId(stat_id);
}

/*
bool
AosShuffleStatId::saveControlDoc(
		const AosRundataPtr &rdata,
		map<int, u64> &bsnap_ids)
{
	if (!mIsDirty) return true;
	mIsDirty = false;

	aos_assert_r(mCtrlDoc && mCtrlDoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, false);
	//AosModifyBinaryDoc(mCtrlDoc, mCtrlBuff, rdata);
	
	u64 snap_id = 0;
	if (bsnap_ids.size() > 0)
	{
		int vid = AosGetCubeId(mCtrlDoc->getAttrU64(AOSTAG_DOCID, 0));
		aos_assert_r(bsnap_ids.count(vid) != 0, false);
		snap_id = bsnap_ids[vid];
	}

	AosDocClientObj::getDocClient()->modifyBinaryDocByStat(mCtrlDoc, mCtrlBuff, 0, snap_id, rdata);
	return true;
}
*/
