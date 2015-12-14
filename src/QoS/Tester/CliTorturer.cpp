////////////////////////////////////////////////////////////////////////////
//
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ApplicationProxyTester.cpp
//// Description:
////   
////
//// Modification History:
//// 11/27/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#include "QoS/Tester/CliTorturer.h"

#include "Util/OmnNew.h"
#include "KernelInterface/CliProc.h"
#include "Random/Ptrs.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

//#include "QoS/qos_cli.h"

//extern int AosQos_init();

AosQoSCliTorturer::AosQoSCliTorturer()
{
//	AosQos_init();
	start();
}


bool AosQoSCliTorturer::start()
{
cout << "To run the command" << endl;

	aosRunCli("qos status on", true);

    return true;
}

