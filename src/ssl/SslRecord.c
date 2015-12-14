////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslRecord.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#define AOS_DEBUG
#include "ssl/SslRecord.h"

#include "linux/net.h" // liqin

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Slab.h"
#include "aosUtil/List.h"
#include "KernelSimu/string.h"
#include "KernelSimu/skbuff.h"
#include "ssl/SslCommon.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslMisc.h"
#include "ssl/Ssl.h"
#include "ssl/ssl_skbuff.h"

extern struct AosSlab *gAosSlabRecord;

AosSslRStateEntry_t ssl_record_state[] = {
	{ eAosSslRState_start, 		AosSslRecord_start},
	{ eAosSslRState_waitBody, 	AosSslRecord_waitbody},
	{ eAosSslRState_ready, 		AosSslRecord_ready},
//	{ eAosSslRState_encrypt, 	AosSslRecord_encrypt},
	{ eAosSslRState_decrypt, 	NULL},
};

struct AosSslRecord* AosSslRecord_create(struct AosSslContext *context, u16 record_size, u8 direct_type)
{
	struct AosSslRecord *record = NULL;
	struct sk_buff *skb;

	record = (struct AosSslRecord*)AosSlab_get( gAosSlabRecord );
	if (!record) return NULL;
//aos_trace("liqin record %x created", record);
	memset(record, 0, sizeof(struct AosSslRecord));

	/*record->buffer = (char *)aos_malloc_atomic(record_size);
	if ( !record->buffer )
	{
		AosSlab_release( gAosSlabRecord, (void*)record );
		return NULL;
	}
	record->buffer_len = record_size;*/

	if ( context )
	{
		record->context = context;
		//AosSslContext_hold ( record->context );
	}
	AOS_INIT_LIST_HEAD(&record->list);

	// init record skb queue
	skb_queue_head_init(&record->rskb_queue);
	if (record_size)
	{
		skb = alloc_skb(record_size, GFP_ATOMIC);
		if (!skb) goto error;	
		skb_queue_tail(&record->rskb_queue, skb);
		record->buffer_len = record_size;
		record->rbuf = (char*)skb->data;
	}
	
	record->msg_cur_start = SSL_RECORD_HEADER_SIZE;
	record->direct_type = direct_type;
	return record;
	
error:
	AosSlab_release(gAosSlabRecord, (void*) record);
	return NULL;
}

struct AosSslRecord* AosSslRecord_skbCreate(
	struct AosSslContext *context, 
	struct sk_buff *skb, 
	int extend_head,
	int extend_tail, 
	u8 direct_type)
{
	struct AosSslRecord *record = NULL;
	struct sk_buff *newskb;

	record = (struct AosSslRecord*)AosSlab_get( gAosSlabRecord );
	if (!record) {
		kfree_skb(skb);
		return NULL;
	}
	memset(record, 0, sizeof(struct AosSslRecord));

	if ( context )
	{
		record->context = context;
	}
	AOS_INIT_LIST_HEAD(&record->list);

	// init record skb queue
	skb_queue_head_init(&record->rskb_queue);

	newskb = __aos_skb_expand(skb, extend_head, extend_tail, GFP_ATOMIC);
	if (!newskb) goto error;	

	skb_queue_tail(&record->rskb_queue, newskb);
	record->buffer_len = newskb->len;
	record->rbuf = (char*)newskb->data;
	
	record->msg_cur_start = SSL_RECORD_HEADER_SIZE;
	record->direct_type = direct_type;
	return record;
	
error:
	AosSlab_release(gAosSlabRecord, (void*) record);
	return NULL;
}


int AosSslRecord_release(struct AosSslRecord *record)
{
//aos_trace("liqin record %x release", record);
	/*if (record->buffer)
	{
		aos_free(record->buffer);
	}*/

	if ( record->context )
	{
		//AosSslContext_put( record->context );
	}

	skb_queue_purge(&record->rskb_queue);
	
	AosSlab_release( gAosSlabRecord, (void*)record );

	return 0;
}

