////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "CliClient/CliClient.h"
#include "Porting/ThreadDef.h"
#include "OCSPServer/Ptrs.h"
#include "OCSPServer/OCSPServer.h"
#include "OCSPServer/OCSPServerConfig.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util1/Time.h"
#include "XmlParser/XmlParser.h"


extern OmnTracer *		OmnTracerSelf;
static void 			startSingletonClasses();
static OmnXmlParserPtr 	sgParser;
static OmnSingletonMgr 	sgSingletonMgr;


int 
main(int argc, char **argv)
{	
	OmnApp theApp(argc, argv);
	OmnString errMsg;

	OmnTracer* tracer = OmnTracer::getSelf();

	try
	{
		sgParser = new OmnXmlParser(sgOCSPServerConfigStr);
		tracer->config(sgParser);
		AosCliClientPtr	cliClient = new AosCliClient();
		if(!cliClient->config(sgParser))
		{
			cout << "\nError Config.\n" << endl;
			return 0;
		}

        if(!cliClient->start())
        {
            cout << "\nError in start.\n" << endl;
            return 0;
        }


		startSingletonClasses();

				
		
//		if(!AosOCSPServerSelf->config(sgParser))
//		{
//			cout << "\nError Config.\n" << endl;
//			return 0;
//		}

//		if(!AosOCSPServerSelf->start())
//		{
//			cout << "\nError in start.\n" << endl;
//			return 0;
//		}
		
		AosOCSPServerSelf->setCliClient(cliClient);
		AosOCSPServerSelf->registerCliCallbackFunc();

		tracer->start();
	}

	catch (const OmnExcept &e)
	{
		cout << "******* " << endl;
		OmnTracerSelf->closeTracer();
		exit(0);
	}
	
	theApp.appLoop();

	sgSingletonMgr.stopSingleton();
	sgSingletonMgr.deleteSingleton();
	OmnTracerSelf->closeTracer();

	exit(0);
} 


void
startSingletonClasses() 
{
	//
	// Create singleton objects
	//
	OmnTrace << "Create singleton objects" << endl;


	sgSingletonMgr.addSingleton(new AosOCSPServerSingleton());
	sgSingletonMgr.start(sgParser);
}

