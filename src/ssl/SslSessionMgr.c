////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslSessionMgr.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include <linux/wait.h>
#include <linux/sched.h>
#endif

#include "aosUtil/Mutex.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/HashBin.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/string.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/jiffies.h"
#include "Porting/TimeOfDay.h"

#include "ssl/ReturnCode.h"
#include "ssl/SslSessionMgr.h"
#include "ssl/SslCommon.h"

static aos_mutex_t gsession_hashtable_lock;
static struct AosHashBin *gsession_hashtable = NULL;
static int ssl_session_mgr_init = 0;

u32 session_check_interval = 3600;	// one hour each time
u32 session_resume_life = 24*3600;	// 24 hours default


#ifdef __KERNEL__
wait_queue_head_t gsession_wait;
#endif
 

//
//Decryption:
//	Find the input sessionid in the saved session tree
//Parameters:
//	Input:
//		sessionid: the search session id
//Return:
//	not null: the finded session struct pointer
//	null: not found
//
struct aosSslSession* AosSsl_SessionFind(uint8 *sessionid, int sessionid_len)
{
	struct aosSslSession* session = NULL;
	int ret = eAosRc_Success;

	if (sessionid_len < eAosSSL_SessionIdLen)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "sessionid length is too long");
		return NULL;
	}
	
	aos_lock(gsession_hashtable_lock);
	ret = AosHashBin_get(gsession_hashtable, (char*)sessionid, sessionid_len, (void**)(&session), 0);
	aos_unlock(gsession_hashtable_lock);
	return session;
}

//
//Decryption:
//	save the resumed session
//Parameters:
//	Input:
//	 sessionid: the search session id
//	 sessionid_len: the length of session
//	 premaster_secret: the resumed premaster_secret
//	 premaster_secret_len: the length of the premaster secret
//	 expired: the resumed seesion expire time
//Return:
//	0: success
//	<0: failed
//
int AosSsl_SessionSave(
	char* name, 
	uint8 *sessionid, 
	int sessionid_len,
	uint8 *master_secret,
	int master_secret_len,
	uint8 *client_random,
	uint8 *server_random,
	asn_data_t *serial_number,
	uint32 expired)
{
	struct aosSslSession *session = NULL;
	int ret = eAosRc_Success;
	
	// alloc session struct memory
	session = (struct aosSslSession*)aos_malloc_atomic(sizeof(struct aosSslSession));
	if (!session)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to alloc session struct memory");
		return -eAosRc_MemErr;
	}
	
	// init session struct value
	aos_assert1((uint32)sessionid_len <= sizeof(session->sessionid));
	aos_assert1((uint32)master_secret_len <= sizeof(session->master_secret));
	strncpy(session->name, name, sizeof(session->name));
	memcpy(session->sessionid, sessionid, sessionid_len);
	memcpy(session->master_secret, master_secret, master_secret_len);
	memcpy(session->client_random, client_random, RANDOM_LENGTH);
	memcpy(session->server_random, server_random, RANDOM_LENGTH);
	if (serial_number)
		memcpy(&session->serial_number, serial_number, sizeof(asn_data_t));
	//session->time_expired = expired;
	session->time_expired = jiffies+session_resume_life*1000;
	
	// add session into hash table 
	aos_lock(gsession_hashtable_lock);
	ret = AosHashBin_add(gsession_hashtable, (char*)session->sessionid, sessionid_len, session, 1); 
	if( ret < 0 )
	{
		aos_free(session);
		aos_trace( "error to add session into hash table, entries=%d, max=%d, ret=%d",
			gsession_hashtable->numEntries, gsession_hashtable->maxSize, ret );
	}
	aos_unlock(gsession_hashtable_lock);
	
	return ret;
}

int AosSsl_SessionSetTimer(u32 interval)
{
#ifdef __KERNEL__
	session_check_interval = interval;
	wake_up(&gsession_wait);
#endif
	return eAosRc_Success;
}

