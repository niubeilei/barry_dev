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
// Created: 09/06/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/IILSaveDfmLog.h"

#include "Alarm/Alarm.h"
#include "DfmUtil/DfmDoc.h"
#include "XmlUtil/XmlTag.h"
#include "Snapshot/SnapShot.h"
#include "Snapshot/SnapShotInfo.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosIILSaveDfmLogSingleton, 
				 AosIILSaveDfmLog, 
				 AosIILSaveDfmLogSelf, 
				"AosIILSaveDfmLog");

AosIILSaveDfmLog::AosIILSaveDfmLog()
:
mSnapShotMgr(0),
mLock(OmnNew OmnMutex()),
mShowLog(false)
{
}


AosIILSaveDfmLog::~AosIILSaveDfmLog()
{
}


bool
AosIILSaveDfmLog::start()
{
	mSnapShotMgr = OmnNew AosSnapShotMgr(AosSnapShotType::eIILTransSnapShot, eEntrySize, mShowLog); 
	mSnapShotMgr->clearData();
	return true;
}


bool
AosIILSaveDfmLog::stop()
{
	return true;
}

bool
AosIILSaveDfmLog::config(const AosXmlTagPtr &config)
{
	return true;
}


void
AosIILSaveDfmLog::addEntryToMaps(
		const u64 iilid,
		const u64 snap_id)
{
	mLock->lock();
	mIILMaps.insert(make_pair(iilid, snap_id));	
	mLock->unlock();
}


void
AosIILSaveDfmLog::removeEntryFromMaps(
		const u64 iilid,
		const u64 snap_id)
{
	mLock->lock();
	mapitr_t itr = mIILMaps.find(iilid);
	aos_assert(itr != mIILMaps.end());
	aos_assert(itr->second == snap_id);

	mIILMaps.erase(itr);	
	mLock->unlock();
}


u64
AosIILSaveDfmLog::getSnapIdByIILID(const u64 iilid)
{
	mLock->lock();
	mapitr_t itr = mIILMaps.find(iilid);
	if (itr != mIILMaps.end())
	{
		mLock->unlock();
		return itr->second;
	}
	mLock->unlock();
	return 0;
}


bool
AosIILSaveDfmLog::saveDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docfilemgr,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const u64 snap_id)
{
	if (snap_id == 0)
	{
 		return docfilemgr->saveDoc(rdata, trans_ids, doc);
	}

	aos_assert_r(mSnapShotMgr, false);
	AosSnapShotInfoPtr info;
	u32 dfm_id = docfilemgr->getId();
	u32 virtual_id = docfilemgr->getVirtualId();

	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(
			rdata, snap_id, virtual_id, dfm_id, info, true);
	aos_assert_r(snap_shot, false);
	doc->setOpr(AosDfmDoc::eSave);
	return snap_shot->addEntry(rdata, doc, trans_ids);
}


AosDfmDocPtr
AosIILSaveDfmLog::readDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 snap_id,
		const u64 local_iilid,
		const bool read_body)
{
	AosDfmDocPtr doc;
	if (snap_id != 0)
	{
		aos_assert_r(mSnapShotMgr, 0);
		AosSnapShotInfoPtr info;
		u32 dfm_id = docfilemgr->getId();
		u32 virtual_id = docfilemgr->getVirtualId();

		AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(
			rdata, snap_id, virtual_id, dfm_id, info, false);
		if(snap_shot)
		{
			doc = snap_shot->readDoc(rdata, local_iilid);
			if (doc) 
			{
				//Linda, 2013/09/23
				if (doc->getOpr() == AosDfmDoc::eDelete) 
				{
					return 0;
				}
				return doc;
			}
		}
	}
	doc = docfilemgr->readDoc(rdata, local_iilid);
	return doc;
}


