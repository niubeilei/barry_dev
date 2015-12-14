////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: access_manage.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/kmod.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/in.h>
#include <linux/list.h>

#include "aos/aosKernelApi.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "CertVerify/aostcpapi.h"
#include "CertVerify/aosAccessManage.h"
#include "CertVerify/amm_priv.h"
#include "Ktcpvs/tcp_vs_def.h"

static AMM_CONFIG amm_config;
AMM_CONFIG *amc = &amm_config;
static struct task_struct * amm_thread;
static int amm_init(void)	{
	memset(amc, 0, sizeof(AMM_CONFIG));
	strcpy(amc->AZN_C_ORG_NAME, "AZN_ORG_NAME");
	strcpy(amc->AZN_C_APPSYS_NAME, "AZN_APPSYS_NAME");
	//strcpy(amc->org, "/l=公安部");
	amc->request_timeout = 300; //default is 300 seconds

	spin_lock_init(&amc->request_lock);
	INIT_LIST_HEAD(&amc->request_list);

	amc->flags = AMM_FLAGS_NONE;
	aos_debug_log(eAosMD_AMM, "AMM inited as: %s", amc->org);

	return 0;
}

int aos_amm_set_proxy(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	if (parms->mNumIntegers < 2) {
		aos_debug_log(eAosMD_AMM, "[amm set addr]: parm number not enough");
		return -1;
	}

	amc->addr = parms->mIntegers[0];
	amc->port = parms->mIntegers[1];

	amc->flags |= AMM_PROXY_SET;
	aos_debug_log(eAosMD_AMM, "PMI Server set");
	*length = 0;
	return 0;
}


int aos_amm_set_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	int on = -1;

	if (parms->mNumStrings < 1) {
		aos_debug_log(eAosMD_AMM, "[amm set status]: no parm ?");
		return -1;
	}

	if (strcmp(parms->mStrings[0], "on") == 0)
		on = 1;
	if (strcmp(parms->mStrings[0], "off") == 0)
		on = 0;

	switch (on) {
	case 1:
		amc->flags |= AMM_STATUS_ON;
		aos_debug_log(eAosMD_AMM, "amm opened");
		break;
	case 0:
		amc->flags &= ~AMM_STATUS_ON;
		aos_debug_log(eAosMD_AMM, "amm closed");
		break;
	default:
		aos_debug_log(eAosMD_AMM, "[amm set status]: parm error");
		return -1;
	}
	
	*length = 0;
	return 0;
}


int aos_amm_set_org(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	if (parms->mNumStrings < 1)  {
		aos_debug_log(eAosMD_AMM, "[amm set org]: no parameter?");
		return -1;
	}
	if (strlen(parms->mStrings[0]) > AMM_NAME_MAX_SIZE - 1) {
		aos_debug_log(eAosMD_AMM, "[amm set org]: string length is bigger than %d", AMM_NAME_MAX_SIZE);
		return -1;
	}
	memset(amc->org, 0, AMM_NAME_MAX_SIZE);
	strcpy(amc->org, parms->mStrings[0]);
	amc->flags |= AMM_ORG_SET;

	aos_debug_log(eAosMD_AMM, "[amm set org] set as: %s", amc->org);
	*length = 0;
	return 0;
}

int aos_amm_set_timeout(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen) {
	if (parms->mNumIntegers < 1 ||
	    parms->mIntegers[0] < 5)	{//must >= 5 seconds
	    	printk(KERN_ALERT"The value of timeout is too small\n");
		return -1;
	}

	amc->request_timeout = parms->mIntegers[0];

	aos_debug_log(eAosMD_AMM, "[amm set org] set as: %d", amc->request_timeout);
	*length = 0;
	return 0;
}

int aos_amm_show_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0; ptr = buf;
	
	len += sprintf(ptr + len, "PMI information\n");
	len += sprintf(ptr + len, "\tstatus: %s\n", 
			(amc->flags & AMM_STATUS_ON) ? "on" : "off");
	
	if (amc->flags & AMM_PROXY_SET) {
		unsigned int addr = amc->addr;
		int port = amc->port;

		len += sprintf(ptr + len, "\tserver: %u.%u.%u.%u:%d\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff,
			(addr >> 24) & 0xff, port);
	} else
		len += sprintf(ptr + len, "\tserver: unset\n");
	
	if (amc->flags & AMM_ORG_SET)
		len += sprintf(ptr + len, "\torg: %s\n", amc->org);
	else
		len += sprintf(ptr + len, "\torg: unset\n");
	
	len += sprintf(ptr + len, "\ttimeout:%d\n", (int)amc->request_timeout);	

	*length = len;
	
	return 0;
}

