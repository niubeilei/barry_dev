////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslRecord.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_SslRecord_h
#define Aos_ssl_SslRecord_h

#ifdef __KERNEL__
#include <asm/current.h>
#include <linux/sched.h>
#include "Porting/TimeOfDay.h"
#endif

#include "aosUtil/Types.h"
#include "aosUtil/System.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tcp.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/string.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/ReturnCode.h"



// 
// It constructs a new record (containing one handshake message) in 'record' 
// based on the parameters passed in.
// If the record is constructed successfully, it moves 'msg_start' to the next
// message start.
//
static inline int AosSslRecord_createHandshake(
			AosSslRecord_t *record,
			u8 msg_type,
			u16 msg_body_len)
{
	u32 msg_start = record->msg_start;
	u32 rec_start = record->record_start;

	aos_assert1(record->msg_start >= 5);
	aos_assert1(msg_start + msg_body_len < record->buffer_len);

	if (record->record_type == eAosContentType_Handshake)
	{
		record->record_body_len += msg_body_len + 4;
		record->record_data_len += msg_body_len + 4;
	}
	else
	{
		aos_assert1(record->num_records < eAosSSL_MaxNumRecords);
		record->record_types[record->num_records++] = eAosContentType_Handshake;
		record->record_type = eAosContentType_Handshake;
		record->record_body[msg_start-5] = eAosContentType_Handshake;
		record->record_start = msg_start-5;
		record->record_body_len += msg_body_len + 4;
		record->record_data_len += msg_body_len + 4 + 5;
		record->record_body[msg_start-4] = eAosSSL_VersionMajor;
		record->record_body[msg_start-3] = eAosSSL_VersionMinor;
		record->record_ver_major = eAosSSL_VersionMajor;
		record->record_ver_minor = eAosSSL_VersionMinor;
	}

	record->record_body[rec_start+3] = (((unsigned int)record->record_body_len) >> 8);
	record->record_body[rec_start+4] = (((unsigned int)record->record_body_len) & 0xff);
	record->record_body[msg_start++] = msg_type;
	record->record_body[msg_start++] = 0;
	record->record_body[msg_start++] = (msg_body_len >> 8);
	record->record_body[msg_start++] = (msg_body_len & 0xff);

	record->msg_type = msg_type;
	record->msg_body_len = msg_body_len;

	return 0;
}


static inline int AosSslRecord_moveToNextMsg(AosSslRecord_t *record)
{
	if (record->msg_start + record->msg_body_len + 4 >= record->buffer_len)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Out of bound: %d, %d, %d", record->msg_start, 
			record->msg_body_len, record->record_body_len);
	}

	if (record->record_type == eAosContentType_ChangeCipherSpec)
	{
		record->record_start += 6;
		record->msg_start = record->record_start + 5;
	}
	else if (record->record_type == eAosContentType_PlainData)
	{
		record->msg_start += record->msg_body_len;
	}
	else
	{
		if (record->msg_type == eAosSSLMsg_Finished)
		{
			record->msg_start += record->msg_body_len;
		}
		else
		{
			record->msg_start += record->msg_body_len + 4;
		}
	}
	return 0;
}


// 
// It constructs a new record (containing one change cipher spec message) in 'record' 
// If the record is constructed successfully, it moves 'msg_start' to the next
// message start.
//
static inline int AosSslRecord_createChangeCipherSpec(AosSslRecord_t *record)
{
	u32 index;

	if (record->record_type != eAosContentType_NotSet)
	{
	 	record->msg_start += 5;
	}

	index = record->msg_start;

	aos_assert1(record->msg_start >= 5);
	aos_assert1(index + 1 < record->buffer_len);
	aos_assert1(record->num_records < eAosSSL_MaxNumRecords);

	record->record_start = index-5;
	record->record_data_len += 6;
	record->record_body[index-5] = eAosContentType_ChangeCipherSpec;
	record->record_body[index-4] = eAosSSL_VersionMajor;
	record->record_body[index-3] = eAosSSL_VersionMinor;
	record->record_body[index-2] = 0;
	record->record_body[index-1] = 1;
	record->record_body[index] = 1;

	record->record_body_len = 1;
	record->record_type = eAosContentType_ChangeCipherSpec;
	record->record_types[record->num_records++] = eAosContentType_ChangeCipherSpec;
	record->msg_body_len = 1;

	return 0;
}


