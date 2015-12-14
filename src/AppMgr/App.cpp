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
// 06/02/2012: added mBaseDir and its configuration. Tag: Change06022012_1
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Alarm/AlarmMgr.h"
#include "API/AosApi.h"
#include "Debug/ExitHandler.h"
#include "Message/MsgFactory.h"
#include "Porting/WSA.h"
#include "Porting/WSA.cpp"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Docid.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonMgr.h"
#include "SingletonClass/SingletonObj.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/FileTypes.h"
#include "Util/SystemDefs.h"
#include "Util1/TimeDriver.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "Util1/MemMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlParser/XmlParser.h"
#include "UtilComm/CommUtil.h"

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>


bool OmnAppExecutionIsActive = true;

static OmnAppPtr sgAppSelf;
static AosLocale::E sgLocale = AosLocale::eUSA;
static OmnMutex		sgLock;
static OmnString sgConfigFname;
static bool			sgInited = false;

// Static member data declarations
OmnApp::AppStatus 	OmnApp::smAppStatus = OmnApp::eActive;
AosXmlTagPtr 		OmnApp::smAppConfig;
AosRundataPtr		OmnApp::smRundata;
AosRundataPtr		OmnApp::smDeleteObjRdata;
bool				OmnApp::smStarted = false;
OmnString			OmnApp::smVersion;
bool				OmnApp::smNetworkEnabled = true;
bool				OmnApp::smFinished = false;
OmnXmlParserPtr		OmnApp::smParser;
OmnSingletonMgrPtr	OmnApp::smSingletonMgr;
OmnString			OmnApp::smAppName;
OmnString   		OmnApp::smUOI;
int64_t     		OmnApp::smInstanceId = 0;
int         		OmnApp::smBuildNo = 0;
OmnString   		OmnApp::smBuildTime;
OmnString   		OmnApp::smPatch;
OmnIpAddr   		OmnApp::smSignalAddr;
int         		OmnApp::smSignalPort = 0;
OmnIpAddr   		OmnApp::smStreamerAddr;
int         		OmnApp::smStreamerPort = 0;
OmnString			OmnApp::smLogId;
OmnString			OmnApp::smSeqno;
OmnString			OmnApp::smUser;
OmnString			OmnApp::smInstanceName;
bool				OmnApp::mFinished = false;
OmnSingletonMgrPtr	OmnApp::mSingletonMgr;
bool				OmnApp::smIsFrontEndServer = false;
int					OmnApp::smServerId = -1;
int					OmnApp::smSystemId = -1;
OmnString			OmnApp::smBaseDir;
OmnString			OmnApp::smTransDir;
u32					OmnApp::smStartSec;
bool				OmnApp::smCheckIgnoredAlarms = false;
bool				OmnApp::smIsSvrProxy = false;;
OmnString			OmnApp::smAppDir;
OmnString			OmnApp::smConfigDir;
int					OmnApp::smDocClientVersion = OmnApp::eDftDocClientVersion;	// Chen Ding, 2014/11/09
int					OmnApp::smBindObjidVersion = OmnApp::eDftBindObjidVersion;	// Ken Lee, 2014/12/01
int					OmnApp::smDFMVersion = OmnApp::eDFMVersion;					// Gavin, 2015/09/01

OmnAppPtr
OmnApp::getSelf(const int argc, char **argv)
{
	if (sgAppSelf) return sgAppSelf;
	sgAppSelf = OmnNew OmnApp(argc, argv);
	return sgAppSelf;
}


OmnAppPtr
OmnApp::getSelf()
{
 	aos_assert_r(sgAppSelf, 0);
 	return sgAppSelf;
}


OmnApp::OmnApp(int argc, char **argv)
{
	mFinished = false;
	mSingletonMgr = OmnNew OmnSingletonMgr();
	OmnRslt rslt = init(argc, argv);
	if (!rslt)
	{
		OmnExcept e(OmnFileLine, rslt.getErrId(), rslt.toString());
		throw e;
	}
}