#ifdef __KERNEL__
static int AosSsl_SessionMgrThread(void *data)
{
	struct aos_list_head *cur, *next;
	struct aosSslSession *session;
	struct aosSslSession *session_del;
	int	counts = 0;
	int 	counts_del;
	int total;
	while(1)
	{
		//aos_trace("Aos ssl session mgr timer");
		
		aos_lock(gsession_hashtable_lock);
		aos_list_for_each_safe (cur, next, &gsession_hashtable->list) 
		{
			counts++;
			session = (struct aosSslSession *)(((struct AosHashBin_entry*)cur)->value);
			if (!session) 
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "session pointer is null in session resumed hash table");
				aos_unlock(gsession_hashtable_lock);
				return -1;
			}
			if  (jiffies  > session->time_expired)
			{
				//aos_trace_hex("session expired", session->sessionid, sizeof(session->sessionid));
				AosHashBin_get(gsession_hashtable, session->sessionid, sizeof(session->sessionid), (void**)(&session_del), 1);
			}	

		}
		aos_unlock(gsession_hashtable_lock);

		if (counts >= SESSION_TABLE_SIZE)
		{
			total = counts;
			counts_del = (counts * SESSION_REMOVE_THRESHOLD)/100;
			counts = 0;
			aos_lock(gsession_hashtable_lock);
			aos_list_for_each_safe (cur, next, &gsession_hashtable->list) 
			{
				counts++;
				if( counts > counts_del) break;

				session = (struct aosSslSession *)(((struct AosHashBin_entry*)cur)->value);
				if (!session) 
				{
					aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "session pointer is null in session resumed hash table");
					aos_unlock(gsession_hashtable_lock);
					return -1;
				}
				//aos_trace("session index %d, counts_del %d, total %d", counts, counts_del, total);
				//aos_trace_hex("session expired", session->sessionid, sizeof(session->sessionid));
				AosHashBin_get(gsession_hashtable, session->sessionid, sizeof(session->sessionid), (void**)(&session_del), 1);
			}
			aos_unlock(gsession_hashtable_lock);
		}
		interruptible_sleep_on_timeout(&gsession_wait, session_check_interval*1000);
	}// while(1)
	
}

#endif 

int AosSsl_SessionMgrInit()
{
	int ret;

	if (ssl_session_mgr_init)
	{
		aos_min_log(eAosMD_SSL, "ssl session manager already inited"); 
		return eAosRc_Success;
	}

	AOS_INIT_MUTEX(gsession_hashtable_lock);
	// create session hash table
	if ((ret=AosHashBin_constructor(&gsession_hashtable, SESSION_TABLE_SIZE)) < 0)
	{
		aos_min_log(eAosMD_SSL, "session hash talbe allocation failed");
		return ret;
	}

	// 
	// Chen Ding, 11/09/2005
	//
	gsession_hashtable->maxSize = 2000;

	// init sesssion expired checking thread
#ifdef __KERNEL__
	if (kernel_thread(AosSsl_SessionMgrThread, NULL, 0)<0)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "session thread start failed");
		return -1;
	}
	init_waitqueue_head(&gsession_wait);	
#endif
	
	ssl_session_mgr_init = 1;
	aos_min_log(eAosMD_SSL, "ssl session manager inited"); 
	return eAosRc_Success;
}


// 
// The array used to store sessions for clients
//
struct AosClientSession
{
	u32					addr;
	int					second;
	aosSslSession_t		session;
};

static struct AosClientSession sgSslClientSessions[eAosSsl_MaxClientSessions];
static aos_mutex_t 		sgClientSessionLock;
static int				sgSslClientSessionInit = 0;


int AosSsl_clientSessionInit(void)
{
	if (sgSslClientSessionInit) return 0;

	AOS_INIT_MUTEX(sgClientSessionLock);
	memset(sgSslClientSessions, 0, sizeof(sgSslClientSessions));
	sgSslClientSessionInit = 1;
	return 0;
}


