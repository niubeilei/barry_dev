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
// 2015/03/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"

#include "Alarm/Alarm.h"
#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h" 
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "JimoProg/JimoProg.h"
#include "JQLParser/JQLParser.h"
#include "JQLStmtUtil/JqlStatement.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DllMgrObj.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/JimoProgObj.h"
#include "SEInterfaces/JqlStatementObj.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
#include "aosDLL/DllMgr.h"
#include "UtilData/ClusterNames.h"
#include <signal.h>  

//#include "JimoLogicNew/JimoLogicCreateStat.h"
//#include "JimoParser/JimoParser.h"
#include "JimoProg/JimoProgMgr.h"

int gAosShowNetTraffic = 0;

static void AosExitApp()
{
}

static bool needStop = false;
static int sgStatusFreq = 600;

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int 
main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, aosSignalHandler);

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.1");

	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

	AosXmlTagPtr app_config;

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(app_config, false);
			OmnApp::setConfig(app_config);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			int max_alarms = atoi(argv[index+1]);
			index += 2;
			OmnAlarmMgr::setMaxAlarms(max_alarms);
			continue;
		}
		
		index++;
	}


	OmnWait::getSelf()->start();
	OmnTimeDriver::getSelf()->start();

	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());
	AosRundataPtr rdata = OmnNew AosRundata();

//	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosJimoParser", 1);
//	aos_assert_r(jimo, -1);

//	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosJimoProgMgr", 1);
//	aos_assert_r(jimo, -1);
//	AosJimoProgMgrObj *prog_mgr = dynamic_cast<AosJimoProgMgrObj *>(jimo.getPtr());
//	aos_assert_r(prog_mgr, -1);
//	AosJimoProgObjPtr prog = prog_mgr->getJimoProg(rdata.getPtr(), "job1");

	// prog_mgr.getJimoProg(rdata.getPtr(), "job1");

//	AosJimoLogicCreateStat job(1);
//	AosJimoParser ji(rdata.getPtr(),1);
//	 AosJimoProgMgr ss(1);
//	AosJimoParserObj *jimo_parser = dynamic_cast<AosJimoParserObj *>(jimo.getPtr());
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata.getPtr());
	aos_assert_r(jimo_parser, false);
//	AosJimoProgObjPtr prog = Jimo::jimoGetJimoProg(rdata.getPtr(),"jimo_prog");
	OmnString stmt;
	vector<AosJqlStatementPtr> statements;
	bool rslt;
	/*
	stmt("create job job1;");
	jimo_parser->parse(rdata.getPtr(),stmt,statements);
	aos_assert_r(statements.size() == 1, -1);
	AosJimoProgObjPtr prog;
	rslt = statements[0]->runJQL(rdata.getPtr(), "");
	aos_assert_r(rslt, false);

	prog = Jimo::jimoGetJimoProg(rdata.getPtr(), "job1");  
	aos_assert_r(prog, false);
	*/

	/*
	stmt << "create statistics mystat "
	 	 <<"("
	 	 <<" \"table\":\"dat_top_aaa_hour_bsid8\","
	 	 <<" \"keyfields\":[\"bsid8_id\", \"hour\",\"service_option\",\"roamflag\"],"
	 	 <<" \"measures\":[\"sum(ppp_num)\",\"sum(cost_num)\", \"sum(active_time)\",\"sum(up_flow)\","
					   <<" \"sum(down_flow)\",\"sum(up_pack)\",\"sum(down_pack)\",\"count(*)\"],"
		 <<" \"Time Field\":\"stat_time\","
		 <<" \"format\":\"yyyy-mm-dd hh:mm:ss\","
		 <<" \"inputs\":\"input1\","
		 <<" \"Filters\":\"cond1\","
	 	 <<" \"timeUnit\": \"_day\""
	 	 <<");";
	
	stmt << "create statistics mystat"                                                                   
		 << "("                                                                                           
	 	 << " \"table\":\"mydb_mytab\","                                                       
	 	 << " \"keyfields\":[\"f1\"],"
		 << " \"measures\":[\"sum(f2)\",\"count(*)\"],"         
	 	 << " \"Time Field\":\"t1\","                                                               
	 	 << " \"Format\":\"yyyy-mm-dd hh:mm:ss\","                                                         
	 	 << " \"Inputs\":\"input1\","                                                                      
	 	 << " \"Filters\":\"cond1\","                                                                      
	 	 << " \"timeUnit\": \"_day\""                                                                      
		 << ");"; 
	*/


	stmt << "create statistics mystat"                                                                   
		 << "("                                                                                           
	 	 << " \"table\":\"db_t1\","                                                       
	 	 << " \"keyfields\":[\"k1\"],"
		 << " \"measures\":[\"sum(v2)\"],"         
	 	 << " \"Inputs\":\"t1_dataset\""                                                                     	   << ");"; 

	statements.clear();
	jimo_parser->parse(rdata.getPtr(),stmt,statements);
	aos_assert_r(statements.size() == 1, -1);
	AosJimoProgObj *prog;
	rslt = statements[0]->runJQL(rdata.getPtr());
	aos_assert_r(rslt, false);

	while (1)
	{
		OmnSleep(1);
		if (needStop)
		{
			AosExitApp();
			break;
		}
	}

	theApp.exitApp();
	return 0;
} 