bool
OmnApp::init(int argc, char **argv)
{
	if (sgInited) return true;
	aos_assert_r(!sgInited, false);
	aos_assert_r(argc > 0, false);
	smAppName = argv[0];
	sgInited = true;

	smStartSec = OmnGetSecond();
	OmnString errmsg;
	if (!procCommandLine(argc, argv, errmsg))
	{
		return false;
	}

	OmnThreadedObjPtr thisPtr(this, false);
	OmnThreadPtr mainThread = OmnNew OmnThread(thisPtr,
		"mainThread", 0, false, true, __FILE__, __LINE__);
	mainThread->setThreadId(OmnGetCurrentThreadId());

	OmnThreadMgr::setThread(mainThread);

	// Start the network (this is required for Microsoft 
	if (!OmnInitSocketEnv(errmsg))
	{
		OmnRslt rslt(OmnFileLine, "Failed to start the network: ");
		rslt << errmsg;
		return rslt;
	}

	// Start OmnWait, OmnTime, and OmnAlarmMgr
	OmnRslt rslt;

	OmnAlarmMgr::init();
	OmnThreadMgr::getSelf(); // rslt = thrdMgr->start();
	OmnWait::getSelf();     //rslt += waitSelf->start();
	OmnTimeDriver::getSelf();	    //rslt += timeSelf->start();

	// Start the tracer
	OmnTracer::getSelf();

	//Jozhi 2014-10-22 add the app workhome
	char buff[256];
	int len = readlink("/proc/self/exe", buff, sizeof(buff));
	if (len < 0 || (len > 0 && (u32)len > (sizeof(buff) - 4)) )
	{
		return false;
	}
	buff[len]='\0';
	smAppDir = buff;
	len = smAppDir.find('/', true);
	if (len < 0)
	{
		return false;
	}
	smAppDir.setLength(len);

	// Chen Ding, 2013/05/29
	// OmnConfigReader config(argc, argv);

	setenv("TZ", "GMT-8", 1);
	tzset();
	// Start the basic singletons
	return startBasicSingletons();
}


OmnRslt
OmnApp::startBasicSingletons()
{
//	mSingletonMgr->addSingleton(OmnNew OmnTimerSingleton());
//	mSingletonMgr->addSingleton(OmnNew OmnMemMgrSingleton());
//	mSingletonMgr->addSingleton(OmnNew OmnNmsSingleton());
//	mSingletonMgr->addSingleton(OmnNew OmnNetworkSingleton());
//	mSingletonMgr->addSingleton(OmnNew OmnLoggerMgrSingleton());
//	mSingletonMgr->addSingleton(OmnNew OmnMemMtrSingleton());	
//	mSingletonMgr->addSingleton(OmnNew OmnMsgFactorySingleton());	
//	mSingletonMgr->addSingleton(OmnNew OmnThreadShellMgrSingleton());	

//	if (!OmnFileTypes::config(smParser))
//	{
//		return OmnAlarm << "Failed to configure FileTypes" << enderr;
//	}

	return true;
}


OmnRslt
OmnApp::startSingleton(OmnSingletonObj* singleton)
{
	OmnTrace << "Create singleton classes: " << singleton->getSysObjName();

	OmnRslt rslt = mSingletonMgr->addSingleton(singleton);
	if (!rslt)
	{
		OmnAlarm << "The singleton is already created: " 
			<< singleton->getSysObjName() << enderr;
	}

	rslt = mSingletonMgr->startSingleton(singleton->getSysObjId(), smAppConfig);
	return true;
}


bool	
OmnApp::appLoop()
{
	while (1)
	{
		/*
		char c;
		cin >> c;
		switch (c)
		{
		case '0':
			 return true;

		default:
			 break;
		}
		*/

		OmnSleep(1000);
	}

	return true;
}


bool	
OmnApp::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	return true;
}


bool	
OmnApp::signal(const int threadLogicId)
{
	// 
	// This is the main thread. We can't signal it.
	//
	return true;
}


void    
OmnApp::heartbeat(const int tid)
{
}


bool    
OmnApp::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


void
OmnApp::exitApp()
{
	OmnAppExecutionIsActive = false;
}


bool
OmnApp::procCommandLine(int argc, char **argv, OmnString &errmsg)
{
	// 
	// OmnApp will parse the following command line parameters:
	//
	//	-name <string>		Application Name
	//  -in <string>		Application Instance Domain Name
	//  -dn <string>		Application Domain Name
	//  -systemid <id>		System ID
	//

	// 
	// By default, the program name serves as the application name.
	//
	if (!argv) return true;
	smAppName = argv[0];

	int index = 0;
	while (index < argc)
	{
		OmnString parm = argv[index++];
		parm.toLower();

		if (OmnString(argv[index]) == OmnString("-s"))
		{
			// -s for tracer
			OmnTracer::mStdio = true;
			index++;
			continue;
		}

		if (OmnString(argv[index]) == OmnString("-t"))
		{
			// -t for tracer output to the screen and file Simultaneously
			OmnTracer::mStdioAndFile = true;
			index++;
			continue;
		}

		if (parm == "-name")
		{
			// It is "-name <string>"
			if (index >= argc)
			{
				errmsg = "Incorrect -name argument: missing the Application Name";
				return false;
			}

			OmnString appname(argv[index]);
			index++;
			smAppName = appname;
			continue;
		}

		if (parm == "-dn")
		{
			// It is the Application Domain Name
			if (index >= argc)
			{
				errmsg = "Incorrect -dn argument: missing the Application Domain Name";
				return false;
			}

			OmnString dn(argv[index++]);
			if (dn.length() <= 0)
			{
				errmsg = "Incorrect -dn argument: Application Domain Name is null";
				return false;
			}
			smUOI = dn;
			continue;
		}

		if (parm == "-in")
		{
			// It is the Application Domain Name
			if (index >= argc)
			{
				errmsg = "Incorrect -in argument: missing the Application Instance Name";
				return false;
			}

			OmnString in(argv[index++]);
			if (in.length() <= 0)
			{
				errmsg = "Incorrect -in argument: Application Instance Name is null";
				return false;
			}
			smInstanceId = aos_atoll(in.getBuffer());
			continue;
		}

		if (parm == "-systemid")
		{
			// It is the system id: -systemid xxx
			if (index >= argc)
			{
				errmsg = "Incorrect -systemid parm";
				return false;
			}

			smSystemId = atoi(argv[index++]);
			continue;
		}
	}

	return true;
}


