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
#include "LogServer/SFLog.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "util_c/strutil.h"
#include <iostream.h>
#include "Porting/GetTime.h"
#include "util2/value.h"


AosSFLog::AosSFLog(const OmnDbRecordPtr &record)
:
AosLog(record),
mLock(OmnNew OmnMutex())
{
    OmnRslt rslt;	
	AosLogFilePtr logfile = OmnNew AosLogFile();
	OmnString filename = 	record->getStr(10, "NoName", rslt);
	u64 maxSize =   		record->getU64(11, 0, rslt);
	u64 filesize =          record->getU64(12, 0, rslt);
	int truncatemethod =    record->getInt(13, 0, rslt);
	u64 totalentries =      record->getU64(14, 0, rslt);

	logfile->setFile(filename);
	logfile->setMaxFileSize(maxSize);
	logfile->setFileSize(filesize);
	logfile->setTruncateMethod(truncatemethod);
	logfile->setTotalEntries(totalentries);
	mLogFile = logfile;
	
}


bool
AosSFLog::setLogFile(const OmnString &logid, const u64 &maxsize, const int &truMethod)
{   
    aos_assert_r(logid != "", false);
	aos_assert_r(maxsize >= eMinFilesize, false);
	OmnString filename;
	(filename = logid) << ".txt";
	mLogFile = OmnNew AosLogFile(filename, maxsize, truMethod);
	return true;
} 


bool 
AosSFLog::procReq(const AosNetRequestPtr &request)
{
	// Process request!
	const OmnString oper = request->getOperator();
OmnTrace << "++++++++++++Operator is : " << oper << endl;
	// reset Log and add a record in the file
	if (oper == "appStarted")
	{
		// 	<request ... operator="appStarted"/>
		mLock->lock();
		aos_assert_g(openLog(), unlock_return_false);

		//  get contents from request
		TiXmlNode *logentry = request->getContents();
		aos_assert_gm(mLogFile->addLogEntry(logentry), 
			unlock_return_false, "Failed to add log entry");
		aos_assert_gm(updateLog(), unlock_return_false);
		mLock->unlock();
		return true;	
	}

	/*
	if(!getStatus())
    {
        OmnAlarm << "Log is closed and can not process request!" << enderr;
        return false;
    }
	*/
	
	// Get contents from request and add logentry to file
	if (oper == "add")
	{
		TiXmlNode *logentry = request->getContents();
		mLock->lock();
		if (!getStatus())
		{
			OmnAlarm << "Log is closed!" << enderr;
			mLock->unlock();
			return false;
		}

		aos_assert_r(mLogFile->addLogEntry(logentry), false);
		aos_assert_r(updateLog(), false);
		mLock->unlock();
		return true;
	}
/*

	if (oper == "getPage")
	{
		//<request logid="xxx" operation="set_page_size" page_size="200"/>
		//<request logid="xxx" operation="get_page" page_size="200" page_num="nnn"/>
		OmnString pageSize = request->getPageSize();
		OmnString pagenum  = request->getPageNum();
		getPage(pageSize, pageNum);
		
	}
*/
	if (oper == "reconnect")
	{
		//It treats the request the same as Restarting a log except that 
		//it will create a record in the database indicating that 
		//a continuation of a log has been created.At the time of retrieving logs, 
		//the reconnected logs are copied to the original log 
		//and are concatenated to the original log
		
		//aos_assert_r(createLogRecord(), false);
		aos_assert_r(openLog(), false);
		aos_assert_r(updateLog(), false);
        OmnNotImplementedYet;
		return true;
	}

	if (oper == "closeLog")
	{
		aos_assert_r(closeLog(), false);
		return true;
	}

	if (oper == "appStopped")
	{
		TiXmlNode *logentry = request->getContents();
        aos_assert_r(mLogFile->addLogEntry(logentry), false);
		if(getLogIdType() == 'a' || getLogIdType() == 'c')
		{
			aos_assert_r(closeLog(), false);
		}
        aos_assert_r(updateLog(), false);
        return true;
	}

	if (oper == "delete")
	{
		aos_assert_r(destroyLog(), false);
		return true;	
	}

    OmnNotImplementedYet;
	return true;

unlock_return_false:
	mLock->unlock();
	return false;
}


bool
AosSFLog::createLog(TiXmlNode *config)
{
	// <request ...>
	//      <config ...>
	//          ...
	//      </config>
	// </request>
	// config Log!
	aos_assert_r(configLog(config), false);
	
	// define logid!
	if(mLogIdType == 'a' || mLogIdType == 'b' || mLogIdType == 'c')
	{
		mLogId << "sf" << mAppName;
	}
	else 
	{
		OmnAlarm << "Invalid logid type: " << mLogIdType << enderr;
        return false;
	}

	// create LogFile!
	TiXmlElement *logconf = config->FirstChild()->ToElement();
    u64 maxSize;
    aos_assert_r(!aos_value_atoull(logconf->Attribute("maxSize"), &maxSize), false);
	int truMethod = aos_atoi_dft(logconf->Attribute("truncateMethod"), 1);
    aos_assert_r(setLogFile(mLogId, maxSize, truMethod), false);

	// Insert Log!
	if(checkLogExist(mLogId))	
	{
		OmnAlarm << "The application had a logid before!" << enderr;
		return false;
	}
	aos_assert_r(insertLog(), false);

	// Add record into file!	
		
	return true;

}


