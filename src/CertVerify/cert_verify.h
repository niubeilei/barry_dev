////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cert_verify.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __CERT_VERIFY__
#define __CERT_VERIFY__

#include "PKCS/asn1.h"
#include "PKCS/x509.h"

#include "aostcpapi.h"

#include "CertVerify/aosCertVerify.h"


//typedef int (*OCSP_SOCKET_CALLBACK)(char * data, int len, int sock, void *userdata, int rc);



/*-------------------------------------------------------*/

typedef struct _x509_verify_reqest {
	struct list_head 	link;
	int 			result;

	X509_CERT_INFO		*certinfo;
	void			*context;
	SSL_REQUEST_CALLBACK	callback;
}	X509_VERIFY_REQUEST;


#define OCSP_CONN_NONE		0x00000000

/* ocsp flags */
#define OCSP_CONN_WAITTING_SOCK	0x00000001
#define OCSP_CONN_WAITTING_DATA	0x00000002
#define OCSP_CONN_WAITTING_MORE	0x00000004

/* socket answer flags */
#define OCSP_CONN_SOCK_SUCCESS	0x00000010
#define OCSP_CONN_DATA_RECV	0x00000020
#define OCSP_CONN_ERROR		0x00000040

#define OCSP_CONN_REQUEST	(OCSP_CONN_WAITTING_SOCK | OCSP_CONN_WAITTING_DATA | OCSP_CONN_WAIT_MORE)
#define OCSP_CONN_RESPONSE	(OCSP_CONN_SOCK_SUCCESS | OCSP_CONN_ERROR | OCSP_CONN_DATA_RECV)

typedef struct _x509_ocsp_conn {
	struct list_head	link;		/* link to list */

	int			flags;
	X509_VERIFY_REQUEST *	request;	/* request from ssl of lijin */

	/* if we use mult-ocsp server, register server here */
	unsigned int		addr;
	unsigned short		port;
	unsigned short		res;

	BYTE  *			data;		/* receive data */
	int			len;		/* data len */

	struct aos_tcpapi_data *sock;		/* connection */
}	X509_OCSP_CONN;


#define X509_OCSP_OPEN		0x00000001
#define X509_OCSP_ADDR_SET	0x00000002

#define X509_OCSP_ON		(X509_OCSP_OPEN | X509_OCSP_ADDR_SET)
#define X509_OCSP_CAN_CHECK	X509_OCSP_ON

#define X509_CRL_OPEN		0x00000010
#define X509_CRL_ADDR_SET	0x00000020
#define X509_CRL_URL_SET	0x00000040
#define X509_CRL_IMPORTED	0x00000080

#define X509_CRL_ON		(X509_CRL_OPEN | X509_CRL_ADDR_SET | X509_CRL_URL_SET)
#define X509_CRL_CAN_CHECK	(X509_CRL_OPEN | X509_CRL_IMPORTED)

#define X509_CRL_WAITTING_SOCK	0x00000001
#define X509_CRL_SOCK_CREATED	0x00000002
#define X509_CRL_WAITTING_DATA	0x00000004

#define X509_CRL_SOCK_CLOSED	0x00000010
#define X509_CRL_DATA_RECV	0x00000020
#define X509_CRL_SOCK_ERROR	0x00000040



#define CRL_HASH_BUCKETS	512
#define OCSP_HASH_BUCKETS	128

typedef struct x509_ocsp_entry {
	struct list_head	link_of_time;
	struct list_head	link_of_hash;

	asn_data_t		sn;
	int			status;
	time_t			resp_time;	/* CURRENT_TIME when we received */
}	X509_OCSP_ENTRY;


typedef struct _x509_verify_set {
	int		 flags;			/* global flags for ocsp check */

	/* currently we only set one ocsp server , it may be modified in future */
	UINT32		 ocsp_server_addr;
	WORD		 ocsp_server_port;
	WORD		 res;			/* not used */


	UINT32		 crl_server_addr;
	WORD		 crl_server_port;
	char *		 crl_url;
	int		 crl_flags;
	struct aos_tcpapi_data *crl_sock;
	BYTE *		 crl_data;
	int		 crl_data_len;
	UINT32		 crl_timeout;
	UINT32		 crl_next_update;

	rwlock_t	 crl_lock;
	X509_CRL_ENTRY * crl_hash_tbl[CRL_HASH_BUCKETS];	/* crl hash table */

	rwlock_t	 request_lock;
	struct list_head request_list;				/* any SSL certificate request is put into this list */

	rwlock_t	 response_lock;
	struct list_head response_list;			/* OCSP response list */
}	X509_VERIFY_SET;

#define clear_flag(flags, flag)  \
	do {\
		(flags) &= ~(flag); \
	} while(0)
#define set_flag(flags, flag)	\
	do { \
		(flags) |= (flag); \
	} while(0)
#define is_flag_set(flags, flag)	(((flags) & (flag)) == (flag))

/* config routine */
extern int aos_ocsp_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_ocsp_set_addr(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_status(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_set_addr(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_url_set(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_timeout_set(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_update(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_show_list(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_crl_flush(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_ocsp_save_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_ocsp_clear_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_show_ocsp_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int aos_show_crl_config(char *data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
#endif