AosLocale::E
OmnApp::getLocale()
{
	return sgLocale;
}


void
OmnApp::stopApp(const OmnString &username, 
		const OmnString &passwd, 
		const OmnString &appname)
{
	smAppStatus = eStopping;
}


bool
OmnApp::isAppStopped()
{
	return (smAppStatus == eStopping);
}

bool 
OmnApp::setConfig(const char *fname)
{
	return setConfig(readConfig(fname));
}

AosXmlTagPtr 
OmnApp::readConfig(const char* fname)
{
	if (!fname || strlen(fname) == 0) return NULL;

	if (!fname)
	{
		OmnExitApp("Missing config file name");
	}

	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		OmnString errmsg = "Failed to open the config file: ";
		errmsg << fname;
		OmnExitApp(errmsg);
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnString errmsg = "Failed to read config: ";
		errmsg << fname;
		OmnExitApp(errmsg);
	}

	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!config)
	{
		OmnString errmsg = "Configuration file incorrect: ";
		errmsg << fname;
		OmnExitApp(errmsg);
	}
	sgConfigFname = fname;
	bool bb = smCheckIgnoredAlarms;
	smCheckIgnoredAlarms = true;
	checkIgnoredAlarms();
	smCheckIgnoredAlarms = bb;

	//Jozhi 2014-10-22 set config directory
	smConfigDir = fname;
	int len = smConfigDir.find('/', true);
	if (len > 0)
	{
		smConfigDir.setLength(len);
	}
	else
	{
		// Young Ketty 2014-10-22
		smConfigDir = "";
	}
	return config;
}

bool 
OmnApp::setConfig(const AosXmlTagPtr &xml)
{
	smAppConfig = xml;

	smBaseDir = smAppConfig->getAttrStr("base_dir");
	if (smBaseDir == "")
	{
		OmnString errmsg;
		errmsg << "Missing Application Base Directory Name. "
			<< "Please add 'base_dir' root-level attribute to specify"
			<< " its base directory.";
		OmnExitApp(errmsg);
	}
	
	// Ketty 2013/05/16
	u32 len = smBaseDir.length();
	if(smBaseDir.data()[len-1] != '/') smBaseDir << "/";
	//createSysDir(smBaseDir);	// Ketty 2013/12/03
	createDir(smBaseDir);

	// Ketty 2013/05/10	
	//if (chdir(smBaseDir.data()) < 0)
	//{
	//	OmnAlarm << smBaseDir << ": " << errno << enderr;
	//	OmnExitApp("");
	//}
	
	if(!smIsSvrProxy)
	{
		// svrproxy no trans dir.
		OmnString trans_dir = smAppConfig->getAttrStr("trans_dir", "Trans");
		smTransDir = createSysDir(trans_dir);
	}
	
	smAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");

	OmnTracer* tracer = OmnTracer::getSelf();
	tracer->config(smAppConfig);
	tracer->openTracer(smAppName, smVersion, smPatch, smBuildTime, smBuildNo);

	return true;
}


// Ketty 2013/05/16
OmnString
OmnApp::createSysDir(const OmnString &sub_dir)
{
	aos_assert_r(sub_dir != "", "");
	OmnString full_dir;
	if(sub_dir.data()[0] == '/')
	{
		full_dir = sub_dir;
	}
	else
	{
		full_dir = getAppBaseDir();
		full_dir << sub_dir;	
	}

	bool rslt = createDir(full_dir);

	aos_assert_r(rslt, "");
	return full_dir;
}


AosXmlTagPtr
OmnApp::getAppConfig()
{
	return smAppConfig;
}


// Chen Ding, 05/06/2012 
AosXmlTagPtr
OmnApp::getConfigSubtag(const OmnString &xpath)
{
	aos_assert_r(smAppConfig, 0);
	return smAppConfig->xpathGetFirstChild(xpath);
}


