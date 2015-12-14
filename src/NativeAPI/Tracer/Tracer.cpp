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

#include "Tracer/Tracer.h"

#include "AppMgr/App.h"
#include "Config/ConfigMgr.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
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
#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"

/*
OmnSingletonImpl(OmnTracerSingleton,
                 OmnTracer,
                 OmnTracerSelf,
                "OmnTracer");
*/

bool	OmnTracer::mCreateCallLog = false;
int		OmnTracer::mNumFiles    = 10;
bool	OmnTracer::mStdio		= true;
bool	OmnTracer::mTraceComm 	= false;
bool	OmnTracer::mTraceThread = false;
bool	OmnTracer::mTraceCP 	= false;
bool	OmnTracer::mTraceDb 	= false;
bool	OmnTracer::mTraceTimer 	= false;
bool	OmnTracer::mTraceWrite 	= false;
bool	OmnTracer::mTraceRead 	= false;
bool	OmnTracer::mTraceHB 	= false;
bool	OmnTracer::mTraceHK 	= false;
bool	OmnTracer::mTraceNM 	= false;
bool	OmnTracer::mTraceRT 	= false;
bool	OmnTracer::mTraceMO 	= false;
bool	OmnTracer::mTraceKernelApi = false;
bool	OmnTracer::mTraceUser 	= false;
bool	OmnTracer::mTraceSysAlarm = false;

OmnFilePtr	OmnTracer::mCallLog;
OmnFilePtr	OmnTracer::mSysAlarmLog;
OmnFilePtr	OmnTracer::mUserLog;

// OmnString	OmnTracer::mBaseName = "log/Tracer";
OmnString	OmnTracer::mBaseName = "Tracer";


static OmnMutex		sgLock;


//static OmnTraceEntry	sgTraceEntries[OmnTracer::eNumEntries];


OmnTracer::OmnTracer()
:
mFlushFileTimeTick(0),
mCrtFileIdx(0)
{
}


bool
OmnTracer::start()
{
	return true;
}


void
OmnTracer::createSysAlarmLog()
{
	// 
	// SysAlarm log is used by AlarmServer to log alarms generatd 
	// from other modules
	//
	if (mSysAlarmLog.isNull())
	{
		mSysAlarmLog = OmnNew OmnFile("log/SysAlarm.log", OmnFile::eAppend AosMemoryCheckerArgs);
		if (!mSysAlarmLog->isGood())
		{
			std::cout << "<" << __FILE__ << ":" << __LINE__
				<< "> ********** Failed to open SysAlarm Log file."
				<< endl;
			mSysAlarmLog = 0;
		}
	}
}


void
OmnTracer::writeSysAlarmLog(const OmnString &log)
{
	sgLock.lock();
	if (mSysAlarmLog)
	{
		mSysAlarmLog->put(OmnFile::eNoSeek, log.data(), log.length(), false);
	}

	if (mTraceSysAlarm)
	{
		cout << log.data() << endl;
	}
	sgLock.unlock();
}


bool
OmnTracer::openTracer(const OmnString &productName,
					  const OmnString &version,
					  const OmnString &patch,
					  const OmnString &buildTime, // Jenny Gu, 2003-0231
					  const int buildNo)
{
	if (mStdio)
	{
		return true;
	}

	mProductName = productName;
	mVersion = version;
	mPatch = patch;
	mBuildNo = buildNo;

	mBuildTime = buildTime;

	if (!mTracerFile)
	{
		if (!createFiles())
		{
			return false;
		}

		return true;
	}

	cout << "******** Tracer file is already open: " << endl;
	return true;
}


OmnTracer::~OmnTracer()
{
}


