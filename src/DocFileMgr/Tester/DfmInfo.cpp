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
#include "DocFileMgr/Tester/DfmInfo.h"

#include "SEInterfaces/DocFileMgrObj.h"
#include "DocFileMgr/Tester/DocInfo.h"
#include "DocFileMgr/Tester/SnapInfo.h"
#include "DfmUtil/DfmDocNorm.h"
#include "Thread/RwLock.h"

AosDfmInfo::AosDfmInfo(
		const AosRundataPtr &rdata,
		AosDfmConfig &config)
:
mLock(OmnNew OmnMutex()),
mSnapLock(OmnNew OmnRwLock()),
mMaxDocid(1),
mTotalSnapNum(config.mSnapShotNum)
{
	mDfm = AosRetrieveDocFileMgr(rdata, 0, config);
	if(!mDfm)
	{
		mDfm = AosCreateDocFileMgr(rdata, 0, config);
	}
	mTmpDoc = AosDfmDoc::cloneDoc(config.mDocType);
}


AosDfmInfo::~AosDfmInfo()
{
}


bool
AosDfmInfo::hasMemory()
{
	u32 total_doc_num = mDocInfo.size();
	
	map<u32, AosSnapInfoPtr>::iterator itr;
	for(itr = mSnaps.begin(); itr != mSnaps.end(); itr++)
	{
		total_doc_num += (itr->second->getDocNum());
	}
	return (total_doc_num < eMaxDocs);
}


bool
AosDfmInfo::addDoc(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();

	mLock->lock();
	if(!hasMemory())
	{
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}
	
	u64 new_docid = mMaxDocid++;
	AosDocInfoPtr doc_info = OmnNew AosDocInfo(new_docid);
	mDocInfo.insert(make_pair(new_docid, doc_info));
	
	doc_info->lock();
	mLock->unlock();
	
	AosDfmDocPtr dfm_doc = mTmpDoc->clone(new_docid);
	dfm_doc->setBodyBuff(doc_info->getData());

	AosTransId trans_id;
	bool rslt = mDfm->saveDoc(rdata, trans_id, dfm_doc);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return 0;
	}

	OmnScreen << "createDoc; "
		<< "docid:" << new_docid << "; "
		<< "size:" << doc_info->getDocSize() << "; "
		<< "pattern:" << doc_info->getPattern() << "; "
		<< "repeat:" << doc_info->getRepeat() << "; "
		<< endl;
	doc_info->unlock();
	
	mSnapLock->unlock();
	return true; 
}



bool
AosDfmInfo::randModifyDoc(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();
	
	mLock->lock();
	AosDocInfoPtr doc_info = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}
	
	doc_info->lock();
	mLock->unlock();

	doc_info->reset();
	
	AosDfmDocPtr dfm_doc = mTmpDoc->clone(doc_info->getDocid());
	dfm_doc->setBodyBuff(doc_info->getData());

	AosTransId trans_id;
	bool rslt = mDfm->saveDoc(rdata, trans_id, dfm_doc);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}

	OmnScreen << "modifyDoc; "
		    << "docid:" << doc_info->getDocid() << "; " 
			<< "size:" << doc_info->getDocSize() << "; "
			<< "pattern:" << doc_info->getPattern() << "; "
			<< "repeat:" << doc_info->getRepeat() << "; "
			<< endl;
	doc_info->unlock();
	
	mSnapLock->unlock();
	return true; 
}


bool
AosDfmInfo::randDeleteDoc(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();

	mLock->lock();
	AosDocInfoPtr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	u64 docid = doc_info->getDocid();
	mDocInfo.erase(mDocInfo.find(docid));
	mLock->unlock();
	
	AosDfmDocPtr dfm_doc = mTmpDoc->clone(doc_info->getDocid());
	
	AosTransId trans_id;
	bool rslt = mDfm->deleteDoc(rdata, trans_id, dfm_doc);    
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}

	OmnScreen << "deleteDoc; "
		<< "docid:" << docid << "; " 
		<< "size:" << doc_info->getDocSize() << "; "
		<< "pattern:" << doc_info->getPattern() << "; "
		<< "repeat:" << doc_info->getRepeat() << "; "
		<< endl;
	
	doc_info->unlock();
	mSnapLock->unlock();
	return true;
}


bool
AosDfmInfo::randReadDoc(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();

	mLock->lock();
	AosDocInfoPtr doc_info  = randGetDocInfo(); 
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	mLock->unlock();
	
	u32 docid = doc_info->getDocid();
	OmnScreen << "start readDoc"
		<< "; docid:" << docid
		<< "; "
		<< endl;

	AosDfmDocPtr dfm_doc = mDfm->readDoc(rdata, docid);
	aos_assert_r(dfm_doc, false);

	bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
	if(!is_same)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}
	
	OmnScreen << "readDoc end."
		<< "; docid:" << docid
		<< "; "
		<< endl;
	
	doc_info->unlock();
	mSnapLock->unlock();
	return true;
}


