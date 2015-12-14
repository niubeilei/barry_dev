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
//	This class has a thread. All log entries are added into a queue. 
//  The thread then distributes the log entries to the corresponding
//  logger. When adding an entry, it will make sure the corresponding
//  logger exists. If not, it will not put the entry into the queue.
//  
//  This class assumes all loggers are managed by this class. When starts,
//  this class will create all the loggers.    
//
// Modification History:
// 2013/03/17 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Logger/LoggerMgr.h"

#include "Alarm/Alarm.h"
#include "Logger/LoggerDb.h"
#include "Logger/LoggerFile.h"
#include "Logger/LoggerStdio.h"
#include "Logger/LoggerGroup.h"
#include "Logger/LogListener.h"
#include "Logger/Logger.h"
#include "Logger/LogEntry.h"
#include "NMS/Nms.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlParser.h"


OmnSingletonImpl(OmnLoggerMgrSingleton,
				 OmnLoggerMgr,
				 OmnLoggerMgrSelf,
				"OmnLoggerMgr");


OmnLoggerMgr::OmnLoggerMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


OmnLoggerMgr::~OmnLoggerMgr()
{
}


bool
OmnLoggerMgr::start()
{
	OmnNmsSelf->addHouseKeepObj(this);
	return true;
}


bool
OmnLoggerMgr::config(const OmnXmlParserPtr &parser)
{
	// 
	// It assumes:
	// 	<LoggerConfig>
	// 		<Log>
	// 			<LogName>
	// 			...
	// 		</Log>
	// 		...
	// 	</Logger>
	//
	
	// 
	//  Retrieve CallLog config
	//
	if (!parser)
	{
		return true;
	}

	OmnXmlItemPtr conf = parser->tryItem(OmnXmlItemName::eLoggerConfig);
	if (conf.isNull())
	{
		return true;
	}

	conf->reset();
	while (conf->hasMore())
	{
		OmnXmlItemPtr logdef = conf->next();

		OmnString logName = logdef->getStr(OmnXmlItemName::eLogName, "NoName");
		OmnString type = logdef->getStr("LogType", "");
		OmnLogType::E logType = OmnLogType::toEnum(type);
		if (!OmnLogType::isValid(logType))
		{
			OmnWarn << OmnErrId::eWarnConfigError
				<< "Incorrect log type: " << type << enderr;
			return false;
		}

		// 
		// Check whether the name is unique
		//
		OmnLoggerPtr logger = getLogger(logName);
		if (logger)
		{
			// 
			// The name has been used by another logger
			//
			OmnAlarm << "Log name not unique: " << logName << enderr;
			return false;
		}

		logger = createLogger(logType, logName, logdef);
		if (!logger)
		{
			// 
			// Failed to create the logger. This should not happen.
			//
			OmnAlarm << "Failed to create the logger: " << logName << enderr;
			return false;
		}

		if (!addLogger(logger))
		{
			// 
			// Failed to add the logger. Most likely, there are too many 
			// loggers. This should not normally happen.
			//
			OmnAlarm << "Failed to add log: " << logName << enderr;
			return false;
		}
	}

	return true;
}


bool 
OmnLoggerMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnLogEntryPtr entry;
    while (state == OmnThrdStatus::eActive)
    {
		mThreadStatus = true;
        mLock->lock();
        if (mQueue.entries() <= 0)
        {
            //
            // No more messages to process.
            //
            mCondVar->wait(mLock);
            mLock->unlock();
            continue;
        }

        //
        // There are something in the InQueue.
        //
        entry = mQueue.popFront();
        mLock->unlock();

		entry->log();
    }
	
	return true;
}


bool
OmnLoggerMgr::addLogger(const OmnLoggerPtr &logger)
{
	//
	// It adds the logger into its list. Make sure the logger is not already
	// in the list. If yes, it should return an error and the logger is 
	// not added. Note that this function may be called by multiple threads.
	// It must use mutex to protect concurrency.
	//
	mLock->lock();
	int i;
	for (i=0; i<eMaxLoggers; i++)
	{
		if (mLoggers[i]->getLoggerName() == logger->getLoggerName())
		{
			mLock->unlock();
			OmnAlarm << "Logger already exists: " << logger->getLoggerName() << enderr;
			return false;
		}
	}

	// 
	// The logger is not in the list. Add it.
	//
	for (i=0; i<eMaxLoggers; i++)
	{
		if (!mLoggers[i])
		{
			// 
			// Found an empty slot. Add the logger in this slot
			//
			mLoggers[i] = logger;
			logger->setLogId(i);
			mLock->unlock();
			return true;
		}
	}

	// 
	// This means there are too many loggers
	//
	mLock->unlock();
	OmnAlarm << "Too many loggers. Maximum allowed: " << eMaxLoggers << enderr;
	return false;
}


bool
OmnLoggerMgr::removeLogger(const OmnLoggerPtr &logger)
{
	//
	// It removes the logger from its list. If the logger is not in the 
	// list, it returns an error. Make sure to use mutex to protect 
	// concurrency.
	//
	return false;
}


OmnLoggerPtr
OmnLoggerMgr::getLogger(const OmnString &name) const
{
	mLock->lock();
	for (int i=0; i<eMaxLoggers; i++)
	{
		if (!mLoggers[i])
		{
			mLock->unlock();
			return 0;
		}

		if (mLoggers[i]->getLoggerName() == name)
		{
			OmnLoggerPtr logger = mLoggers[i];
			mLock->unlock();
			return logger;
		}
	}

	mLock->unlock();
	return 0;
}


void		
OmnLoggerMgr::procHouseKeeping(const int64_t &tick)
{
}


bool
OmnLoggerMgr::stop()
{
	return true;
}


OmnRslt	
OmnLoggerMgr::addEntry(const OmnLogEntryPtr &entry)
{
	// 
	// Retrieve the logID from the entry. If no logger for the ID, 
	// try to create it. If failed, this is an error.
	//
	int id = entry->getLoggerId();
	if (id < 0 || id >= eMaxLoggers)
	{
		return OmnAlarm << OmnErrId::eInvalidLogId
			<< "Invalid LogId: " << id
			<< ". Maximum allowed: " << eMaxLoggers << enderr;
	}

	if (!mLoggers[id])
	{
		return OmnAlarm << OmnErrId::eLoggerIsNull
			<< "Logger not created: " << id << enderr;
	}

	entry->setLogger(mLoggers[id]);
	mLock->lock();
	mQueue.append(entry.getPtr());
	mCondVar->signal();
	mLock->unlock();
	return true;
}


OmnLoggerPtr
OmnLoggerMgr::createLogger(const OmnLogType::E type, 
						   const OmnString &name, 
						   const OmnXmlItemPtr &config)
{
	switch (type)
	{
	case OmnLogType::eLoggerFile:
		 return (OmnNew OmnFileLogger(name, config));

	case OmnLogType::eLoggerDb:
		 return OmnNew OmnDbLogger(name, config);

	case OmnLogType::eLoggerStdio:
		 return OmnNew OmnStdioLogger(name);

	case OmnLogType::eLoggerGroup:
		 return OmnNew OmnGroupLogger(name, config);

	default:
		 OmnAlarm << "Unrecognized logger type: " << type << enderr;
		 return 0;
	}

	OmnShouldNeverComeToThisPoint;
	return 0;
}


bool 
OmnLoggerMgr::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool 
OmnLoggerMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return mThreadStatus;
}
#endif
