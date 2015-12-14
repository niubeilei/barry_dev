////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTesterKernelApi.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "CertVerify/Tester/OcspTestDriver.h"
#include "aos/aosKernelApi.h"
#include "CertVerify/Tester/OcspTesterKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Tracer.h"



int aosTestOcspStartApi(
			char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen)
{
	// 
	// ocsp api req_id serial_num serial_len num_tries max_conns
	// 
	int req_id = parms->mIntegers[0];
	char * serial_num = parms->mStrings[0];
	int serial_len = parms->mIntegers[1];
	int num_tries = parms->mIntegers[2];
	int max_conns = parms->mIntegers[3];
	int ret;

    aos_trace("In aosOcspApi kernel\n");
	ret = AosOcspTester_start(req_id, serial_num, serial_len, num_tries, max_conns);

	*length = 0;
    return ret;
}


int aosTestOcspRetrieveApi(
			char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen)
{
	// 
	// ocsp api req_id serial_num serial_len num_tries max_conns
	// 
	int req_id = parms->mIntegers[0];
	u32 passed, denied, failed;
	u8 status;
	int ret;
	char local[200];
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;

    aos_trace("In aosOcspApi kernel\n");
	ret = AosOcspTester_retrieveRslt(req_id, &status, &passed,
            &denied, &failed);

aos_trace("In retrieveRslt KernelAPI: requestID:%d, status:%d, passed:%d,\
			denied:%d, failed:%d",
        req_id, status,
        passed,
        denied,
        failed);

    sprintf(local, "%u %u %u %u", status, passed, denied, failed);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
    return ret;
}


