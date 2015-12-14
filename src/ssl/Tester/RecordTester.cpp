////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RecordTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/Tester/RecordTester.h"

#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "ssl/SslCommon.h"
#include "ssl/SslRecord.h"
#include "ssl/ReturnCode.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"


// One record, contain ClientHello, length = 20
static char msg1[] = 
{
	0x16, 0x03, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x14, 0x35, 
	0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 
	0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35
}; 


bool AosSslRecordTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	oneRecordMultiMsg();
	return true;
}


bool AosSslRecordTester::basicTest()
{
	OmnBeginTest << "Test SSL Record";
	mTcNameRoot = "Test_Record";
	int ret;
/*
	struct tcp_vs_conn *conn = new tcp_vs_conn;
	memset(conn, 0, sizeof(tcp_vs_conn));
	conn->front_context = new (AosSslContext);

	// 
	// Test 'data' == 0
	//
	//ret = AosSslRecord_getNextRecord(0, 0, 0, conn->front_context);
	OmnTC(OmnExpected<int>(eAosRc_NoMsg), OmnActual<int>(ret)) << endtc;

	//
	// Test 'data' contains a complete record. Length = 15.
	//
	AosSslRecord_t rec;
	char *buff = (char *)aos_malloc(5000);
	memset(buff, 0, 5000);
	rec.record_start = 0;
	rec.record_body_len = 0;
	rec.record_body = buff;
	rec.buffer_len = 5000;
	rec.msg_start = 5;

	ret = AosSslRecord_createHandshake(&rec, eAosSSLMsg_ClientHello, 20);	
	char *b = AosSslRecord_getMsgBody(&rec);
	memset(b, '5', 20);

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(eAosContentType_Handshake), OmnActual<int>(buff[0])) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(buff[1])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(buff[2])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(buff[3])) << endtc;
	OmnTC(OmnExpected<int>(24), OmnActual<int>(buff[4])) << endtc;
	OmnTC(OmnExpected<int>(eAosSSLMsg_ClientHello), 
		OmnActual<int>(buff[5])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(buff[6])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(buff[7])) << endtc;
	OmnTC(OmnExpected<int>(20), OmnActual<int>(buff[8])) << endtc;

	OmnTC(OmnExpected<int>(1), OmnActual<int>(rec.num_records)) << endtc;
	OmnTC(OmnExpected<int>(eAosContentType_Handshake), 
		OmnActual<int>(rec.record_types[0])) << endtc;
	OmnTC(OmnExpected<int>(eAosContentType_Handshake), 
		OmnActual<int>(rec.record_type)) << endtc;
	OmnTC(OmnExpected<int>(24), OmnActual<int>(rec.record_body_len)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rec.record_body)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rec.record_start)) << endtc;
	OmnTC(OmnExpected<int>(29), OmnActual<int>(rec.record_data_len)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(rec.record_ver_major)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rec.record_ver_minor)) << endtc;
	OmnTC(OmnExpected<int>(5), OmnActual<int>(rec.msg_start)) << endtc;
	OmnTC(OmnExpected<int>(20), OmnActual<int>(rec.msg_body_len)) << endtc;
	OmnTC(OmnExpected<int>(eAosSSLMsg_ClientHello), OmnActual<int>(rec.msg_type)) << endtc;
	OmnTC(OmnExpected<int>((int)&buff[5]), OmnActual<int>((int)AosSslRecord_getMsg(&rec))) << endtc;

	OmnTC(OmnExpected<int>(0), OmnActual<int>(memcmp(buff, msg1, 29))) << endtc;

	AosSslRecord_moveToNextMsg(&rec);
	OmnTC(OmnExpected<int>(29), OmnActual<int>(rec.msg_start)) << endtc;

	b = AosSslRecord_getMsgBody(&rec);
	memset(b, '6', 10);
	ret = AosSslRecord_createHandshake(&rec, eAosSSLMsg_ServerHello, 10);	
	//OmnTC(OmnExpected<int>(0), OmnActual<int>(memcmp(buff, msg1, 43))) << endtc;
	AosSslRecord_moveToNextMsg(&rec);

	b = AosSslRecord_getMsgBody(&rec);
	memset(b, '7', 10);
	ret = AosSslRecord_createHandshake(&rec, eAosSSLMsg_Certificate, 10);	
	AosSslRecord_moveToNextMsg(&rec);

	ret = AosSslRecord_createFinishedMsg(&rec, 10);	
	b = AosSslRecord_getMsg(&rec);
	memset(b, '8', 10);
	AosSslRecord_moveToNextMsg(&rec);

	b = AosSslRecord_getMsgBody(&rec);
	ret = AosSslRecord_createChangeCipherSpec(&rec);	
	AosSslRecord_moveToNextMsg(&rec);

	ret = AosSslRecord_getNextRecord(buff, 78, 0, conn->front_context);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	if (ret)
	{
		return false;
	}
	
	AosSslRecord *record= &conn->front_context->record;

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(eAosContentType_Handshake), OmnActual<int>(record->record_type)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(record->record_ver_major)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(record->record_ver_minor)) << endtc;
	OmnTC(OmnExpected<int>(52), OmnActual<int>(record->record_body_len)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(record->msg_start)) << endtc;
	OmnTC(OmnExpected<int>(20), OmnActual<int>(record->msg_body_len)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(strncmp("55555555555555555555", 
		AosSslRecord_getMsgBody(record), 
		record->msg_body_len))) << AosSslRecord_getMsgBody(record) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(conn->front_context->bufferedData != 0)) << endtc;
	OmnTC(OmnExpected<int>(21), OmnActual<int>(conn->front_context->bufferedDatalen)) << endtc;
	
	// 
	// Next message should be ServerHello
	//
	ret = AosSslRecord_nextMsg(record, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	b = AosSslRecord_getMsgBody(record);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(strncmp("6666666666", b, 10))) << endtc;
	OmnTC(OmnExpected<int>(eAosContentType_Handshake), OmnActual<int>(record->record_type)) << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(record->record_ver_major)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(record->record_ver_minor)) << endtc;
	OmnTC(OmnExpected<int>(52), OmnActual<int>(record->record_body_len)) << endtc;
	OmnTC(OmnExpected<int>(24), OmnActual<int>(record->msg_start)) << endtc;
	OmnTC(OmnExpected<int>(10), OmnActual<int>(record->msg_body_len)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(conn->front_context->bufferedData != 0)) << endtc;
	OmnTC(OmnExpected<int>(21), OmnActual<int>(conn->front_context->bufferedDatalen)) << endtc;
*/	
	return true;
}	


