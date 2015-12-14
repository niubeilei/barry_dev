////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 5/22/2007: Created by cding
////////////////////////////////////////////////////////////////////////////

#include "AmClient/AmCltApi.h"
#include "AmClient/AmClient.h"
#include "AmClient/Ptrs.h"
#include "AmUtil/ReturnCode.h"
#include "alarm/Alarm.h"
#include "Util/OmnNew.h"


AosAmClientPtr gAmClient;
static bool sgAmClientInit = false;

// extern AosAmClientPtr gAosAmClient;
// extern static bool sgAmClientInit;


//
// Parameter:
// Input
// 		conn_str <addr>:<port>:<numPorts>:<svr_name>
// 
int aos_am_clt_init(const char ** addr, int *port, int *numPorts, const char ** svr_name, const int addr_num)
{
	int i = 0;
	
	gAmClient = OmnNew AosAmClient();
	sgAmClientInit = true;

//	sscanf(conn_str, "%s:%d:%d:%s", addr, &port, &numPorts, svr_name);
	for(i = 0; i < addr_num; i++)
	{
		if(!gAmClient->addServer(OmnIpAddr(addr[i]), port[i], numPorts[i], OmnString(svr_name[i])))
		{
			return -1;
		}
	}
	if(!gAmClient->start())
	{
		return -2;
	}
	return 0;
}
int aos_am_clt_uninit()
{
	if (!sgAmClientInit)
	{
		return 0;
	}
	if(!gAmClient)
	{
		return 0;
	}
	
	sgAmClientInit = false;
	gAmClient->stop();
	return 0;
}
int aos_am_clt_test_conn()
{
	if(gAmClient->isGood())
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


//
// Return code: 
// 			0 	means successful
// 			< 0 means internal error of the program
// 			> 0 means the user operation error
//
int aos_am_clt_user_based_access(const char * app, 
						const char * user, 
						const char * opr, 
						const char * rsc, 
						char * errmsg, 
						const int errmsglen)
{
    aos_assert_r(gAmClient, (-eAosRc_AmUnknownError));
    
    AosAmRespCode::E respCode;
    OmnString strApp(app), strUser(user), strOpr(opr), strRsc(rsc), strErrmsg;
    if (gAmClient->userBasedAccess(strApp, strUser, 
    							   strOpr, strRsc, 
    							   respCode, strErrmsg))
    {
		return 0;
	}
	strncpy(errmsg, strErrmsg.data(), errmsglen);
	errmsg[errmsglen-1] = 0;
	return respCode;
}


//
// Return code: 
// 			0 	means successful
// 			< 0 means internal error of the program
// 			> 0 means the user operation error
//
extern 
int aos_am_clt_user_based_access(const char * user, 
								 const unsigned int ip, 	/* DWORD BigEndian */
								 const short port,
								 char * errmsg, 
								 const int errmsglen)
{
    aos_assert_r(gAmClient, (-eAosRc_AmUnknownError));
    
    AosAmRespCode::E respCode;
    OmnString strErrmsg;
    if (gAmClient->userBasedAccess(user, 
    							   OmnIpAddr(ip), (u16)port, 
    							   respCode, strErrmsg))
    {
		return 0;
	}
	strncpy(errmsg, strErrmsg.data(), errmsglen);
	errmsg[errmsglen-1] = 0;
	return respCode;
}


//
// Return code: 
// 			0 	means successful
// 			< 0 means internal error of the program
// 			> 0 means the user operation error
//
extern 
int aos_am_clt_user_authenticate(const char * user, 
								 const char * pwd,
								 char * errmsg, 
								 const int errmsglen)
{
    aos_assert_r(gAmClient, (-eAosRc_AmUnknownError));
    
    AosAmRespCode::E respCode;
    OmnString strUser(user), strPwd(pwd), strErrmsg;
    if (gAmClient->authUser(strUser, strPwd, 
    						respCode, strErrmsg))
    {
		return 0;
	}
	strncpy(errmsg, strErrmsg.data(), errmsglen);
	errmsg[errmsglen-1] = 0;
	return respCode;
}

