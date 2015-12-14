/////////////////////////////////////////////////////////////////////////
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
// 03/20/2009: Created by Sally Sheng
// 12/31/2012: Turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogServer/Log.h"
#include "LogServer/SFLog.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"


AosLog::AosLog(const OmnDbRecordPtr &record)
{
	OmnRslt rslt;
    mLogId = record->getStr(0, "NoLogid", rslt);
    mLogIdType = record->getChar(1, 'n', rslt);
    mAppName = record->getStr(2, "NoAppName", rslt);
    mAppInstance = record->getStr(3, "NoAppinstance", rslt);
    mLevel = (Level)record->getInt(4, 0, rslt);
    mStatus = record->getBool(5, -1, rslt);
    mReset = (Reset)record->getInt(6, 0, rslt);
    mSecurityLevel = record->getInt(7, 0, rslt);
    mReliability = record->getInt(8, 0, rslt);
    mArchive = (Archive)record->getInt(9, 0, rslt);
}


bool 
AosLog::configLog(TiXmlNode *config)
{
    TiXmlElement *log = config->ToElement();
	mAppName = log->Attribute("appName");
    mAppInstance = log->Attribute("appInstance");
	
	TiXmlElement *logconf = log->FirstChild()->ToElement();
	mLogIdType = (logconf->Attribute("logIdType"))[0];
    mLevel = (Level)aos_atoi_dft(logconf->Attribute("level"), 1);
	mStatus = true;
    //mStatus = aos_a2bool(logconf->Attribute("status"), true);
    mReset = (Reset)aos_atoi_dft(logconf->Attribute("reset"), 1);
    mSecurityLevel =aos_atoi_dft(logconf->Attribute("securityLevel"), 1);
    mReliability = aos_atoi_dft(logconf->Attribute("reliability"), 1);
    mArchive = (Archive)aos_atoi_dft(logconf->Attribute("archive"), 1);

	return true;
}


AosLogPtr
AosLog::createLogInDb(const AosNetRequestPtr &request)
{
	TiXmlNode *config = request->getContents();
	TiXmlElement *logconf = config->FirstChild()->ToElement();
	AosLogType logtype = (AosLogType)aos_atoi_dft(logconf->Attribute("logType"), 1);

	AosLogPtr log;
	if(logtype == eAosLogType_SFLog)
	{
		log = OmnNew AosSFLog();
		if(!log->createLog(config))
		{
			OmnAlarm << "Failed to create SFLog! " << enderr;
			return 0;
		}
		return log;
	}

	else if(logtype == eAosLogType_RounRobin)
   	{
       	OmnNotImplementedYet;
		return 0;
   	}

   	else if(logtype == eAosLogType_Multiple)
   	{
       	OmnNotImplementedYet;
		return 0;
   	}
	else 
	{
		OmnAlarm << "Unrecognized logtype: " << logtype << enderr;
        return 0;
    }

	OmnShouldNeverComeToThisPoint;
   	return 0;
}


bool  
AosLog::checkLogExist(const OmnString &logid)
{
	// Retrieve all the records from the table.
	OmnString stmt;	
	if(logid.hasPrefix("sf"))
    {  
		stmt = "select * from sflog where log_id = '";
		stmt << logid << "'"; 
	}
	else if( logid.hasPrefix("RoundRobin"))
	{
	    OmnNotImplementedYet;
	}   
	else if(logid.hasPrefix("MutiApp"))
	{
	    OmnNotImplementedYet;
	}   
	else
	{
		OmnAlarm << "Logid does not exist: " << logid << enderr;
		return false;
	}

   	OmnDbTablePtr records;
   	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
   	OmnRslt rslt = store->query(stmt, records);
   	if (!rslt)
   	{
       	OmnAlarm << "Failed to query the database!" << enderr;
       	return false;
   	}

   	if(records->entries() == 1) 
		return true;
   	else 
		return false;
}


AosLogPtr
AosLog::createLogFromDb(const OmnString &logid)
{
	OmnString stmt;
	if(logid.hasPrefix("sf"))
    {
        stmt = "select * from sflog where log_id = '";
        stmt << logid << "'";
    }
 
    else if( logid.hasPrefix("RoundRobin"))
    {   
        OmnNotImplementedYet;
    }   
     
    else if(logid.hasPrefix("MutiApp"))
    {
        OmnNotImplementedYet;
    }   

    OmnDbTablePtr records;
    OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
    OmnRslt rslt = store->query(stmt, records);
    if (!rslt)
    {
         OmnAlarm << "Failed to query the database!" << enderr;
         return 0;
    }

    records->reset();
    aos_assert_r(records->entries() == 1, false);
    OmnDbRecordPtr record = records->next();
    AosLogPtr log = OmnNew AosSFLog(record);
    return log;
}
#endif