int AosSslRecord_skbSetLen(struct AosSslRecord *record)
{
	struct sk_buff *skb;

	skb = skb_peek(&record->rskb_queue);
	aos_assert1(skb);

	if (record->record_total_len > (int)(skb->end - skb->data))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "no more room for skb");
		return -1;
	}
	skb->tail = skb->data + record->record_total_len;
	skb->len  = record->record_total_len;

	return 0;
}

// we only stored one received ssl record in AosSslRecord
// while if we create AosSslRecord to response our ssl handshake, 
// we can stored multiple output ssl record in AosSslRecord
int AosSslRecord_setMsg(AosSslRecord_t *record, char expectFinished)
{
	u16 msg_start = record->msg_cur_start;
	int ret = 0;
	
	if (!record->rbuf);
	{
		record->rbuf = AosSslRecord_getRecordBuf(record, 0);
		aos_assert1(record->rbuf);
	}

	if ( record->ssl_version == VER2_0 )
	{
		// sslv2 client hello
		if ( record->record_cur_type != 1 )
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"Handshake type %d is not clienthello in SSLv2 record", record->record_cur_type );
		}
		record->msg_cur_len = record->record_cur_body_len;
		record->msg_cur_type = record->record_cur_type;
		return ret;
	}

	switch ( record->record_cur_type )
	{
	case eAosContentType_PlainData:
		record->msg_cur_type = eAosSSLMsg_Unknown;
	 	// the whole record contain only one msg, app record has no message header
		record->msg_cur_len = record->msg_cur_body_len = record->record_cur_body_len;
		break;
	
	case eAosContentType_Handshake:
		 if ( expectFinished )
		 {
			// 
			// Finished message must be processed differently since we cannot see 
		 	// the message header. 
			//
			record->msg_cur_type = eAosSSLMsg_Unknown;
			record->msg_cur_len = record->msg_cur_body_len = record->record_cur_body_len; 
			break;
		 }
			
		 if ( record->record_cur_start + record->record_cur_len - msg_start < SSLV3_MESSAGE_HEADER_SIZE ) 
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"handshake message too short: record length %d, record body length%d", 
				record->record_cur_len, record->record_cur_body_len);
			break;
		 }

		 record->msg_cur_type = (unsigned char)record->rbuf[msg_start];
		 record->msg_cur_body_len = ((unsigned char)record->rbuf[msg_start+1] << 16) + 
						((unsigned char)record->rbuf[msg_start+2] << 8) + 
						(unsigned char)record->rbuf[msg_start+3];
		 record->msg_cur_len = record->msg_cur_body_len + SSLV3_MESSAGE_HEADER_SIZE;
		 if ( (msg_start + record->msg_cur_len) > record->record_cur_start + record->record_cur_len )
		 {
			aos_trace_hex( "record body", record->rbuf, record->record_total_len );
			return aos_alarm( eAosMD_SSL, eAosAlarm_SSLSynErr,
				"handshake message out of bound: msg_cur_start %d, msg_cur_len %d, record_cur_len %d", 
				record->msg_cur_start, record->msg_cur_len, record->record_cur_len );
		 }

		 break;

	case eAosContentType_ChangeCipherSpec:
		// this message lonely in one record
		 if ( record->record_cur_start + record->record_cur_len - msg_start < 1 )
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"ChangeCipherSpec too short: record_cur_len %d, msg_start %d", record->record_cur_len, msg_start );
			break;
		 }

		 record->msg_cur_type = eAosContentType_ChangeCipherSpec;
		 record->msg_cur_len = record->msg_cur_body_len = 1; // no msg header
		 break;

	case eAosContentType_AppData:
	  	 record->msg_cur_type = eAosContentType_AppData;
		 record->msg_cur_len = record->msg_cur_body_len = record->record_cur_body_len; // no msg header
		 break;

	case eAosContentType_Alert:
		 record->msg_cur_type = eAosContentType_Alert;
		 if ( record->record_cur_start + record->record_cur_len - msg_start < 2 )
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Alert message too short: record_cur_len %d, msg_start %d", record->record_cur_len, msg_start);
			break;
		 }

		 record->msg_cur_len = record->msg_cur_body_len = 2; // no msg header
		 break;

	default:
		 ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"Unrecognized Content Type: %d", record->record_cur_type);
	}

	return ret;
}