bool
AosDfmInfo::addDocToSnap(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();

	AosSnapInfoPtr snap = randGetSnap();
	if(!snap)
	{
		mSnapLock->unlock();
		return true;	
	}

	mLock->lock();
	if(!hasMemory())
	{
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}
	
	u64 new_docid = mMaxDocid++;
	AosDocInfoPtr doc_info = snap->addDoc(new_docid);
	
	doc_info->lock();
	mLock->unlock();
	
	AosDfmDocPtr dfm_doc = mTmpDoc->clone(new_docid);
	dfm_doc->setBodyBuff(doc_info->getData());

	vector<AosTransId> tids;
	bool rslt = mDfm->saveDoc(rdata,
			snap->getSnapId(), dfm_doc, tids);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return 0;
	}

	OmnScreen << "createDoc to Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << new_docid << "; "
		<< "size:" << doc_info->getDocSize() << "; "
		<< "pattern:" << doc_info->getPattern() << "; "
		<< "repeat:" << doc_info->getRepeat() << "; "
		<< endl;
	doc_info->unlock();
	
	mSnapLock->unlock();
	return true; 
}


bool
AosDfmInfo::randModifyDocFromSnap(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();

	AosSnapInfoPtr snap = randGetSnap();
	if(!snap)
	{
		mSnapLock->unlock();
		return true;	
	}

	mLock->lock();
	AosDocInfoPtr doc_info = snap->randModifyDoc(); 
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	mLock->unlock();

	AosDfmDocPtr dfm_doc = mTmpDoc->clone(doc_info->getDocid());
	dfm_doc->setBodyBuff(doc_info->getData());

	vector<AosTransId> tids;
	bool rslt = mDfm->saveDoc(rdata, snap->getSnapId(),
			dfm_doc, tids);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}

	OmnScreen << "modifyDoc from Snapshot; "
			<< "snap_id:" << snap->getSnapId() << "; "
		    << "docid:" << doc_info->getDocid() << "; " 
			<< "size:" << doc_info->getDocSize() << "; "
			<< "pattern:" << doc_info->getPattern() << "; "
			<< "repeat:" << doc_info->getRepeat() << "; "
			<< endl;
	doc_info->unlock();
	mSnapLock->unlock();
	return true; 
}


bool
AosDfmInfo::randDeleteDocFromSnap(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();
	
	AosSnapInfoPtr snap = randGetSnap();
	if(!snap)
	{
		mSnapLock->unlock();
		return true;	
	}

	mLock->lock();
	AosDocInfoPtr doc_info = snap->randDeleteDoc(); 
	
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	mLock->unlock();
	
	u64 docid = doc_info->getDocid();
	
	AosDfmDocPtr dfm_doc = mTmpDoc->clone(docid);
	vector<AosTransId> tids;
	bool rslt = mDfm->deleteDoc(rdata,
			snap->getSnapId(), dfm_doc, tids);    
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}

	OmnScreen << "deleteDoc from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid << "; " 
		<< "size:" << doc_info->getDocSize() << "; "
		<< "pattern:" << doc_info->getPattern() << "; "
		<< "repeat:" << doc_info->getRepeat() << "; "
		<< endl;
	
	doc_info->unlock();
	mSnapLock->unlock();
	return true;
}


bool
AosDfmInfo::randReadDocFromSnap(const AosRundataPtr &rdata)
{
	mSnapLock->readlock();
	
	AosSnapInfoPtr snap = randGetSnap();
	if(!snap)
	{
		mSnapLock->unlock();
		return true;	
	}

	mLock->lock();
	AosDocInfoPtr doc_info = snap->randReadDoc(); 
	if(!doc_info)
	{
		mLock->unlock();
		mSnapLock->unlock();
		return true;
	}

	doc_info->lock();
	mLock->unlock();
	
	u32 docid = doc_info->getDocid();
	OmnScreen << "start readDoc from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid
		<< "; "
		<< endl;

	AosDfmDocPtr dfm_doc = mDfm->readDoc(rdata, 
			snap->getSnapId(), docid, true);
	aos_assert_r(dfm_doc, false);

	bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
	if(!is_same)
	{
		OmnAlarm << "error!" << enderr;
		mLock->unlock();
		mSnapLock->unlock();
		return false;
	}
	
	OmnScreen << "readDoc end from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid
		<< "; "
		<< endl;
	
	doc_info->unlock();
	mSnapLock->unlock();
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
	OmnScreen << "create snapshot;"
		<< "; snap_id:" << snap_id
		<< endl;
	
	AosDfmInfoPtr thisptr(this, false); 
	AosSnapInfoPtr snap = OmnNew AosSnapInfo(snap_id, thisptr);
	mSnaps.insert(make_pair(snap_id, snap));
	
	return true;
}

bool
AosDfmInfo::rollback(const AosSnapInfoPtr &snap)
{
	OmnScreen << "start rollback;"
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.find(snap->getSnapId());
	aos_assert_r(itr != mSnaps.end(), false);

	mSnaps.erase(itr);
	
	OmnScreen << "rollback end;"
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	return true;
}


