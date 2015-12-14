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

AosSnapInfo::AosSnapInfo(const u32 snap_id, map<u64, AosDocInfoPtr> &doc_infos)
:
mLock(OmnNew OmnMutex()),
mSnapId(snap_id)
{
	map<u64, AosDocInfoPtr>::iterator itr = doc_infos.begin();
	for(; itr != doc_infos.end(); itr++)
	{
		u64 docid = itr->first;
		AosDocInfoPtr doc = itr->second;
		AosDocInfoPtr new_doc = doc->clone();
		
		mDocInfo.insert(make_pair(docid, new_doc));
	}
	
	OmnScreen << "New SnapInfo: snapid:"
		<< snap_id
		<< "; doc_num:" << mDocInfo.size()
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

	
void
AosSnapInfo::removeDocInfo()
{ 
	mLock->lock();
	mDocInfo.clear();
	mLock->unlock();
};

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



