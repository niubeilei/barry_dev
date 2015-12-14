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

#include "ssl2/SslRecord.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/string.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/ReturnCode.h"
#include "ssl2/SslStatemachine.h"



static int AosSslRecord_setMsg(AosSslRecord_t *msg, char expectFinished)
{
	u16 msgStart = msg->msg_start;
	int ret = 0;
	switch (msg->record_type)
	{
	case eAosContentType_PlainData:
		 return 0;
	
	case eAosContentType_NotSet:
		 ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Invalid record type: NotSet");
		 break;

	case eAosContentType_Handshake:
		 if (expectFinished)
		 {
			// 
			// Finished message must be processed differently since we cannot see 
		 	// the message header. 
			//
			msg->msg_type = eAosSSLMsg_Unknown;
			msg->msg_body_len = 0;
			break;
		 }
			
		 if (msg->record_body_len - msgStart < 4) 
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Handshake message too short: %d, %d", 
				msg->record_body_len, msg->msg_body_len);
			break;
		 }

		 msg->msg_type = (unsigned char)msg->record_body[msgStart];
		 msg->msg_body_len = ((unsigned char)msg->record_body[msgStart+1] << 16) + 
						((unsigned char)msg->record_body[msgStart+2] << 8) + 
						(unsigned char)msg->record_body[msgStart+3];
		 if (msgStart + msg->msg_body_len + 4 > msg->record_body_len) 
		 {
			aos_trace_hex("record body", msg->record_body, msg->record_body_len+4);
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Handshake message out of bound: %d, %d, %d", 
				msg->msg_start, msg->msg_body_len, msg->record_body_len);
		 }

		 break;

	case eAosContentType_ChangeCipherSpec:
		 if (msg->record_body_len - msgStart < 1)
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"ChangeCipherSpec too short: %d, %d", msg->record_body_len, msgStart);
			break;
		 }

		 msg->msg_type = eAosContentType_ChangeCipherSpec;
		 msg->msg_body_len = 1;
		 break;

	case eAosContentType_AppData:
	  	 msg->msg_type = eAosContentType_AppData;
		 msg->msg_body_len = msg->record_body_len;
		 break;

	case eAosContentType_Alert:
		 msg->msg_type = eAosContentType_Alert;
		 if (msg->record_body_len - msgStart < 2)
		 {
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Alert message too short: %d, %d", msg->record_body_len, msgStart);
			break;
		 }

		 msg->msg_body_len = 2;
		 break;

	default:
		 ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"Unrecognized Content Type: %d", msg->record_type);
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
	if (record->record_type == eAosContentType_PlainData ||
	    record->msg_type == eAosSSLMsg_Finished) 
	{
		return eAosRc_NoMsg;
	}

	record->msg_start += record->msg_body_len + 4;
	if (record->msg_start + 4 > record->record_body_len)
	{
		// 
		// No more messages
		//
		return eAosRc_NoMsg;
	}

	return AosSslRecord_setMsg(record, expectFinished);
}

	
// 
// Description:
//	It retrieves a record from 'data'. If 'data' does not contain
//  a complete message, the function returns eAosRc_NoMsg. Otherwise, 
//  it retrieves one record from 'data' and allocates
//  memory for the record body and sets it to 'record'.
//
int AosSslRecord_getOneRecord(
				const char *data, 
				const unsigned int data_len, 
				u32 flags,
				struct AosSslRecord *record, 
				unsigned int *remain_len, 
				char expectFinished)
{
	unsigned int recordLen;
	*remain_len = data_len;

	if (flags & eAosSSLFlag_Plain)
	{
		// 
		// If it is plain data, all data in 'data' are considered
		// in the same record. 
		//
		if (data_len == 0 || !data)
		{	
			return eAosRc_NoMsg;
		}

		if (record->buffer_len < data_len)
		{
			// if (record->buffer_len != 0)	
			if (record->record_body)	
			{
				aos_assert1(record->buffer_len);
				aos_free(record->record_body);
			}

			record->record_body = (char *)aos_malloc(data_len);
			aos_assert1(record->record_body);
			record->buffer_len = data_len;
		}

		record->record_start = 0;
		record->record_data_len = 0;
		record->record_type = eAosContentType_PlainData;
		record->record_types[0] = eAosContentType_PlainData;
		record->num_records = 1;
		record->record_body_len = data_len;
		record->msg_start = 5;
		memcpy(record->record_body+5, data, data_len);
		*remain_len = record->buffer_len - data_len - 5;
		return 0;
	}
	
	// 
	// A record must be at least 5 bytes long
	//
	if (data_len <= 5)
	{
		return eAosRc_NoMsg;
	}

	// 
	// Retrieve the record length
	//
	recordLen = (((unsigned char)data[3]) << 8)	+ (unsigned char)data[4];
	if (recordLen + 5 > data_len)
	{
		// 
		// 'data' does not contain a complete message. 
		//
		return eAosRc_NoMsg;
	}

	if (record->buffer_len < recordLen)
	{
		// if (record->buffer_len != 0)
		if (record->record_body)
		{
			aos_assert1(record->buffer_len);
			aos_free(record->record_body);
		}

		record->record_body = (char *)aos_malloc(recordLen);
		aos_assert1(record->record_body);
		record->buffer_len = recordLen;
	}

	*remain_len = data_len - recordLen - 5;

	record->record_start = 0;
	record->record_data_len = recordLen + 5;
	record->record_type = data[0];
	record->record_types[0] = data[0];
	record->num_records = 1;
	record->record_body_len = recordLen;
	memcpy(record->record_body, &data[5], recordLen);
	record->record_ver_major = data[1];
	record->record_ver_minor = data[2];
	record->msg_start = 0;
	return AosSslRecord_setMsg(record, expectFinished);
}


