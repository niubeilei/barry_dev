////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: jns_ocsp.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef __KERNEL__

#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#endif
#include "aos/aosKernelApi.h"
#include "ssl/SslMisc.h"
#include "ssl/SslCipher.h"
#include "asn1.h"
#include "x509.h"
#include "CertVerify/jns_ocsp.h"
#include "PKCS/CertMgr.h"


static unsigned int sgOcsp_resp_mode = X509_OCSP_RESP_MODE_JNS;

#define MD5_LEN 16
/* encode the body of request message */
static DATA_OBJECT *jns_encode_body(JNS_OCSP_REQUEST *request) {
	DATA_OBJECT *object;
	BYTE status = 0;
	int len, ret;

	len = request->nonce_len + request->sn->len + 16;
	object = asn_malloc_object(len);
	if (!object)
		return NULL;

	ret = asn_encode_integer(object, &status, 1, DEFAULT_TAG);
	if (ret == 0) {
		ret = asn_encode_bitstring_hole(object, request->nonce_len, DEFAULT_TAG);
		if (ret == 0)
			ret = asn_write(object, request->nonce, request->nonce_len);
	}
	if (ret == 0)
		ret = asn_encode_octet_string(object, request->sn->data,
				request->sn->len, DEFAULT_TAG);
	if (ret == 0)
		object = asn_merge_objects(&object, NULL, JNS_QR);

	if (ret == 0)
		return object;

	asn_free_object(object);
	return NULL;
}
#define TRANS_ID_LEN	10
DATA_OBJECT *jns_encode_ocsp_request(JNS_OCSP_REQUEST *request) {
	static char *OCSQS_Name = "SCA_OCSQS";
	DATA_OBJECT *object = NULL, *body = NULL, *nonce_object = NULL;
	BYTE transID[TRANS_ID_LEN]={0x2,0x4,0xef};
	BYTE pvno = 0x01;
	int len, ret;

	body = jns_encode_body(request);
	if (body)
		body = asn_merge_objects(&body, NULL, MAKE_CTAG_PRIMITIVE(2));
	if (!body) 		return NULL;

	nonce_object = asn_malloc_object(request->nonce_len + 6);
	if (!nonce_object)
		goto error;
	ret = asn_encode_octet_string(nonce_object, request->nonce,
				request->nonce_len, DEFAULT_TAG);
	if (ret == 0)
		nonce_object = asn_merge_objects(&nonce_object, NULL, MAKE_CTAG_PRIMITIVE(0));
	if (ret || !nonce_object)
		goto error;

	len = request->nonce_len + strlen(OCSQS_Name) + TRANS_ID_LEN +
			asn_sizeof_generalized_time() + 32;
	object = asn_malloc_object(len);
	if (!object)
		goto error;

	ret = asn_encode_integer(object, &pvno, 1, DEFAULT_TAG);
	if (ret == 0)
		ret = asn_encode_octet_string(object, request->nonce,
				request->nonce_len, DEFAULT_TAG);
	if (ret == 0)
		ret = asn_encode_octet_string(object, (BYTE *)OCSQS_Name,
				strlen(OCSQS_Name), DEFAULT_TAG);
	if (ret == 0)
		ret = asn_encode_octet_string(object, transID, TRANS_ID_LEN, DEFAULT_TAG);
	if (ret == 0)
		ret = asn_encode_generalized_time(object, request->time, DEFAULT_TAG);

	if (ret != 0)
		goto error;
	
	// comment by liqin 2006/01/13
	//object = asn_merge_objects(&object, &nonce_object, BER_SEQUENCE);
	//if (!object)
	//	goto error;

	// the following three lines are added by liqin 2006/01/13
	object = asn_merge_objects(&object, NULL, BER_SEQUENCE);
	if (!object)
		goto error;

	object = asn_merge_objects(&object, &body, BER_SEQUENCE);

	return object;

error:
	if (body)	asn_free_object(body);
	if (object)	asn_free_object(object);
	if (nonce_object) asn_free_object(nonce_object);
	return NULL;
}

	
static int verify_ocsp_response(unsigned char * msg, int msg_len, unsigned char * digest, int digest_len)
{
	unsigned char md5_result[MD5_LEN+1];
	X509_CERT_INFO * cert;
	struct R_RSA_PUBLIC_KEY pubkey;
	uint8 veri_rcvd[MAX_RSA_MODULUS_LEN];
	int veri_rcvd_len;

	md5_result[MD5_LEN]=0;
	MD5_arth(msg,msg_len,md5_result);
	//aos_trace("ocsp response md5 result is:%s",md5_result);

	cert= AosCertMgr_getCertByName("JNS_OCSP");
	// cert = AosCertMgr_getOcspCert(0);
	if(!cert)
	{
		aos_alarm(eAosMD_SSL,eAosAlarm_SSLProcErr,"can not get ocsp cert");
		return 0;
	}
	pubkey.bits = (cert->pubkey.modulus.len << 3);
	memcpy(pubkey.modulus, cert->pubkey.modulus.data, cert->pubkey.modulus.len);
	memset(pubkey.exponent, 0, MAX_RSA_MODULUS_LEN);
	pubkey.exponent[MAX_RSA_MODULUS_LEN-1] = 1;
	pubkey.exponent[MAX_RSA_MODULUS_LEN-3] = 1;
	aos_trace("verify ocsp response, get pubkey from cert ok");
	if (0 != aos_ssl_asycipher.pubdec (
					(unsigned char*)veri_rcvd, 
					(unsigned int*)&veri_rcvd_len, 
					(unsigned char*)digest, 
					digest_len, 
					&pubkey))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "decrypt ocsp response failed");
		return 0;
	}	
	
	if( 0 == memcmp(veri_rcvd,md5_result,MD5_LEN) )
	{
		aos_trace("verify ocsp response, verify ok");
		return 1;
	}
	aos_trace("verify ocsp response, verify failed");
	return 0;
}


