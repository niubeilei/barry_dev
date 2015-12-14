////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 09/04/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
//
#include "Snapshot/SnapShotInfo.h"

#include "Snapshot/SnapShotMgr.h"
#include "Snapshot/SnapShotIILTrans.h"
#include "Rundata/Rundata.h"


AosSnapShotInfo::AosSnapShotInfo(
		const AosSnapShotMgrPtr &snap_shot_mgr,
		const u32 id, 
		const AosSnapShotType::E type,
		const bool show_log)
:
mSnapShotMgr(snap_shot_mgr),
mSnapSeqno(id),
mType(type),
mSts(AosSnapShot::eAddSnap),
mShowLog(show_log)
{

}


AosSnapShotInfo::~AosSnapShotInfo()
{
}


void
AosSnapShotInfo::init(
		const AosRundataPtr &rdata,
		const bool create)
{
	OmnScreen << "AAAAAAAAAAA init " << mSnapSeqno << " ; " << (create?"save":"read")<< endl;
	if (create)
	{
		saveInfoToFile();
		return;
	}
	readInfoFromFile(rdata);
}


bool
AosSnapShotInfo::clearData()
{
	AosBuffPtr conf_buff = mSnapShotMgr->readFromFile(mSnapSeqno);
	if(!conf_buff) return true;

	u32 seqno = conf_buff->getU32(0);
	if (seqno == 0) return true;

	aos_assert_r(seqno == mSnapSeqno, false);
	mSts = (AosSnapShot::Status)conf_buff->getU8(0);

	u32 size = conf_buff->getU32(0);
	for (u32 i = 0; i < size; i++)		
	{
		AosSnapShotKey key;
		AosSnapShotPtr snap_shot = serializeFromMapEntry(key, conf_buff);
		aos_assert_r(snap_shot, false);

		if (mSts == AosSnapShot::eCommit)
		{
			snap_shot->recoverCommit(key.siteid);
			continue;
		}

		if (mSts == AosSnapShot::eRollback)
		{
			snap_shot->recoverRollBack(key.siteid);
			continue;
		}

		snap_shot->clearData();
	}
	return true;
}


AosSnapShotPtr  
AosSnapShotInfo::serializeFromMapEntry(
		AosSnapShotKey &key,
		const AosBuffPtr &conf_buff)
{	
	aos_assert_r(conf_buff->dataLen() - conf_buff->getCrtIdx() >= eSnapShotKeyLen, 0);
	bool rslt = key.serializeFrom(conf_buff);
	aos_assert_r(rslt, 0);

	AosSnapShot::Status sts = (AosSnapShot::Status)conf_buff->getU8(0);
	return  newSnapShotObj(key.virtual_id, key.dfm_id, sts);
}


bool
AosSnapShotInfo::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &conf_buff)
{
	mSnapSeqno = conf_buff->getU32(0);
	mSts = (AosSnapShot::Status)conf_buff->getU8(0);
	u32 size = conf_buff->getU32(0);
	for (u32 i = 0; i < size; i++)		
	{
		AosSnapShotKey key;
		AosSnapShotPtr snap_shot = serializeFromMapEntry(key, conf_buff);
		mSnapShot.insert(make_pair(key, snap_shot));
	}
	return true;
}


bool 
AosSnapShotInfo::serializeTo(const AosBuffPtr &conf_buff)
{
	// u32 snap_seqno
	// u8  snap_sts
	// u32 map_size 
	// [
	// u32 virtual_id
	// u32 siteid 
	// u32 dfm_id
	// u8 sts           
	// ]
	// [...] ...
	conf_buff->setU32(mSnapSeqno);
	conf_buff->setU8(mSts);
	conf_buff->setU32(mSnapShot.size());
	mapitr_t itr = mSnapShot.begin();
	for (; itr != mSnapShot.end(); ++ itr)
	{
		u32 crt_idx = conf_buff->getCrtIdx();
		AosSnapShotKey key = itr->first;
		key.serializeTo(conf_buff);

		AosSnapShot::Status sts = (itr->second)->getStatus();
		aos_assert_r(conf_buff->dataLen() - crt_idx == eSnapShotKeyLen, false);
		conf_buff->setU8(sts);
	}
	return true;
}


void
AosSnapShotInfo::readInfoFromFile(const AosRundataPtr &rdata)
{
OmnScreen << "AAAAAAAAAAA read Info From File " << mSnapSeqno << endl;
	AosBuffPtr buff = mSnapShotMgr->readFromFile(mSnapSeqno);
	if (!buff) return;

	serializeFrom(rdata, buff);
}


void
AosSnapShotInfo::saveInfoToFile()
{
OmnScreen << "AAAAAAAAAAA save Info To File " << mSnapSeqno << endl;
	u32 entry_size = mSnapShotMgr->getEntrySize();
	AosBuffPtr buff = OmnNew AosBuff(entry_size AosMemoryCheckerArgs);
	memset(buff->data(), 0, entry_size);
	serializeTo(buff);
	mSnapShotMgr->saveToFile(mSnapSeqno, buff);
}


AosSnapShotPtr	
AosSnapShotInfo::retrieveSnapShot(
		const AosRundataPtr &rdata,
		const u32 virtual_id,
		const u32 dfm_id,
		const bool create_flag)
{
	AosSnapShotKey key(virtual_id, rdata->getSiteid(), dfm_id);
	mapitr_t itr = mSnapShot.find(key);
	if (itr != mSnapShot.end())
	{
		return itr->second;
	}

	if (!create_flag) return 0;

	AosSnapShotPtr snap_shot = newSnapShotObj(virtual_id, dfm_id, mSts);
	aos_assert_r(snap_shot, 0);

	mSnapShot.insert(make_pair(key, snap_shot));
	saveInfoToFile();
	return snap_shot;
}


AosSnapShotPtr 
AosSnapShotInfo::newSnapShotObj(
		const u32 virtual_id,
		const u32 dfm_id,
		const AosSnapShot::Status sts)
{
	AosSnapShotPtr snap_shot;
	if (mType == AosSnapShotType::eIILTransSnapShot)
	{
		u64 snap_id = ((u64)mType << 32) + mSnapSeqno;
		snap_shot = OmnNew AosSnapShotIILTrans(
				snap_id, virtual_id, dfm_id, sts, mShowLog);
	}
	return snap_shot;
}


bool 
AosSnapShotInfo::updateInfo(
		const AosRundataPtr &rdata,
		const AosSnapShot::Status sts,
		const u32 virtual_id,
		const u32 dfm_id)
{
	if (mSts != sts) mSts = sts;
	OmnScreen << "AAAAAAAAAAA update Info " << mSnapSeqno << " ; " << sts << endl;

	AosSnapShotKey key(virtual_id, rdata->getSiteid(), dfm_id);
	mapitr_t itr = mSnapShot.find(key);
	aos_assert_r(itr != mSnapShot.end(), false);

	mSnapShot.erase(itr);

	if (mSnapShot.size() > 0)
	{
		saveInfoToFile();
		return true;
	}

	OmnScreen << "AAAAAAAAAAA remove Info To File " << mSnapSeqno << endl;
	mSnapShotMgr->removeSnapShotInfo(mSnapSeqno);
	return true;
}


void
AosSnapShotInfo::updateStatus(const AosSnapShot::Status sts)
{
	OmnScreen << "AAAAAAAAAAA update Status " << mSnapSeqno << " ; " << sts << endl;
	mSts = sts;
	saveInfoToFile();
}
