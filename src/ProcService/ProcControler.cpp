////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
#include "ProcService/ProcControler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "ProcUtil/ManagedServer.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/SeXmlParser.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"

#include <deque>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/procfs.h>
#include <unistd.h>
#include <stropts.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <signal.h>


using namespace std;

OmnSingletonImpl(AosProcControlerSingleton,
                 AosProcControler,
                 AosProcControlerSelf,
                "AosProcControler");

AosProcControler::AosProcControler()
:
mLock(OmnNew OmnMutex())
{
}


AosProcControler::~AosProcControler()
{
}

bool
AosProcControler::start()
{
	return true;
}


bool
AosProcControler::start(const AosXmlTagPtr &config)
{
	// 	<config ...>
	// 		<procmgr>
	// 			<servers>
	// 				<server ip="xxx" port="xxx"/>
	// 				<server ip="xxx" port="xxx"/>
	// 				...
	// 			</servers>
	// 			<processes>
	// 				<process name="xxx">
	// 					<startup><![CDATA[./htmlserver.exe -s -log2]]></startup>
	// 				</process>
	// 			</processes>
	// 		</procmgr>
	// 		...
	// 	</config>
	
	mConfig = config;
	aos_assert_r(mConfig, false);
	AosXmlTagPtr processes = mConfig->getFirstChild("processes");
	aos_assert_r(processes, false);
	//get the process
	AosXmlTagPtr process = processes->getFirstChild();
	while(process)
	{
		OmnString name = process->getAttrStr("name", "");
		if(name == "")
		{
			OmnString errmsg = "Failed to get the process name!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
		OmnString startupstr = process->getNodeText("startup");
		if(startupstr == "")
		{
			OmnString errmsg = "Failed to get the process startupstr!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
		AosProcess value;
		value.mStartUpCmd = startupstr;
		mProcesses[name] = value;
		process = processes->getNextChild();
	}

	OmnThreadPtr thread;
	OmnThreadedObjPtr thisPtr(this, false);
	thread = OmnNew OmnThread(thisPtr, "ProcControlerThrd", 0, true, true, __FILE__, __LINE__);
	thread->start();


	
	//test function
	//testFunction();
	return true;
}

bool
AosProcControler::getSysInfo(AosXmlTagPtr &cmd, OmnString &resp)
{
	//response format:
	//<cmd>
	//	<content>
	//	</content>
	//</cmd>
	
	resp << "<record machine_name=\"" << mConfig->getAttrStr("machine_name", "") << "\" ><![CDATA[";
	FILE *fp;
	if(NULL == (fp = fopen("/proc/meminfo", "r")))
	{
		perror("fopen --------");
	}
	char sLine[2000];
//	while(!feof(fp))
	while(fgets(sLine, 2000, fp) != NULL)
	{
//		fgets(sLine, 2000, fp);
OmnScreen << "get zhao ----:" << sLine << endl;
		OmnString line(sLine);
		OmnStrParser1 parser(line, ":");
		OmnString name = parser.nextWord();
		OmnString value = parser.nextWord();
		resp << name << "=\"" << value << "\" ";
	}
	fclose(fp);
	resp << "]]></record>";
	return true;
}

bool
AosProcControler::controlProcess(AosXmlTagPtr &cmd, OmnString &resp)
{
	//request:
	//<proccess name="xxx" action="shutdown"/>
	//
	//response format:
	//<cmd>
	//	<content>
	//	</content>
	//</cmd>
	
	OmnString pname = cmd->getAttrStr("process_name", "");
	if(pname == "")
	{
		OmnString errmsg = "Failed to get the process name!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	OmnString action = cmd->getAttrStr("action", "");
	if(action == "getinformation")
	{
		getSysInfo(cmd, resp);
	}
	else if(action == "shutdown")
	{
		killProcess(pname, resp);
	}
	else if(action == "startup")
	{
		startProcess(pname, resp);
	}
	else
	{
		OmnString errmsg = "the process action is unrecognizable!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}

bool
AosProcControler::getPidByName(const OmnString &name, pid_t &pid)
{
	DIR *   dirHandle;  /* direct handler   */
	struct dirent * dirEntry;   /* single direct */
	bool find = false;
	pid = -1;

	if ( ( dirHandle = opendir( "/proc" ) ) == NULL )
	{
		return	false;
	}
	chdir( "/proc" );  /* go to "/proc" direct */
	while ( ( dirEntry = readdir( dirHandle ) ) != NULL )
	{
		//read buff
		FILE *fp;
		char str[500];
		string filename;
		filename += "/proc/";
		filename += dirEntry->d_name;
		filename += "/cmdline";
		const char * fn = filename.data();
		if((fp=fopen(fn,"rt"))==NULL)
		{
			//can not open the file
			continue;
		}
		fgets(str,500,fp);
		if(strcmp(str, name.data()) == 0)
		{
			find = true;
			pid = ( pid_t )atoi( dirEntry->d_name );
			printf("<find ---- >%s\n",str);
			break;  /* exit while */
		}
		fclose(fp);
	}
	closedir( dirHandle );
	return	find;
}

bool
AosProcControler::startProcess(const OmnString &pname, OmnString &resp)
{
	bool find = false;
	map<OmnString, AosProcess>::iterator it;
	for( it = mProcesses.begin(); it!=mProcesses.end(); it++)
	{
		if(it->first == pname)
		{
			find = true;
			OmnString cmd = it->second.mStartUpCmd;
			system(cmd.data());
			break;
		}
	}
	if(find)
	{
		resp << "Success to start the process";
		return true;
	}
	else
	{
		resp << "Fail to start the process";
		return false;
	}
}

bool
AosProcControler::killProcess(const OmnString &pname ,OmnString &resp)
{
	pid_t pid;
	bool find = getPidByName(pname, pid);
	if(find)
	{
		int rslt = kill(pid, 14);
		if(rslt != 0)
		{
			//force to kill
			kill(pid, 9);
		}
		resp << "Success to kill the process";
	}
	else
	{
		resp << "Cannot find the process";
	}
	return true;
}

bool
AosProcControler::checkProcessExist()
{
	//1.check the processes exist or not
	//2.if some processes are not exist, then startup
	map<OmnString, AosProcess>::iterator it;
	for( it = mProcesses.begin(); it!=mProcesses.end(); it++)
	{
		//process is not exist. startup the process
		OmnString resp;
		OmnString pname = it->first;
		startProcess(pname, resp);
	}
	return true;
}

bool
AosProcControler::stop()
{
    return true;
}


bool
AosProcControler::config(const AosXmlTagPtr &def)
{
	return true;
}


bool    
AosProcControler::signal(const int threadLogicId)
{
	return true;
}


bool    
AosProcControler::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosProcControler::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
//		checkProcessExist();
		OmnSleep(10);
	}
	return true;
}

void
AosProcControler::testFunction()
{
	//test kill process ------start
/*	OmnString cmd;
	cmd << "<proccess name=\"/home/dev/AOS/src/HtmlServer/htmlserver.exe\" action=\"shutdown\"/>";
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(cmd, "");
	OmnString resp;
	controlProcess(root, resp);
*/	//test kill process ------end
	
	//test startup proces ------start
	OmnString cmd;
	cmd << "<proccess process_name=\"/home/dev/AOS/src/HtmlServer/htmlserver.exe\" action=\"startup\"/>";
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(cmd, "" AosMemoryCheckerArgs);
	OmnString resp;
	controlProcess(root, resp);
	//test startup proces ------end
	
}