AosLogPtr
AosSFLog::createInstanceLog(const OmnString &appInstance)
{
    mLogId << "_" << appInstance;
	mAppInstance = appInstance;

    // create LogFile!
   	OmnString filename;
    (filename = mLogId) << ".txt";
    mLogFile->createFile(filename); 

    // Insert Log!
    if(checkLogExist(mLogId))
    {
        OmnAlarm << "The application had a logid before!" << enderr;
	    return false;
	}
	aos_assert_r(insertLog(), false);

    // Add record into file!

    return this;
}


bool
AosSFLog::insertLog() const
{
	OmnString stmt = "insert into sflog (";
    stmt << "log_id, logid_type, app_name, app_instance, "
		 << "level, status, reset, security_level, "
	     << "reliability, archive, file_name, max_size, "	
		 << "file_size, truncate_method, total_entries) "
		 << "values ('";
    stmt << mLogId << "', '"
		 << mLogIdType <<"', '"
		 << mAppName << "', '"
		 << mAppInstance << "', " 
		 << mLevel << " , "
		 << mStatus << ", "
		 << mReset << ", "
		 << mSecurityLevel << ", " 
		 << mReliability << ", "
		 << mArchive << ", '"
	     << mLogFile->getFile()->getFileName() << "', "
		 << mLogFile->getMaxFileSize() << ", "
		 << mLogFile->getFileLength() << ", "
		 << mLogFile->getTruncateMethod() << ", "
		 << mLogFile->getTotalEntries() << ")";
 
    OmnTrace << "To insert record: " << stmt << endl;
 
    OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
    OmnRslt rslt = store->runSQL(stmt);
    if (!rslt)
    {
		OmnAlarm << "Failed to insert into the database!" << enderr;
        return false;
    }

    return true;
}


bool
AosSFLog::deleteLog()
{
	//It create a SQL statement to delete a record
	
	OmnString stmt = "delete from sflog where log_id = '";
    stmt << mLogId << "'";
	
	OmnTrace << "To delete record: " << stmt << endl;

    OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
    OmnRslt rslt = store->runSQL(stmt);
    if (!rslt)
    {
        OmnAlarm << "Failed to delete the record!" << enderr;
        return false;
    }

    return true;
}


bool
AosSFLog::updateLog() 
{
	OmnString stmt = "update sflog set status = ";
	stmt << mStatus << ", file_size = ";
	stmt << mLogFile->getFileLength() << ", total_entries = ";
	stmt << mLogFile->getTotalEntries();
	stmt << " where log_id = '";
    stmt << mLogId << "'";

    OmnTrace << "To update a record: " << stmt << endl;

    OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
    OmnRslt rslt = store->runSQL(stmt);
    if (!rslt)
    {
        OmnAlarm << "Failed to update the element!" << enderr;
        return false;
    }

    return true;

}


bool 
AosSFLog::destroyLog()
{
	aos_assert_r(mLogFile->deleteFile(), false);
   	aos_assert_r(deleteLog(), false);
	return true;
}


bool 
AosSFLog::openLog()
{
	// Reset Log.
	// If the log is not opened yet, this function will open the log.
	if(getReset() == eApplicationRestart)		
	{
		if(getLogIdType() == 'a' || getLogIdType() == 'c')
		{
			aos_assert_r(mLogFile->clearFile(), false);
		}
		else if(getLogIdType() == 'b')
		{
			if (!mLogFile->getFile()->isGood())
			{
				aos_assert_r(mLogFile->openFile(), false);
			}
		}
		else 
	    {
   			OmnAlarm << "Invalid logid type: " << getLogIdType() << enderr;
        	return false;
    	}

		setStatus(true);
		return true;
	}
	else if(getReset() == eNever)
	{
		//A log entry is inserted to indicate that a new instance was started. 	
		if (!mLogFile->getFile()->isGood())
        {
        	aos_assert_r(mLogFile->openFile(), false);
        }
		setStatus(true);
		return true;
	}
	else
	{
		OmnAlarm << "Invalid reset type: " << getReset() << enderr;
		return false;
	}
	
	OmnShouldNeverComeToThisPoint;
	return true;
}


bool
AosSFLog::closeLog()
{
	aos_assert_r(mLogFile->closeFile(), false);
	setStatus(false);
	return true;
}

/*
setPageSize()
{
<request logid="xxx" operation="set_page_size" page_size="200"/>
}
<request logid="xxx" operation="get_page" page_size="200" page_num="nnn"/>

OmnString
AosSFLog::getPage(const int pageNum, const int pageSize)
{
	// It will return an xml:
	// 	<page remain_pages="nnn">
	// 		<contents>
	// 			<entry ...>
	// 			<entry ...>
	// 			<entry ...>
	// 		</contents>
	// 	</page>
	mLogFile->getPage(mLogId);
	OmnString page = mLogFile->getPage(pageNum, pageSize);
	return page;
}

*/
#endif
