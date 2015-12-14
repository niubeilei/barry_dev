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
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLogTester/DfmInfo.h"

#include "SEInterfaces/DocFileMgrObj.h"
#include "DocFileMgr/DfmLogTester/DocInfo.h"
#include "DocFileMgr/DfmLogTester/SnapInfo.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "Thread/RwLock.h"
#include "FmtMgr/FmtMgr.h"

AosDfmInfo::AosDfmInfo(
		const AosRundataPtr &rdata,
		const u32 vid, 
		AosDfmConfig &config)
:
mLock(OmnNew OmnMutex()),
mSnapLock(OmnNew OmnRwLock()),
mVId(vid),
mSiteid(rdata->getSiteid()),
mCrtDocInfoNum(0),
mTotalSnapNum(config.mSnapShotNum)
{
	mDfm = AosRetrieveDocFileMgr(rdata, vid, config);
	if(!mDfm)
	{
		mDfm = AosCreateDocFileMgr(rdata, vid, config);
	}
	mDfmId = mDfm->getId();
}


AosDfmInfo::AosDfmInfo()
:
mLock(OmnNew OmnMutex()),
mSnapLock(OmnNew OmnRwLock())
{
}


AosDfmInfo::~AosDfmInfo()
{
}


AosDocInfoPtr
AosDfmInfo::addDoc(const AosRundataPtr &rdata, const u64 docid)
{
	//mSnapLock->readlock();

	mLock->lock();
	if(mCrtDocInfoNum >= eMaxDocs)
	{
		mLock->unlock();
		//mSnapLock->unlock();
		return 0;
	}
	
	AosDocInfoPtr doc_info = OmnNew AosDocInfo(docid);
	mDocInfo.insert(make_pair(docid, doc_info));	
	mCrtDocInfoNum++;
	/*
	if (mCrtDocInfoNum % 1000 == 0)
	{
		static int time1 = OmnGetSecond();
		int time2 = OmnGetSecond();
		int cost = time2 - time1;
		if (cost)
		{
			OmnScreen <<  mCrtDocInfoNum/cost << " docs/per second." << endl;
		}
	}
	*/
	doc_info->lock();
	mLock->unlock();
	
	u64 loc_docid = doc_info->getLocalDocid();
	//AosDfmDocPtr dfm_doc = mTempDoc->clone(loc_docid);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(loc_docid); 
	dfm_doc->setBodyBuff(doc_info->getData());
	//dfm_doc->setNeedCompress(true);

	vector<AosTransId> v;
	bool rslt = mDfm->saveDoc(rdata, v, dfm_doc);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		//mSnapLock->unlock();
		return 0;
	}

//	OmnScreen << "createDoc"
//		<< "; act_docid:" << docid
//		<< "; docid:" << loc_docid 
//		<< "; virtual_id:" << mVId << "; "
//		<< "dfmId:" << mDfmId << "; "
//		<< "size:" << doc_info->getDocSize() << "; "
//		<< "pattern:" << doc_info->getPattern() << "; "
//		<< "repeat:" << doc_info->getRepeat() << "; "
//		<< endl;
	doc_info->unlock();
	
	//mSnapLock->unlock();
	return doc_info; 
}


bool
AosDfmInfo::randModifyDoc(const AosRundataPtr &rdata)
{
	//mSnapLock->readlock();
	
	mLock->lock();
	AosDocInfoPtr doc_info = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		//mSnapLock->unlock();
		return true;
	}
	
	doc_info->lock();
	mLock->unlock();

	doc_info->reset();
	
	u64 loc_docid = doc_info->getLocalDocid();
	//AosDfmDocPtr dfm_doc = mTempDoc->clone(loc_docid);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(loc_docid); 
	dfm_doc->setBodyBuff(doc_info->getData());
	
	vector<AosTransId> v;
	bool rslt = mDfm->saveDoc(rdata, v, dfm_doc);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		//mSnapLock->unlock();
		return false;
	}

//	OmnScreen << "modifyDoc"                                        
//		    << "; act_docid:" << doc_info->getDocid() 
//			<< "; docid:" << doc_info->getLocalDocid() 
//		    << "; virtual_id:" << mVId << "; "
//			<< "dfmId:" << mDfmId << "; "
//			<< "size:" << doc_info->getDocSize() << "; "
//			<< "pattern:" << doc_info->getPattern() << "; "
//			<< "repeat:" << doc_info->getRepeat() << "; "
//			<< endl;
	doc_info->unlock();
	
	//mSnapLock->unlock();
	return true; 
}