bool
AosIILSaveDfmLog::deleteDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docfilemgr,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const u64 snap_id)
{
	if (snap_id == 0)
	{
 		return docfilemgr->deleteDoc(rdata, trans_ids, doc);
	}

	aos_assert_r(mSnapShotMgr, false);

	AosSnapShotInfoPtr info;
	u32 dfm_id = docfilemgr->getId();
	u32 virtual_id = docfilemgr->getVirtualId();

	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(
			rdata, snap_id, virtual_id, dfm_id, info, true);
	aos_assert_r(snap_shot, false);
	doc->setOpr(AosDfmDoc::eDelete);
	return snap_shot->addEntry(rdata, doc, trans_ids);
}


bool
AosIILSaveDfmLog::addSnapShot(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		u64 &snap_id)
{
	bool rslt = mSnapShotMgr->addSnapShot(rdata, snap_id);
	aos_assert_r(rslt, false);

	addEntryToMaps(root_iilid, snap_id);

u32 snap_seqno = (u32)snap_id;
OmnScreen << "ccccccccccccccccccccccccccccccccccccccccccccccccc:" <<  snap_id << ", " << snap_seqno << endl;
	return true;
}


bool
AosIILSaveDfmLog::removeSnapShot(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		const u64 snap_id,
		const u32 virtual_id,
		const u32 dfm_id)
{
	removeEntryFromMaps(root_iilid, snap_id);

	AosSnapShotInfoPtr info;
	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(rdata, snap_id, virtual_id, dfm_id, info, false);
	aos_assert_r(snap_shot && info, false);

	bool rslt = snap_shot->removeSnapShot(rdata);
	aos_assert_r(rslt, false);

	rslt = mSnapShotMgr->updateSnapShotInfo(rdata, info, AosSnapShot::eRemoveSnap, virtual_id, dfm_id);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILSaveDfmLog::rollBack(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		const u64 snap_id,
		const u32 virtual_id,
		const u32 dfm_id)
{
	removeEntryFromMaps(root_iilid, snap_id);

	AosSnapShotInfoPtr info;
	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(rdata, snap_id, virtual_id, dfm_id, info, false);
	aos_assert_r(snap_shot && info, false);

	mSnapShotMgr->updateSnapShotStatus(AosSnapShot::eRollback, info);
	bool rslt = snap_shot->rollBack(rdata);
	aos_assert_r(rslt, false);

u32 snap_seqno = (u32)snap_id;
OmnScreen << "rollBack eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee:" <<  snap_id << ", " << snap_seqno << endl;
	return rslt;
}


bool
AosIILSaveDfmLog::rollBackCb(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const u32 virtual_id,
		const u32 dfm_id)
{
	OmnScreen << " AAAAAAAAAAA rollBack Cb " << snap_id << endl;
	AosSnapShotInfoPtr info;
	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(rdata, snap_id, virtual_id, dfm_id, info, false);
	aos_assert_r(snap_shot && info, false);

	bool rslt = mSnapShotMgr->updateSnapShotInfo(rdata, info, AosSnapShot::eRollback, virtual_id, dfm_id);
	return rslt;
}


bool
AosIILSaveDfmLog::commit(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		const AosDocFileMgrObjPtr &docFileMgr,
		const u64 snap_id)
{
	removeEntryFromMaps(root_iilid, snap_id);
	
	AosSnapShotInfoPtr info;
	u32 dfm_id = docFileMgr->getId();
	u32 virtual_id = docFileMgr->getVirtualId();
	AosSnapShotPtr snap_shot = mSnapShotMgr->retrieveSnapShot(rdata, snap_id, virtual_id, dfm_id, info, false);
	if (!snap_shot)
	{
u32 snap_seqno = (u32)snap_id;
OmnScreen << "commit eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee:" <<  snap_id << ", " << snap_seqno << endl;
		return true;
	}

	mSnapShotMgr->updateSnapShotStatus(AosSnapShot::eCommit, info);
	bool rslt = snap_shot->commit(rdata, docFileMgr);
	aos_assert_r(rslt, false);

	rslt = mSnapShotMgr->updateSnapShotInfo(rdata, info, AosSnapShot::eCommit, virtual_id, dfm_id);
	aos_assert_r(rslt, false);

u32 snap_seqno = (u32)snap_id;
OmnScreen << "commit eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee:" <<  snap_id << ", " << snap_seqno << endl;
	return rslt;
}