OmnRslt
OmnTracer::config(const AosXmlTagPtr &conf)
{
	//
	// It assumes the following:
	//
	//	<TracerConfig>
	//		<TracerFileName>
	//		<AlarmSenderCommGroup>
	//		<TraceComm>
	//		<TraceThread>
	//		<TraceMgcp>
	//		<TraceMsg>
	//		<TraceCP>
	//		<TraceTimer>
	//		<TraceWrite>
	//		<TraceRead>
	//		<TraceHouseKeep>
	//		<TraceKernelApi>
	//		<TraceNM>
	//		<TraceMO>		; For Managed objects
	//		<TraceRT>
	//		<TraceSysAlarm>
	//	</TracerConfig>
	//	
	// If not present, they defaults to false. 
	//
/*
	if (!conf)
	{
		// No configuration 
		return true;
	}

    AosXmlTagPtr def = conf->getFirstChild(OmnXmlItemName::eTracerConfig);
    if (def.isNull())
    {
		mBaseName = "log/Tracer";
		return true;
    }

	mBaseName = def->getAttrStr(OmnXmlItemName::eTracerFileName, "log/Tracer");

	mTraceComm 	 = def->getAttrBool(OmnXmlItemName::eTraceComm, false);
	mTraceThread = def->getAttrBool(OmnXmlItemName::eTraceThread, false);
	mTraceCP     = def->getAttrBool(OmnXmlItemName::eTraceCP, false);
	mTraceDb     = def->getAttrBool(OmnXmlItemName::eTraceDb, false);
	mTraceTimer  = def->getAttrBool(OmnXmlItemName::eTraceTimer, false);
	mTraceWrite  = def->getAttrBool(OmnXmlItemName::eTraceWrite, false);
	mTraceRead   = def->getAttrBool(OmnXmlItemName::eTraceRead, false);
	mTraceHB     = def->getAttrBool(OmnXmlItemName::eTraceHB, false);
	mTraceHK     = def->getAttrBool(OmnXmlItemName::eTraceHK, false);
	mTraceNM     = def->getAttrBool(OmnXmlItemName::eTraceNM, false);
	mTraceRT     = def->getAttrBool(OmnXmlItemName::eTraceRT, false);
	mTraceMO     = def->getAttrBool(OmnXmlItemName::eTraceMO, false);
	mTraceUser   = def->getAttrBool(OmnXmlItemName::eTraceUser, false);
	mTraceKernelApi = def->getAttrBool("TraceKernelApi", false);
	mTraceSysAlarm = def->getAttrBool(OmnXmlItemName::eTraceSysAlarm, false);

*/
	return true;
}
	

OmnTraceEntry &
OmnTracer::getTraceEntry(const char *file, 
						 const int line,
						 const bool flag)
{
	static OmnTraceEntry lsDummyEntry(false);
	static OmnTraceEntry lsDummyEntry2(true);

	if (!flag)
	{
		lsDummyEntry.setFileLine(file, line);
		return lsDummyEntry;
	}

	OmnThreadPtr thread = OmnThreadMgr::getCurrentThread();
	if (!thread)
	{
		// This may happen if the main thread has not been registered yet.
		u32 start_time = OmnApp::getAppStartTime();
		u32 crt_sec = OmnGetSecond();
		if (crt_sec < start_time)
		{
			OmnAlarm << "Internal error" << enderr;
			lsDummyEntry2.setFileLine(file, line);
			return lsDummyEntry2;
		}

		if (start_time == 0 || crt_sec - start_time < 10)
		{
			// We give the application some time to start up.
			lsDummyEntry2.setFileLine(file, line);
			return lsDummyEntry2;
		}

		//OmnAlarm << "Failed retrieve the thread" << enderr;
		lsDummyEntry2.setFileLine(file, line);
		return lsDummyEntry2;
	}

	OmnTraceEntry &entry = thread->getTraceEntry();
	entry.setFileLine(file, line);
	return entry;
}


bool
OmnTracer::writeTraceEntry(const char *str, const int length)
{
	if (mStdio)
	{
		cout << str;
		return true;
	}

	sgLock.lock();
	writeTraceEntryPrivate(str, length);
	sgLock.unlock();
	return true;
}


bool
OmnTracer::writeTraceEntryPrivate(const char *str, const int length)
{
	if (mTracerFile)
	{
		mTracerFileSize += length;
		if (mTracerFileSize >= eMaxFileSize)
		{
			createFiles();
		}
		
		bool flush_flag = true; 
		int crt_time_tick = OmnTime::getCrtSec();
		if(mFlushFileTimeTick == crt_time_tick)
		{
			flush_flag = false;
		}
		else
		{
			flush_flag = true;
			mFlushFileTimeTick = crt_time_tick;
		}

		if (!mTracerFile->put(OmnFile::eNoSeek, str, strlen(str), flush_flag))
		{
			// 
			// Filed to write to the file. One possibility is that
			// the hoursekeeping thread fails. It did not rotate the
			// log files. This file has been accumulating to too big.
			// We will erase the current file, hoping it solves the
			// problem.
			//
			std::cout << str;

			std::cout << "<" << __FILE__ << ":" << __LINE__
				<< "> ********** " 
				<< "Failed to write to the log file: "
				<< mTracerFileName.getBuffer() << std::endl;
		}

	}

	return true;
}


