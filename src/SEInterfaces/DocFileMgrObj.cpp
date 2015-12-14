////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DocFileMgrObj.h"

//AosDocFileMgrObjPtr AosDocFileMgrObj::smCreator;

AosDocFileMgrObj::AosDocFileMgrObj()
{
}


AosDocFileMgrObj::~AosDocFileMgrObj()
{
}


bool
AosDocFileMgrObj::removeFromList()
{
	// This function removes this entry from the list.
	if (mPrevEntry) mPrevEntry->mNextEntry = mNextEntry;
	if (mNextEntry) mNextEntry->mPrevEntry = mPrevEntry;

	mPrevEntry = 0;
	mNextEntry = 0;
	return true;
}


bool
AosDocFileMgrObj::moveToFront(const AosDocFileMgrObjPtr &docFileMgr)
{
	// This function moves 'docFileMgr' to be the previous
	// entry of this object.
	// It is not thread safe!!!!!!!!!!!!!!!!!
	if (this == docFileMgr.getPtr()) return true;
	aos_assert_r(docFileMgr->mPrevEntry && docFileMgr->mNextEntry, false);

	bool rslt = docFileMgr->removeFromList();
	aos_assert_r(rslt, false);

	rslt = insertAt(docFileMgr);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocFileMgrObj::insertAt(const AosDocFileMgrObjPtr &docFileMgr)
{
	// This function inserts 'docFileMgr' in front of this entry.
	aos_assert_r(docFileMgr, false);
	aos_assert_r(mPrevEntry && mNextEntry, false);
	docFileMgr->mPrevEntry = mPrevEntry;
	mPrevEntry->mNextEntry = docFileMgr;
	mPrevEntry = docFileMgr;
	docFileMgr->mNextEntry = this;
	return true;
}


