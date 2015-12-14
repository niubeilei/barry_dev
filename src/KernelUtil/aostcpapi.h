////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aostcpapi.h
// Description:
//	These APIs provide the way to make tcp connection, 
//  receive and send data through tcp in kernel.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

 
#ifndef __AOS_TCP_API_H__
#define __AOS_TCP_API_H__

#define tcpapi_read_timeout 180	// seconds
#define tcpapi_send_timeout 60000	// seconds*HZ, 60*1000
struct aos_tcpapi_data;

typedef int (*aos_tcpapi_callback) (struct aos_tcpapi_data *conndata, char *data, int data_len, void *userdata, int rc); 

enum{
    eAosRc_Failed,
    eAosRc_ConnSuccess,
	eAosRc_DataRead,
	
	eAosRc_Timeout,
	eAosRc_SocketClosed,
	eAosRc_SocketError,
	eAosRc_InvalidSocket,
	eAosRc_FailedToSend,
	eAosRc_InvalidData,
	eAosRc_InvalidDataLen,
	eAosRc_Nomem,
	eAosRc_ConnectFailed
};

struct aos_tcpapi_data{
	uint32_t remote_addr;			// remote ip address
	uint16_t remote_port;			// remote port
	struct socket *sock;			// BSD socket
	void *userdata;					// the data set by the api caller
	int timeout;					// the time interval the socket does nothing
	aos_tcpapi_callback rcvcb;		// callback function, when receive data or error occur
	
	struct list_head list;			// list entry
	unsigned long lastupdated;		// last time received or sent data 
	wait_queue_t wait; 				// wait queue for receiving data
	atomic_t refcnt;
	int return_code;				// remember the return code, callback will use it
};

struct aos_tcpapi_statistics{
    unsigned long create_requests;
    unsigned long conn_success;
    unsigned long conn_failed;
    unsigned long bytes_sent;
    unsigned long bytes_rcvd;
    unsigned long current_conns;
};

extern int aos_tcp_client_create( 
	uint32_t remote_addr,
	uint16_t remote_port,
	struct aos_tcpapi_data **pconndata,
	void *userdata,
	int timeout,
	aos_tcpapi_callback revcallback);

//extern int aos_tcp_close(struct aos_tcpapi_data *conndata);

extern int aos_tcp_send_data(struct aos_tcpapi_data *conndata, char *data, int data_len);

extern int aos_tcp_sendbuffer(struct socket *sock, const char *buffer, const size_t length);

extern int aos_tcpapi_start(void);

#endif