bool
OmnTracer::writeAlarmEntry(const char *str, const int length)
{
	if (mStdio)
	{
		cout << str;
		return true;
	}

	sgLock.lock();
	if (mAlarmFile)
	{
		mAlarmFileSize += length;
		if (mAlarmFileSize >= eMaxFileSize)
		{
			createFiles();
		}

		if (!mAlarmFile->put(OmnFile::eNoSeek, str, strlen(str), false))
		{
			// 
			// Filed to write to the file. One possibility is that
			// the hoursekeeping thread fails. It did not rotate the
			// log files. This file has been accumulating to too big.
			// We will erase the current file, hoping it solves the
			// problem.
			//
			std::cout << str;

			std::cout << "<" << __FILE__ << ":" << __LINE__
				<< "> ********** " 
				<< "Failed to write to the log file: "
				<< mAlarmFileName.data() << std::endl;

			mAlarmFile->resetFile();	
		}
	}

	// 
	// Alarms are also written into the tracer file
	//
	writeTraceEntryPrivate(str, length);
	sgLock.unlock();
	return true;
}


bool
OmnTracer::writeEventEntry(const char *str, const int length)
{
	// 
	// Currently, Events are written in the Alarm log file.
	//
	return writeAlarmEntry(str, length);
}


bool
OmnTracer::determineFileNames()
{
	// 
	// It assumes 10 files in the ./log directory, named as:
	// 		<progname>_0.txt
	//		<progname>_1.txt
	//		...
	//		<progname>_<mNumFiles>.txt
	// Parallel to logs, there is a set of alarm_x files.
	//
	// Each log file contains a value obtained from gettimeofday()
	// as the first line of each log file. 
	//
	// When this program starts, if no log files, it will use log_0.
	// Otherwise, it compares the timestamp in each existing log files
	// and chooses the oldest one to override.
	//

	/*
	int timestamp = 1000;
	int fileIndex = -1;

	OmnString filename;
	for (int i=0; i<mNumFiles; i++)
	{
		(filename = mBaseName) << "_" << i << ".txt";
		OmnFile theFile(filename, OmnFile::eReadOnly);
		if (theFile.isGood())
		{
			// 
			// The file exists. The first word should be an integer.
			//
			int fileTime;
			if (theFile.readInt(fileTime))
			{
				if (fileTime < timestamp)
				{
					// 
					// This file is older than the previously selected one.
					// Choose this one.
					timestamp = fileTime;
					fileIndex = i;
				}
			}
			else
			{
				// 
				// Either the file is not readable, does not exist, 
				// or not a valid log file. In any case, we choose
				// this 'bad' one.
				//
				fileIndex = i;
				break;
			}
		}
		else
		{
			// 
			// The file does not exist. This is the one to use.
			//
			fileIndex = i;
			break;
		}
	}

	if (fileIndex < 0)
	{
		// 
		// Didn't find any file. This is possible if there isn't any
		// log file in the directory. Select the first one: log_0.
		//
		fileIndex = 0;
	}

	(mTracerFileName = mBaseName) << "_" << fileIndex << ".txt";
	(mAlarmFileName = mBaseName) << "Alarm_" << fileIndex << ".txt";
	return true;
	*/
	mCrtFileIdx++;
	// if (mCrtFileIdx > eMaxFiles) mCrtFileIdx = 0;
	if (mCrtFileIdx > 20) mCrtFileIdx = 0;
	(mTracerFileName = mBaseName) << "_" << mCrtFileIdx << ".txt";
	(mAlarmFileName = mBaseName) << "Alarm_" << mCrtFileIdx << ".txt";
	return true;
}


void
OmnTracer::procHouseKeeping()
{
	/*
	// 
	// It checks whether the tracing file is too big. If it is, it
	// changes the file to a new name. 
	//
	if (mStdio)
	{
		return;
	}

	if (mTracerFileSize < eMaxFileSize && mAlarmFileSize < eMaxFileSize)
	{
		// 
		// Don't need to do anything.
		//
		return;
	}

	if (mTracerFile.isNull())
	{
		// 
		// This is wrong.
		//
		cout << "<" << __FILE__ << ":" << __LINE__ 
			<< " ********** WARNING ********** \n"
			<< "Tracer file is null\n"
			<< " ********** WARNING ********** \n"
			<< endl;
		return;
	}

	if (mAlarmFile.isNull())
	{
		// 
		// This is wrong.
		//
		cout << "<" << __FILE__ << ":" << __LINE__ 
			<< " ********** WARNING ********** \n"
			<< "Alarm file is null\n"
			<< " ********** WARNING ********** \n"
			<< endl;
		return;
	}


	//
	// To create the file
	//
	createFiles();
	*/
}


