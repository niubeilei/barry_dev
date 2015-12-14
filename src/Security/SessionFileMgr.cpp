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
// There are a number of session files. When creating a new session, 
// it appends the record to the current session file. Session files are
// circularly used. 
//
// Each session file is in the following format:
// 	<Control area>
// 	<Record 1>
// 	<Record 2>
// 	...
//
// Each record is fixed size. 
//
// Session IDs are random letter strings of 16 characters, with the 
// following changes:
// 	session[eSeqnoByte] 	seqno
// 	session[eRecordId1]		the lower 5 bits
// 	session[eRecordId2]		the next 5 bits
// 	session[eRecordId3]		the next 5 bits (use only 15 bits)
//
// Modification History:
// 10/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Security/SessionFileMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "AppMgr/App.h"
#include "Security/SimpleAuth.h"
#include "SEBase/SecReq.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/XmlRc.h"


const OmnString sgDftSessionFname = "session";

AosSessionFileMgr::AosSessionFileMgr()
:
mLock(OmnNew OmnMutex()),
mCrtSeqno(0),
mCrtRecordId(0)
{
	start();
}


AosSessionFileMgr::~AosSessionFileMgr()
{
}


bool
AosSessionFileMgr::start()
{
	// Session configuration assumes:
	// 	<...>
	// 		<AOSCONFIG_SECURITY>
	// 		</AOSCONFIG_SECURITY>
	// 		...
	// 	</...>

	aos_assert_r(OmnApp::getAppConfig(), false);
	AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_SECURITYMGR);
	aos_assert_r(def, false);

	mDirname = def->getAttrStr(AOSCONFIG_DIRNAME, "");
	mFname = def->getAttrStr(AOSCONFIG_SESSION_FNAME, sgDftSessionFname);

	// Session files are in the form:
	// 		<fname>_<nn>
	// where <nn> is a sequence number. The first one records the current
	// session file seqno and the record id.
	bool rslt = openFile(0);
	aos_assert_r(rslt, false);
	rslt = readCrtSeqno();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSessionFileMgr::setCrtSeqno()
{
	aos_assert_r(mFiles[0], false);
	mFiles[0]->setU32(eSeqnoOffset, mCrtSeqno, false);
	mFiles[0]->setU32(eRecordIdOffset, mCrtRecordId, false);
	return true;
}


bool
AosSessionFileMgr::readCrtSeqno()
{
	aos_assert_r(mFiles[0], false);
	mCrtSeqno = mFiles[0]->readBinaryU32((u32)eSeqnoOffset, 0);
	aos_assert_r(mCrtSeqno < eMaxSeqno, false);
	mCrtRecordId = mFiles[0]->readBinaryU32((u32)eRecordIdOffset, 0);
	aos_assert_r(mCrtRecordId >= 0, false);
	return true;
}


bool
AosSessionFileMgr::getLocation(
		u32 &seqno, 
		u32 &record_id)
{
	mLock->lock();
	seqno = mCrtSeqno;
	if (mCrtRecordId++ >= eMaxRecordId)
	{
		mCrtSeqno++;
		if (mCrtSeqno >= eMaxSeqno)
		{
			mCrtSeqno = 0;
		}
		mCrtRecordId = 0;
		setCrtSeqno();
	}

	seqno = mCrtSeqno;
	record_id = mCrtRecordId;
	mLock->unlock();
	return true;
}


bool
AosSessionFileMgr::openFile(const u32 seqno)
{
	aos_assert_r(seqno < eMaxSeqno, false);
	mLock->lock();
	if (mFiles[seqno])
	{
		mLock->unlock();
		return true;
	}

	// Ketty 2013/05/16
	OmnString fname = OmnApp::getAppBaseDir();
	fname << mDirname;
	//OmnString fname = mDirname;
	if(fname.find('/', true) == fname.length()-1 || fname == "")       
	{
		fname << mFname;
	}
	else
	{
		fname << "/" << mFname;
	}
	fname << "_" << seqno;
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_rl(file, mLock, false);
	if (!file->isGood())
	{
		file = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_rl(file && file->isGood(), mLock, false);
	}
	mFiles[seqno] = file;
	mLock->unlock();
	return true;
}


OmnFilePtr	
AosSessionFileMgr::getFile(const u32 seqno)
{
	if (seqno >= eMaxSeqno) return 0;
	return mFiles[seqno];
}

