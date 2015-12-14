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
#include "DataStructs/RemoteDistBlockMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "DataStructs/StatIdMgr.h"
#include "DataStructs/DataStructsUtil.h"

AosRemoteDistBlockMgr::AosRemoteDistBlockMgr(
		const AosXmlTagPtr &control_doc,
		const AosBuffPtr &buff,
		const bool use_snapshot)
:
mDocsPerDistBlock(0),
mUseSnapShot(use_snapshot)
{
	mDocsPerDistBlock = control_doc->getAttrU64("zky_doc_per_distblock", 0);
	if (mDocsPerDistBlock == 0)
	{
		OmnAlarm << "XXXXXXXXXXx" << enderr;
	}
	mStatIdMgr = OmnNew AosStatIdMgr(buff, mDocsPerDistBlock);
}


AosRemoteDistBlockMgr::~AosRemoteDistBlockMgr()
{
}

bool
AosRemoteDistBlockMgr::getStatDoc(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &stat_docid,
		const bool create,
		char *&stat_doc,
		int64_t &stat_doc_len)
{
	bool rslt = getDistBlock(rdata, dist_block, stat_docid, true);
	aos_assert_r(rslt, false);

	stat_doc = dist_block.getStatDoc(stat_docid);
	stat_doc_len = dist_block.getStatDocSize();
	aos_assert_r(stat_doc_len > 0, false);
	return true;
}


bool
AosRemoteDistBlockMgr::getDistBlock(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &stat_docid,
		const bool create)
{
	int docs_per_distblock = dist_block.getDocsPerDistBlock();
	aos_assert_r(docs_per_distblock && mDocsPerDistBlock == (u64)docs_per_distblock, false);
	u32 idx = stat_docid / docs_per_distblock;
	if (dist_block.getGroupId() == idx) return true;

	// The current distribution block is not what 'stat_ocid' needs. 
	// Check whether it needs to save. 
	bool rslt = false;
	if (dist_block.dataLen() > 0) 
	{
		rslt = saveDistBlock(rdata, dist_block);	
		aos_assert_r(rslt, false);
		dist_block.clear();
	}

	if (idx >= mStatIdMgr->getNumDistBlockEntries())
	{
		OmnAlarm << "internal_error" << enderr;
		return false;
	}

	// This means the requested distribution block has not been 
	// loaded into memory yet. Load it if it exists. Otherwise, 
	// create it (if create is true).
	aos_assert_r(mStatIdMgr->isCheck(idx), false);

	bool new_dist_block = false;
	u64 did = mStatIdMgr->getEntry(idx);
	AosBuffPtr buff;
	AosXmlTagPtr doc = retrieveBinaryDoc(rdata, did, buff);
	if (doc)
	{
		aos_assert_r(doc && buff->dataLen() > 0, false);
	}
	else
	{
		buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
		new_dist_block = true;
	}

	dist_block.set(buff, did, idx, new_dist_block);
	return true;
}



bool
AosRemoteDistBlockMgr::saveDistBlock(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block)
{
	if (dist_block.dataLen() <= 0)  return true;

	// It needs to save. 
	u64 docid = dist_block.getDocid();
	if (dist_block.isNew())
	{
		AosXmlTagPtr doc = getDocByDocid(rdata, docid);
		aos_assert_r(!doc, false);

		u32 crt_groupid = dist_block.getGroupId();
		aos_assert_r(mStatIdMgr->getEntry(crt_groupid)== docid, false);

		int vid = dist_block.getDistBlockVid();
		aos_assert_r(AosGetCubeId(docid) == vid, false);

		doc = createBinaryDoc(
				rdata,
				docid,
				dist_block.getDistributionBlock());
		aos_assert_r(doc, false);

		docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid, false);
		aos_assert_r(mStatIdMgr->getEntry(crt_groupid) == docid, false);

		sanityCheck(docid, crt_groupid, dist_block.getDocsPerDistBlock());
	}
	else
	{
		AosXmlTagPtr doc = getDocByDocid(rdata, docid);
		aos_assert_r(doc, false);
		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, false);
		modifyBinaryDoc(rdata, doc, dist_block.getDistributionBlock());

		sanityCheck(docid, dist_block.getGroupId(), dist_block.getDocsPerDistBlock());
	}

	dist_block.clear();
	return true;
}


