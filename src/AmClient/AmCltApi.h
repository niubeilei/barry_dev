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
#ifndef Aos_AmClient_AmCltApi_h
#define Aos_AmClient_AmCltApi_h

//
// Parameter:
// Input
// 		conn_str <addr>:<port>:<numPorts>:<svr_name>
// 
// Return code: 
// 			0 	means successful
// 			< 0 means internal error of the program
// 			> 0 means the user operation error
//
extern 
int aos_am_clt_init(const char ** addr, int *port, int *numPorts, const char ** svr_name, const int addr_num);
extern 
int aos_am_clt_uninit();
extern 
int aos_am_clt_test_conn();


//
// Return code: 
// 			0 	means successful
// 			< 0 means internal error of the program
// 			> 0 means the user operation error
//
extern 
int aos_am_clt_user_based_access(const char * app, 
						const char * user, 
						const char * opr, 
						const char * rsc, 
						char * errmsg, 
						const int errmsglen);


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
								 const int errmsglen);


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
								 const int errmsglen);


#endif

