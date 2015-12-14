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
//	This class is used to manage log and version files, 
//	All documents are stored in 'mTransFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mTransFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILProcMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILUtil/IILLog.h"
#include "IILClient/IILProc.h"
#include "Porting/Sleep.h"
#include "SEUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Util/File.h"


#include <dirent.h>
#include <sys/types.h>

static int sgnum = 0;

OmnSingletonImpl(AosIILProcMgrSingleton,
				AosIILProcMgr,
				AosIILProcMgrSelf,
				"AosIILProcMgr");


AosIILProcMgr::AosIILProcMgr()
:
mSaveFlag(false),
mLock(OmnNew OmnMutex()),
mQueueLock(OmnNew OmnMutex()),
mProcLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mCondVar2(OmnNew OmnCondVar()),
mCondVar3(OmnNew OmnCondVar()),
mBatchFreq(eDftBatchFreq),
mNumTrans(0),
mLastSaveSec(0),
mNumTransPerBatch(eDftNumTransPerBatch),
mNeedToSwitch(false)
{
}


AosIILProcMgr::~AosIILProcMgr()
{
	if (mThread) mThread->stop();
	OmnThreadMgr::getSelf()->removeThread(mThread);
	mThread = 0;
}


bool
AosIILProcMgr::start(const AosXmlTagPtr &config)
{
	mNumTransPerBatch = config->getAttrInt("trans_per_batch", eDftNumTransPerBatch);
	mBatchFreq = config->getAttrInt("iil_batch_freq", eDftBatchFreq);

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "log", 0, true, true, __FILE__, __LINE__);
	mThread->start();

	AosXmlTagPtr def = config->getFirstChild("iilwrap");
	if (!def)  
	{
		OmnAlarm << "Fail to find iilwrap" << enderr;
		return false;
	}

	OmnString datadir = config->getAttrStr("dirname");
	aos_assert_r(datadir != "", false);
	mDirName = def->getAttrStr("iildir");
	if (mDirName == "")
	{
		mDirName = datadir;
		mDirName << "/IILData";
	}
	
	OmnString cmd = "mkdir -p ";
	cmd << mDirName;
	system(cmd);
	DIR *dir;
	if ((dir = opendir(mDirName)) == NULL)
	{
	 	OmnAlarm << "iilwrap file not created!" << enderr;
	 	closedir(dir);
	 	return false;
	}

	// Configure the archive dirname
	mArchivename = def->getAttrStr("iilarchivedir");
	if (mArchivename == "")
	{
		mArchivename = datadir;
		mArchivename << "/IILArchive";
	}

	cmd = "mkdir -p ";
	cmd << mArchivename;
	system(cmd);
	if ((dir = opendir(mArchivename)) == NULL)
	{
		OmnAlarm << "Failed to create IIL Archive directory" << enderr;
		closedir(dir);
		return false;
	}

	mFileName = def->getAttrStr("file");
	if (mFileName == "")
	{
		mFileName = "iil_log";
	}

	mMasterProc = OmnNew AosIILProc(mDirName, mArchivename, eNumProcs);
	return true;
}


bool
AosIILProcMgr::stop()
{
	return true; 
}


bool    
AosIILProcMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
	case 0:
		 return timerThreadFunc(state, thread);

	default:
		 OmnAlarm << "Incorrect thread id: " 
			 << thread->getLogicId() << enderr;
		 return false;
	}

	return false;
}

	
bool
AosIILProcMgr::timerThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// This thread waits for a given number of seconds. When wakes up,
	// it saves the current log, and goes back to sleep again. 
	while (state == OmnThrdStatus::eActive)
	{
		try
		{
			OmnSleep(mBatchFreq);
			mLock->lock();
			saveLog();
			mLock->unlock();
		}

		catch (const OmnExcept &e)
		{
			OmnAlarm << "Caught an exception: " << e.getErrmsg()
				<< ". Please check the supporting team to solve"
				<< " this alarm. This class will continue!" << enderr;
		}
	}
	return true;
}


bool
AosIILProcMgr::signal(const int threadLogicId)                              
{
	return true;
}


