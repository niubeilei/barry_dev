////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: usbkey.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AppProxy_usbkey_h
#define AppProxy_usbkey_h

#ifdef __KERNEL__
#include <net/sock.h>
#endif

#define USBKEYMAX	1024
#define USBKEYLEN	18

#define IS_USBKEY_USER(context) (JNSTLS_RSA_WITH_NULL_MD5 == (context)->cipher_selected || JNSTLS_RSA_WITH_NULL_SHA == (context)->cipher_selected )


int	is_the_same_lan(struct socket *socket);
int	in_usbkey_list(char * id);

int aos_usbkey_add(char *data, unsigned *length, struct aosKernelApiParms *parms, char*errmsg, const int errlen);
int aos_usbkey_remove(char *data, unsigned *length, struct aosKernelApiParms *parms, char*errmsg, const int errlen);
int aos_usbkey_show(char *data, unsigned *length, struct aosKernelApiParms *parms, char*errmsg, const int errlen);
int aos_usbkey_save_config(char *data, unsigned *length, struct aosKernelApiParms *parms, char*errmsg, const int errlen);
int aos_usbkey_clear_config(char *data, unsigned *length, struct aosKernelApiParms *parms, char*errmsg, const int errlen);

#endif