// 
// This function tests one record, multiple messages.
//
bool
AosSslRecordTester::oneRecordMultiMsg()
{
/*	
	//
	// One record, contain:
	//		 ClientHello, length = 20
	//		 ServerHello, length = 10
	//
	char data[] = 
	{
 		0x16, 0x03, 0x00, 0x00, 0x26, 0x01, 0x00, 0x00, 0x14, 0x35, 
 		0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 
 		0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x02, 
 		0x00, 0x00, 0x0a, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
 		0x36, 0x36, 0x36 
	}; 
	int data_len = 43;

	struct tcp_vs_conn *conn = new tcp_vs_conn;
	memset(conn, 0, sizeof(tcp_vs_conn));
	conn->front_context = new AosSslContext;
	struct AosSslContext *context = conn->front_context;

	int ret = AosSslRecord_getNextRecord(data, data_len, 
		eAosSSLFlag_Front | eAosSSLFlag_Receive, 
		conn->front_context);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(context->record.msg_start)) << endtc;

//    ret = AosSslRecord_hasMoreMsg(&context->record);
//	OmnTC(OmnExpected<int>(1), OmnActual<int>(ret)) << endtc;

    ret = AosSslRecord_nextMsg(&context->record, 0);
	OmnTC(OmnExpected<int>(24), OmnActual<int>(context->record.msg_start)) << endtc;
	OmnTC(OmnExpected<int>(10), OmnActual<int>(context->record.msg_body_len)) << endtc;

//    ret = AosSslRecord_hasMoreMsg(&context->record);
//	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

    ret = AosSslRecord_nextMsg(&context->record, 0);
	OmnTC(OmnExpected<int>(eAosRc_NoMsg), OmnActual<int>(ret)) << endtc;

 //   ret = AosSslRecord_hasMoreMsg(&context->record);
//	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
*/
	return true;
}


