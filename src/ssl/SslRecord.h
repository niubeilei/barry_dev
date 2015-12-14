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
#include "ssl/SslCommon.h"
#include "ssl/ReturnCode.h"
#include "ssl/ssl_skbuff.h"

#define SSL_RECORD_MAX_COUNTS 4000
#define SSL_RECORD_EXTEND_BYTES 64
#define SSL_RECORD_HEADER_SIZE 5
#define SSLV3_MESSAGE_HEADER_SIZE 4

#define RECORD_SKB_QUEUE(record) (record)->rskb_queue
#define SSL_RSTATE_NEXT(record, new_state) (record)->rstate = (new_state);

typedef enum
{
	eAosContentType_NotSet 			= 0,
	eAosContentType_PlainData 			= 1,

	eAosContentType_ChangeCipherSpec 	= 20,
	eAosContentType_Alert				= 21,
	eAosContentType_Handshake			= 22,
	eAosContentType_AppData			= 23
} AosContentType;

typedef enum
{
	eAosSslRecord_Input = 0,
	eAosSslRecord_Output =1,
} AosSslRecordDirectType;

struct AosSslContext;

//
// AosSslRecord record struct can contain multiple records,
// and each record can contain multiple messages
//
typedef struct AosSslRecord
{
	struct aos_list_head	list;
	struct AosSslContext	*context;
	AosSSLStateFunc 		state_func;	

	int cipher;
	u8  rstate;
	// char *	buffer;
	u16		buffer_len;

	char  		*rbuf;		// record buffer point to the first skb buffer in rskb_queue
	struct sk_buff_head rskb_queue;

	u16 	record_total_len;

	u16		record_cur_start;
	u16		record_cur_len;
	u16		record_cur_body_len;
	u16		record_cur_type;

	u16 		msg_cur_start;
	u16		msg_cur_len;
	u16		msg_cur_body_len;
	u8		msg_cur_type;

	u8		direct_type;
	SslVersion	ssl_version;

//	u64 	seq;
} AosSslRecord_t;

typedef enum
{
	eAosSslRState_start = 0,
	eAosSslRState_waitBody,
	eAosSslRState_ready,
	eAosSslRState_decrypt,
	eAosSslRState_encrypt,

	eAosSslRState_max
} AosSslRState_t;

//
// state function typedef for record layer
//
typedef int (*rstate_fun_t)(struct AosSslRecord *, SSL_PROC_CALLBACK callback);

typedef struct AosSslRStateEntry {
	AosSslRState_t   rstate;         // SSL record state
	rstate_fun_t     rsfunc;       // record state handler function
} AosSslRStateEntry_t;

extern struct AosSslRecord* AosSslRecord_create(
	struct AosSslContext *context, 
	u16 record_size, 
	u8 direct_type);

extern int AosSslRecord_release(struct AosSslRecord *record);

extern int AosSslRecord_nextMsg(AosSslRecord_t *record, char expectFinished);

extern int AosSslRecord_setMsg(AosSslRecord_t *record, char expectFinished);

extern int AosSslRecord_collectRecord(
	struct AosSslContext *context,
	char *data, 
	unsigned int data_len);

extern int AosSslRecord_skbSetLen(struct AosSslRecord *record);

static inline char* AosSslRecord_getRecordBuf(struct AosSslRecord *record, int size)
{
	return (char*)__aos_skbq_getbuf(&record->rskb_queue, size);
}

static inline char *AosSslRecord_getMsgBody(struct AosSslRecord *record)
{
	if ( !record->rbuf )
	{
		record->rbuf = AosSslRecord_getRecordBuf(record, 0);
		if (!record->rbuf)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Record body is null");
			return 0;
		}
	}

	if ( record->record_cur_type == eAosContentType_PlainData )
	{
		return &record->rbuf[record->msg_cur_start];
	}
	else
	{
		return &record->rbuf[record->msg_cur_start + SSLV3_MESSAGE_HEADER_SIZE];
	}
}

static inline u16 AosSslRecord_getMsgBodyLen(AosSslRecord_t *record)
{
	return record->msg_cur_body_len;
}

static inline char *AosSslRecord_getMsg(struct AosSslRecord *record)
{
	if (!record->rbuf)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Record body is null");
		return 0;
	}

	return &record->rbuf[record->msg_cur_start];
}

static inline int AosSslRecord_hasMoreMsg(struct AosSslRecord *record)
{
	if ( record->record_cur_type == eAosContentType_PlainData ) return 0;
	if ( record->msg_cur_type == eAosSSLMsg_Finished ) return 0;

	return record->msg_cur_start + SSLV3_MESSAGE_HEADER_SIZE < record->record_cur_len;
}	