bool
AosDfmInfo::commit(const AosSnapInfoPtr &snap)
{
	OmnScreen << "start commit ;"
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.find(snap->getSnapId());
	aos_assert_r(itr != mSnaps.end(), false);

	mSnaps.erase(itr);

	map<u64, AosDocInfoPtr> snap_docs = snap->getDocs();
	map<u64, AosDocInfoPtr>::iterator m_itr, crt_itr;
	for(m_itr = snap_docs.begin(); m_itr != snap_docs.end(); m_itr++)
	{
		u64 docid = m_itr->first;
		
		crt_itr = mDocInfo.find(docid);
		if(crt_itr == mDocInfo.end())
		{
			mDocInfo.insert(make_pair(docid, m_itr->second));
			
			OmnScreen << "commit add doc;"
				<< "; snap_id:" << snap->getSnapId()
				<< "; docid:" << docid
				<< "; "
				<< endl;
		}
		else
		{
			crt_itr->second = m_itr->second;
			OmnScreen << "commit modify doc;"
				<< "; snap_id:" << snap->getSnapId()
				<< "; docid:" << docid
				<< "; "
				<< endl;
		}
	}
	
	set<u64> delete_docs = snap->getDeletedDocs();
	set<u64>::iterator s_itr; 
	for(s_itr = delete_docs.begin(); s_itr != delete_docs.end(); s_itr++)
	{
		u64 docid = *s_itr;
		
		crt_itr = mDocInfo.find(docid);
		//aos_assert_r(crt_itr != mDocInfo.end(), false);	
		if(crt_itr != mDocInfo.end())
		{
			mDocInfo.erase(crt_itr);
			OmnScreen << "commit delete doc;"
				<< "; snap_id:" << snap->getSnapId()
				<< "; docid:" << docid
				<< "; "
				<< endl;
		}
	}
	
	OmnScreen << "commit end ;"
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	return true;
}


bool
AosDfmInfo::sanitycheck(const AosRundataPtr &rdata)
{
	OmnScreen << "start sanitycheck;" << endl;

	u32 check_did = 1;
	map<u64, AosDocInfoPtr>::iterator itr = mDocInfo.begin();
	AosDfmDocPtr dfm_doc;
	bool rslt;
	for(; itr != mDocInfo.end(); itr++)
	{
		AosDocInfoPtr doc_info = itr->second;
		doc_info->lock();
		
		u32 docid = doc_info->getDocid();
		for(; check_did < docid; check_did++)
		{
			OmnScreen << "readDoc sanitycheck not exist"
				<< "; crt_docid:" << check_did
				<< endl;

			dfm_doc = mDfm->readDoc(rdata, check_did, false);
			aos_assert_rl(!dfm_doc, doc_info->getLock(), false);
		}
		check_did++;

		OmnScreen << "readDoc sanitycheck"
			<< "; act_docid:" << docid 
			<< endl;
		dfm_doc = mDfm->readDoc(rdata, docid);
		aos_assert_rl(dfm_doc, doc_info->getLock(), false);

		bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
		aos_assert_rl(is_same, doc_info->getLock(), false);
		
		doc_info->unlock();
	}
	
	OmnScreen << "sanitycheck end;" << endl;
	return true;
}

bool
AosDfmInfo::merge(
		const AosSnapInfoPtr &target_snap,
		const AosSnapInfoPtr &merger_snap)
{
	OmnScreen << "start merge;"
		<< "; target_snap_id:" << target_snap->getSnapId()
		<< "; merger_snap_id:" << merger_snap->getSnapId()
		<< endl;
	
	map<u32, AosSnapInfoPtr>::iterator itr = mSnaps.find(merger_snap->getSnapId());
	aos_assert_r(itr != mSnaps.end(), false);

	mSnaps.erase(itr);
	target_snap->merge(merger_snap);
	
	OmnScreen << "merge end ;"
		<< "; target_snap_id:" << target_snap->getSnapId()
		<< "; merger_snap_id:" << merger_snap->getSnapId()
		<< endl;
	return true;
}


bool
AosDfmInfo::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	u32 dfm_id = buff->getU32(0);
	aos_assert_r(mDfm->getId() == dfm_id, false);

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
		if(docid > mMaxDocid) mMaxDocid = docid;
	}
	
	mMaxDocid++;
	//mCrtDocInfoNum = buff->getU32(0);
	mTotalSnapNum = buff->getU32(0);

	return true;
}
	

bool
AosDfmInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	OmnScreen << "DfmInfo:: serializeTo"
		<< "; doc Num:" << mDocInfo.size() 
		<< "; snap Num:" << mSnaps.size()
		<< endl;
	
	buff->setU32(mDfm->getId());
	
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

	//buff->setU32(mCrtDocInfoNum);
	buff->setU32(mTotalSnapNum);
	return true;
}


