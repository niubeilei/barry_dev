////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestDriver.c
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
#include <linux/delay.h>

#include "aos/aosKernelApi.h"
#include "aosUtil/Memory.h"

#include "PKCS/asn1.h"
#include "PKCS/x509.h"
#include "CertVerify/cert_verify.h"
///////////////////////////////////////////////////////////////////////
#include "CertVerify/Tester/OcspTestDriver.h"

#include "aosUtil/Tracer.h"


// 
// This function tests OCSP implementation. It will use 'serial_num'
// as the initial serial number. During the testing, each OCSP query
// is constructed by increment 'serial_num'. The total OCSP queries
// is 'num_tries'. 
// 
// Parameters:
//	'req_id':IN
//		The request ID. This is set by the caller. 
//	'const char *serial_num': IN
//		The initial serial number. 
//	'serial_len': IN
//		The length of the serial number.
//	'num_tries': IN
//		The number of queries for the test.
//	'max_conns': IN
//		The maximum concurrent requests during the testing. 
//
// Return Values:
//	eAosRc_Success:	the call is accepted.
//
#define OCSP_TEST_FINISHED	0
#define OCSP_TEST_DOING		1	

typedef struct __ocsp_test {
	u32 id;
	
	unsigned char sn[ASN_SMALL_DATA_LEN];
	u32 sn_len;
	
	u32 num_tries;
	u32 max_conns;
	
	atomic_t current_conns;
	
	u32 left;
	atomic_t rtn_num;

	u32 rtn_valid;
	u32 rtn_invalid;
	atomic_t rtn_failed;
	
	u32 flags;
} OCSP_TEST;

static struct task_struct * test_thread = NULL;
static pid_t test_pid = 0;

static void bin_inc(unsigned char * bin, int len) {
	int i;
	unsigned char *uc;
	
	uc = &bin[len - 1];
	
	i = len - 1;
	while (i >= 0) {
		*uc += 1;
		if (*uc != 0)
			break;
		else {
			uc--;
			i--;
		}
	}
}


static int test_callback(void * info,void *context, int rc) {
	X509_CERT_INFO *certinfo = (X509_CERT_INFO *)info;
	OCSP_TEST *tocsp = (OCSP_TEST *)context;
	
	printk("ocsp test: received one response\n");
certinfo->serial_number.data[certinfo->serial_number.len] = 0;
printk("sn:%s\n", certinfo->serial_number.data);
	switch (rc) {
	case eAosRc_CertificateValid:
		tocsp->rtn_valid++;
		printk("cert_callback: user cert sn is valid\n");
		break;
	case eAosRc_CertificateInvalid:
		tocsp->rtn_invalid++;
		printk("cert_callback: user cert sn is invalid\n");
		break;
	case eAosRc_Error:
		atomic_inc(&tocsp->rtn_failed);
		printk("cert_callback: error returned when checking user cert sn\n");
		break;
	default:
		printk("cert_callback: unknown return code\n");
		break;
	}
	
	atomic_dec(&tocsp->current_conns);
	atomic_inc(&tocsp->rtn_num);
	aos_free(certinfo);
	if (test_thread)
		wake_up_process(test_thread);
	return 0;
}

