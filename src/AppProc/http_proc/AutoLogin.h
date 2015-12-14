////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AutoLogin.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AppProc_hpptproc_AutoLogin_h
#define Aos_AppProc_hpptproc_AutoLogin_h

#include "aosUtil/List.h"
#include "AppProc/ReturnCode.h"

enum
{
	eAosHttp_AspReqSize     = 3000,
	eAosHttp_MaxAloginFileNameSize = 128,
	eAosHttp_MaxUsernameLen = 32,

	eAosHttp_ParamType_None = 0,
	eAosHttp_ParamType_Cert = 1,
	eAosHttp_ParamType_Other = 2,

	eAosHttp_CertField_None = 0,
	eAosHttp_CertField_Serialno = 1,
	eAosHttp_CertField_Username = 2,
	eAosHttp_CertField_Id = 3,
	eAosHttp_CertField_Usergroup1 = 4,
	eAosHttp_CertField_Usergroup2 = 5,
	eAosHttp_CertField_Usergroup3 = 6,
	eAosHttp_CertField_Organize = 7,
	eAosHttp_CertField_Department = 8,
	eAosHttp_CertField_Headship = 9,
	eAosHttp_CertField_Rights = 10,
	eAosHttp_CertField_Max,
};

struct AosHttp_AloginParam
{
	struct aos_list_head list;	
	uint8_t type;			//parameter type: none=0, cert_type=1, other_type=2;
	uint8_t name[eAosHttp_MaxUsernameLen+1]; //the name of parameter variable
	uint8_t cert_field;		//which filed in certificate the parameter is come
	uint8_t name_size;	// stands for format in GUI 
};

struct AosHttp_AloginFileConf
{
	struct aos_list_head list;
	char	alogin_filename[eAosHttp_MaxAloginFileNameSize+1];
	struct aos_list_head param_list;
};

struct AosHttp_AloginConf
{
	uint8_t auto_login;
	struct aos_list_head alogin_file_list;
};

struct AosCertificate;
struct aosKernelApiParms;
struct tcp_vs_service;
extern int aosHttpAutoLogin_SetStatusCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aosHttpAutoLogin_saveConfCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aosHttpAutoLogin_configAutoLoginCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aosHttpAutoLogin_proc(struct tcp_vs_service *service, 
						  const char *url, 
						  const struct AosCertificate *cert,
						  char **data);
extern int aosHttpAutoLogin_init(void);

#endif

