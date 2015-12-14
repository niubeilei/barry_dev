////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOS_SSL_SERVER_H__
#define __AOS_SSL_SERVER_H__

extern int AosSslServer_ClientHello(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);

extern int AosSsl_Established(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);

#endif //#ifndef __AOS_SSL_SERVER_H__
