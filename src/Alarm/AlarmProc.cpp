////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tracer.cpp
// Description:
// 	File name convension:
// 		mmddyy_   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/AlarmProc.h"

#include "AppMgr/App.h"
#include "Config/ConfigMgr.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Tracer/TraceEntry.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
//#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(OmnAlarmProcSingleton,
                 OmnAlarmProc,
                 OmnAlarmProcSelf,
                "OmnAlarmProc");

OmnAlarmProc::OmnAlarmProc()
:
mLock(OmnNew OmnMutex())
{
}

OmnAlarmProc::~OmnAlarmProc()
{
}


bool
OmnAlarmProc::start()
{
	//mThread->start();
	return true;
}


bool
OmnAlarmProc::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);	

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "AlarmProc", 0, true, true, __FILE__, __LINE__);
	
	AosRundataPtr rdata = OmnApp::getRundata();
	
	OmnString base_dir = OmnApp::getAppBaseDir();
	aos_assert_r(base_dir != "", false);
	
	OmnString parent_dir = OmnApp::getParentDir(base_dir);
	OmnString dir_name = OmnApp::getDirName(base_dir);

	mModuleName = conf->getAttrStr("module_name", "");
	aos_assert_r(mModuleName != "", false);

	OmnString log_dir;
	log_dir << parent_dir << "alarm_log/";
	OmnApp::createDir(log_dir);
	mFileName = log_dir;
	mFileName << dir_name << "_" << mModuleName << "_SysAlarms.log";

	mAlarmFile.open(mFileName.data(), fstream::in | fstream::out | fstream::trunc); 
	if (!(mAlarmFile.is_open()))
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** Failed to create SysAlarms Log file."
			<< endl;
		return false;
	}
	return true;
}
	

bool
OmnAlarmProc::stop()
{
	mThread->stop();
	return true;
}

	
bool
OmnAlarmProc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	vector<string> docstrs;
	
	while(state == OmnThrdStatus::eActive)
	{
		if (!mAlarmFile)
		{
			std::cout << "<" << __FILE__ << ":" << __LINE__
				<< "> ********** Failed to get SysAlarms Log file : "
				<< mFileName << endl;
			return false;
		}

		docstrs.clear();
		
		mLock->lock();
		mAlarmFile.seekg(0);
		while(!mAlarmFile.eof())
		{
		    string str(10000, 0);
		    mAlarmFile.getline((char *)(str.data()), 10000);
		    str.resize(mAlarmFile.gcount());
			if (str != "")
			{
//				cout << "##### alarm : " << str << endl;
				docstrs.push_back(str);
			}
		}
		resetFile();
		mLock->unlock();

		OmnSleep(1);
	}

	return true;
}

bool 	
OmnAlarmProc::saveAlarmToFile(const OmnString &docstr)
{
	return true;
	if (!mAlarmFile)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** Failed to get SysAlarms Log file : "
			<< mFileName << endl;
		return false;
	}

	mLock->lock();
	mAlarmFile << docstr.data();
	if (mAlarmFile.fail())
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** Failed to write SysAlarms Log file : "
			<< mFileName << endl;
		mLock->unlock();
		return false;
	}
	mAlarmFile.flush();
	mLock->unlock();
	return true;
}


void
OmnAlarmProc::resetFile()
{
	mAlarmFile.close();
	mAlarmFile.open(mFileName.data(), fstream::in|fstream::out|fstream::trunc);
	if (!mAlarmFile)
	{
		std::cout << "<" << __FILE__ << ":" << __LINE__
			<< "> ********** Failed to open SysAlarms Log file : "
			<< mFileName << endl;
	}
}