bool
AosDfmInfo::randDeleteDoc(const AosRundataPtr &rdata)
{
	//mSnapLock->readlock();

	mLock->lock();
	AosDocInfoPtr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		//mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	u64 docid = doc_info->getDocid();
	mDocInfo.erase(mDocInfo.find(docid));
	mCrtDocInfoNum--;
	mLock->unlock();
	
	u64 loc_docid = doc_info->getLocalDocid();
	//AosDfmDocPtr dfm_doc = mTempDoc->clone(loc_docid);
	AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(loc_docid); 
	vector<AosTransId> v;
	bool rslt = mDfm->deleteDoc(rdata, v, dfm_doc);    
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		//mSnapLock->unlock();
		return false;
	}

//	OmnScreen << "deleteDoc"
//		<< "; act_docid:" << docid 
//		<< "; docid:" << loc_docid 
//		<< "; virtual_id:" << mVId << "; "
//		<< "dfmId:" << mDfmId << "; "
//		<< "size:" << doc_info->getDocSize() << "; "
//		<< "pattern:" << doc_info->getPattern() << "; "
//		<< "repeat:" << doc_info->getRepeat() << "; "
//		<< endl;
	
	doc_info->unlock();
	//mSnapLock->unlock();
	return true;
}


bool
AosDfmInfo::randReadDoc(const AosRundataPtr &rdata)
{
	//mSnapLock->readlock();

	mLock->lock();
	AosDocInfoPtr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		//mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	mLock->unlock();
	
	u32 loc_docid = doc_info->getLocalDocid();

//	OmnScreen << "start readDoc"
//		<< "; act_docid:" << doc_info->getDocid() 
//		<< "; docid:" << loc_docid  
//		<< "; virtual_id:" << mVId 
//		<< "; dfmId:" << mDfmId
//		<< endl;

	AosDfmDocPtr dfm_doc = mDfm->readDoc(rdata, loc_docid);
	aos_assert_r(dfm_doc, false);

	bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
	if(!is_same)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		//mSnapLock->unlock();
		return false;
	}
	
	doc_info->unlock();
	//mSnapLock->unlock();
	return true;
}


AosDocInfoPtr
AosDfmInfo::randGetDocInfo()
{
	if(mDocInfo.size() == 0)	return 0;
	
	u32 idx = rand() % mDocInfo.size();
	map<u64, AosDocInfoPtr>::iterator itr = mDocInfo.begin();
	for(u32 i=0; i<idx; i++)	itr++;

	return itr->second;
}


/*
bool
AosDfmInfo::addSnapShot(const AosRundataPtr &rdata)
{
	mSnapLock->writelock();

	if(mCrtDocInfoNum >= eMaxDocs)
	{
		mSnapLock->unlock();
		return false;
	}
	
	u32 dfm_snap_id = 0;
	bool rslt = mDfm->addSnapShot(rdata, dfm_snap_id);
	aos_assert_rl(rslt && dfm_snap_id, mSnapLock, false);
	
	AosSnapInfoPtr snap = OmnNew AosSnapInfo(dfm_snap_id, mDocInfo);

	mSnaps.insert(make_pair(dfm_snap_id, snap));
	mCrtDocInfoNum += mDocInfo.size();	
	
	if(mSnaps.size() > mTotalSnapNum)
	{
		map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
		AosSnapInfoPtr snap = itr->second;
		aos_assert_rl(snap, mSnapLock, false);

		u32 doc_num = snap->getDocNum();
		snap->removeDocInfo();
		mSnaps.erase(itr);
		mCrtDocInfoNum -= doc_num;
	}
	mSnapLock->unlock();
	
	OmnScreen << "add SnapShot"
		<< "; virtual_id:" << mVId 
		<< "; dfmId:" << mDfmId
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	return true;
}


bool
AosDfmInfo::randRemoveSnapShot(const AosRundataPtr &rdata)
{
	if(mSnaps.size() == 0)	return false;
	
	mSnapLock->writelock();

	u32 idx = rand() % mSnaps.size();
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
	for(u32 i=0; i<idx; i++)	itr++;

	AosSnapInfoPtr snap = itr->second;
	aos_assert_rl(snap, mSnapLock, false);

	u32 doc_num = snap->getDocNum();
	snap->removeDocInfo();
	mSnaps.erase(itr);
	mCrtDocInfoNum -= doc_num;

	bool rslt = mDfm->removeSnapShot(rdata, snap->getSnapId());
	aos_assert_rl(rslt, mSnapLock, false);
	mSnapLock->unlock();
	
	OmnScreen << "remove SnapShot"
		<< "; virtual_id:" << mVId 
		<< "; dfmId:" << mDfmId
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	return true;
}

bool
AosDfmInfo::randRollBack(const AosRundataPtr &rdata)
{
	if(mSnaps.size() == 0)	return false;
	
	mSnapLock->writelock();

	u32 idx = rand() % mSnaps.size();
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
	for(u32 i=0; i<idx; i++)	itr++;

	AosSnapInfoPtr snap = itr->second;
	aos_assert_rl(snap, mSnapLock, false);

	mDocInfo = snap->getDocInfo();
	map<u32, AosSnapInfoPtr>::iterator tmp_itr = itr;
	for(; tmp_itr != mSnaps.end(); tmp_itr++)
	{
		tmp_itr->second->removeDocInfo();
	}
	mSnaps.erase(itr, mSnaps.end());
	mCrtDocInfoNum = mDocInfo.size();
	for(tmp_itr = mSnaps.begin(); tmp_itr != mSnaps.end(); tmp_itr++)
	{
		mCrtDocInfoNum += tmp_itr->second->getDocNum(); 
	}

	bool rslt = mDfm->rollBack(rdata, snap->getSnapId());
	aos_assert_rl(rslt, mSnapLock, false);

	checkRollBack(rdata);
	
	mSnapLock->unlock();
	
	OmnScreen << "rollBack SnapShot"
		<< "; virtual_id:" << mVId 
		<< "; dfmId:" << mDfmId
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	return true;
}


bool
AosDfmInfo::commit(const AosRundataPtr &rdata)
{
	if(mSnaps.size() == 0)	return true;
	
	mSnapLock->writelock();

	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
	for(; itr != mSnaps.end(); itr++)
	{
		u32 doc_num = itr->second->getDocNum();
		itr->second->removeDocInfo();
		mCrtDocInfoNum -= doc_num;
	}
	mSnaps.clear();
	aos_assert_rl(mCrtDocInfoNum == mDocInfo.size(), mSnapLock, false);
	
	bool rslt = mDfm->commit(rdata);
	aos_assert_rl(rslt, mSnapLock, false);
	mSnapLock->unlock();
	
	OmnScreen << "commit SnapShot"
		<< "; virtual_id:" << mVId << "; "
		<< "dfmId:" << mDfmId << "; "
		<< endl;
	
	return true;
}

*/

