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
//	LoggerFile is a logger that saves log entries into DB. The DB can 
//  be either local or remote. In addition, it may also send log 
//  entries to a remote application. 
// 
//  This class may limit the database size. It may clear the database
//  on a regular basis. 
// 
//  When the database size grows bigger than a threshold value, it will
//  generate an event. 
// 
//	When it removes log entries, it will generate a log entry indicating
//  how the entries were removed.    
//
// Modification History:
// 2013/03/17 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Logger/LoggerFile.h"

#include "Debug/Debug.h"
#include "Logger/LogEntry.h"
#include "Logger/LogListener.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


OmnFileLogger::OmnFileLogger(const OmnString &name,
							 const OmnString &fileName,
							 const OmnString &banner)
:
OmnLogger(OmnLogType::eLoggerFile, name),
mFileName(fileName),
mBanner(banner)
{
}


OmnFileLogger::OmnFileLogger(const OmnString &name,
							 const OmnXmlItemPtr &conf)
							 :
OmnLogger(OmnLogType::eLoggerFile, name)
{
	config(conf);
}


OmnFileLogger::~OmnFileLogger()
{
}


bool
OmnFileLogger::openLogger()
{
	if (mFile.isNull())
	{
		if (mAppend)
		{
			mFile = OmnNew OmnFile(mFileName, OmnFile::eAppend);
		}
		else
		{
			mFile = OmnNew OmnFile(mFileName, OmnFile::eCreate);
		}

		if (!mFile->isGood())
		{
			std::cout << "<" << __FILE__ << ":" << __LINE__
				<< "> ********** Failed to open Log file: "
				<< mFileName << endl;
			mFile = 0;
		}
	}

	return true;
}


void
OmnFileLogger::log(const OmnString &contents, const bool flush)
{
	// if (mFile)
	// {
	// 	mFile->put(contents.data(), contents.length(), flush);
	// }
}


bool
OmnFileLogger::config(const OmnXmlItemPtr &conf)
{
	//
	// It assumes the following:
	//
	//	<Log>
	//		<FileName>
	//		<Banner>
	//		<Append>
	//	</Log>
	//	

    if (conf.isNull())
    {
		return true;
    }

	mFileName = conf->getStr(OmnXmlItemName::eFileName, "");
	if (mFileName.length() <= 0)
	{
		cout << __FILE__ << ":" << __LINE__ 
			<< "********** Missing log file name: " 
			<< conf->toString() << endl;
		return false;
	}

	mBanner = conf->getStr(OmnXmlItemName::eBanner, "  ");
	mAppend = conf->getBool(OmnXmlItemName::eAppend, false);
	return true;
}
	

bool
OmnFileLogger::closeLogger()
{
	mFile = 0;
	return true;
}
#endif
