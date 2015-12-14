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
// This file contains types used for communications.
//
// Modification History:
// 02/22/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_net_types_h
#define aos_net_types_h

#include "porting_c/mutex.h"
#include "util_c/types.h"

#define AOS_MAX_CONN_READ_BUFF 0xffff

typedef enum
{
	eAosSockType_Invalid = 0,

	eAosSockType_Tcp, 
	eAosSockType_Udp, 
	eAosSockType_Unix, 

	eAosSockType_Last

} aos_sock_type_e;

static inline int aos_sock_type_check(const aos_sock_type_e type)
{
	return (type > eAosSockType_Invalid &&
			type < eAosSockType_Last)?1:0;
}

const char * aos_sock_type_2str(const aos_sock_type_e type);
aos_sock_type_e aos_sock_type_2enum(const char * const type);


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


#endif


