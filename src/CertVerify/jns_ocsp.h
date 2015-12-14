////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: jns_ocsp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef _JNS_OCSP_
#define _JNS_OCSP_

#include "PKCS/asn1.h"

#define JNS_QR		0x36	// OCSQS客户请求消息QR
#define JNS_QP		0x37	// OCSQS服务器响应消息QP

#define JNS_NONCE_LEN	64
#define JNS_NONCE_BITS	(JNS_NONCE_LEN * 8)

#define X509_OCSP_RESP_MODE_EASYMODE 1
#define X509_OCSP_RESP_MODE_JNS      2
#define X509_OCSP_RESP_MODE_STANDARD 3


enum {
	JNS_OCSP_CERTSTATUS_ERROR = -1,
	JNS_OCSP_CERTSTATUS_GOOD,
	JNS_OCSP_CERTSTATUS_REVOKED,
	JNS_OCSP_CERTSTATUS_TIMEOUT,
	JNS_OCSP_CERTSTATUS_NOTFOUND,
};

typedef struct _jns_ocsp_request {
	asn_data_t *		sn;

	BYTE 			nonce[JNS_NONCE_LEN];
	int			nonce_len;
	UINT32			time;
}	JNS_OCSP_REQUEST;

#define JNS_MIN_RESPONSE_SIZE	4

extern DATA_OBJECT *jns_encode_ocsp_request(JNS_OCSP_REQUEST *request);
extern int jns_decode_ocsp_response(DATA_OBJECT *object);

extern int AosOcsp_setRespModeCli(char *data,
				                   unsigned int *length,
				                   struct aosKernelApiParms *parms,
				                   char *errmsg,
				                   const int errlen);
extern int AosOcsp_setRespModeinit(void);

#endif