aosSslSession_t *
AosSsl_getClientSession(const u32 addr)
{
	int i;
	aosSslSession_t *session;

	if (sgSslClientSessionInit == 0) AosSsl_clientSessionInit();

	aos_lock(sgClientSessionLock);
	for (i=0; i<eAosSsl_MaxClientSessions; i++)
	{
		if (sgSslClientSessions[i].addr == addr)
		{
			session = &sgSslClientSessions[i].session;
			aos_unlock(sgClientSessionLock);
			return session;
		}
	}

	aos_unlock(sgClientSessionLock);
	return 0;
}


int AosSsl_removeClientSession(const char *sessionId)
{
	int i;

	if (sgSslClientSessionInit == 0) AosSsl_clientSessionInit();

	aos_lock(sgClientSessionLock);
	for (i=0; i<eAosSsl_MaxClientSessions; i++)
	{
		if (memcpy(sgSslClientSessions[i].session.sessionid, sessionId, 
				eAosSSL_SessionIdLen) == 0)
		{
			//
			// Found the entry
			//
			memset(&sgSslClientSessions[i], 0, 
				sizeof(struct AosClientSession));
			aos_unlock(sgClientSessionLock);
			return 0;
		}
	}

	aos_unlock(sgClientSessionLock);
	return -eAosRc_EntryNotFound;
}


int 
AosSsl_setClientSession(
			const u32 addr, 
			const char *sid, 
			const char *master_secret,
			uint8 *client_random,
			uint8 *server_random)
{
	int i;
	int found = -1;
	struct timeval tp;

	if (sgSslClientSessionInit == 0) AosSsl_clientSessionInit();

	OmnGetTimeOfDay(&tp);

	aos_lock(sgClientSessionLock);
	for (i=0; i<eAosSsl_MaxClientSessions; i++)
	{
		if (sgSslClientSessions[i].addr == addr)
		{
			memcpy(sgSslClientSessions[i].session.sessionid, sid, 
				eAosSSL_SessionIdLen);
			memcpy(sgSslClientSessions[i].session.master_secret, master_secret, 
				MAX_RSA_MODULUS_LEN);
			memcpy(sgSslClientSessions[i].session.client_random, client_random, RANDOM_LENGTH);
			memcpy(sgSslClientSessions[i].session.server_random, server_random, RANDOM_LENGTH);
			sgSslClientSessions[i].second = tp.tv_sec;
			aos_unlock(sgClientSessionLock);
			return 0;
		}
		else if (sgSslClientSessions[i].addr == 0)
		{
			found = i;
			break;
		}
	}

	if (found < 0)
	{
		// 
		// No empty slot. Remove the oldest one. 
		//
		int second = sgSslClientSessions[i].second;
		found = 0;

		for (i=1; i<eAosSsl_MaxClientSessions; i++)
		{
			if (sgSslClientSessions[i].second < second)
			{
				second = sgSslClientSessions[i].second;
				found = i;
			}
		}
	}
		
	sgSslClientSessions[found].addr = addr;
	memcpy(sgSslClientSessions[found].session.sessionid, sid, 
		eAosSSL_SessionIdLen);
	memcpy(sgSslClientSessions[found].session.master_secret, master_secret, 
		MAX_RSA_MODULUS_LEN);
 	memcpy(sgSslClientSessions[i].session.client_random, client_random, RANDOM_LENGTH);
    memcpy(sgSslClientSessions[i].session.server_random, server_random, RANDOM_LENGTH);
	sgSslClientSessions[found].second = tp.tv_sec;
	aos_unlock(sgClientSessionLock);
	return 0;
}


int AosSsl_resetClientSessions()
{
	if (sgSslClientSessionInit == 0) AosSsl_clientSessionInit();

	aos_lock(sgClientSessionLock);
	memset(sgSslClientSessions, 0, sizeof(sgSslClientSessions));
	aos_unlock(sgClientSessionLock);
	return 0;
}