// 
// Description:
//	One record may contain multiple messages. This function advances to the
//  next message, if any. 
//
int AosSslRecord_nextMsg(AosSslRecord_t *record, char expectFinished)
{
	if (record->record_cur_type == eAosContentType_PlainData ||
	    record->msg_cur_type == eAosSSLMsg_Finished) 
	{
		return eAosRc_NoMsg;
	}

	record->msg_cur_start += record->msg_cur_len;
	if ( record->msg_cur_start > record->buffer_len ||
	      record->msg_cur_start >= record->record_cur_start + record->record_cur_len)
	{
		// 
		// No more messages
		//
		return eAosRc_NoMsg;
	}

	return AosSslRecord_setMsg(record, expectFinished);
}

int AosSslRecord_setMsgLen(struct AosSslRecord *record, int msg_len) 
{ 
	(record)->msg_cur_len = msg_len;
	// TBD liqin
	//skbq_cut((record)->mskb_queue, len);
	return msg_len;
}

// 
// Description:
//	It parses a received record data. If 'skb_list' does not contain
//  a complete record, the function returns eAosRc_NoRecord.
// 
// Params:
//   'record': OUT
//		return parsed record
//
static int AosSslRecord_skbParseRecordHeader(struct AosSslRecord *record, uint8 *rhead_buf)
{
	uint16 record_body_len;

	// 
	// Retrieve the message length
	//
	if ( (uint8)(rhead_buf[0]) == 0x80 )
	{
		// SSLv2 record
		// [0][1] length
		// [2] record type
		// [3][4] version
		record_body_len = rhead_buf[1];
		record->ssl_version = VER2_0;
		record->record_cur_type = rhead_buf[2]; // 1 stands for client hello
		aos_trace( "SSLv2 received, length %d, record type %d, major.minor version %d.%d", 
				rhead_buf[1], rhead_buf[2], rhead_buf[3], rhead_buf[4] );
		
		record->record_cur_start = 0;
		record->record_cur_len = 0;
		record->record_cur_body_len = record_body_len;
		record->msg_cur_start = 2;
	}
	else
	{
		// SSLv3 TLS record
		// [0] record type
		// [1][2] version
		// [3][4] length
		if ( rhead_buf[1] == 3 && rhead_buf[2] == 0 )
		{
			record->ssl_version = VER3_0;
		}
		else if ( rhead_buf[1] == 3 && rhead_buf[2] == 1 )
		{
			record->ssl_version = VER_TLS;
		}
		else
		{
			aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "wrong record version %d.%d", 
					rhead_buf[1], rhead_buf[2] );
			return -1;
		}
		
		record_body_len = (((unsigned char)rhead_buf[3]) << 8) + (unsigned char)rhead_buf[4];
		if ( record_body_len >= 16500 )
		{
			aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "wrong record length %x %x", 
					rhead_buf[3], rhead_buf[4] );
			aos_trace_hex("record", (char*)rhead_buf, SSL_RECORD_HEADER_SIZE );
			return -1;
		}
			
		record->record_cur_type = rhead_buf[0];
		
		record->record_cur_start = 0;
		record->record_cur_len = 0;
		record->record_cur_body_len = record_body_len;
		record->msg_cur_start = SSL_RECORD_HEADER_SIZE;
		
	}
	
	return eAosRc_Success;
}

