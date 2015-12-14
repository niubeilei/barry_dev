////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestDriverUser.cpp
// Description:
//				Ocsp testing file - User Wrapper   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelApi.h"
#include "Util/String.h"
#include "Util/OmnNew.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "CertVerify/Tester/OcspTestDriver.h"
#include "CertVerify/Tester/OcspTestDriverUser.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tracer/TraceEntry.h"

bool
AosOcspTester_Start_User(u32 req_id,
                const char *serial_num,
                u32 serial_len,
                u32 num_tries,
                u32 max_conns)
{
	OmnString rslt;
    char cmd[100];
	
	//
	//	Prepare the command
	//
    sprintf(cmd, "ocsp api start %ld %s %ld %ld %ld", req_id, serial_num, serial_len, num_tries, max_conns);

	//	
	//	Call the function through the KAPI
	//
    bool r = OmnCliProc::getSelf()->runCli(cmd, rslt);
    if (!r)
    {
    	return false;
    }

	return true;
}

int
AosOcspTester_RetrieveRslt_User(
            u32 req_id,
			u8 *status,
            u32 *valid,
            u32 *invalid,
            u32 *failed)
{
    OmnString rslt;
    char cmd[100];

	unsigned int i1, i2, i3, i4;

    //
    //  Prepare the command
    //
    sprintf(cmd,"ocsp api request %ld", req_id);

    //
    //  Call the function through the KAPI
    //
    int tries = 0;
    while (tries++ < 10)
    {
        bool r = OmnCliProc::getSelf()->runCli(cmd, rslt);
        if (!r)
        {
            return false;
        }

        // 
        // The result is in the form:
        // status, passed, denied, failed
        //
        sscanf(rslt.data(), "%u %u %u %u", &i1, &i2, &i3, &i4);

        if (i1 == 0)
		{	
			*status = i1;
			*valid = i2;
			*invalid = i3;
			*failed = i4;
        	return true;
        }

		OmnSleep(1);
    }

    return false;
}