// Chen Ding, 08/03/2011
bool
OmnApp::appStart(
		const int argc, 
		char **argv) 
{
	// This function is called when an application starts. This means
	// that one should call this function at the beginning of main().
	// This allows OmnApp to prepare the application. 
	// 
	// IMPORTANT: This function shall not be called multiple times. 
	// Otherwise, it will be ignored. 
	sgLock.lock();
	if (smStarted)
	{
		OmnAlarm << "Application already started!" << enderr;
		sgLock.unlock();
		return false;
	}
	aos_assert_r(!smRundata, false);
	smRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	smRundata->setUserid(AOSDOCID_APPROOT);
	smRundata->setAppname(AOSAPPNAME_APPLICATION);

	smDeleteObjRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	smDeleteObjRdata->setUserid(AOSDOCID_DELOBJROOT);
	smDeleteObjRdata->setAppname(AOSAPPNAME_APPLICATION);
	smStarted = true;
	sgLock.unlock();
	return true;
}


AosRundataPtr
OmnApp::getRundata()
{
	return smRundata;
}


AosRundataPtr
OmnApp::getDeleteObjRundata()
{
	return smDeleteObjRdata;
}


OmnString
OmnApp::getRemoteWorkingDir(const int server_id)
{
	aos_assert_r(server_id != -1, "");
	OmnString dirname = getAppBaseDir();
	dirname << "/" << AOSSYSTEMDEF_WORKINGDIRNAME << "/";
	dirname << "m" << server_id << "/";
	return dirname;
}


OmnString 
OmnApp::getLocalWorkingDir()
{
	int physical_id = AosGetSelfServerId();
	aos_assert_r(physical_id >= 0, "");
	return getRemoteWorkingDir(physical_id);
}


bool
OmnApp::createDir(const OmnString &dirname)
{
	OmnString command = "mkdir -p ";
	command << dirname;
	int rslt = system(command.data());
	return (rslt == 0);
	//int rslt = mkdir(dirname.data(), 0755); 
	// if (rslt != 0)
	// {
	// 	if (errno == EEXIST)
	// 		return ;
	// 	OmnAlarm << "Faild to create directry: " << dirname << enderr;
	// }
}


bool
OmnApp::checkIgnoredAlarms()
{
	if (!smCheckIgnoredAlarms) return true;
	if (sgConfigFname == "") return true;

	smCheckIgnoredAlarms = false;
	OmnFile f(sgConfigFname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood()) return true;

	OmnString contents;
	bool rslt = f.readToString(contents);
	if (!rslt) return true;

	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(contents, "" AosMemoryCheckerArgs);
	aos_assert_r(config, false);

	AosXmlTagPtr conf = config->getFirstChild("ignored_alarms");
	if (!conf) return true;

	OmnString str = conf->getNodeText();
	if (str == "") return true;
	return OmnAlarmMgr::setIgnoredAlarms(str);
}                                                                                 

OmnString 
OmnApp::getParentDir(const OmnString &dir)
{
	aos_assert_r(dir != "", "");
	OmnString tmp = dir;
	if (tmp[tmp.length() - 1] == '/')
	{
		tmp.setLength(dir.length() - 1);
	}
	int idx = tmp.indexOf(0, '/', true);
	return tmp.substr(0, idx);
}

OmnString
OmnApp::getDirName(const OmnString &dir)
{
	aos_assert_r(dir != "", "");
	OmnString tmp = dir;
	if (tmp[tmp.length() - 1] == '/')
	{
		tmp.setLength(dir.length() - 1);
	}
	int idx = tmp.indexOf(0, '/', true);
	return tmp.substr(idx + 1);
}


void 
OmnApp::getSelfMemoryUsage(const int proc_pid, double &vm_usage, double & resident_set)
{
	using std::ios_base;
	using std::ifstream;
	using std::string;
	vm_usage = 0.0;
	resident_set = 0.0;
	OmnString str;
	str << "/proc/" << proc_pid << "/stat";
	//ifstream stat_stream("/proc/self/stat",ios_base::in);
	ifstream stat_stream(str.data(),ios_base::in);
	string pid, comm, state, ppid, pgrp, session, tty_nr;
	string tpgid, flags, minflt, cminflt, majflt, cmajflt;
	string utime, stime, cutime, cstime, priority, nice;
	string O, itrealvalue, starttime;
	unsigned long vsize;
	long rss;
	stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
		>> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
		>> utime >> stime >> cutime >> cstime >> priority >> nice
		>> O >> itrealvalue >> starttime >> vsize >> rss; 
	stat_stream.close();
	long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; 
	vm_usage     = vsize / 1024.0;
	resident_set = rss * page_size_kb;
}