int aos_amm_save_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	char *buf = aosKernelApi_getBuff(data);
	char *ptr;
	int len;

	len = 0;
	ptr = buf;

	if (amc->flags & AMM_STATUS_ON)
		len += sprintf(ptr + len, "<Cmd>amm status on</Cmd>\n");
	if (amc->flags & AMM_PROXY_SET) {
		unsigned int addr = amc->addr;
		int port = amc->port;

		len += sprintf(ptr + len, "<Cmd>amm set addr %u.%u.%u.%u %d</Cmd>\n",
			(addr) & 0xff, (addr >> 8) & 0xff, (addr >> 16) &0xff,
			(addr >> 24) & 0xff, port);
	}

	if (amc->flags & AMM_ORG_SET)
		len += sprintf(ptr + len, "<Cmd>amm set org %s</Cmd>\n", amc->org);

	len += sprintf(ptr + len, "<Cmd>amm set timeout %d</Cmd>\n", (int)amc->request_timeout);

	*length = len;
	return 0;
}

int aos_amm_clear_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen)
{
	amm_init();
	*length = 0;

	return 0;
}


static int amm_tcp_callback(struct aos_tcpapi_data *conndata, char *data, int data_len,
			void *userdata, int rc);

//for liqi to call it
int aos_check_permit(struct aos_httpmsg *msg,
			int operation,
			struct aos_user *user,
			char *domain,
			char *sysname,
			char *resource_name,
			AMM_CALLBACK callback) {
			
	AMM_REQUEST *request;
	
	if (!user || !sysname || !resource_name)
		return eAosRc_AccessError;
	//if socket not created, just return error;
	if ((amc->flags & AMM_PMI_READY) != AMM_PMI_READY) {
		aos_debug_log(eAosMD_AMM, "return eAosRc_AccessError amc not ready");
		return eAosRc_AccessError;
	}

	//check string length, we assume max length is AMM_REOURCE_NAME_MAX
	if (strlen(domain) > AMM_REOURCE_NAME_MAX)		return eAosRc_AccessError;
	if (user->len > AMM_REOURCE_NAME_MAX)			return eAosRc_AccessError;
	if (strlen(resource_name) > AMM_REOURCE_NAME_MAX)	return eAosRc_AccessError;

	request = (AMM_REQUEST *)aos_zmalloc(sizeof(AMM_REQUEST));
	if (!request)
		return eAosRc_AccessError;

	request->msg = msg;
	request->operation = operation;
	request->user = user;
	request->domain = domain;
	request->sysname = sysname;
	request->resource_name = resource_name;
	request->callback = callback;

	spin_lock(&amc->request_lock);
	list_add_tail(&request->link, &amc->request_list);
	spin_unlock(&amc->request_lock);

	aos_debug_log(eAosMD_AMM, "[aos_check_permit]: recv a request of [user: %s, domain: %s, sys: %s, rn: %s]",
		request->user->data, request->domain, request->sysname, request->resource_name);
	wake_up_process(amm_thread);
	return eAosRc_AccessAsync;
}

char user[15] = "A0200000000748";
static int do_amm_send_request(AMM_REQUEST *request) {
	char *inbuf = NULL, *rc = NULL, *p;
	char *op;
	AMM_PROTO_HEADER *pData;
	int len, ret;

	if(request->operation & CLIENT_USERTYPE_USB) 
		op = "opr=直接访问";
	else 
		op = "opr=加密访问";

	rc = aos_zmalloc(512);
	if (!rc)
		goto error;

	snprintf(rc, 511, "org=%s,sys=%s,obj=%s", amc->org, request->sysname, request->resource_name);
	//strncpy(request->user->data, user, 14);
	//request->user->len = 14;
	len = 0;
	len += strlen(amc->AZN_C_ORG_NAME) + 1;
	len += strlen(amc->org) + 1;
	len += strlen(amc->AZN_C_APPSYS_NAME) + 1;
	len += strlen(request->sysname) + 1;
	len += request->user->len + 1;
	len += strlen(rc) + 1;
	len += strlen(op) + 1;
	len += AMM_PROTO_DATA_LEN;

	inbuf = aos_zmalloc(len);
	if (!inbuf)
	{
		aos_alarm(eAosMD_CertVerify, eAosAlarm_ProgErr, "inbuf for pmi request allocate failed");
		goto error;
	}
	pData = (AMM_PROTO_HEADER *)inbuf;

	pData->length=len;
	pData->sum = 0;
	pData->serial=0;
	pData->command=1000;
	pData->content=2;

	p = (char *)(&(pData->data));
	p += sprintf(p,amc->AZN_C_ORG_NAME);		p++;
	p += sprintf(p,amc->org);			p++;
	p += sprintf(p,amc->AZN_C_APPSYS_NAME);		p++;
	p += sprintf(p,request->sysname);		p++;
	memcpy(p, request->user->data, request->user->len);
	p += request->user->len; *p=0; p++;
	p += sprintf(p,rc);				p++;
	p += sprintf(p,op);				p++;
	//aos_trace("%s", op);
	ret = aos_tcp_send_data(request->sock, inbuf, len);
	//not full data sent? it's imposible, so we always assume it an error
	if (ret != len) {
		aos_debug_log(eAosMD_AMM, "send tcp data failed!");
		goto error;
	} else
		aos_debug_log(eAosMD_AMM, "send tcp data ok!");

	request->flags |= AMM_DATA_WAITTING;

	aos_free(rc);
	aos_free(inbuf);
	return 0;
error:
	if (rc)		aos_free(rc);
	if (inbuf)	aos_free(inbuf);

	return -1;
}