static inline char *AosSslRecord_getRecord(struct AosSslRecord *record)
{
	return &record->rbuf[record->record_cur_start];
}

static inline u16 AosSslRecord_getRemainBufferLen(struct AosSslRecord *record)
{
	return record->buffer_len - record->msg_cur_start;
}

static inline int AosSslRecord_send(AosSslRecord_t *record, struct AosSslContext *context)
{
	// 
	// There shall be one and only one record. 
	//
	//aos_eng_log(eAosMD_SSL, "Data to send %d, pid %d, usec %u", 
	//		record->record_data_len, current->pid, AosGetUsec());
	//		
/*#ifdef __KERNEL__
	return AosSslWrapper_sendRecordData( 
			(AosSockWrapperObject_t*)context->wo, 
			record->buffer, 
			record->record_total_len);
#else
	return AosTcp_write(context->sock, record->buffer, record->record_total_len);
#endif
*/
	aos_trace("record len %d", record->record_total_len);
	aos_list_add_tail(&record->list, &context->record_out_list);

	return eAosRc_Pending;
}


static inline u16 AosSslRecord_getMsgLen(AosSslRecord_t *record)
{
	return record->msg_cur_len;
}

static inline u8 AosSslRecord_getMsgType(AosSslRecord_t *record)
{
	return record->msg_cur_type;
}


static inline u16 AosSslRecord_getMsgStart(AosSslRecord_t *record)
{
	return record->msg_cur_start;
}


static inline u16 AosSslRecord_getBuffLen(AosSslRecord_t *record)
{
	return record->buffer_len;
}


static inline int AosSslRecord_moveToNextMsg(AosSslRecord_t *record)
{
	if ( record->msg_cur_start + record->msg_cur_len >= record->buffer_len )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Out of bound: %d, %d, %d", record->msg_cur_start, 
			record->msg_cur_len, record->buffer_len);
	}

	record->msg_cur_start += record->msg_cur_len;
	record->msg_cur_type = 0;
	record->msg_cur_len = 0;
	record->msg_cur_body_len = 0;
	
	return 0;
}

static inline int AosSslRecord_moveToNextRecord(AosSslRecord_t *record)
{
	if ( record->record_cur_start + record->record_cur_len >= record->buffer_len )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Out of bound: %d, %d, %d", record->record_cur_start, 
			record->record_cur_len, record->buffer_len);
	}

	record->record_cur_start += record->record_cur_len;
	record->record_cur_len = 0;
	record->record_cur_body_len = 0;
	record->record_cur_type = 0;
	
	record->msg_cur_start = record->record_cur_start + SSL_RECORD_HEADER_SIZE;
	record->msg_cur_len = 0;
	record->msg_cur_body_len = 0;
	record->msg_cur_type = 0;
	
	return 0;
}

// 
// ChangeCipher message can only stored in one ssl record, 
// and this record can not contain any other message
//
static inline int AosSslRecord_createChangeCipherSpecRecord(AosSslRecord_t *record)
{
	uint16 index = record->record_cur_start;
	aos_assert1( record->buffer_len - index >= SSL_RECORD_HEADER_SIZE + 1 );

	record->record_cur_len = SSL_RECORD_HEADER_SIZE + 1;
	record->rbuf[index] = eAosContentType_ChangeCipherSpec;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = 0;
	record->rbuf[index+4] = 1;
	record->rbuf[index+5] = 1;

	record->record_total_len += record->record_cur_len;
	AosSslRecord_skbSetLen(record);
	return 0;
}

// One ssl record can only store one Finished Message
// and this record can not contain any other message
static inline int AosSslRecord_createFinishedRecord( AosSslRecord_t *record, u32 msg_len )
{
	uint16 index = record->record_cur_start;
	aos_assert1( record->buffer_len - index >= (int)(msg_len + SSL_RECORD_HEADER_SIZE) );

	record->record_cur_body_len = msg_len;
	record->record_cur_len = msg_len + SSL_RECORD_HEADER_SIZE;
	record->rbuf[index] = eAosContentType_Handshake;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = ((record->record_cur_body_len) >> 8);
	record->rbuf[index+4] = ((record->record_cur_body_len) & 0xff);

	record->record_total_len += record->record_cur_len;
	
	record->msg_cur_type = eAosSSLMsg_Finished;
	record->msg_cur_len = msg_len;
	record->msg_cur_body_len = msg_len;

	AosSslRecord_skbSetLen(record);
	return 0;
}

