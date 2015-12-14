////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslSessionMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __SSL_SESSION_MGR_H__
#define __SSL_SESSION_MGR_H__

#include "ssl/SslCommon.h"
#include "PKCS/asn1.h"

#define SESSION_TABLE_SIZE 2048
#define SESSION_REMOVE_THRESHOLD 20


extern u32 session_check_interval;    // one hour each time
extern u32 session_resume_life;    // 24 hours default

typedef struct aosSslSession{
	char name[16];
	uint8 sessionid[eAosSSL_SessionIdLen];
	uint8 master_secret[MASTER_SECRET_LENGTH];
	uint8 client_random[RANDOM_LENGTH];
	uint8 server_random[RANDOM_LENGTH];
	asn_data_t serial_number;
	uint32 time_updated;	// secodes, time updating this value
	uint32 time_expired;	// seconds, normally we set this 24 hours= 24*60*60
	//??? timer needed
} aosSslSession_t;

extern int AosSsl_SessionMgrInit(void);
extern aosSslSession_t * AosSsl_getClientSession(const u32 addr);
extern int AosSsl_setClientSession(
			const u32 addr, 
			const char *session_id,
			const char *master_secret,
			uint8 *client_random,
            uint8 *server_random);
extern int AosSsl_resetClientSessions(void);
extern int AosSsl_removeClientSession(const char *sessionId);


extern int AosSsl_SessionSave(
	char* name, 
	uint8 *sessionid, 
	int sessionid_len,
	uint8 *master_secret,
	int master_secret_len,
	uint8 *client_random,
	uint8 *server_random,
	asn_data_t *serial_number,
	uint32 expired);

extern struct aosSslSession* AosSsl_SessionFind(uint8 *sessionid, int sessionid_len);
extern int AosSsl_SessionSetTimer(u32 interval);
#endif // #ifndef __SSL_SESSION_MGR_H__

