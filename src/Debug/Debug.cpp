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
// 
////////////////////////////////////////////////////////////////////////////
#include "Debug/Debug.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "Util/LogBag.h"
#include "Util/File.h"


static OmnFilePtr	sgDebugFile;
static OmnString	sgDebugFilename = "aos_debug_dump.txt";
static OmnMutex		sgDebugLock;

// AosLogBag gAosLogBag1("", true);

int		OmnDebug::mDebugLevel = 1;
bool	OmnDebug::mTraceComm = false;
bool	OmnDebug::mTraceThread = false;
bool	OmnDebug::mTraceMgcp = false;
bool	OmnDebug::mTraceCP = false;
bool	OmnDebug::mTraceDb = false;
bool	OmnDebug::mTraceTimer = false;
bool	OmnDebug::mTraceWrite = false;
bool	OmnDebug::mTraceRead = false;
bool	OmnDebug::mTraceHK = false;
bool	OmnDebug::mTraceMR = false;
bool	OmnDebug::mTraceNM = false;
bool	OmnDebug::mTraceRT = false;
bool	OmnDebug::mTraceMO = false;


OmnDebug::OmnDebug()
{
	OmnRslt rslt;
	if (rslt)
	{
		return;
	}
}

OmnDebug::~OmnDebug()
{
}


OmnRslt
OmnDebug::config(const OmnXmlParserPtr &conf)
{
	//
	// It assumes the following:
	//
	//	<DebugConfig>
	//		<TraceComm>
	//		<TraceThread>
	//		<TraceMgcp>
	//		<TraceMsg>
	//		<TraceCP>
	//		<TraceTimer>
	//		<TraceWrite>
	//		<TraceRead>
	//		<TraceHouseKeep>
	//		<TraceMR>
	//		<TraceNM>
	//		<TraceMO>		; For Managed objects
	//		<TraceRT>
	//	</DebugConfig>
	//	
	// If not present, they defaults to false. 
	//

	/*
	if (!conf)
	{
		return true;
	}

    OmnXmlItemPtr def = conf->tryItem("DebugConfig");
    if (def.isNull())
    {
		return true;
    }

	mTraceComm = def->getBool(OmnXmlItemName::eTraceComm, false);
	if (mTraceComm)
	{
		cout << "TraceComm is turned on" << endl;
	}

	mTraceThread = def->getBool(OmnXmlItemName::eTraceThread, false);
	if (mTraceThread)
	{
		cout << "TraceThread is turned on" << endl;
	}

	// mTraceMgcp = def->getBool(OmnXmlItemName::eTraceMgcp, false);
	// if (mTraceMgcp)
	// {
	// 	cout << "TraceMgcp is turned on" << endl;
	// }

	// mTraceMR = def->getBool(OmnXmlItemName::eTraceMR, false);
	// if (mTraceMR)
	// {
	// 	cout << "TraceMR is turned on" << endl;
	// }
	

	mTraceCP = def->getBool(OmnXmlItemName::eTraceCP, false);
	if (mTraceCP)
	{
		cout << "TraceCP is turned on" << endl;
	}

	mTraceDb = def->getBool(OmnXmlItemName::eTraceDb, false);
	if (mTraceDb)
	{
		cout << "TraceDb is turned on" << endl;
	}

	mTraceTimer = def->getBool(OmnXmlItemName::eTraceTimer, false);
	if (mTraceTimer)
	{
		cout << "TraceTimer is turned on" << endl;
	}

	mTraceWrite = def->getBool(OmnXmlItemName::eTraceWrite, false);
	if (mTraceWrite)
	{
		cout << "TraceWrite is turned on" << endl;
	}

	mTraceRead = def->getBool(OmnXmlItemName::eTraceRead, false);
	if (mTraceRead)
	{
		cout << "TraceRead is turned on" << endl;
	}

	mTraceHK = def->getBool(OmnXmlItemName::eTraceHK, false);
	if (mTraceHK)
	{
		cout << "TraceHK is turned on" << endl;
	}

	mTraceNM = def->getBool(OmnXmlItemName::eTraceNM, false);
	if (mTraceNM)
	{
		cout << "TraceNM is turned on" << endl;
	}

	mTraceRT = def->getBool(OmnXmlItemName::eTraceRT, false);
	if (mTraceRT)
	{
		cout << "TraceRT is turned on" << endl;
	}

	mTraceMO = def->getBool(OmnXmlItemName::eTraceMO, false);
	if (mTraceMO)
	{
		cout << "TraceMO is turned on" << endl;
	}
	*/

	return true;
}
	

bool
OmnDebug::appendDebugContents(
		const char *fname, 
		const int line, 
		const OmnString &contents)
{
	sgDebugLock.lock();
	if (!sgDebugFile)
	{
		OmnString fname = AosGetBaseDirname();
		fname << "/" << sgDebugFilename;
		sgDebugFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}

	if (!sgDebugFile->isGood())
	{
		sgDebugLock.unlock();
		OmnAlarm << "Failed opening debug file: " << sgDebugFile->getFileName() << enderr;
		return false;
	}

	OmnString ss;
	ss << fname << ":" << line << ": " << contents;

	sgDebugFile->append(ss.data(), ss.length(), true);
	sgDebugLock.unlock();
	return true;
}


