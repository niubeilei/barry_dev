////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmLogger.h
// Description:
//	This is an interface used by AlarmMgr to log alarms.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Alarm_AlarmLogger_h
#define Omn_Alarm_AlarmLogger_h

#include "Alarm/Ptrs.h"
#include "Alarm/AlarmLogProc.h"
#include "Thread/Ptrs.h"
#include "Util/Object.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#ifndef OmnLogAlarm
#define OmnLogAlarm(errmsg, alarm_id) OmnAlarmLogger::logAlarm(__FILE__, __LINE__, (alarm_id), (errmsg)
#endif

class OmnAlarmLogger : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	static OmnString	mDirname;
	static OmnString	mFname;
	static OmnString	mModuleId;
	static OmnFilePtr	mLogFile;
	static OmnMutexPtr	mLock;
	static u64 			mCrtId;
	static int			mNumIds;

	enum
	{
		eLogIdStart = 100,
		eUpdateSize = 1000
	};

public:
	OmnAlarmLogger()
	{
	}

	virtual ~OmnAlarmLogger() {}

	static bool init(const AosXmlTagPtr &config);
	static void setLogProc(const OmnAlarmLogProcPtr &proc);
	static bool getLogIds();
	static OmnString logAlarm(
					const OmnString &fname, 
					const int line, 
					const u64 &alarm_id,
					const OmnString &contents);
	static void setProc(const OmnAlarmLogProcPtr &proc);
};

#endif
#endif
