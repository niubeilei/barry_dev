////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: amm_priv.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __amm_priv__
#define __amm_priv__

#define AMM_NAME_MAX_SIZE	128
typedef struct _amm_config {
	unsigned int 		flags;

	//Amm config items
	unsigned int		addr;
	unsigned short		port;

	char 			AZN_C_ORG_NAME[AMM_NAME_MAX_SIZE];
	char			AZN_C_APPSYS_NAME[AMM_NAME_MAX_SIZE];
	char			org[AMM_NAME_MAX_SIZE];

	time_t			request_timeout;

	//list of request, so on...
	spinlock_t 		request_lock;
	struct list_head 	request_list;
}	AMM_CONFIG;


#define AMM_FLAGS_NONE		0x00000000

#define AMM_STATUS_ON		0x00000001
#define AMM_PROXY_SET		0x00000002
#define AMM_ORG_SET		0x00000004

#define AMM_PMI_READY		(AMM_STATUS_ON | AMM_PROXY_SET | AMM_ORG_SET)

#define AMM_SOCKET_CREATED	0x00000100

#define AMM_SOCK_WAITTING	0x00001000
#define AMM_DATA_WAITTING	0x00002000
#define AMM_DATA_WAITTING_MORE	0x00004000
#define AMM_SOCK_ERROR		0x00008000

//response of access proxy
#define AOS_ACCESS_FAILED	-1
#define AOS_ACCESS_PERMIT	0
#define AOS_ACCESS_DENY		1

typedef struct {
	UINT32 length;
	UINT32 sum;
	UINT32 serial;
	UINT32 command;
	UINT32 content;
	UINT32 data[0];
}	AMM_PROTO_HEADER;

#define AMM_PROTO_DATA_LEN	(sizeof(AMM_PROTO_HEADER))

#define AMM_REOURCE_NAME_MAX	255

typedef struct _access_request {
	UINT32			flags;

	struct list_head	link;

	struct aos_tcpapi_data *sock;

	struct aos_httpmsg	*msg;
	int 			operation;
	struct aos_user 	*user;
	char 			*domain;
	char			*sysname;
	char			*resource_name;
	AMM_CALLBACK		callback;
}	AMM_REQUEST;

#endif
