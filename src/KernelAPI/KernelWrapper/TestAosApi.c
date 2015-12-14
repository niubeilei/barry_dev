////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestAosApi.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "aos/aosKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/netdevice.h"
#include "KernelUtil/aostcpapi.h"

int TestCallBack(struct aos_tcpapi_data *conndata, char *data, int data_len, void *userdata, int rc);

static char userdata[256];
static char senddata[256];
struct aos_tcpapi_data *g_conndata = NULL;

int aosTestTcpApi(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen)
{
    uint32_t addr = parms->mIntegers[0];
	uint16_t port = parms->mIntegers[1];
	int rc;

    printk("In aosTestTcpApi kernel\n");
	sprintf(userdata, "This is user data\n");
	if(g_conndata)
	{
		aos_tcp_close(g_conndata);
		g_conndata = NULL;
	}
	if(eAosRc_Success != (rc=aos_tcp_client_create(addr, port, &g_conndata, userdata, 50*HZ, TestCallBack)))
	{
		printk("aos tcp client create failed %d\n", rc);
		return -1;
	}
    return 0;
}

int TestCallBack(struct aos_tcpapi_data *conndata, char *data, int data_len, void *userdata, int rc)
{
	int len;
	printk("TestCallBack entry\n");
	switch(rc)
	{
		case eAosRc_ConnSuccess:
			printk("TestCalBack:Connect to %d:%d success\n", conndata->remote_addr, conndata->remote_port);
			g_conndata = conndata;
			sprintf(senddata, "This is a send test\n");
			printk("TestCallBack: senddata is %s\n", senddata);
			len = aos_tcp_send_data(conndata, senddata, sizeof(senddata));  
			if(len <= 0)
				printk("TestCallBack:Data sent error %d\n",len);
			else
				printk("TestCallBack:Sent %d Bytes\n",len);
			break;
		case eAosRc_Timeout:
			printk("TestCallBack:Get TimeOut Rc\n");	
			aos_tcp_close(conndata);
			break;
		case eAosRc_DataRead:
			printk("TestCallBack:Read Data %d Bytes\n", data_len);
			printk("TestCallBack:The Data is %s\n", data);
			aos_tcp_close(conndata);
			break;
		default:
			printk("TestCallBack:RC is %d\n",rc);
			aos_tcp_close(conndata);
			break;
	}
	return 0;
}

