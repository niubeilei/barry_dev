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
// 03/20/2009 Created by Sally Sheng 
// 12/31/2012 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_SFLog_h
#define AOS_LogServer_SFLog_h

#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "LogServer/Log.h"
#include "LogServer/LogFile.h"
#include "LogServer/LogEntry.h"
#include "util_c/strutil.h"


class AosSFLog :  public AosLog
{

public:
	enum
	{
		eMinFilesize = 1000,
	};

protected:
	AosLogFilePtr  	mLogFile; 
	OmnMutexPtr		mLock;


public:
	AosSFLog(){};
	AosSFLog(const OmnDbRecordPtr &record);
	~AosSFLog(){};
	
	// AosLog Interface
	virtual bool       procReq(const AosNetRequestPtr &request);
	virtual AosLogType getType() const {return eAosLogType_SFLog;}
	virtual bool       createLog(TiXmlNode *config);
	virtual AosLogPtr  createInstanceLog(const OmnString &appInstance);

	AosLogFilePtr      getLogFile() {return mLogFile;}

private:
	bool               destroyLog();
	bool               openLog();
	bool               closeLog();
	bool               addEntry(const AosLogEntryPtr &logentry);
	int                getPage();
	u64                getTotalEntries();


	bool               insertLog() const;
    bool               deleteLog();
    bool               updateLog();
	bool               setLogFile(const OmnString &logid, const u64 &maxsize, const int &truMethod);
	void               setLogFile(const AosLogFilePtr &logfile) {mLogFile = logfile;}

};

#endif
#endif