/*
static inline int AosSslRecord_createPlainData(AosSslRecord_t *record, u32 data_len)
{
	aos_assert1(record->record_start + data_len < record->buffer_len);
	aos_assert1(record->msg_start >= 5);

	record->msg_start -= 5;
	record->record_start = record->msg_start;
	record->record_data_len += data_len;

	record->record_type = eAosContentType_PlainData;
	record->record_types[record->num_records++] = eAosContentType_PlainData;
	record->msg_body_len = data_len;

	return 0;
}
*/


static inline int AosSslRecord_createFinishedMsg(
			AosSslRecord_t *record,
			u32 msglen)
{
	u32 msg_start;

	msg_start = record->msg_start;

	aos_assert1(record->msg_start >= 5);
	aos_assert1(msg_start + msglen < record->buffer_len);

	aos_assert1(record->num_records < eAosSSL_MaxNumRecords);
	record->record_body_len = msglen;
	record->record_data_len += msglen + 5;
	record->record_types[record->num_records++] = eAosContentType_Handshake;
	record->record_type = eAosContentType_Handshake;
	record->record_body[msg_start-5] = eAosContentType_Handshake;
	record->record_body[msg_start-4] = eAosSSL_VersionMajor;
	record->record_body[msg_start-3] = eAosSSL_VersionMinor;
	record->record_body[msg_start-2] = ((record->record_body_len) >> 8);
	record->record_body[msg_start-1] = ((record->record_body_len) & 0xff);
	record->record_start = msg_start-5;
	record->record_ver_major = eAosSSL_VersionMajor;
	record->record_ver_minor = eAosSSL_VersionMinor;
	record->msg_type = eAosSSLMsg_Finished;
	record->msg_body_len = msglen;

	return 0;
}


static inline int AosSslRecord_createAppData(
			AosSslRecord_t *record,
			u32 msglen)
{
	u32 msg_start;

	msg_start = record->msg_start;
	
	aos_assert1(record->msg_start >= 5);
	aos_assert1(msg_start + msglen <= record->buffer_len);

	aos_assert1(record->num_records < eAosSSL_MaxNumRecords);
	record->record_body_len = msglen;
	record->record_data_len += msglen + 5;
	record->record_types[record->num_records++] = eAosContentType_AppData;
	record->record_type = eAosContentType_AppData;
	record->record_body[msg_start-5] = eAosContentType_AppData;
	record->record_body[msg_start-4] = eAosSSL_VersionMajor;
	record->record_body[msg_start-3] = eAosSSL_VersionMinor;
	record->record_body[msg_start-2] = ((record->record_body_len) >> 8);
	record->record_body[msg_start-1] = ((record->record_body_len) & 0xff);
	record->record_start = msg_start-5;
	record->record_ver_major = eAosSSL_VersionMajor;
	record->record_ver_minor = eAosSSL_VersionMinor;
	record->msg_type = eAosContentType_AppData;
	record->msg_body_len = msglen;

	return 0;
}


static inline int AosSslRecord_createAppDataMsg(
			AosSslRecord_t *record,
			u32 msglen)
{
	u32 start = record->msg_start;

	aos_assert1(record->msg_start >= 5);
	aos_assert1(start + msglen < record->buffer_len);
	aos_assert1(record->num_records < eAosSSL_MaxNumRecords);
	record->record_start = start - 5;
	record->record_data_len += msglen + 5;
	record->record_body[start-5] = eAosContentType_AppData;
	record->record_body[start-4] = eAosSSL_VersionMajor;
	record->record_body[start-3] = eAosSSL_VersionMinor;
	record->record_body[start-2] = 0;
	record->record_body[start-1] = msglen;

	record->record_type = eAosContentType_AppData;
	record->record_types[record->num_records++] = eAosContentType_AppData;
	record->msg_body_len = msglen;

	return 0;
}