AosSnapInfoPtr
AosDfmInfo::randGetSnap()
{
	if(mSnaps.size() == 0)	return 0;
	
	u32 idx = rand() % mSnaps.size();
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
	for(u32 i=0; i<idx; i++)	itr++;

	AosSnapInfoPtr snap = itr->second;
	aos_assert_r(snap, 0);
	
	return snap;
}


void
AosDfmInfo::snapWriteLock()
{
	mSnapLock->writelock();
}

void
AosDfmInfo::snapUnLock()
{
	mSnapLock->unlock();
}


bool
AosDfmInfo::addSnapShot(const u32 snap_id)
{
	AosSnapInfoPtr snap = OmnNew AosSnapInfo(snap_id, mDocInfo);

	mSnaps.insert(make_pair(snap_id, snap));
	mCrtDocInfoNum += mDocInfo.size();	
	
	if(mSnaps.size() > mTotalSnapNum)
	{
		map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
		AosSnapInfoPtr snap = itr->second;
		aos_assert_rl(snap, mSnapLock, false);

		u32 doc_num = snap->getDocNum();
		snap->removeDocInfo();
		mSnaps.erase(itr);
		mCrtDocInfoNum -= doc_num;
	}
	
	return true;
}


bool
AosDfmInfo::removeSnapShot(const AosSnapInfoPtr &snap)
{
	u32 doc_num = snap->getDocNum();
	snap->removeDocInfo();
	
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.find(snap->getSnapId());
	aos_assert_r(itr != mSnaps.end(), false);

	mSnaps.erase(itr);
	mCrtDocInfoNum -= doc_num;
	
	return true;
}

bool
AosDfmInfo::rollBack(const AosSnapInfoPtr &snap)
{
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.find(snap->getSnapId());
	aos_assert_r(itr != mSnaps.end(), false);
	map<u32, AosSnapInfoPtr>::iterator tmp_itr = itr;

	mDocInfo = snap->getDocInfo();
	for(; tmp_itr != mSnaps.end(); tmp_itr++)
	{
		tmp_itr->second->removeDocInfo();
	}
	mSnaps.erase(itr, mSnaps.end());
	mCrtDocInfoNum = mDocInfo.size();
	for(tmp_itr = mSnaps.begin(); tmp_itr != mSnaps.end(); tmp_itr++)
	{
		mCrtDocInfoNum += tmp_itr->second->getDocNum(); 
	}

	return true;
}


