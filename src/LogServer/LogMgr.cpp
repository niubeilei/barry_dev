//////////////////////////////////////////////////////////////////////////
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
// 03/20/2009: Created by Sharon Shen
// 01/01/2013: Turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogServer/LogMgr.h"

#include "LogServer/Log.h"
#include "LogServer/NetMgr.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"

#include <list>

AosLogMgr::AosLogMgr()
{
}


AosLogMgr::~AosLogMgr()
{
}


AosLogPtr	
AosLogMgr::getLog(const AosNetRequestPtr &request)
{
	OmnString logId = request->getTargetId();
	list<AosLogPtr>::iterator it;
	for (it = mLogList.begin(); it != mLogList.end(); ++it)
	{
		if(logId == (*it)->getLogId())
		{	
			if((*it)->getLogIdType() == 'c')
			{
				AosLogPtr instanceLog = getInstanceLog(*it, request);
				if(!instanceLog)
            	{
                	OmnAlarm << "Fail to get appInstance log!" << enderr;
                	return 0;
            	}
				return instanceLog;
			}
			return *it;
		}
	}

	// We need to see whether the log is defined in database.
	// If not, we need to check the log type and create an instance for it.
	AosLogPtr theLog;
	if (AosLog::checkLogExist(logId))
	{
		theLog = AosLog::createLogFromDb(logId);
		if (!theLog)
	    {
    	    OmnAlarm << "Failed to create Log from database: " << logId << enderr;
    	    return 0;
    	}

		if(theLog->getLogIdType() == 'c')
		{
			AosLogPtr instanceLog = getInstanceLog(theLog, request);
			if(!instanceLog)	
			{
				OmnAlarm << "Fail to get appInstance log!" << enderr;
				return 0;
			}
			return instanceLog;
		}
		mLogList.push_front(theLog);
		return theLog;
	}
	else
	{
		OmnAlarm << "Log Id not exists: " << logId << enderr;
		return 0;
	}

	OmnShouldNeverComeToThisPoint;
	return 0; 
}


AosLogPtr
AosLogMgr::createLog(const AosNetRequestPtr &request)
{
	AosLogPtr theLog;
	theLog = AosLog::createLogInDb(request);
	if (!theLog)
    {
		OmnAlarm << "Failed to create Log in database! " << enderr;
        return 0;
	}
	mLogList.push_front(theLog);
	return theLog;
}


AosLogPtr
AosLogMgr::getInstanceLog(const AosLogPtr &theLog, const AosNetRequestPtr &request)
{
    list<AosLogPtr>::iterator it;
	OmnString instanceLogId;
    instanceLogId << request->getTargetId() << "_" << request->getAppInstance();
    for (it = mLogList.begin(); it != mLogList.end(); ++it)
	{
		if(instanceLogId == (*it)->getLogId())
        {
        	return *it;
        }
    }

    AosLogPtr instanceLog;
    if (AosLog::checkLogExist(instanceLogId))
    {
    	instanceLog = AosLog::createLogFromDb(instanceLogId);
    	if (!instanceLog)
        {
        	OmnAlarm << "Failed to create Log from database: " << instanceLogId << enderr;
            return 0;
        }
		mLogList.push_front(instanceLog);
        return instanceLog;
    }
    else
    {
        OmnString appInstance = request->getAppInstance();
        instanceLog = theLog->createInstanceLog(appInstance);
        if (!instanceLog)
        {
    		OmnAlarm << "Failed to create Log in database: " << instanceLogId << enderr;
        	return 0;
        }
		mLogList.push_front(instanceLog);
        return instanceLog;
    }

	OmnShouldNeverComeToThisPoint;
    return 0;
}
#endif