// One ssl record can only store one Finished Message
// and this record can not contain any other message
static inline int AosSslRecord_createAppDataRecord( AosSslRecord_t *record, u32 msg_len )
{
	uint16 index = record->record_cur_start;
	aos_assert1( record->buffer_len - index >= (int)(msg_len + SSL_RECORD_HEADER_SIZE) );

	record->record_cur_len = msg_len + SSL_RECORD_HEADER_SIZE;
	record->record_cur_body_len = msg_len;
	record->record_cur_type = eAosContentType_AppData;
	record->rbuf[index] = eAosContentType_AppData;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = ((record->record_cur_body_len) >> 8);
	record->rbuf[index+4] = ((record->record_cur_body_len) & 0xff);

	record->record_total_len += record->record_cur_len;
	
	record->msg_cur_type = eAosContentType_AppData;
	record->msg_cur_len = msg_len;
	record->msg_cur_body_len = msg_len;

	return 0;
}

static inline int AosSslRecord_createPlainAppRecord( AosSslRecord_t *record, char* msg, u32 msg_len)
{
	uint16 index = record->record_cur_start;
	aos_assert1( record->buffer_len - index >= (int)(msg_len + SSL_RECORD_HEADER_SIZE) );

	record->record_cur_len = msg_len + SSL_RECORD_HEADER_SIZE;
	record->record_cur_body_len = msg_len;
	record->record_cur_type = eAosContentType_PlainData;
	record->rbuf[index] = eAosContentType_AppData;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = (((unsigned int)record->record_cur_body_len) >> 8);
	record->rbuf[index+4] = (((unsigned int)record->record_cur_body_len) & 0xff);

	memcpy( record->rbuf + index + SSL_RECORD_HEADER_SIZE,  msg, msg_len);

	//record->record_total_len += record->record_cur_len;

	return 0;
}

static inline int AosSslRecord_skbCreatePlainAppRecord(AosSslRecord_t *record, int extend_head, int extend_tail)
{
	struct sk_buff *skb;
	int msg_len;
	int index = record->record_cur_start;

	skb = skb_peek(&record->rskb_queue);

	aos_assert1(skb_headroom(skb) >= extend_head);
	aos_assert1(skb_tailroom(skb) >= extend_tail);

	msg_len = skb->len;
	
	skb_push(skb, extend_head);
	skb_put(skb, extend_tail);
	
	record->rbuf = (char*)skb->data;
	record->buffer_len = skb->tail - skb->data;
	
	record->record_cur_len = skb->len;
	record->record_cur_body_len = msg_len;
	record->record_cur_type = eAosContentType_PlainData;
	record->rbuf[index] = eAosContentType_AppData;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = (((unsigned int)record->record_cur_body_len) >> 8);
	record->rbuf[index+4] = (((unsigned int)record->record_cur_body_len) & 0xff);


	return 0;
}

static inline int AosSslRecord_createAlertRecord(AosSslRecord_t *record, u32 msg_len)
{
	uint16 index = record->record_cur_start;
	aos_assert1( record->buffer_len - index >= (int)(msg_len + SSL_RECORD_HEADER_SIZE) );

	record->record_cur_len = msg_len + SSL_RECORD_HEADER_SIZE;

	record->rbuf[index] = eAosContentType_Alert;
	record->rbuf[index+1] = eAosSSL_VersionMajor;
	record->rbuf[index+2] = eAosSSL_VersionMinor;
	record->rbuf[index+3] = (msg_len >> 8);
	record->rbuf[index+4] = (msg_len & 0xff);

	record->record_cur_body_len = msg_len;
	record->record_total_len += record->record_cur_len;

	record->msg_cur_body_len = msg_len;
	record->msg_cur_len = msg_len;

	return 0;
}

static inline void AosSslRecord_skbPull(AosSslRecord_t *record)
{
	skbq_clear(&record->rskb_queue, record->record_cur_len - record->record_cur_body_len);
}

extern int AosSslRecord_createHandshake(
	AosSslRecord_t *record,
	u8 msg_type,
	u16 msg_body_len);

extern int AosSslRecord_stateCsp1Established(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);

extern int AosSslRecord_stateEstablished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);

extern int AosSslRecord_encFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	uint16 *errcode);

extern int AosSslRecord_decFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	uint16 *errcode);	

extern int AosSslRecord_start(struct AosSslRecord *record, SSL_PROC_CALLBACK callback);
extern int AosSslRecord_waitbody(struct AosSslRecord *record, SSL_PROC_CALLBACK callback);
extern int AosSslRecord_ready(struct AosSslRecord *record, SSL_PROC_CALLBACK callback);
extern int AosSslRecord_stm(struct AosSslRecord *record, SSL_PROC_CALLBACK callback);	
extern int AosSslRecord_skbCollectRecord(struct AosSslContext *context, struct sk_buff *skb);
extern struct AosSslRecord* AosSslRecord_skbCreate(
	struct AosSslContext *context,
	struct sk_buff *skb,
	int extend_head,
	int extend_tail,
	u8 direct_type);

#endif

