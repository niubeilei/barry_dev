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
#include "DocFileMgr/Tester/SnapInfo.h"

#include "DocFileMgr/Tester/DocInfo.h"
#include "DocFileMgr/Tester/DfmInfo.h"
#include "DocFileMgr/Tester/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"

AosSnapInfo::AosSnapInfo(const u32 snap_id, const AosDfmInfoPtr &dfm)
:
mLock(OmnNew OmnMutex()),
mDfmInfo(dfm),
mSnapId(snap_id)
{
	OmnScreen << "New SnapInfo: snapid:"
		<< snap_id
		<< endl;
}

AosSnapInfo::AosSnapInfo()
:
mLock(OmnNew OmnMutex()),
mSnapId(0)
{
}

AosSnapInfo::~AosSnapInfo()
{
}


AosDocInfoPtr
AosSnapInfo::randReadDoc()
{
	u32 flag = (rand() % 2);
	if(flag == 0 && mDocs.size())
	{
		// rand read from mDocs.
		u32 idx = rand() % mDocs.size();
		map<u64, AosDocInfoPtr>::iterator itr = mDocs.begin();
		for(u32 i=0; i<idx; i++)	itr++;
		
		return itr->second;
	}

	// rand read from mDfmInfo.
	AosDocInfoPtr old_doc = mDfmInfo->randGetDocInfo();
	if(!old_doc)	return 0;
	
	u64 docid = old_doc->getDocid();
	map<u64, AosDocInfoPtr>::iterator itr = mDocs.find(docid);
	if(itr != mDocs.end()) return itr->second;		// this doc has new version in this snap.
	
	set<u64>::iterator s_itr = mDeletedDocs.find(docid);
	if(s_itr != mDeletedDocs.end())	return 0;		// means this doc has deleted in this snap.

	return old_doc;
}


AosDocInfoPtr
AosSnapInfo::addDoc(const u64 docid)
{
	AosDocInfoPtr doc_info = OmnNew AosDocInfo(docid);
	mDocs.insert(make_pair(docid, doc_info));
	
	return doc_info;
}


AosDocInfoPtr
AosSnapInfo::randModifyDoc()
{
	AosDocInfoPtr doc;
	u32 flag = (rand() % 2);
	if(flag == 0 && mDocs.size())
	{
		// rand modify from mDocs.
		u32 idx = rand() % mDocs.size();
		map<u64, AosDocInfoPtr>::iterator itr = mDocs.begin();
		for(u32 i=0; i<idx; i++)	itr++;
		
		doc = itr->second;
		doc->reset();
		return doc;
	}

	AosDocInfoPtr old_doc = mDfmInfo->randGetDocInfo();
	if(!old_doc)	return 0;
	
	u64 docid = old_doc->getDocid();
	map<u64, AosDocInfoPtr>::iterator itr = mDocs.find(docid);
	if(itr != mDocs.end())
	{
		doc = itr->second;
		doc->reset();
		return doc;
	}
	
	set<u64>::iterator s_itr = mDeletedDocs.find(docid);
	if(s_itr != mDeletedDocs.end())
	{
		// means this doc has deleted in this snap.
		mDeletedDocs.erase(s_itr);
	}

	doc = OmnNew AosDocInfo(docid);
	mDocs.insert(make_pair(docid, doc));
	return doc;	
}


AosDocInfoPtr
AosSnapInfo::randDeleteDoc()
{
	AosDocInfoPtr doc;
	u32 flag = (rand() % 2);
	if(flag == 0 && mDocs.size())
	{
		// rand delete from mDocs.
		u32 idx = rand() % mDocs.size();
		map<u64, AosDocInfoPtr>::iterator itr = mDocs.begin();
		for(u32 i=0; i<idx; i++)	itr++;
		
		doc = itr->second;
		mDocs.erase(itr);
		return doc;
	}

	AosDocInfoPtr old_doc = mDfmInfo->randGetDocInfo();
	if(!old_doc)	return 0;

	u64 docid = old_doc->getDocid();
	map<u64, AosDocInfoPtr>::iterator itr = mDocs.find(docid);
	if(itr != mDocs.end())
	{
		doc = itr->second;
		mDocs.erase(itr);
		return doc;
	}
	
	set<u64>::iterator s_itr = mDeletedDocs.find(docid);
	if(s_itr != mDeletedDocs.end())
	{
		// means this doc has deleted in this snap.
		return 0;
	}

	doc = old_doc->clone();
	mDeletedDocs.insert(docid);
	return doc;
}


bool
AosSnapInfo::merge(const AosSnapInfoPtr &snap)
{
	map<u64, AosDocInfoPtr> snap_docs = snap->getDocs();
	map<u64, AosDocInfoPtr>::iterator m_itr, crt_itr;
	for(m_itr = snap_docs.begin(); m_itr != snap_docs.end(); m_itr++)
	{
		u64 docid = m_itr->first;
		
		crt_itr = mDocs.find(docid);
		if(crt_itr == mDocs.end())
		{
			mDocs.insert(make_pair(docid, m_itr->second));
		}
		else
		{
			crt_itr->second = m_itr->second;
		}
		
		set<u64>::iterator d_itr = mDeletedDocs.find(docid);
		if(d_itr != mDeletedDocs.end())	mDeletedDocs.erase(d_itr);
	}
	
	set<u64> delete_docs = snap->getDeletedDocs();
	set<u64>::iterator s_itr; 
	for(s_itr = delete_docs.begin(); s_itr != delete_docs.end(); s_itr++)
	{
		u64 docid = *s_itr;
		
		crt_itr = mDocs.find(docid);
		if(crt_itr != mDocs.end())
		{
			mDocs.erase(crt_itr);
			//continue;	
			// this is merge. means this doc need delete. need add to mDeletedDocs.
			// if docid in merger_snap's status is deleted.
			// then in DocFileMgr It must be exist.
		}
		
		mDeletedDocs.insert(docid);
	}
	
	return true;
}


bool
AosSnapInfo::serializeFrom(const AosBuffPtr &buff)
{
	return false;
}


bool
AosSnapInfo::serializeTo(const AosBuffPtr &buff)
{
	return false;
}

/*	
bool
AosSnapInfo::serializeFrom(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	mSnapId = buff->getU32(0);
	
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
	return true;
}


bool
AosSnapInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	mLock->lock();
		
	OmnScreen << "DfmInfo:: serializeTo snap:" 
		<< "; snap_id:" << mSnapId
		<< "; doc num:" << mDocInfo.size() 
		<< endl;

	buff->setU32(mSnapId);
	
	buff->setU32(mDocInfo.size());
	map<u64, AosDocInfoPtr>::iterator doc_itr = mDocInfo.begin();
	for(; doc_itr != mDocInfo.end(); doc_itr++)
	{
		AosDocInfoPtr doc = doc_itr->second;
		aos_assert_r(doc, false);
		OmnScreen << "DfmInfo:: SnapInfo:: serialize doc:" 
			<< "; docid:" << doc->getDocid() 
			<< "; pattern:" << doc->getPattern() 
			<< endl;
		doc->serializeTo(buff);
	}
	
	mLock->unlock();
	
	return true;
}

*/
