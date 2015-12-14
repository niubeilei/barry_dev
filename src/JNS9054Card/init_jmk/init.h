////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: init.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef INIT_MULTIJMK_HEAD_H
#define INIT_MULTIJMK_HEAD_H






#define MAX_CA_LENGTH 2048

#define SEND_CA_TO_JMK 0
#define RECEIVE_CA_FROM_JMK 1

struct data_buf{
	int len;
	unsigned char buf[MAX_CA_LENGTH];
};

struct io_buf{
	
	struct data_buf ca0;
	struct data_buf ca1;
	struct data_buf ca2;
	struct data_buf server_cert;
	//struct data_buf server_pub;
	struct data_buf pub2;
	struct data_buf ocsp;
	struct data_buf pri2;
};

struct struct_ioctl{
	unsigned int cmd;
	int send_direct;
	struct io_buf buf;
};

#define INIT_MULTIJMK	15


#endif