// 
// It constructs a new record (containing one handshake message) in 'record' 
// based on the parameters passed in.
// If the record is constructed successfully, it moves 'msg_start' to the next
// message start.
//
int AosSslRecord_createHandshake(
	AosSslRecord_t *record,
	u8 msg_type,
	u16 msg_body_len)
{
	u32 msg_start = record->msg_cur_start;
	u16 index = record->record_cur_start;

	aos_assert1( msg_start + msg_body_len + SSLV3_MESSAGE_HEADER_SIZE < record->buffer_len );

	record->msg_cur_type = msg_type;
	record->msg_cur_body_len = msg_body_len;
	record->msg_cur_len = record->msg_cur_body_len + SSLV3_MESSAGE_HEADER_SIZE;


	if ( record->record_cur_type == eAosContentType_Handshake )
	{
		// multiple msg in one ssl record
		record->record_cur_body_len += record->msg_cur_len;
		record->record_cur_len += record->msg_cur_len;
		record->record_total_len += record->msg_cur_len;
	}
	else
	{
		// first msg in ssl record
		record->record_cur_type = eAosContentType_Handshake;
		record->record_cur_body_len = record->msg_cur_len;
		record->record_cur_len = record->record_cur_body_len + SSL_RECORD_HEADER_SIZE;
		record->record_total_len += record->record_cur_len;

		record->rbuf[index] = eAosContentType_Handshake;
		record->rbuf[index+1] = eAosSSL_VersionMajor;
		record->rbuf[index+2] = eAosSSL_VersionMinor;
	}



	record->rbuf[index+3] = (((unsigned int)record->record_cur_body_len) >> 8);
	record->rbuf[index+4] = (((unsigned int)record->record_cur_body_len) & 0xff);
	record->rbuf[msg_start++] = msg_type;
	record->rbuf[msg_start++] = 0;
	record->rbuf[msg_start++] = (msg_body_len >> 8);
	record->rbuf[msg_start++] = (msg_body_len & 0xff);

	AosSslRecord_skbSetLen(record);

	return 0;
}


int AosSslRecord_stateCsp1Established(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	aos_trace("Csp1: SSL %s Established entry", context->is_client?"client":"server");
	aos_assert1(context);
	errcode = 0;

	// if record type is eAosContentType_AppData, then do decryption
	// if record type is eAosContentType_PlainData, then do encryption
	// if record type is eAosContextType_ChangeCipherSpec
	context->callback = callback;

//	record->seq = context->server_num++;
//aos_debug( "record %p, enc seq num %d, record->buffer %p\n", record, (int)(record->seq), record->buffer );
	if (record->record_cur_type == eAosContentType_AppData)
	{
		aos_trace("Csp1: Established received application record\n");
		record->state_func = AosSslRecord_decFinished;
		if ( eAosRc_Success != AosSsl_csp1RecordDec( context, record ) )
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
		}
		return eAosRc_Pending;
	}
	else if (record->record_cur_type == eAosContentType_PlainData)
	{
		aos_trace("Csp1: Established send plain application record\n");
		record->state_func = AosSslRecord_encFinished;
		if ( eAosRc_Success != AosSsl_csp1RecordEnc( context, record, eAosContentType_AppData ) )
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
		}

		return eAosRc_Pending;
	}
	else if (record->record_cur_type == eAosContentType_ChangeCipherSpec)
	{
		aos_trace("Established received ChangeCipherSpec record\n");
	}
	else
		aos_trace("Established received application record\n");

	return eAosRc_Success;
}

int AosSslRecord_stateEstablished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	//AosSslRecord_t record_out;
	char *msg;//, *msg_created;
	int msg_len;//, msg_created_len;
	int ret;
		
	aos_trace("SSL %s Established entry\n", context->is_client?"client":"server");
	aos_assert1(context);
	msg = AosSslRecord_getMsg(record);
	msg_len = AosSslRecord_getMsgLen( record ); 

	// if record type is eAosContentType_AppData, then do decryption
	// if record type is eAosContentType_PlainData, then do encryption
	// if record type is eAosContextType_ChangeCipherSpec
	
	if (record->record_cur_type == eAosContentType_AppData)
	{
		//aos_trace("Established received application record\n");
		// do decryption
		if ( eAosRc_Success != AosSsl_DecAppData(context, (uint8*)msg, &msg_len))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
		}
		AosSslRecord_skbPull(record);
		if (callback)
			callback(eAosContentType_AppData, context->wo, (char*)record, 0);
		return eAos_Success;
	}
	else if (record->record_cur_type == eAosContentType_PlainData)
	{
		//aos_eng_log(eAosMD_SSL, "Established send plain application record\n");
		
		//aos_assert1(msg_len+100 <= record->buffer_len);	

		if ( eAosRc_Success != AosSsl_EncAppData(
				context, 
				(uint8*)msg,
				&msg_len, 
				eAosContentType_AppData))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
		}
		//aos_trace("msg_created_len %d", msg_created_len);
		
		AosSslRecord_createAppDataRecord(record, msg_len);
		// 
		// Send the message
		//
		if ((ret = AosSslRecord_send(record, context)) < 0 )
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data send failed, %d", ret);
		}

	}
	else if (record->record_cur_type == eAosContentType_ChangeCipherSpec)
	{
		aos_trace("Established received ChangeCipherSpec record\n");
	}
	else
		aos_trace("Established received application record\n");

	errcode = 0;
	return eAosRc_Success;
}

