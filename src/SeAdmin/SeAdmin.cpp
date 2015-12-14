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
// 	Created: 06/23/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeAdmin/SeAdmin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Porting/Sleep.h"
#include "Query/QueryMgr.h"
#include "SearchEngine/DocServer.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/XmlTag.h"
#include "SEModules/LogMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util/Buff.h"


const OmnString AOSCONFIG_FULLBACKUP_HOUR 	= "fullbkup_hour";
const OmnString AOSCONFIG_FULLBACKUP_MINUTE = "fullbkup_min";


OmnSingletonImpl(AosSeAdminSingleton,
                 AosSeAdmin,
                 AosSeAdminSelf,
                "AosSeAdmin");


AosSeAdmin::AosSeAdmin()
:
mLock(OmnNew OmnMutex()),
mBackupFreq(eDftBackupFreq),
mThreadFreq(eDftThreadFreq),
mLastBackup(0),
mFullBkHour(-1),
mFullBkMinute(-1),
mLastFullBackup(false)
{
}


AosSeAdmin::~AosSeAdmin()
{
}


bool      	
AosSeAdmin::start()
{
	return true;
}


bool
AosSeAdmin::start(const AosXmlTagPtr &config)
{
	if (!config) return true;
	AosXmlTagPtr tag = config->getFirstChild(AOSCONFIG_SYSTEMADMIN);
	if (!tag) return true;

	mBackupDir = tag->getAttrStr(AOSCONFIG_BACKUPDIR);
	mDatapath = tag->getAttrStr(AOSCONFIG_DATAPATH);
	mBackupFreq = tag->getAttrInt(AOSCONFIG_BACKUPFREQ, eDftBackupFreq);
	mLastBackup = OmnGetSecond();
	mFullBkHour = tag->getAttrInt(AOSCONFIG_FULLBACKUP_HOUR, eDftFullBackupHour);
	mFullBkMinute = tag->getAttrInt(AOSCONFIG_FULLBACKUP_MINUTE, eDftFullBackupMinute);

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "SeAdminThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool        
AosSeAdmin::stop()
{
	return true;
}


OmnRslt     
AosSeAdmin::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosSeAdmin::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSeAdmin::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSeAdmin::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(mThreadFreq);
		procAdmin();
	}

	return true;
}


bool
AosSeAdmin::procAdmin()
{
	checkBackup();
	return true;
}


bool
AosSeAdmin::checkBackup()
{
	u32 crtsec = OmnGetSecond();
	if (mLastBackup > 0 && crtsec < (u32)(mLastBackup + mBackupFreq)) 
	{
		return true;
	}

	mLastBackup = crtsec;

	// Backup is done by:
	// 	tar -czvf <filename> <path>/doc_*
	// where <filename> is in the form:
	//     <backuppath>/backup_yyyymmddmmss
	// and <path> points to the search engine data directory
	aos_assert_r(mBackupDir != "", false);
	aos_assert_r(mDatapath != "", false);

	OmnString year, month, day, hour, minute;
	OmnGetTime(year, month, day, hour, minute);
	if (mFullBkHour >= 0)
	{
		int hh = atoi(hour.data());
		if (hh == mFullBkHour)
		{
			if (!mLastFullBackup)
			{
				doFullBackup();
				mLastFullBackup = true;
			}
		}
		else
		{
			mLastFullBackup = false;
		}
	}

	doDataBackup();
	return true;
}


bool
AosSeAdmin::doDataBackup()
{
	OmnString year, month, day, hour, minute;
	OmnGetTime(year, month, day, hour, minute);

	OmnString cmd = "tar czf ";
	cmd << mBackupDir << "/data_" << year << month << day << hour << minute
		<< " " << mDatapath << "/doc_*"
		<< " " << mDatapath << "/version_*"
		<< " " << mDatapath << "/zykielog_*";

	// Lock the server
	//AosIILClient::getSelf()->saveAllIILsPublic(false);
	AosIILClientObj::getIILClient()->saveAllIILsPublic(false);
	OmnScreen << "To backup system: " << cmd << endl;
	system(cmd);
	return true;
}


bool	
AosSeAdmin::shutdownServer(AosXmlRc &errcode, OmnString &errmsg)
{
	OmnScreen << "To stop server!" << endl;

	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgrSelf->stop();
	OmnSleep(1);

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryMgrSelf->stop();

	OmnScreen << "To stop Requester!" << endl;
	AosSeReqProc::stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocServerSelf->exitSearchEngine();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

	AosDocServerSelf = 0;
	AosLogMgrSelf = 0;

	exit(0);
	return true;
}


bool
AosSeAdmin::doFullBackup()
{
	// Need to do the full backup. Note that this is extremely
	// dangerous because it takes lots of disk spaces. 
	// The command is:
	// 	tar -czvf <filename> <path>/*
	OmnString year, month, day, hour, minute;
	OmnGetTime(year, month, day, hour, minute);
	OmnString cmd = "tar czf ";
	OmnString fname = mBackupDir;
	fname << "/full_" << year << month << day << hour << minute;
	// cmd << mBackupDir << "/backup_" << year << month << day << hour << minute
	cmd << fname << " " << mDatapath << "/*";
	AosIILClientObj::getIILClient()->saveAllIILsPublic(false);
	OmnScreen << "To backup system: " << cmd << endl;
	u32 starttime = OmnGetSecond();
	system(cmd);
	u32 endtime = OmnGetSecond();

	AosLogMgr::getSelf()->addSystemBackupLogEntry(AOS_DFTSITEID, 
			AOSSTYPE_FULL_BACKUP, 
			fname,
			starttime,
			endtime, 
			AOSLOGSTATUS_SUCCESS,
			cmd);
	return true;
}

