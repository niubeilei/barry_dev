////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SystemMonitor/ProcessMonitor.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SeLogClient/SeLogClient.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"

#include <fstream>

#define SYSMONITOR_FILENAME_SECOND "status.sh"
#define SYSMONITOR_FILENAME_HOUR "hourstatus.sh"
#define SYSMONITOR_FILENAME_DAY "daystatus.sh"
OmnSingletonImpl(AosProcessMonitorSingleton, AosProcessMonitor, AosProcessMonitorSelf, "AosProcessMonitor");

OmnString sgScriptFileName = "./status.sh";

AosProcessMonitor::AosProcessMonitor()
:
mStoped(false)
{
	FILE* hostfile = popen("hostname", "r");
	char hostbuff[128];
	int len = fread(hostbuff, 1, 128, hostfile); 
	hostbuff[len-1]=0;
	mHostname = hostbuff;
}


AosProcessMonitor::~AosProcessMonitor()
{
}


bool
AosProcessMonitor::start()
{
	OmnFile ff(sgScriptFileName, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!ff.isGood())
	{
		OmnScreen << "Faild to find script!" << endl;
	}
//	OmnThreadedObjPtr thisPtr(this, false);
//	mThread = OmnNew OmnThread(thisPtr, "ProcessMonitorThread", 0, true, true);
//	mThread->start();
	u64 e = OmnGetTimestamp();
	u64 us = e % 1000000;
	u64 s = e / 1000000;
	OmnTimerObjPtr thisptr(this, false);
	mTimerid = OmnTimer::getSelf()->startTimer("AosTimerMgr", 5-(s%5), 1000000-us, thisptr, 0);
	return true;
}


bool 
AosProcessMonitor::stop()
{
	mStoped = true;
	return true;
}


bool 
AosProcessMonitor::config(const AosXmlTagPtr &config)
{
	return true;
}


void 
AosProcessMonitor::timeout(const int timerId, const OmnString &timerName, void *parm)
{
	aos_assert(timerId == mTimerid);
	OmnString ctime = OmnGetTime();
	addLog(ctime);
	u64 e = OmnGetTimestamp();
	u64 us = e % 1000000;
	u64 s = e / 1000000;
	OmnTimerObjPtr thisptr(this, false);
	mTimerid = OmnTimer::getSelf()->startTimer("AosProcessMonitor", 5-(s%5), 1000000-us, thisptr, 0);
}

bool
AosProcessMonitor::addLog(const OmnString &ctime)
{
	bool daytype = false;
	bool munitetype = false;
	bool secondtype = false;
	getMonitorType(daytype, munitetype, secondtype, ctime);
	if(daytype)
	{
		monitor(eDay, ctime);
	}
	if(munitetype)
	{
		monitor(eHour, ctime);
	}
	if(secondtype)
	{
		monitor(eDefTimeFreq, ctime);
	}
	return true;
}

bool
AosProcessMonitor::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		if (mStoped)
		{
			state = OmnThrdStatus::eExit;
			return true;
		}
		bool daytype = false;
		bool munitetype = false;
		bool secondtype = false;
		OmnString ctime;
		getMonitorType(daytype, munitetype, secondtype, ctime);
		if(daytype)
		{
			monitor(eDay, ctime);
		}
		if(munitetype)
		{
			monitor(eHour, ctime);
		}
		if(secondtype)
		{
			monitor(eDefTimeFreq, ctime);
		}
	}
	return true;
}

bool
AosProcessMonitor::getMonitorType(bool &daytype, bool &munitetype, bool &secondtype, const OmnString &ctime)
{
	char * s = OmnNew char[3]; 
	char * m = OmnNew char[3]; 
	char * h = OmnNew char[3]; 
	s[2] = 0;
	m[2] = 0;
	h[2] = 0;
   	memcpy(s, ctime.data()+ctime.length()-2, 2);
   	memcpy(m, ctime.data()+ctime.length()-5, 2);
   	memcpy(h, ctime.data()+ctime.length()-8, 2);
	u16 second = atoi(s);
	u16 minute = atoi(m);
	u16	hour = atoi(h);

	if( !(second % 5) )
	{
		secondtype = true;
	}
	if( (second == 0) && (minute == 0) )
	{
		munitetype = true;
	}
	if( (second == 0) && (minute == 0) && (hour == 0))
	{
		daytype = true;
	}
	delete s;
	delete m;
	delete h;
	return true;
}

bool
AosProcessMonitor::monitor(const int type, const OmnString &ctime)
{
	//check file exist

	OmnString cmd = "";
	if(type == eDay)
	{
		fstream file_day;
		file_day.open(SYSMONITOR_FILENAME_DAY,ios::in);
		if(!file_day)
		{
			file_day.close();
			return true;
		}
		file_day.close();

		cmd = "./daystatus.sh ";
	}
	else if(type == eHour)
	{
		fstream file_hour;
		file_hour.open(SYSMONITOR_FILENAME_HOUR,ios::in);
		if(!file_hour)
		{
			file_hour.close();
			return true;
		}
		file_hour.close();

		cmd = "./hourstatus.sh ";
	}
	else
	{
		fstream file_second;
		file_second.open(SYSMONITOR_FILENAME_SECOND,ios::in);
		if(!file_second)
		{
			file_second.close();
			return true;
		}
		file_second.close();
		cmd = "./status.sh ";
	}

	cmd << OmnApp::getAppBaseDir();
	FILE* ff = popen(cmd.data(), "r");
	if (!ff) return true;


	OmnString log = "<processmonitor ";
	if(type == eDay)
	{
		log << "rate" << "=\"per day\" ";
		log << "type" << "=\"day\" ";
	}
	else if(type== eHour)
	{
		log << "rate" << "=\"per hour\" ";
		log << "type" << "=\"hour\" ";
	}
	else
	{
		log << "rate" << "=\"five seconds\" ";
		log << "type" << "=\"seconds\" ";
	}
	
	log << AOSTAG_SYSSERVER << "=\"true\" ";
	log << AOSTAG_LOGNAME << "=\"processmonitor\" ";
	log << "servername" << "=\"" << mHostname << "\" ";
	log << AOSTAG_CTIME << "=\"" << ctime << "\" ";	

	char buff[1024];
	char *c;
	while((c=fgets(buff, 1024, ff)))
	{
		OmnString str(buff);
		str.removeTailWhiteSpace();
		vector<OmnString> strs;
		AosStrSplit::splitStrByChar(str, ":", strs, 2); 
		if (strs[0] != "" && strs[1] != "")
		{
			log << strs[0] << "=\"" << strs[1] << "\" ";
		}
	};
	pclose(ff);
	log << "/>";
	//AosCreateDoc(log, true, OmnApp::getRundata());
	// AosXmlParser parser;
	// AosXmlTagPtr logxml = parser.parse(log, "" AosMemoryCheckerArgs);
	// aos_assert_r(logxml, false);
	//return AosSeLogClient::getSelf()->addLog(logxml, 
	//		AOSCTNR_SYSLOGCTNR, "processmonitor", OmnApp::getRundata());
	return AosAddLog(OmnApp::getRundata(), AOSCTNR_SYSLOGCTNR,
			"processmonitor", log);
}

