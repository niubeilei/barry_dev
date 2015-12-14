////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCertVerify.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __ssl_certificate_check__
#define __ssl_certificate_check__

/* func interface
 *------------------------------------------------------*/

/* according to Deng's interface request */
#define eAosRc_CertificateError	       -1
#define eAosRc_AsyncResult		0


#define	eAosRc_CertificateValid		0
#define eAosRc_CertificateInvalid	1
#define eAosRc_Error		       -1

/* callback return :
 *	eAosRc_CertificateValid: 	certificate is valid
 *	eAosRc_CertificateInvalid: 	certificate is invalid
 *	eAosRc_Error:			internal or socket error include timeout
 ****************************************************************/
typedef int (*SSL_REQUEST_CALLBACK)(void * certinfo, void *context, int rc);

/*int aos_cert_verify_init(void);*/

/* return:
 *      eAosRc_CertificateError: 	just for error;
 *	eAosRc_AsyncResult:		return result by callback
 ******************************************************************/
extern int aos_verify_cert(void *certinfo, void *context, SSL_REQUEST_CALLBACK callback);

#endif