static int ocsp_test_thread(void *p) {
	OCSP_TEST *tocsp = (OCSP_TEST *)p;
	
	daemonize("ocsp_test");
	allow_signal(SIGKILL);

	test_thread = current;

	while (1) {
		if (atomic_read(&tocsp->rtn_num) == tocsp->num_tries) {
			tocsp->flags = OCSP_TEST_FINISHED;
			test_thread = NULL;
			test_pid = 0;
			break;
		}
		
		if (tocsp->left) {
			while(atomic_read(&tocsp->current_conns) < tocsp->max_conns && tocsp->left > 0) {
				X509_CERT_INFO *certinfo;
				int ret;
				
				printk("left request:%d\n", tocsp->left);
				certinfo = (X509_CERT_INFO *)aos_zmalloc(sizeof(X509_CERT_INFO));
				if (!certinfo) {
					printk("aos_zmalloc failed\n");
					continue;
				}
				
				certinfo->serial_number.len = tocsp->sn_len;
				memcpy(certinfo->serial_number.data, tocsp->sn, tocsp->sn_len);
				
				printk("begin aos_verify_cert request...[");
				for(ret=0;ret<certinfo->serial_number.len;ret++)
					printk("%02x ", certinfo->serial_number.data[ret]);
				printk("]\n"); 
				tocsp->left--;
				ret = aos_verify_cert(certinfo, tocsp, test_callback);
				if (ret == eAosRc_CertificateError) {
					atomic_inc(&tocsp->rtn_failed);
					atomic_inc(&tocsp->rtn_num);
					aos_free(certinfo);
					printk("aos_verify_cert return failed\n");
					continue;
				}
				bin_inc(tocsp->sn, tocsp->sn_len);
				atomic_inc(&tocsp->current_conns);
			}
		}
		
// test_sleep:
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		__set_current_state(TASK_RUNNING);

	}

	printk("test_ocsp_thread stoped\n");

	return 0;
}

//global var for ocsp test
static OCSP_TEST tocsp;

int AosOcspTester_start(
			u32 req_id,
			const char *serial_num, 
			u32 serial_len,
			u32 num_tries,
			u32 max_conns)
{
	
	if (!serial_num || serial_len == 0 || serial_len > ASN_SMALL_DATA_LEN ||
		num_tries == 0 || max_conns == 0) {
		aos_trace("param error");
		return -1;
	}

	if (tocsp.flags == OCSP_TEST_DOING) {
		aos_trace("waitting for finished, ocsp test is doing...	");
		return -1;
	}
#if 0
	if (test_pid) {
		kill_proc(test_pid, SIGKILL, 1);
		test_pid = 0;
		test_thread = NULL;
	}
#endif
		
	aos_trace("It is in AosOcspTester_start: %d, %s, %d, %d, %d", 
		req_id, serial_num, serial_len, num_tries, max_conns);

	memset(&tocsp, 0, sizeof(tocsp));
	
	tocsp.flags = OCSP_TEST_DOING;
	tocsp.id = req_id;
	memcpy(tocsp.sn, serial_num, serial_len);
	tocsp.sn_len = serial_len;
	tocsp.num_tries = num_tries;
	tocsp.max_conns = max_conns;
	tocsp.left = num_tries;
	
	aos_trace("begin to do ocsp test...");

	//	'kernel_thread'
	//	 modified by hugo 
	if (kernel_thread(ocsp_test_thread, &tocsp, 0) < 0)
	{
		aos_trace("failed to start ocsp_test_thread");
		return -1;
	}
		
	return 0;
}
			
// 
// This function retrieves the test results. 
//
// Parameters:
//	'req_id':IN
//		This is the request ID that is used to identify the request.
//	'status': OUT
//		1: Test not finished yet
//		0: Test finished.
//	'num_success': OUT
//		The number of successful queries
//	'num_failed': OUT
//		The number of failed queries
//
int AosOcspTester_retrieveRslt(
			u32 req_id,
			u8  *status,
			u32 *num_valid,
			u32 *num_invalid, 
			u32 *num_failed)
{
	*status = tocsp.flags;
	*num_valid = tocsp.rtn_valid;
	*num_invalid = tocsp.rtn_invalid;
	*num_failed = atomic_read(&tocsp.rtn_failed);
	
	aos_trace("In retrieveRslt: requestid:%d, status:%u, num_valid:%u,\
				 num_invalid:%u, num_failed:%u", 
		req_id, 
		(unsigned int)*status, 
		(unsigned int)*num_valid, 
		(unsigned int)*num_invalid,
		(unsigned int)*num_failed);

	return 0;
}

