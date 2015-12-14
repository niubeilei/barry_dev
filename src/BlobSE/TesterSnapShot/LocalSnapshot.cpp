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
#include "BlobSE/Tester/LocalSnapshot.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "BlobSE/Tester/LocalDoc.h"

AosLocalSnapshot::AosLocalSnapshot(
		const u32 snap_id,
		const u64 max_docid)
:
//mLock(OmnNew OmnMutex()),
mSnapId(snap_id),
mMaxDocid(max_docid)
{
	OmnScreen << "New SnapInfo: "
		<< "snapid:" << snap_id
		<< endl;
}

AosLocalSnapshot::~AosLocalSnapshot()
{
	map<u64, AosLocalDoc *>::iterator itr;
	for(itr = mDocs.begin(); itr != mDocs.end(); itr++)
	{
		OmnDelete itr->second;
	}
}


AosLocalDoc *
AosLocalSnapshot::addDoc(const u64 docid)
{
	aos_assert_r(mMaxDocid <= docid, 0);
	mMaxDocid = docid;

	AosLocalDoc * doc = OmnNew AosLocalDoc(docid);
	mDocs.insert(make_pair(docid, doc));

	doc->resetData();
	return doc;
}


AosLocalDoc *
AosLocalSnapshot::randModifyDoc()
{
	u32 docid = (rand() % mMaxDocid) + 1;
	
	AosLocalDoc * doc;
	map<u64, AosLocalDoc *>::iterator itr = mDocs.find(docid);

	if(itr != mDocs.end())
	{
		doc = itr->second;
	}
	else
	{
		doc = OmnNew AosLocalDoc(docid);
		mDocs.insert(make_pair(docid, doc));
	}
	
	doc->resetData();
	return doc;	
}


AosLocalDoc *
AosLocalSnapshot::randDeleteDoc()
{
	u32 docid = rand() % mMaxDocid;	
	
	AosLocalDoc * doc;
	map<u64, AosLocalDoc *>::iterator itr = mDocs.find(docid);

	if(itr != mDocs.end())
	{
		doc = itr->second;
	}
	else
	{
		doc = OmnNew AosLocalDoc(docid);
		mDocs.insert(make_pair(docid, doc));
	}
	
	doc->markDeleted();
	return doc;	
}


u64
AosLocalSnapshot::randPickDocid()
{
	return rand() % mMaxDocid;	
}


AosLocalDoc *
AosLocalSnapshot::readDoc(const u64 docid)
{
	AosLocalDoc *doc;
	map<u64, AosLocalDoc *>::iterator itr = mDocs.find(docid);

	if(itr != mDocs.end())
	{
		doc = itr->second;
		return doc;
	}
	
	return 0;
}


bool
AosLocalSnapshot::merge(const AosLocalSnapshotPtr &snap)
{
	map<u64, AosLocalDoc *>& snap_docs = snap->getDocs();
	map<u64, AosLocalDoc *>::iterator m_itr, crt_itr;
	AosLocalDoc *old_doc;
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
			old_doc = crt_itr->second;
			OmnDelete old_doc;
			crt_itr->second = m_itr->second;
		}
	}

	snap->cleanDocs();
	return true;
}


