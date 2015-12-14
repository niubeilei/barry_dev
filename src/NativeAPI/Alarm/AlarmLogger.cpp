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
// The alarm logger creates a log entry and sends it to the system logger.
// Each log entry will have system-wide log id.
//
// Modification History:
// 12/31/2011	Re-Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Alarm/AlarmLogger.h"

#include "Alarm/Alarm.h"
#include "Alarm/AlarmLogProc.h"
#include "Debug/Debug.h"
#include "SEUtil/Objid.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


const int sgAlarmBuffSize = 3000;
static OmnAlarmLogProcPtr sgLogProc;
OmnString	OmnAlarmLogger::mDirname;
OmnString	OmnAlarmLogger::mFname;
OmnString	OmnAlarmLogger::mModuleId;
OmnFilePtr	OmnAlarmLogger::mLogFile;
OmnMutexPtr	OmnAlarmLogger::mLock;
u64 		OmnAlarmLogger::mCrtId = 0;
int			OmnAlarmLogger::mNumIds = 0;


bool
OmnAlarmLogger::init(const AosXmlTagPtr &config)
{
	mLock = OmnNew OmnMutex();
	aos_assert_r(config, false);
	mDirname = config->getAttrStr("dirname");
	aos_assert_r(mDirname != "", false);
	mFname = config->getAttrStr("alarmlog_fname", "alarmlog_fname");
	aos_assert_r(mFname != "", false);
	mModuleId = config->getAttrStr("module_id");
	// if (mModuleId == "")
	// {
	// 	mModuleId << OmnApp::getAppName();
	// }

	// Open the Alarm Log File
	OmnString fname = mDirname;
	fname << "/" << mFname;
	mLogFile = OmnNew OmnFile(fname, OmnFile::eWriteCreate);
	if (!mLogFile->isGood()) mLogFile = 0;
	getLogIds();
	return true;
}


bool
OmnAlarmLogger::getLogIds()
{
	mLock->lock();
	if (mLogFile)
	{
		OmnString ss = mLogFile->readStr(eLogIdStart, 20, "");
		if (ss != "")
		{
			mCrtId = atoll(ss);
			u64 nn = mCrtId + eUpdateSize;
			ss = "";
			ss << nn;
			mLogFile->put(eLogIdStart, ss, ss.length(), true);
			mNumIds = eUpdateSize;
			mLock->unlock();
			return true;
		}
	}

	if (mCrtId == 0) mCrtId = OmnGetSecond();
	mLock->unlock();
	return true;
}


OmnString
OmnAlarmLogger::logAlarm(
		const OmnString &fname, 
		const int line, 
		const u64 &alarm_id,
		const OmnString &contents)
{
	// It creates an alarm entry. The format is:
	OmnString str = "<alarm";
	OmnString ctnr_objid = AosObjid::composeAlarmLogCtnrObjid();
	mLock->lock();
	if (mNumIds <= 0) getLogIds();

	OmnString entry_id = mModuleId;
	entry_id << "_" << mCrtId++;
	mNumIds--;
	
	str << AOSTAG_FILENAME << "=\"" 
		<< fname << "\" "<< AOSTAG_LINE << "=\"" 
		<< line << "\" " << AOSTAG_ID << "=\"" 
		<< alarm_id << "\"" << AOSTAG_PARENTC << "=\"" << ctnr_objid 
		<< alarm_id << "\"" << AOSTAG_ENTRY_ID << "=\"" << entry_id
		<< "\"><![CDATA[" << contents << "]]></alarm>";

	if (sgLogProc) 
	{
		try
		{
			AosXmlTagPtr log = AosXmlParser::parse(str AosMemoryCheckerArgs);
			sgLogProc->addLog(ctnr_objid, log);
		}

		catch (...)
		{
			OmnAlarm << "Failed creating log entry!" << enderr;
			mLock->unlock();
			OmnString id = mModuleId;
			return entry_id;
		}
	}

 	if (OmnTracerSelf)
 	{
 		OmnTracerSelf->writeAlarmEntry(contents.data(), contents.length());
 	}
	mLock->unlock();
 	return entry_id;
}


void
OmnAlarmLogger::setProc(const OmnAlarmLogProcPtr &proc)
{
	sgLogProc = proc;
}
#endif