int AosSslRecord_encFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	uint16 *errcode)	
{
	uint32 msg_len;
	int ret;
	aos_trace("Csp1 Record Encryption Finished");
	msg_len = AosSslRecord_getMsgLen( record );
	AosSslRecord_createAppDataRecord( record, msg_len + context->hash_len );
	if ((ret = AosSslRecord_send( record, context)) < 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data send failed, %d", ret);
	}
	return eAosRc_Success;
}

int AosSslRecord_decFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	int plain_len = 0;
	
	aos_trace("SSL Established: Csp1 Record Decryption Finished");
	switch( context->encrypt_type )
	{
	case eAosEncryptType_RC4_128:
	case eAosEncryptType_Null:
		plain_len = AosSslRecord_getMsgLen( record ) - context->hash_len;
		break;
	default:
		aos_trace("unsupport cipher %d", context->encrypt_type);
		break;
	}
	if (callback)
	{
		callback(eAosContentType_AppData, 
				context->wo, 
				AosSslRecord_getMsg( record ), 
				plain_len );
	}
	return eAosRc_Success;
}

int AosSslRecord_ready(struct AosSslRecord *record, SSL_PROC_CALLBACK callback)
{
	aos_trace("%s entry", __FUNCTION__);
	
	if (!record->cipher) return eAosRc_NotifyProtocol;

	switch (gAosSslAcceleratorType)
	{
	case eAosSslAcceleratorType_CaviumPro:
		if (eAosRc_Success != AosSsl_csp1RecordDec(record->context, record))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
		}

		SSL_RSTATE_NEXT(record, eAosSslRState_decrypt);

		return eAosRc_Pending;

	case eAosSslAcceleratorType_Soft:
		{
			int msg_len;
			uint8 *msg;
			msg = (uint8*)AosSslRecord_getRecord(record);
			if ( eAosRc_Success != AosSsl_DecAppData(record->context, (uint8*)msg, &msg_len))
			{
				return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
			}
			// SSL_RECORD_SET_MSG_LEN(record, msg_len);
			AosSslRecord_setMsgLen(record, msg_len);
			return eAos_Success;
		}		
	default:
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "app data decryption failed");
	}

	return eAosRc_Success;
}

//
// wait for whole record body
//
int AosSslRecord_waitbody(struct AosSslRecord *record, SSL_PROC_CALLBACK callback)
{
	struct AosSslContext *context;
	struct sk_buff_head *rskb_queue;
	int qdata_len = 0;

	context = record->context;
	aos_assert1(context);

	// calculate the receiving data length
	qdata_len = skbq_len(&context->rskb_in_queue);

	// check whether we get full record body
	if ((record->record_cur_body_len + record->msg_cur_start) > 
	     (record->record_cur_len + qdata_len))
	{
		skbq_append(&record->rskb_queue, NULL, &context->rskb_in_queue);
		record->record_cur_len += qdata_len;
		return eAosRc_Success;
	}

	// now we get a full record
	rskb_queue = skbq_data2(&context->rskb_in_queue, 
		(record->record_cur_body_len+record->msg_cur_start) - record->record_cur_len);
	aos_assert1(rskb_queue);
	skbq_append(&record->rskb_queue, NULL, rskb_queue);
	if (rskb_queue != &context->rskb_in_queue) free(rskb_queue);
	

	record->record_cur_len = record->record_cur_body_len + record->msg_cur_start;

	SSL_RSTATE_NEXT(record, eAosSslRState_ready);
	return eAosRc_FullRecord;
}