//static unsigned int sgOcsp_resp_mode = X509_OCSP_RESP_MODE_EASYMODE;

int jns_decode_ocsp_response_easymode(DATA_OBJECT *object) {
	if (!object)
		return JNS_OCSP_CERTSTATUS_ERROR;
	if(object->length >= 5 && (0 == strncmp(object->data,"allow",5)))
	{
		return JNS_OCSP_CERTSTATUS_GOOD;
	}
	else if(object->length >= 6 && (0 == strncmp(object->data,"forbid",6)))
	{
		return JNS_OCSP_CERTSTATUS_REVOKED;
	}
	return JNS_OCSP_CERTSTATUS_ERROR;
}

int jns_decode_ocsp_response_standard(DATA_OBJECT *object) {
	return JNS_OCSP_CERTSTATUS_GOOD;
}

int jns_decode_ocsp_response_jns(DATA_OBJECT *object) {

	int length, c, ret;
	int msglen, len;
	int msg_start,msg_end,sign_start,sign_end;
	

	//aos_trace_hex("object content: ",object->data,object->length);
	if (!object || object->length < JNS_MIN_RESPONSE_SIZE)
		return JNS_OCSP_CERTSTATUS_ERROR;

	ret = asn_decode_sequence(object, &length);
	aos_trace("length=%d",length); //378
	aos_trace("pos=%d",object->pos); //4

	// signed content start here
	msg_start=object->pos;
	aos_trace("msg_start=%d",msg_start);
	if (ret == 0)
		ret = asn_decode_universal(object);
	
	aos_trace("pos=%d",object->pos); //215
	if (ret != 0)
		return JNS_OCSP_CERTSTATUS_ERROR;

	c = asn_getc(object);
	aos_trace("pos=%d",object->pos); //216
	if (c != 0x82)
		return JNS_OCSP_CERTSTATUS_ERROR;
	len = asn_getc(object);
	aos_trace("pos=%d",object->pos); //217 
	if (len <= 0)
		return JNS_OCSP_CERTSTATUS_ERROR;
	// signed content end here
	msg_end=len+object->pos;
	aos_trace("msg_end=%d",msg_end);

	c = asn_getc(object);	
	aos_trace("pos=%d",object->pos);//218
	if (c != JNS_QP)
		return JNS_OCSP_CERTSTATUS_ERROR;
	msglen = asn_getc(object);
	aos_trace("pos=%d",object->pos); //219 
	len = asn_getc(object);
	aos_trace("pos=%d",object->pos); //220
	if ((msglen - len) < JNS_MIN_RESPONSE_SIZE)
		return JNS_OCSP_CERTSTATUS_ERROR;

	asn_skip(object, len);
	aos_trace("pos=%d",object->pos); //234
	c = asn_getc(object); // this is the result
	aos_trace("pos=%d",object->pos); //235

	asn_seek(object,msg_end);
	aos_trace("asn_seek to %d",msg_end);
	/*
	ret = asn_decode_octet_string_hole(object, &length, -1); 
	if(ret<0)
		return JNS_OCSP_CERTSTATUS_ERROR;
	if(length<=0)
		return JNS_OCSP_CERTSTATUS_ERROR;
		*/
	sign_start=object->pos+3;
	sign_end=object->pos+128+3;
	aos_trace("sign start at %d sign end at %d",sign_start,sign_end);

	if(	verify_ocsp_response(&object->data[msg_start],msg_end-msg_start,
							&object->data[sign_start],sign_end-sign_start) )
		return c;
	else
		return JNS_OCSP_CERTSTATUS_ERROR;
}

int jns_decode_ocsp_response(DATA_OBJECT *object) {
	switch(sgOcsp_resp_mode)
	{
	case X509_OCSP_RESP_MODE_STANDARD:
		return jns_decode_ocsp_response_standard(object);
		
	case X509_OCSP_RESP_MODE_EASYMODE:
		return jns_decode_ocsp_response_easymode(object);
		
	case X509_OCSP_RESP_MODE_JNS:
		return jns_decode_ocsp_response_jns(object);
	default:
		return JNS_OCSP_CERTSTATUS_ERROR;
	}

	return JNS_OCSP_CERTSTATUS_ERROR;
}

//
// add by lxx 0323 for set ocsp response mode cli
//

int AosOcsp_setRespModeCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{

	// 
	// system busyloop <duration-in-sec> [<cpu-usage>]
	//

	char* type = parms->mStrings[0];

	*length = 0;
	if (!type)
	{
		sprintf(errmsg, "Missing parameters");
		return -1;
	}

	if (strcmp(type, "easymode") == 0)
	{
		sgOcsp_resp_mode = X509_OCSP_RESP_MODE_EASYMODE;
		return 0;
	}

	if (strcmp(type, "jns") == 0)
	{
		sgOcsp_resp_mode = X509_OCSP_RESP_MODE_JNS;
		return 0;
	}

	if (strcmp(type, "standard") == 0)
	{
		sgOcsp_resp_mode = X509_OCSP_RESP_MODE_STANDARD;
		return 0;
	}

	sprintf(errmsg, "Wrong parameters %s", type);
	return -1;
}

int AosOcsp_setRespModeinit()
{
	OmnKernelApi_addCliCmd("ocsp_set_resp_mode", AosOcsp_setRespModeCli);	

	return 0;
}