// 
// Description:
//	It retrieves the next message from the buffered data. If 
//  the buffered data does not contain a complete message, 
//  no message is returned. 
//
// Parameters:
//	'data': IN
//		If it is not null, it is the newly received data. 
//
//	'data_len': IN
//		The length of 'data'. 0 means there is no received data.
//
//	'conn': IN
//		The connection from which data is received.
//
//	'msg': OUT
//		If there is a complete message, the message is returned 
//		through this parameter.
//	
int AosSslRecord_getNextRecord(
			 char *data, 
			 unsigned int data_len,
			 u32 flags,
			 struct aosSslContext *context)
{
	int ret;
	unsigned int remain_len;
	aos_assert1(context);
	if (context->state == eAosSSLState_Pending)
	{
		aos_debug_log(eAosMD_SSL, "ssl state pending, context %p", context);
		if (data)
		{
			if (context->bufferedData)
			{
				// 
				// 'data' is not null and context->bufferedData is not null. 
				// Need to append 'data' to context->bufferedData.
				//
				char *newdata = (char *)aos_malloc(data_len + context->bufferedDatalen);
				aos_assert1(newdata);

				memcpy(newdata, context->bufferedData, context->bufferedDatalen);
				memcpy(&newdata[context->bufferedDatalen], data, data_len);
				aos_free(context->bufferedData);
				context->bufferedData = newdata;
				context->bufferedDatalen = data_len + context->bufferedDatalen;
			}
			else
			{
				context->bufferedData = (char *)aos_malloc(data_len);
				aos_assert1(context->bufferedData);
				memcpy(context->bufferedData, data, data_len);
				context->bufferedDatalen = data_len;	
			}
		}
		return eAosRc_NoMsg;
	}
	if (data)
	{
		if (!context->bufferedData)	
		{
			ret = AosSslRecord_getOneRecord(data, data_len, flags, 
					&context->record, &remain_len,
					(context->flags & eAosSSLFlag_ExpectFinished)?1:0);

			if (ret == eAosRc_NoMsg)
			{
				// 
				// This means the data does not contain a complete 
				// message. Store the data into context and return.
				//
				context->bufferedData = (char *)aos_malloc(data_len);
				aos_assert1(context->bufferedData);

				memcpy(context->bufferedData, data, data_len);
				context->bufferedDatalen = data_len;
				return ret;
			}

			if (ret)
			{
				//
				// Errors occured. Return.
				//
				return ret;
			}

			// 
			// Retrieved a message. Check whether there is remaining
			// data. If yes, we need to store it into context.
			//
			if (remain_len)
			{
				// 
				// There are remaining data. 
				//
				context->bufferedData = (char *)aos_malloc(remain_len);
				aos_assert1(context->bufferedData);
				memcpy(context->bufferedData, 
					&data[data_len - remain_len], remain_len);
				context->bufferedDatalen = remain_len;
			}

			return 0;
		}
		else
		{
			// 
			// 'data' is not null and context->bufferedData is not null. 
			// Need to append 'data' to context->bufferedData.
			//
			char *newdata = (char *)aos_malloc(data_len + context->bufferedDatalen);
			aos_assert1(newdata);

			memcpy(newdata, context->bufferedData, context->bufferedDatalen);
			memcpy(&newdata[context->bufferedDatalen], data, data_len);
			aos_free(context->bufferedData);
			context->bufferedData = newdata;
			context->bufferedDatalen = data_len + context->bufferedDatalen;
		}
	}

	ret = AosSslRecord_getOneRecord(context->bufferedData, context->bufferedDatalen, 
			flags, &context->record, 
			&remain_len, 
			(context->flags & eAosSSLFlag_ExpectFinished)?1:0);

	if (ret)
	{
		//
		// Errors occured. Returned.
		//
		return ret;
	}

	// 
	// It successfully retrieved one record. Now, 'remain_len' must be
	// less than context->bufferedDataLen. 
	//
	if (remain_len && remain_len >= context->bufferedDatalen)
	{
		ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"Remain_len %d is not decreasing: %d", 
			remain_len, context->bufferedDatalen);
		return ret;
	}

	// 
	// Retrieved a message. Check whether there is remaining
	// data. If yes, we need to store it into context.
	//
	if (remain_len)
	{
		//
		// There are remaining data. 
		//
		char *newdata = (char *)aos_malloc(remain_len);
		aos_assert1(newdata);

		memcpy(newdata, &(context->
			bufferedData[context->bufferedDatalen - remain_len]), 
			remain_len);
		aos_free(context->bufferedData);
		context->bufferedData = newdata;
		context->bufferedDatalen = remain_len;
	}
	else
	{
		if (context->bufferedData)
		{
			aos_free(context->bufferedData);
			context->bufferedData = 0;
			context->bufferedDatalen = 0;
		}
	}

	return 0;
}


