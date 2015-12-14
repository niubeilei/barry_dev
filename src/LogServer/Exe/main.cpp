#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "LogServer/LogMgr.h"
#include "LogServer/LogReq.h"
#include "LogServer/Processor.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream.h>
#include <pthread.h>
#include <string.h>
#include <queue>


static AosReqDistrPtr sgDistributor;

static OmnString sgConfig =
        "<config local_addr=\"loopback\""
            "local_port=\"5729-5730\""
            "service_rcd=\"true\""
            "service_name=\"ReqDistrTest\""
            "support_actions=\"true\""
            "req_delinerator=\"first_word_high\""
            "max_proc_time=\"10000\""
            "max_conns=\"400\">"
        "</config>";

int main(int argc, char **argv)
{
	// Start MySQL and Connect database
    OmnApp theApp(argc, argv);
    try
    {
        OmnDataStoreMySQL::startMySQL("root", "chen0318", "trainingdb");
        theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
    }
    catch (const OmnExcept &e)
    {
        OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
        return -1;
    }

	/*
	AosLogServerPtr logServer = OmnNew AosLogServer();
	logServer->config(sgConfig);
	logServer->start();
	*/
	
	AosNetReqProcPtr processor = OmnNew AosLogProcessor();
	sgDistributor = new AosReqDistr(processor);
	if (!sgDistributor->config(sgConfig))
    {
        OmnAlarm << "Failed the configuration: " << sgConfig << enderr;
        return false;
    }
    sgDistributor->start();	
	
	while(1)
	{
		sleep(1);
	}

	return 0;
}
