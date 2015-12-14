////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: interface.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Ktcpvs/interface.h"
int aos_ssl_proc(
		char *data, 
		unsigned int data_len,
		struct tcp_vs_conn *conn, 
		char **rslt_data, 
		unsigned int *rslt_data_len, 
		sslcallback callback)
{
	return 0;
}

// release buffer allocated by ssl module		
int aos_ssl_release_buf(char *rslt_data)
{
	return 0;
}
