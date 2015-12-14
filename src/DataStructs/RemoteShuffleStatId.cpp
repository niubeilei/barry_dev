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
// 04/04/2014	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/RemoteShuffleStatId.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocTrans/AddStatBinaryDocidTrans.h"
#include "DocTrans/GetStatBinaryDocidTrans.h"
#include "DataStructs/DataStructsUtil.h"

AosRemoteShuffleStatId::AosRemoteShuffleStatId(const AosXmlTagPtr &control_doc)
:
mDocsPerDistBlock(0),
mCtrlDocid(0),
mCrtIdx(-1),
mCrtDocid(0)
{
	mCtrlDocid = control_doc->getAttrU64(AOSTAG_DOCID, 0);
	mDocsPerDistBlock = control_doc->getAttrU64("zky_doc_per_distblock", 0);
}


AosRemoteShuffleStatId::~AosRemoteShuffleStatId()
{
}


int
AosRemoteShuffleStatId::getPhysicalIdByStatId(
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
AosRemoteShuffleStatId::getVirtualIdByStatId(
		const AosRundataPtr &rdata, 
		const u64 &stat_id)
{
	int vid;
	u64 docid = getDocid(rdata, stat_id, vid);
	aos_assert_r(docid, 0);
	return vid;
}


u64 
AosRemoteShuffleStatId::getDocid(
		const AosRundataPtr &rdata,
		const u64 &stat_id,
		int &vid)
{
	vid = AosDataStructsUtil::getVirtualIdByStatid(stat_id, mDocsPerDistBlock);
	u64 docid = getDocidPriv(rdata, vid, stat_id);
	aos_assert_r(docid, 0);

	int vvid = AosGetCubeId(docid);
	aos_assert_r(vvid == vid, 0);
	return docid;
}


u64 
AosRemoteShuffleStatId::getStatBinaryDocidPriv(const AosRundataPtr &rdata, const u64 &stat_id)
{
	u64 snap_id = 0;
	if (mSnapIds.size() > 0)
	{
		int vid = AosGetCubeId(mCtrlDocid);
		aos_assert_r(mSnapIds.count(vid) != 0, false);
		snap_id = mSnapIds[vid];
	}

	AosTransPtr trans = OmnNew AosGetStatBinaryDocidTrans(mCtrlDocid, stat_id, false, false);
	AosBuffPtr resp;
	bool timeout;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_rr(rslt, rdata, false);

	u64 docid = resp->getU64(0);
	return docid;
}


bool
AosRemoteShuffleStatId::addStatBinaryDocidPriv(
		const AosRundataPtr &rdata, 
		const u64 &stat_id, 
		u64 &binary_docid)
{
	u64 snap_id = 0;
	if (mSnapIds.size() > 0)
	{
		int vid = AosGetCubeId(mCtrlDocid);
		aos_assert_r(mSnapIds.count(vid) != 0, false);
		snap_id = mSnapIds[vid];
	}

	AosTransPtr trans = OmnNew AosAddStatBinaryDocidTrans(mCtrlDocid, stat_id, binary_docid, false, false);
	bool timeout;
	AosBuffPtr resp;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_rr(rslt, rdata, false);
	
	bool id_changed = resp->getU8(0);
	if (id_changed)
	{
		u64 old_id = binary_docid;
		binary_docid = resp->getU64(0);
		aos_assert_r(binary_docid, false);
		OmnScreen << "AosRemoteShuffleStatId changed:" << old_id << "->" << binary_docid << ";"<< endl;
	}
	return true;
}


u64 
AosRemoteShuffleStatId::getDocidPriv(
		const AosRundataPtr &rdata,
		const int vid,
		const u64 &stat_id)
{
	aos_assert_r(vid >= 0, 0);
	aos_assert_r(mDocsPerDistBlock, false);

	u32 idx = stat_id / mDocsPerDistBlock;
	if (mCrtIdx == -1 || idx != mCrtIdx)
	{
		mCrtDocid = getStatBinaryDocidPriv(rdata, stat_id);
		if (mCrtDocid == 0)
		{
			u64 new_docid = AosDataStructsUtil::nextSysDocid(rdata, vid);
			aos_assert_r(AosGetCubeId(new_docid) == vid, 0);

			addStatBinaryDocidPriv(rdata, stat_id, new_docid);
			mCrtDocid = new_docid;
		}
		mCrtIdx = idx;
	}
	
	aos_assert_r(mCrtDocid, 0);
	aos_assert_r(mCrtIdx >= 0, 0);
	return mCrtDocid;
}


void
AosRemoteShuffleStatId::setSnapShots(map<int, u64> &bsnap_ids)
{
	mSnapIds = bsnap_ids;
}