AosXmlTagPtr
AosRemoteDistBlockMgr::createBinaryDoc(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const AosBuffPtr &buff)
{
	int vid = AosGetCubeId(docid); 
	OmnString objid = AosObjid::getDftObjid(docid, rdata);

	OmnString docstr = "<binary_doc ";
	docstr << AOSTAG_OTYPE << "=\" zky_binarydoc\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\">"
		<< "</binary_doc>";

	AosXmlTagPtr doc = createBinaryDocPriv(
						rdata,
						docstr,
						buff,
						vid);
	aos_assert_r(doc, 0);
	return doc;
}


bool
AosRemoteDistBlockMgr::sanityCheck(
		const u64 &did,
		const u64 &groupid,
		const int docs_per)
{
	int vid = AosGetCubeId(did);
	int vv = AosDataStructsUtil::getVirtualIdByGroupid(groupid);
	aos_assert_r(vv == vid, false);
	return true;
}


void
AosRemoteDistBlockMgr::setSnapShots(
		map<int, u64> &snap_ids, 
		map<int, u64> &bsnap_ids)
{
	mSnapIds = snap_ids;
	mBinarySnapIds = bsnap_ids;
}


void
AosRemoteDistBlockMgr::getSnapShotId(
		const int vid,
		u64 &snap_id,
		u64 &bsnap_id)
{
	snap_id = bsnap_id = 0;
	aos_assert(mSnapIds.count(vid) != 0 && mBinarySnapIds.count(vid) != 0);
	snap_id = mSnapIds[vid];
	bsnap_id = mBinarySnapIds[vid];

}


AosXmlTagPtr
AosRemoteDistBlockMgr::createBinaryDocPriv(
		const AosRundataPtr &rdata,
		const OmnString &docstr,
		const AosBuffPtr &buff,
		const int &vid)
{
	u64 snap_id = 0;
	u64 bsnap_id = 0;

	if (mUseSnapShot)
	{
		getSnapShotId(vid, snap_id, bsnap_id);
		aos_assert_r(snap_id && bsnap_id, 0);
	}

	return AosDocClientObj::getDocClient()->createBinaryDocByStat(docstr, 
				buff,  vid, snap_id, bsnap_id, rdata);
}


bool
AosRemoteDistBlockMgr::modifyBinaryDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const AosBuffPtr &buff)
{
	u64 snap_id = 0;
	u64 bsnap_id = 0;
	if (mUseSnapShot)
	{
		int vid = AosGetCubeId(doc->getAttrU64(AOSTAG_DOCID, 0));
		getSnapShotId(vid, snap_id, bsnap_id);
		aos_assert_r(snap_id && bsnap_id, false);
	}

	return 	AosDocClientObj::getDocClient()->modifyBinaryDocByStat(
				doc, buff, snap_id, bsnap_id, rdata);
}


AosXmlTagPtr 
AosRemoteDistBlockMgr::retrieveBinaryDoc(
		const AosRundataPtr &rdata,
		const u64 &docid, 
		AosBuffPtr &buff)
{
	u64 snap_id = 0;
	u64 bsnap_id = 0;
	if (mUseSnapShot)
	{
		int vid = AosGetCubeId(docid);
		getSnapShotId(vid, snap_id, bsnap_id);
		aos_assert_r(snap_id && bsnap_id, 0);
	}

	return 	AosDocClientObj::getDocClient()->retrieveBinaryDocByStat(
				docid, buff, snap_id, bsnap_id, rdata);
}


AosXmlTagPtr
AosRemoteDistBlockMgr::getDocByDocid(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	AosBuffPtr buff;
	return retrieveBinaryDoc(rdata, docid, buff);
}


bool
AosRemoteDistBlockMgr::resetStatIdMgr(const AosBuffPtr &buff)
{
	aos_assert_r(mStatIdMgr, false);
	mStatIdMgr->reset(buff);
	return true;
}

u64
AosRemoteDistBlockMgr::getEntryByStatId(const u64 &stat_id)
{
	return mStatIdMgr->getEntryByStatId(stat_id);
}