int AosSslRecord_start(struct AosSslRecord *record, SSL_PROC_CALLBACK callback)
{
	struct AosSslContext *context;
	// struct sk_buff_head  *skb_queue;
	uint8 *rhead_buf;

	// TBD
	if (record->direct_type == eAosSslRecord_Output) {
		if (record->record_cur_type == eAosContentType_PlainData)
		{
			//SSL_RSTATE_NEXT(record, eAosSslRState_encrypt);
			return eAosRc_NotifyProtocol;
		}
		aos_trace("%s send out record", __FUNCTION__);
		return eAosRc_SendOut;
	}

	context = record->context;
	aos_assert1(context);

	// if the size of first skb in the queue is not large than input len, merge the following skb with 
	// the first one, return the size of merged skb.
	if (__aos_skbq_len_merge(&context->rskb_in_queue, SSL_RECORD_HEADER_SIZE) 
		< SSL_RECORD_HEADER_SIZE)
		return eAosRc_Success;

	// retrieve first skb buffer which buffer len is large than SSL_RECORD_HEADER_SIZE
	rhead_buf = __aos_skbq_data(&context->rskb_in_queue, SSL_RECORD_HEADER_SIZE);
	aos_assert1(rhead_buf);

	AosSslRecord_skbParseRecordHeader(record, rhead_buf);
	
	SSL_RSTATE_NEXT(record, eAosSslRState_waitBody);

	return eAosRc_Success;
}

int AosSslRecord_stm(struct AosSslRecord *record, SSL_PROC_CALLBACK callback)
{
	int safe_guard = 0;
	uint16 err_code;
	int ret = eAosRc_Success;
	
	aos_assert1(record);

	while(safe_guard ++ < 10 )
	{
		ret = ssl_record_state[record->rstate].rsfunc(record, callback);	
		switch(ret)
		{
		case eAosRc_Pending:
			 return eAosRc_Pending;	// means state interrupting

		case eAosRc_Success:			// means state continue
			 break;
		case eAosRc_FullRecord:			// received a full record return 
			 ret = eAosRc_Success;
			 goto Out;
		case eAosRc_SendOut:			// this record can be sent out 
			 ret = eAosRc_Success;
			 goto Out;
		case eAosRc_NotifyProtocol:
			ret = AosSslStm_protocol(record->context, record, callback, &err_code);
			goto Out;
		default:						// error found
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "SSL record layer process failed: %d", ret);
			return -eAosAlarm_SSLProcErr; 
		}
	}
Out:
	return ret;
}

// 
// Description:
//	It collect the record from receiving skb 
//
// Parameters:
//	'context': IN
//		SSL context. 
//
//	'skb': IN
//		Receiving skb.
//	
int AosSslRecord_skbCollectRecord(struct AosSslContext *context, struct sk_buff *skb)
{
	int ret;

	if (skb) skb_queue_tail(&context->rskb_in_queue,  skb);

	while(!skb_queue_empty(&context->rskb_in_queue))
	{
		if (!context->record_in)
		{
			context->record_in = AosSslRecord_create(context, 0, eAosSslRecord_Input);
         		if ( !context->record_in )
			{
				aos_alarm(eAosMD_SSL, eAosRc_recordCreateErr, "receiving record creation failed");
				return eAosRc_recordCreateErr;
			}
		}
		ret = AosSslRecord_stm(context->record_in, NULL); 
		if ( ret == eAosRc_SslNoRecord )
		{
			return eAosRc_Success;
		}
		else if ( ret != eAosRc_Success )
		{
			aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "record parsing failed" );
			return eAosRc_Error;
		}

		aos_list_add_tail( &context->record_in->list, &context->record_in_list );

		context->record_in = NULL;
	}

	return eAosRc_Success;
}

