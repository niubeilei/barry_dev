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
// 2013/03/21	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransClient/TransIdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/File.h"


AosTransIdMgr::AosTransIdMgr() 
:
mLock(OmnNew OmnMutex())
{
}


AosTransIdMgr::~AosTransIdMgr()
{
}


bool
AosTransIdMgr::start()
{
	bool rslt = checkIdFile();
	aos_assert_r(rslt && mFile, false);
	
	rslt = init();
	aos_assert_r(rslt, false);
	return true;	
}


bool
AosTransIdMgr::checkIdFile()
{
	// check whether TransId file is exist.
	// if not, need notify all SvrProxy's process	
	
	OmnString fname = OmnApp::getTransDir();
	if(fname != "")	fname << "/";
	fname << "TransId";

	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(mFile && mFile->isGood())	return true;

	// means this id file is new. maybe is a new device.
	mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!mFile || !mFile->isGood())
	{
		OmnAlarm << "Failed to open file: " << fname << enderr;
		return false;
	}
	
	bool rslt = notifyIsNewTransClt();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransIdMgr::notifyIsNewTransClt()
{
	//OmnNotImplementedYet;
	OmnScreen << "TransIdMgr notify is new TransClt not implementedYet;" << endl;
	return true;
}


bool
AosTransIdMgr::init()
{
	aos_assert_r(mFile, false);
	mFile->lock();
	u32 offset = 0;
	u64 seq_in_file = 0;
	
	if(mFile->getLength() != 0)
	{
		seq_in_file = mFile->readBinaryU64(offset, 0);
		aos_assert_r(seq_in_file, false);
	}

	// 0 is error check.
	mNextTransSubId = seq_in_file + 1;
	mNumTransId = eNumTransId;
	u64 new_seq = seq_in_file + eNumTransId;

	mFile->setU64(offset, new_seq, true);  
	mFile->unlock();
	return true;
}


AosTransId
AosTransIdMgr::nextTransId()
{
	mLock->lock();
	aos_assert_rl(mNextTransSubId != 0, mLock, AosTransId::Invalid);
		
	u64 seqno = mNextTransSubId++;
	mNumTransId--;

	if(mNumTransId == 0)
	{
		loadNewTransId(seqno);
		mNumTransId = eNumTransId;
	}

	AosTransId trans_id = {AosGetSelfClientId(), AosGetSelfProcId(), seqno};
	mLock->unlock();

	return trans_id;
}


bool
AosTransIdMgr::loadNewTransId(const u64 crt_seqno)
{
	mFile->lock();

	u32 offset = 0;
	u64 seq_in_file = mFile->readBinaryU64(offset, 0);
	aos_assert_r(seq_in_file, false);

	if(crt_seqno != seq_in_file)
	{
		OmnAlarm << "trans seq id mismatch: " << crt_seqno 
				<< ":" << seq_in_file << enderr;
	}

	u64 new_seqno = seq_in_file + eNumTransId;
	mFile->setU64(offset, new_seqno, true);  
	mFile->unlock();

	return true;
}


