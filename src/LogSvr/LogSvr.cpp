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
// Modification History:
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogSvr/LogSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "LogSvr/LogReq.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"

const OmnString sgIndexInfo = "indexinfo";
const OmnString sgIndexPrefix = "idx";
const OmnString sgLogFilePrefix = "idx";
const u64 sgMaxLogfiles = 1000000;
static AosDocFileMgr	sgDocFileMgr;

OmnSingletonImpl(AosLogSvrSingleton,
                 AosLogSvr,
                 AosLogSvrSelf,
                "AosLogSvr");

AosLogSvr::AosLogSvr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosLogSvr::~AosLogSvr()
{
}


bool
AosLogSvr::start()
{
	return true;
}


bool
AosLogSvr::start(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr log_config = config->getFirstChild(AOSTAG_LOGCONFIG);
	aos_assert_r(log_config, false);

	mLogDir = log_config->getAttrStr(AOSTAG_LOGDIR);
	aos_assert_r(mLogDir != "", false);

	aos_assert_r(sgDocFileMgr.init(mLogDir, sgLogFilePrefix, sgMaxLogfiles), false);

	// Open the index info file to retrieve the current index file seqno
	OmnString fname = mLogDir;
	fname << "/" << sgIndexInfo;
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	aos_assert_r(ff->isGood(), false);

	// Index Info File:
	// 		Current Index Seqno
	aos_assert_r(ff->seek(eAosCurrentIndexSeqnoOffset), false);
	bool finished;
	OmnString seqno_str = ff->getLine(finished);
	aos_assert_r(seqno_str != "", false);
	mCrtSeqno = atoi(seqno_str.data());
	aos_assert_r(mCrtSeqno >= 0, false);

	return true;
}


bool
AosLogSvr::stop()
{
	sgDocFileMgr.stop();
    return true;
}


OmnRslt
AosLogSvr::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosLogSvr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosLogSvr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosLogSvr::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
	AosLogReqPtr req;
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mQueue.size() == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		req = mQueue.front();
		mQueue.pop();
		mLock->unlock();

		switch (req->getReqid())
		{
		case AosLogReq::eCreateLog:
			 createLogPriv(req);
			 break;

		default:
			 OmnAlarm << "Unrecognized log req: " << req->getReqid() << enderr;
			 break;
		}
	}

	return true;
}


bool
AosLogSvr::createLogPriv(const AosLogReqPtr &req)
{
	// This function creates a log. 
	// 1. Save the log into the file.
	// 2. Update the log index entry
	AosXmlTagPtr log = req->getDoc();
	u64 logid = log->getAttrU64(AOSTAG_LOGID, 0);
	aos_assert_r(logid != 0, false);
	u32 seqno = 0;
	u64 offset = 0;
	OmnString docstr = log->toString();
	const char *data = docstr.data();
	const int len = docstr.length();
	bool rslt = sgDocFileMgr.saveDoc(seqno, offset, len, data);
	aos_assert_r(rslt, false);

	u32 idx_seqno = logid / eMaxLogsPerIndex;
	u64 idx_offset = (logid % eMaxLogsPerIndex) * eIndexRecordSize;
	map<u32, OmnFilePtr>::iterator itr = mIdxFiles.find(idx_seqno);
	OmnFilePtr ff;
	if (itr == mIdxFiles.end())
	{
		ff = openIndexFile(idx_seqno);
		aos_assert_r(ff, false);
	}
	else
	{
		ff = itr->second;
	}
	ff->setU64(idx_offset, seqno, false);
	ff->setU64(idx_offset + 8, offset, true);

	int ttl = 0;
	AosDocServer::getSelf()->createDocSafe3(req->getRundata(), req->getDoc(), 
			req->getCloudid(), "", true, true, false, false, false, true, false, ttl);
	return true;
}


bool
AosLogSvr::createLog(
		const AosXmlTagPtr &log, 
		const AosRundataPtr &rdata)
{
	// This function creates a log. It obtains a logid for the log, 
	// set the logid to the log doc; check whether the log has a container. 
	// If not, it is an error. Otherwise, it saves the log.
	if (!log)
	{
		rdata->setError() << "Log is null";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString pctnr = log->getAttrStr(AOSTAG_PARENTC);
	if (pctnr == "")
	{
		rdata->setError() << "Missing parent container";
		return false;
	}

	mLock->lock();
	u64 logid = getLogidPriv();
	log->setAttr(AOSTAG_LOGID, logid);
	AosLogReqPtr req = OmnNew AosLogReq(AosLogReq::eCreateLog, log, rdata);
	mQueue.push(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


u64
AosLogSvr::getLogidPriv()
{
	// This function retrieves the next log id. It checks whether 
	// there is an active index file. If not, it creates one. 
	// It then retrieves the size of the active log file. The next
	// logid is calculated from the active log index file size. 
	OmnFilePtr ff;
	map<u32, OmnFilePtr>::iterator itr = mIdxFiles.find(mCrtSeqno);
	if (itr == mIdxFiles.end())
	{
		ff = openIndexFile(mCrtSeqno);
		aos_assert_r(ff, false);
	}
	else
	{
		ff = itr->second;
	}

	u64 length = ff->getLength();
	aos_assert_r(length > 0 && (length % eIndexRecordSize) == 0, false);
	ff->setU64(length, 0, false);
	ff->setU64(length + 8, 0, false);
	return mCrtSeqno * eMaxLogsPerIndex + (length / eIndexRecordSize);
}


OmnFilePtr
AosLogSvr::openIndexFile(const u32 seqno)
{
	// This is a private function. It opens the index file. 
	aos_assert_r(mLogDir != "", false);

	// Open the index file
	OmnString fname = mLogDir;
	fname << "/" << sgIndexPrefix << "_" << seqno;
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	if (!ff->isGood())
	{
		ff = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(ff->isGood(), false);
	}

	mIdxFiles[seqno] = ff;
	return ff;
}