bool
AosDfmInfo::checkRollBack(const AosRundataPtr &rdata)
{
	u32 check_did = 1;
	map<u64, AosDocInfoPtr>::iterator itr = mDocInfo.begin();
	AosDfmDocPtr dfm_doc;
	bool rslt;
	for(; itr != mDocInfo.end(); itr++)
	{
		AosDocInfoPtr doc_info = itr->second;
		doc_info->lock();
		
		u32 loc_docid = doc_info->getLocalDocid();
		for(; check_did < loc_docid; check_did++)
		{
			OmnScreen << "readDoc check not exist."
				<< "; act_docid:" << doc_info->getDocid() 
				<< "; docid:" << check_did
				<< "; virtual_id:" << mVId 
				<< "; dfmId:" << mDfmId
				<< endl;

			dfm_doc = mDfm->readDoc(rdata, check_did, false);
			aos_assert_rl(!dfm_doc, doc_info->getLock(), false);
		}
		check_did++;

		OmnScreen << "readDoc"
			<< "; act_docid:" << doc_info->getDocid() 
			<< "; docid:" << loc_docid  
			<< "; virtual_id:" << mVId 
			<< "; dfmId:" << mDfmId
			<< endl;
		dfm_doc = mDfm->readDoc(rdata, loc_docid);
		aos_assert_rl(dfm_doc, doc_info->getLock(), false);

		bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
		aos_assert_rl(is_same, doc_info->getLock(), false);
		
		doc_info->unlock();
	}
	
	return true;
}


bool
AosDfmInfo::commit()
{
	if(mSnaps.size() == 0)	return true;
	
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.begin();
	for(; itr != mSnaps.end(); itr++)
	{
		u32 doc_num = itr->second->getDocNum();
		itr->second->removeDocInfo();
		mCrtDocInfoNum -= doc_num;
	}
	mSnaps.clear();
	aos_assert_r(mCrtDocInfoNum == mDocInfo.size(), false);
	
	return true;
}


bool
AosDfmInfo::serializeFrom(
		const AosBuffPtr &buff,
		AosDfmConfig &config)
{
	aos_assert_r(buff, false);
	
	mVId = buff->getU32(0);
	mSiteid = buff->getU32(0);
	mDfmId = buff->getU32(0);

	u32 snap_nums = buff->getU32(0);
	AosSnapInfoPtr snap_info;
	u32 snap_id;
	for(u32 i=0; i<snap_nums; i++)
	{
		snap_info = OmnNew AosSnapInfo();	
		snap_info->serializeFrom(buff);

		snap_id = snap_info->getSnapId();	
		mSnaps.insert(make_pair(snap_id, snap_info));
	}

	u32 doc_nums = buff->getU32(0);
	AosDocInfoPtr doc_info;
	u64 docid;
	for(u32 i=0; i<doc_nums; i++)
	{
		doc_info = AosDocInfo::serializeFromStatic(buff);	
		aos_assert_r(doc_info, false);
		docid = doc_info->getDocid();
		
		mDocInfo.insert(make_pair(docid, doc_info));	
	}

	mCrtDocInfoNum = buff->getU32(0);
	mTotalSnapNum = buff->getU32(0);

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(mSiteid);
	mDfm = AosRetrieveDocFileMgr(rdata, mVId, config);
	aos_assert_r(mDfm, false);
	aos_assert_r(mDfm->getId() == mDfmId, false);
	return true;
}


bool
AosDfmInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	buff->setU32(mVId);
	buff->setU32(mSiteid);
	buff->setU32(mDfmId);
	
	OmnScreen << "DfmInfo:: serializeTo"
		<< "; doc Num:" << mDocInfo.size() 
		<< "; snap Num:" << mSnaps.size()
		<< endl;
	
	buff->setU32(mSnaps.size());
	map<u32, AosSnapInfoPtr>::iterator snap_itr = mSnaps.begin();
	for(; snap_itr != mSnaps.end(); snap_itr++)
	{
		snap_itr->second->serializeTo(buff);
	}
	
	buff->setU32(mDocInfo.size());
	map<u64, AosDocInfoPtr>::iterator doc_itr = mDocInfo.begin();
	for(; doc_itr != mDocInfo.end(); doc_itr++)
	{
		AosDocInfoPtr doc = doc_itr->second;
		aos_assert_r(doc, false);
		OmnScreen << "DfmInfo:: serialize doc:" 
			<< "; docid:" << doc->getDocid() 
			<< "; pattern:" << doc->getPattern() 
			<< endl;
		doc->serializeTo(buff);
	}

	buff->setU32(mCrtDocInfoNum);
	buff->setU32(mTotalSnapNum);
	
	return true;
}

void
AosDfmInfo::stop()
{
	//mDfm->stop();
	//AosFmtMgrObj::getFmtMgr()->stop();
}
