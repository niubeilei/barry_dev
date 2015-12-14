////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Proxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once

typedef enum{TCP_APP=0, HTTP_APP, FTP_APP, FTP_DATA_APP, MAX_APP_TYPE} EAPP_TYPE; 

typedef struct __ProxyServer{
	CHAR sServiceName[128];
	EAPP_TYPE eAppType;
	DWORD dwLocalIP;
	SHORT sLocalPort;
	DWORD dwServerIP;
	SHORT sServerPort;
	BOOL  bSsl;
	BOOL  bStart;
	VOID* pData;
}ProxyServer_t, *PProxyServer_t;
