////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosAccessManage.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __user_access_control__
#define __user_access_control__

#include "CertVerify/common.h"

/* user certificate serial number */
#define SERIAL_NO_MAXLEN 32
struct aos_user {
	unsigned char data[SERIAL_NO_MAXLEN];
	int		len;
};

enum {
	eAosRead,
	eAosWrite,
	eAosModify,
	eAosCreate,
	eAosDelete,
};

/* define of permit returned by callback */
#define eAosRc_AccessPermit	0
#define eAosRc_AccessDeny	1
#define eAosRc_AccessError	-1

struct aos_httpmsg;

typedef int (*AMM_CALLBACK) (struct aos_httpmsg *msg,
                          	int permit,
				struct aos_user *user,
				char *domain,
				char *sysname,
				char *resource_name);


/* return by aos_check_permit */
#define eAosRc_AccessError	-1
#define eAosRc_AccessAsync	0

struct aosKernelApiParms;
extern int aos_amm_set_proxy(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_set_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_set_org(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_set_timeout(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_show_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_clear_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_amm_save_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);

extern int aos_check_permit(struct aos_httpmsg *msg,
			int operation,
			struct aos_user *user,
			char *domain,
			char *sysname,
			char *resource_name,
			AMM_CALLBACK callback);

#endif