bool
OmnTracer::createFiles()
{
	determineFileNames();

	OmnFilePtr theFile = createFile(mTracerFileName AosMemoryCheckerArgs);
	if (!theFile)
	{
		// 
		// Failed to create the tracer file. There will be no tracing. 
		// 
		//cout << "<" << __FILE__ << ":" << __LINE__ 
		//	<< " ********** WARNING ********** \n"
		//	<< "Failed to open tracer file: "
		//	<< mTracerFileName
		//	<< "\n ***************************** \n"
		//	<< endl;
	}
	else
	{
		mTracerFile = theFile;
		mTracerFileSize = 0;
	}

	theFile = createFile(mAlarmFileName AosMemoryCheckerArgs);
	if (!theFile)
	{
		// cout << "<" << __FILE__ << ":" << __LINE__ 
		//	<< " ********** WARNING ********** \n"
		//	<< "Failed to open alarm file: "
		//	<< mAlarmFileName
		//	<< "\n ***************************** \n"
		//	<< endl;
	}
	else
	{
		mAlarmFile = theFile;
		mAlarmFileSize = 0;
	}

	return true;
}


OmnFilePtr
OmnTracer::createFile(const OmnString &filename AosMemoryCheckDecl)
{
	if (filename.length() <= 0)
	{
		return 0;
	}

	OmnFilePtr theFile = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerFileLine);

	if (theFile->isGood())
	{
		// 
		// Need to write the signature into the file
		//
		/*
		char signature[200];
		memset(signature, 0, 200);
		OmnString timestamp = OmnGetTime(OmnApp::getLocale());
		sprintf(signature, "%d %s %s, Patch %s, Build %d, Build Time %s, %s\n",
			10000, 
			mProductName.data(),	
			mVersion.data(),
			mPatch.data(),
			mBuildNo, 
			mBuildTime.data(),
			timestamp.data());


		theFile->put(OmnFile::eNoSeek, signature, strlen(signature), true);
		*/
		return theFile;
	}

	return 0;
}


bool
OmnTracer::closeTracer()
{
	mTracerFile = 0;
	mAlarmFile = 0;
	mCallLog = 0;
	return true;
}


bool
OmnTracer::stop()
{
	return closeTracer();
}


void
OmnTracer::configTracer()
{
	// 
	// This is an interactive tracer configuration function.
	//
	char sel;
	while (1)
	{
		cout << "Configure Tracer: "
			<< "\n0: Finish configuration" 
			<< "\n1: Trace Comm         = " << ((mTraceComm)?"on":"off") 
			<< "\n2: Trace Thread       = " << ((mTraceThread)?"on":"off")
			<< "\n4: Trace Call Proc    = " << ((mTraceCP)?"on":"off")
			<< "\n5: Trace Database     = " << ((mTraceDb)?"on":"off")
			<< "\n6: Trace Housekeeping = " << ((mTraceHK)?"on":"off")
			<< "\n7: Trace Heartbeat    = " << ((mTraceHB)?"on":"off")
			<< "\n9: Trace NMS          = " << ((mTraceNM)?"on":"off")
			<< "\na: Trace Retrans      = " << ((mTraceRT)?"on":"off")
			<< "\nb: Trace Managed Obj  = " << ((mTraceMO)?"on":"off")
			<< endl;

		cout << "\nEnter a number to configure: " << endl;
		cin >> sel;

		if (sel == '0')
		{
			return;
		}

		char val;	
		if (sel >= '1' && sel <= 'a')
		{
			cout << "Enter Value (0/1)" << endl;
			cin >> val;
		}

		switch (sel)
		{
		case '1':
			 mTraceComm = (val == '1')?true:false;
			 break;

		case '2':
			 mTraceThread = (val == '1')?true:false;
			 break;

		case '4':
			 mTraceCP = (val == '1')?true:false;
			 break;

		case '5':
			 mTraceDb = (val == '1')?true:false;
			 break;

		case '6':
			 mTraceHK = (val == '1')?true:false;
			 break;

		case '7':
			 mTraceHB = (val == '1')?true:false;
			 break;

		case '9':
			 mTraceNM = (val == '1')?true:false;
			 break;

		case 'a':
			 mTraceRT = (val == '1')?true:false;
			 break;

		case 'b':
			 mTraceMO = (val == '1')?true:false;
			 break;

		default:
			 break;
		}
	}
}


void
OmnTracer::timeout(	
		const int timerId,
		const OmnString &timerName,
		void *parm)
{
	mTimeStr = OmnGetTime();
	OmnTimerObjPtr thisptr(this, false);
	//OmnTimer::getSelf()->startTimer("tracer", 1, 0, thisptr);
}