static int do_check_permit(void *p)	{
	//DECLARE_WAIT_QUEUE_HEAD(amm_request_wq);
	
	__set_current_state(TASK_RUNNING);

	do {
		AMM_REQUEST * request;
		int ret;


		/* if list is empty or waitting for socket to creat, sleep a while */
		if (list_empty(&amc->request_list)) {
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ/4);
			__set_current_state(TASK_RUNNING);

			//interruptible_sleep_on_timeout(&amm_request_wq, HZ);
			continue;
		}

		/* get the first request */
		spin_lock(&amc->request_lock);
		request = list_entry(amc->request_list.next, AMM_REQUEST, link);
		list_del(&request->link);
		spin_unlock(&amc->request_lock);

		request->flags |= AMM_SOCK_WAITTING;

		ret = aos_tcp_client_create(amc->addr, amc->port, &request->sock,
				request, amc->request_timeout, amm_tcp_callback);

		if (!ret)
			continue;

		//call http callback of failed
		aos_debug_log(eAosMD_AMM, "request return as AOS_ACCESS_FAILED");

		request->callback(request->msg,
				AOS_ACCESS_FAILED,
				request->user,
				request->domain,
				request->sysname,
				request->resource_name);
		aos_free(request);

	} while (!kthread_should_stop());
	return 0;
}

static int amm_tcp_callback(struct aos_tcpapi_data *conndata, char *data, int data_len,
			void *userdata, int rc) {
	AMM_REQUEST *request = (AMM_REQUEST *)userdata;
	AMM_PROTO_HEADER *amm_resp;
	int ret, is_allowed = AOS_ACCESS_FAILED;
	char ack=0;

	switch (rc) {
	case eAosRc_ConnSuccess:
		if ((request->flags & AMM_SOCKET_CREATED) ||
			!(request->flags & AMM_SOCK_WAITTING))
			return 0;

		request->flags &= ~AMM_SOCK_WAITTING;
		request->flags |=  AMM_SOCKET_CREATED;
		request->sock = conndata;

		aos_debug_log(eAosMD_AMM, "socket created");
		ret = do_amm_send_request(request);
		if (ret == 0)
			return 0;
		break;
	case eAosRc_DataRead:
		if ((request->flags & AMM_SOCKET_CREATED) == 0 ||
		    data_len < AMM_PROTO_DATA_LEN)
			return 0;
		amm_resp = (AMM_PROTO_HEADER *)data;
		if (data_len < amm_resp->length) {
			aos_debug_log(eAosMD_AMM, "data received not enough");
			return 0;
		}
		if (amm_resp->content == 0) {
			if (amm_resp->data[0] == 0)
				is_allowed = AOS_ACCESS_PERMIT;
			else
				is_allowed = AOS_ACCESS_DENY;
		} else 
				is_allowed = AOS_ACCESS_DENY;
		aos_debug_log(eAosMD_AMM, "PMI result received: %d", is_allowed);
		aos_tcp_send_data(request->sock, &ack, 1);

		break;
	default:	//received an error, always a socket failed
		request->flags |= AMM_SOCK_ERROR;
		break;
	}

	aos_tcp_close(request->sock);

	//call http callback of result
	aos_debug_log(eAosMD_AMM, "return %d for request: %s", is_allowed, request->resource_name);
	request->callback(request->msg,
			is_allowed,
			request->user,
			request->domain,
			request->sysname,
			request->resource_name);
	aos_free(request);

	return 0;
}


/* module initial function */
static int __init aos_amm_init(void) {

	amm_init();

	amm_thread = kthread_run(do_check_permit, NULL, "amm_thread");
	if (IS_ERR(amm_thread)) {
		printk(KERN_ERR"aos_amm:do_check_permit creation failed\n");
		return -1;
	}

	printk("aos_amm:creation kernel thread for PMI check ------[OK]\n");
	return 0;
}

static void __exit aos_amm_exit(void) {
	AMM_REQUEST *request, *n;

	kthread_stop(amm_thread);

	amc->flags = AMM_FLAGS_NONE;
	spin_lock(&amc->request_lock);
	list_for_each_entry_safe(request, n, &amc->request_list, link) {
		list_del(&request->link);
		aos_free(request);
	}
	spin_unlock(&amc->request_lock);
}
#ifdef MODULE
module_init(aos_amm_init)
module_exit(aos_amm_exit)
MODULE_LICENSE("GPL");
#else
__initcall(aos_amm_init);
#endif
