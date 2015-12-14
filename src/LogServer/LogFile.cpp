////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogFile.cpp
// Description:
//   
//
// Modification History:
// 03/31/2009 Created by Sally Sheng 
// 12/31/2012 Turned off by Chen Dnig
////////////////////////////////////////////////////////////////////////////
#if 0
#include "LogServer/LogFile.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"
#include "iostream.h"
#include "fstream.h"
#include "sstream"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "util2/value.h"
#include "DataStore/StoreMgr.h"
#include "stdio.h"


AosLogFile::AosLogFile()
{
}


AosLogFile::AosLogFile(const OmnString &filename, const u64 &maxsize, const int &truMethod)
:
mFileSize(0),
mMaxFileSize(maxsize),
mTruncateMethod((TruncateMethod)truMethod),
mTotalEntries(0)
{
	mFile = createFile(filename);
}


AosLogFile::~AosLogFile()
{
    if (mFile->getFile())
    {
        mFile->closeFile();
    }
}


bool
AosLogFile::setFile(const OmnString &filename)
{
	aos_assert_r(filename.length() >= 0, false);
    aos_assert_r(mFile.isNull(), false);
	mFile = OmnNew OmnFile();
	aos_assert_r(mFile, false);
	mFile->setName(filename);
	return true;
}
	


OmnFilePtr
AosLogFile::createFile(const OmnString &filename)
{
	aos_assert_r(filename.length() >= 0, false);
	aos_assert_r(mFile.isNull(), false);

	mFile = OmnNew OmnFile(filename, OmnFile::eAppend);

	if (mFile->isGood())
	{
		return mFile;
	}
	
	else
	{
		OmnTrace << "<" << __FILE__ << ":" << __LINE__
               << "> ********** Failed to open Log file : " << filename
               << endl;
        mFile = 0;
		return mFile;
	}

	OmnShouldNeverComeToThisPoint;
	return 0;
}


bool
AosLogFile::addLogEntry(TiXmlNode *logentry)
{
	if(!mFile->isGood())
	{
		OmnTrace << "<" << __FILE__ << ":" << __LINE__
                 << "> ********** Failed to open Log file : " 
				 << mFile->getFileName() << endl;
		return false;
	}
	
	//Set seqno to entry
	OmnString seqNo;
	seqNo << ++mTotalEntries;
	(logentry->ToElement())->SetAttribute("seqno", seqNo);

	ostringstream oss(ostringstream::out);
    oss << *logentry << "\n";
	OmnString buff;
	buff << oss.str();
OmnTrace << "Add buff :" << buff << endl;
   	mFileSize += buff.length();
   	if (mFileSize >= mMaxFileSize)
   	{
       	//truncate();
   	}

   	if (!mFile->put(buff, true))
   	{
    	//
    	// Filed to write to the file. One possibility is that
	    // the hoursekeeping thread fails. It did not rotate the
	    // log files. This file has been accumulating to too big.
	    // We will erase the current file, hoping it solves the
   		// problem.
    	//
		OmnTrace << buff << endl;

       	OmnTrace << "<" << __FILE__ << ":" << __LINE__
           	<< "> ********** "
           	<< "Failed to write to the log file: "
           	<< mFile->getFileName() << endl;

       	mFile->resetFile();
		return false;
   	}	
		
	return true;
}


int
AosLogFile::getPage(const OmnString &logid)
{
	return 1;
}


bool
AosLogFile::openFile()
{
	mFile->openFile(OmnFile::eAppend);

    if (mFile->isGood())
    {
        return true;
    }

    else
    {
        OmnTrace << "<" << __FILE__ << ":" << __LINE__
               << "> ********** Failed to open Log file : " 
			   << mFile->getFileName() << endl;
        mFile = 0;
        return false;
    }

    OmnShouldNeverComeToThisPoint;
    return true;
}


bool
AosLogFile::closeFile()
{
	if (mFile->getFile())
    {
        mFile->closeFile();
    }
	return true;
}


bool
AosLogFile::clearFile()
{
	aos_assert_r(mFile->resetFile(), false);
	mTotalEntries = 0;
	mFileSize = 0;
	//mFile->openFile(OmnFile::eAppend);
	return true;
}


bool
AosLogFile::deleteFile()
{
	if(!remove((mFile->getFileName()).data()) == 0)
	{
		OmnAlarm << "failed to delete file!!" << enderr;
		return false;
	}
	return true;
}
#endif