bool
AosIILProcMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool 
AosIILProcMgr::appendLog(const u64 &iilid, AosBuff &buff)
{
	mLock->lock();
	aos_assert_rl(mMasterProc, mLock, false);
	buff.reset();
	mMasterProc->appendLog(iilid, buff);
	mLock->unlock();
	return true;
}


bool
AosIILProcMgr::docTransStarted(const u32 transid, const bool flag)
{
	// It sets the current transaction id. This is the transaction
	// being processed. The function is called by AosDocServer.
	// If 'flag' is true, it means that it needs to wrap up
	// the current batch and starts the next batch. 
	mLock->lock();
	if (flag || mNeedToSwitch)
	{
		mMasterProc->switchLogs();
		mNeedToSwitch = false;
	}
	mMasterProc->docTransStarted(transid);
	mNumTrans++;
	mTransid = transid;
	mLock->unlock();
	return true;
}


bool
AosIILProcMgr::docTransFinished(const u32 transid)
{
	mLock->lock();
	aos_assert_rl(mTransid == transid, mLock, false);
	mMasterProc->docTransFinished(transid);
	mTransid = 0;
	mLock->unlock();
	return true;
}


bool
AosIILProcMgr::saveLog()
{
	// The caller should have logged the class before calling this
	// function. This function checks whether it should save the
	// logs. If not, it simply returns. 
	u32 sec = OmnGetSecond();
	if (sec < mLastSaveSec)
	{
		OmnAlarm << "Last save second is incorrect: " << mLastSaveSec
			<< enderr;
		mLastSaveSec = 0;
	}

	if (sec - mLastSaveSec >= mBatchFreq || mNumTrans >= mNumTransPerBatch)
	{
		// Time to save
		mLastSaveSec = sec;
		if (mTransid == 0)
		{
			// It is not in the middle of a doc transaction. It can switch
			mMasterProc->switchLogs();
			mNeedToSwitch = false;
		}
		else
		{
			// It is in the middle of a doc transaction. Cannot switch yet.
			mNeedToSwitch = true;
		}
	}
	return true;

}


bool
AosIILProcMgr::creTempFile()
{
	//temp file like this
	//0-2 bytes status
	//3-  transid----
	OmnString fname = mDirName;
	fname << "/" << mFileName << "_temp";

	OmnFilePtr ff;
	ff = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	if (!ff->isGood())
	{
		ff = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(ff && ff->isGood(), 0);
	}

	char temp[eMaxTrans];
	AosBuff buff(temp, eMaxTrans, 0);
	buff.setU32(eStartFlag);
	buff.setU32(0);   // 1-create, 2-save, 3-processes
	buff.setU32(eEndFlag);
	
	ff->put(0, temp, buff.dataLen(), true);
	mTempFile = ff;
	return true;
}

	
bool
AosIILProcMgr::signalSave(int i)
{
	++sgnum;
	if(sgnum == (int)eNumProcs) mSaveFlag = true;
	mCondVar2->signal();
	return true;
}

	
bool
AosIILProcMgr::signalTrans()
{
	mCondVar->signal();
	return true;
}


bool
AosIILProcMgr::changeStats(int i)
{
	//0 do nothing
	//1 hava save log
	//2 hava update seslog
	//3 hava process log
	//
	if(!mTempFile.getPtr()) creTempFile();
	aos_assert_r(mTempFile, NULL);
	mTempFile->setU32(4, i, true);
	return true;
}


bool
AosIILProcMgr::chackStats()
{
	OmnString fname = mDirName;
	fname << "/" << mFileName << "_temp";

	OmnFilePtr ff;
	ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	if (!ff->isGood())
	{
		return 0;
	}

	char temp[50];
	memset(temp, '\0', 50);
	ff->readToBuff(0, 50, temp);
	AosBuff buff(temp, 50, 0);

	u32 startFlag = buff.getU32(0);
	u32 status = buff.getU32(0);
	u32 endFlag= buff.getU32(0);
	if(startFlag!=eStartFlag || endFlag!=eEndFlag) return 0;

	return status;
}