static inline int AosSslRecord_createAlert(
			AosSslRecord_t *record,
			u32 msglen)
{
	u32 msg_start;

	msg_start = record->msg_start;

	aos_assert1(record->msg_start >= 5);
	aos_assert1(msg_start + msglen < record->buffer_len);

	aos_assert1(record->num_records < eAosSSL_MaxNumRecords);
	record->record_body_len = msglen;
	record->record_data_len += msglen + 5;
	record->record_types[record->num_records++] = eAosContentType_Alert;
	record->record_type = eAosContentType_Alert;
	record->record_body[msg_start-5] = eAosContentType_Alert;
	record->record_body[msg_start-4] = eAosSSL_VersionMajor;
	record->record_body[msg_start-3] = eAosSSL_VersionMinor;
	record->record_body[msg_start-2] = ((record->record_body_len) >> 8);
	record->record_body[msg_start-1] = ((record->record_body_len) & 0xff);
	record->record_start = msg_start-5;
	record->record_ver_major = eAosSSL_VersionMajor;
	record->record_ver_minor = eAosSSL_VersionMinor;
	record->msg_type = eAosContentType_Alert;
	record->msg_body_len = msglen;

	return 0;
}


struct tcp_vs_conn;
struct AosSslRecord;
struct aosSslContext;

extern int AosSslRecord_nextMsg(AosSslRecord_t *record, char expectFinished);

extern int AosSslRecord_getNextRecord(
	char *data, 
	unsigned int data_len,
	u32 flags,
	struct aosSslContext *context);

extern int AosSslRecord_getOneRecord(
	const char *data, 
	const unsigned int data_len, 
	u32 flags,
	struct AosSslRecord *record, 
	unsigned int *remain_len, 
	char expectFinished);

static inline char *AosSslRecord_getMsgBody(struct AosSslRecord *record)
{
	if (!record->record_body)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Record body is null");
		return 0;
	}

	if (record->record_type == eAosContentType_PlainData)
	{
		return record->record_body;
	}
	else
	{
		return &record->record_body[record->msg_start+4];
	}
}


static inline char *AosSslRecord_getMsg(struct AosSslRecord *record)
{
	if (!record->record_body)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Record body is null");
		return 0;
	}

	return &record->record_body[record->msg_start];
}

static inline int AosSslRecord_hasMoreMsg(struct AosSslRecord *record)
{
	if (record->record_type == eAosContentType_PlainData) return 0;
	if (record->msg_type == eAosSSLMsg_Finished) return 0;

	return record->msg_start + 4 < record->record_body_len;
}	

static inline char *AosSslRecord_getRecord(struct AosSslRecord *record)
{
	if (record->record_start + 4 >= record->record_data_len) return 0;
	return &record->record_body[record->record_start];
}


static inline u16 AosSslRecord_getRemainBufferLen(struct AosSslRecord *record)
{
	return record->buffer_len - record->msg_start;
}


static inline int AosSslRecord_init( struct AosSslRecord *record, u16 size)
{
	aos_assert1(record);

	memset(record, 0, sizeof(struct AosSslRecord));

	record->record_body = (char *)aos_malloc(size);
	aos_assert1(record->record_body);
	record->buffer_len = size;
	record->msg_start = 5;

	return 0;
}


static inline int AosSslRecord_release(struct AosSslRecord *record)
{

	if (record->buffer_len != 0 && record->record_body)
	{
		aos_free(record->record_body);
		record->buffer_len = 0;
	}
	record->record_body = NULL;

	return 0;
}

static inline int AosSslRecord_send(AosSslRecord_t *record, int sock)
{
	// 
	// There shall be one and only one record. 
	//
	//aos_eng_log(eAosMD_SSL, "Data to send %d, pid %d, usec %u", 
	//		record->record_data_len, current->pid, AosGetUsec());
	return AosTcp_write(sock, record->record_body, record->record_data_len);
}


static inline u16 AosSslRecord_getMsgLen(AosSslRecord_t *record)
{
	return record->msg_body_len;
}


static inline u8 AosSslRecord_getMsgType(AosSslRecord_t *record)
{
	return record->msg_type;
}


static inline u16 AosSslRecord_getMsgStart(AosSslRecord_t *record)
{
	return record->msg_start;
}


static inline u16 AosSslRecord_getRemainLen(AosSslRecord_t *record)
{
	return record->buffer_len - record->msg_start;
}


static inline u16 AosSslRecord_getBuffLen(AosSslRecord_t *record)
{
	return record->buffer_len;
}

#endif

