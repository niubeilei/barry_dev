////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSslProc.h
// Description:
//   Head of aosSslProc.c
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_AosSslProc_h
#define Aos_ssl_AosSslProc_h


#ifndef RSA_HEAD_H
#define RSA_HEAD_H

//RSA key
#define MIN_RSA_MODULUS_BITS 512
#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)
#define MAX_CAVIUM_CRT_LEN (5*MAX_RSA_MODULUS_LEN/2)

#endif //end !RSA_HEAD_H

//type define 
typedef unsigned long long uint64;

#include "KernelSimu/atomic.h"
#include "aosUtil/Types.h"
#include "ssl2/SslApi.h"
#include "ssl2/rsa.h"
#include "ssl2/rc4.h"
#include "PKCS/asn1.h"
#include "ssl2/cavium.h"

enum
{
	eAosSSL_MaxNumRecords = 10,
	eAosSSL_ClientHelloMaxLen = 100,
	eAosCipherNameLen = 40,
	eAosSsl_MaxClientSessions = 10,
};


typedef enum
{
	eAosContentType_NotSet = 0,
	eAosContentType_PlainData = 1,

	eAosContentType_ChangeCipherSpec = 20,
	eAosContentType_Alert			= 21,
	eAosContentType_Handshake		= 22,
	eAosContentType_AppData			= 23
} AosContentType;


// 
// Chen Ding, 09/14/2005
//
typedef struct AosSslRecord
{
	u8		record_types[eAosSSL_MaxNumRecords];	
	u8		num_records;
	u8		record_type;			// Message type
	u16		record_body_len;
	char *	record_body;
	u16		record_start;
	u16		record_data_len;
	u16		buffer_len;
	u8		record_ver_major;	
	u8		record_ver_minor;	
	u16		msg_start;
	u8		msg_type;
	u32		msg_body_len;
} AosSslRecord_t;


typedef enum
{
	eAosSSLMsg_HelloRequest = 0,
	eAosSSLMsg_ClientHello = 1,
	eAosSSLMsg_ServerHello = 2,
	eAosSSLMsg_Certificate = 11,
	eAosSSLMsg_ServerKeyExchange = 12,
	eAosSSLMsg_CertificateRequest = 13,
	eAosSSLMsg_ServerHelloDone = 14,
	eAosSSLMsg_CertificateVerify = 15,
	eAosSSLMsg_ClientKeyExchange = 16,
	eAosSSLMsg_Finished = 20,

	eAosSSLMsg_Unknown = 100
} AosSSL_MsgType;


enum
{
	eAosSSL_SessionIdLen = 32,
	eAosSSL_MaxCipherSuitesSupported = 10,
	eAosSSL_BuffSize = 16000,
	eAosSSL_AlertMaxLen = 261, //256+5

	eAosSSL_ServerHelloMinLen = 38,
	eAosSSL_ClientHelloMsgBodyMinLen = 38, // ClientVersion(2)+ClientRandom(32)+sessionid(1)+ciphersuit(2)+compression(1)
	eAosSSL_VersionMajor = 3,
	eAosSSL_VersionMinor = 0,
	eAosSSL_HandshakeMaxLen = 5000,
	eAosSSL_RecordMaxLen = 16500,
	eAosSSL_RecordHeaderLen = 5,
};


struct tcp_vs_conn;
struct AosSslRecord;
struct aosSslContext;
typedef int (*AosSSLStateFunc)(									
            struct AosSslRecord *record,					
            struct tcp_vs_conn *conn,
			struct aosSslContext *context,			
			SSL_PROC_CALLBACK callback, 
			u16 *errcode);


//RSA public key
typedef struct RSA_PUB_KEY
{
	unsigned int bits;//=1024
	unsigned char modulus[MAX_RSA_MODULUS_LEN];//N
	unsigned char publicExponent[MAX_RSA_MODULUS_LEN];//E
}RSA_PUBLIC_KEY, PUBLICKEY;


#define DBG 1

#define INTELX86

#define CIPHER_LENGTH 2
#define ENCRYPT_PRE_MASTER_SECRET_LENGTH 128
#define PRE_MASTER_SECRET_LENGTH 48
#define SEQ_NUMBER_LENGTH 32
#define RANDOM_LENGTH 32
#define KEY_BLOCK_MAX_LENGTH 120

#define MASTER_SECRET_LENGTH 48
#define FINISH_MESSAGE_LENGTH 40
#define HEAD_LENGTH 9
#define CERT_MESSAGE_MAX_LENGTH 2048
#define SERVER_HELLO_LENGTH 79
#define SERVER_DONE_LENGTH 9
#define CHANGE_CIPHER_SPEC_LENGTH 6
#define MD5_PAD_LENGTH 48
#define SHA1_PAD_LENGTH 40
#define PAD_1 0x36
#define PAD_2 0x5C
#define SENDER_LEN 4
#define MAX_VERIFY_DATA_SIZE 36
#define MAX_ENCRYPTED_VERIFY_DATA_SIZE 128

#ifndef AES_TYPE_SELECTED
#define AES_LENGTH 128/8
#endif

#if __KERNEL__
#else
#define OmnKernelAlloc(x) malloc(x)
#define OmnKernelFree(x)  free(x)
#define OmnKernelMemcpy(x, y, z) memcpy(x, y, z)
#define OmnKernelMemset(x, y, z) memset(x, y, z)
#define  _P(x...) printf(x)
#endif

typedef enum
{
	eAosSslRecordTypeIndex=0,
	eAosSslMajorVersionIndex=1,
	eAosSslMinorVersionIndex=2,
	eAosSslHandshakeTypeIndex=5,
	eAosSslNumberOfCipherSuitesIndex=44,
	eAosSslFirstCipherSuiteIndex=46
}SslHeadIndex;


typedef enum
{
	eAosSSL_ProcSslServer,
	eAosSSL_ProcSslClient
} AosSSLOperation_e;

typedef enum
{
	TLS_RSA_WITH_NULL_MD5=1,
	TLS_RSA_WITH_NULL_SHA=2,
	TLS_RSA_EXPORT_WITH_RC4_40_MD5=3,
	TLS_RSA_WITH_RC4_128_MD5=4,
	TLS_RSA_WITH_RC4_128_SHA=5,
	TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5=6,
	TLS_RSA_WITH_IDEA_CBC_SHA=7,
	TLS_RSA_EXPORT_WITH_DES40_CBC_SHA=8,
	TLS_RSA_WITH_DES_CBC_SHA=9,
	TLS_RSA_WITH_3DES_EDE_CBC_SHA=10,
	TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA=11,
	TLS_DH_DSS_WITH_DES_CBC_SHA=12,
	TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA=13,
	TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA=14,
	TLS_DH_RSA_WITH_DES_CBC_SHA=15,
	TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA=16,
	TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA=17,
	TLS_DHE_DSS_WITH_DES_CBC_SHA=18,
	TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA=19,
	TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA=20,
	TLS_DHE_RSA_WITH_DES_CBC_SHA=21,
	TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA=22,
	TLS_DH_anon_EXPORT_WITH_RC4_40_MD5=23,
	TLS_DH_anon_WITH_RC4_128_MD5=24,
	TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA=25,
	TLS_DH_anon_WITH_DES_CBC_SHA=26,
	TLS_DH_anon_WITH_3DES_EDE_CBC_SHA=27,
	TLS_RSA_EXPORT1024_WITH_RC4_56_MD5=96,
	TLS_RSA_EXPORT1024_WITH_RC2_CBC_56_MD5=97,
	TLS_RSA_EXPORT1024_WITH_DES_CBC_SHA=98,
	TLS_DHE_DSS_EXPORT1024_WITH_DES_CBC_SHA=99,
	TLS_RSA_EXPORT1024_WITH_RC4_56_SHA=100,
	TLS_DHE_DSS_WITH_RC2_56_CBC_SHA=101,
	TLS_DHE_DSS_WITH_RC4_128_SHA=102,
	TLS_DHE_DSS_WITH_NULL_SHA=103,
	SSL2_CK_RC4=65664,
	SSL2_CK_RC4_EXPORT40=131200,
	SSL2_CK_RC2=196736,
	SSL2_CK_RC2_EXPORT40=262272,
	SSL2_CK_IDEA=327808,
	SSL2_CK_DES=393280,
	SSL2_CK_RC464=524416,
	SSL2_CK_3DES=458944,
	TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA=74,
	TLS_ECDH_ECDSA_WITH_RC4_128_SHA=72,
	SSL_RSA_WITH_RC2_CBC_MD5=65408,
	SSL_RSA_WITH_RC4_128_MD5=4,
	TLS_ECDH_ECDSA_WITH_DES_CBC_SHA=73,
	TLS_ECDH_ECDSA_EXPORT_WITH_RC4_56_SHA=65413,
	TLS_ECDH_ECDSA_EXPORT_WITH_RC4_40_SHA=65412,
	// SSL_RSA_WITH_DES_CBC_SHA=9
	
	JNSTLS_RSA_WITH_NULL_MD5 = 0x81,
    JNSTLS_RSA_WITH_NULL_SHA = 0x82,
} CipherType;



typedef enum
{
	eAosSSLClient = 0x434C4E54,
	eAosSSLServer = 0x53525652
} AosSSLSender;

typedef enum
{
	eAosHashType_None = 0,
	eAosHashType_MD5 = 1,
	eAosHashType_SHA1 = 2,
	eAosHashType_MAX,
} AosHashType;

typedef enum
{
	AOSBT1=0,
	AOSBT2=1
} AosRsaBlockType;


typedef enum
{
	eAosAuthType_RSA = 0,
	eAosAuthType_DSS,
	eAosAuthType_None
} AuthType;


typedef enum
{
	eAosKeyExchange_Invalid,
	eAosKeyExchange_RSA, 
	eAosKeyExchange_RSAExport, 
	eAosKeyExchange_DH, 
	eAosKeyExchange_DHExport, 
	eAosKeyExchange_DHE, 
	eAosKeyExchange_DHEExport, 
	eAosKeyExchange_DH_DSS_Export, 
} AosKeyExchange;


typedef enum {
	rsa_sign=1,
	dss_sign=2, 
	rsa_fixed_dh=3, 
	dss_fixed_dh=4,
	rsa_ephemeral_dh=5, 
	dss_ephemeral_dh=6, 
	fortezza_kea=20
} ClientCertificateType;

typedef enum
{
	HELLO_REQUEST=0,
	FINISHED=20,
	CLIENT_HELLO=1,
	SERVER_HELLO=2,
	CERTIFICATE=11,
	SERVER_KEY_EXCHANGE=12,
	CERTIFICATE_REQUEST=13,
	SERVER_HELLO_DONE=14,
	CERTIFICATE_VERIFY=15,
	CLIENT_KEY_EXCHANGE=16
} HandshakeType;

typedef enum
{
	MAJOR_VERSION=3,
	MINOR_VERSION=0
} AosSslVersion;


typedef enum
{
	eAosSSLFlag_Client  	= 1,
	eAosSSLFlag_Server  	= 2,
	eAosSSLFlag_Front   	= 4,
	eAosSSLFlag_Backend 	= 8,
	eAosSSLFlag_ClientAuth 	= 0x10,
	eAosSSLFlag_Send 		= 0x20,
	eAosSSLFlag_Receive		= 0x40,
	eAosSSLFlag_Plain 		= 0x80,
	eAosSSLFlag_ExpectFinished = 0x100,
} AosSSLFlag;

typedef enum
{
	eAosRc_FinishedSuccess,
	eAosRc_ClientKeyExchangeSuccess,
	eAosRc_AppData,
	eAosRc_MsgErr,
	eAosRc_CompErr,
	eAosRc_CipherErr,
	eAosRc_SessionIDErr,
	eAosRc_VerErr,
	eAosRc_WrongMsg,
	eAosRc_WrongContext,
	eAosRc_RandomErr,
	eAosRc_RSADecryptErr,
	eAosRc_Error
	
}SslServerReturnCode;

typedef enum
{
	CONNECT,
	RESUME
} V3Status;

typedef enum
{
	eAosMaxCipher=10
} SslCipher;

typedef enum
{
	V2CLIENT_HELLO=0x00000001,
	CLINET_KEYEXCHANGE=0x00000002,
	CLIENT_FINISHED=0x00000004,
	CLIENT_CHANGE_CIPHER_SPEC=0x00000008,
	CLIENT_RESUME=0x00000010
} ReceiveStatus;

typedef enum
{
	ENCRYPT,
	DECRYPT
} APP_TYPE;

typedef enum
{
	CONF_OCSP=0x00000001,
	CONF_CLIENT_CERT_REQUEST=0X00000002
}ConfigParam;


typedef enum
{
    eAosEncryptType_Invalid = 0,
    eAosEncryptType_Null,
    eAosEncryptType_RC4_40,
    eAosEncryptType_RC4_128,
    eAosEncryptType_RC2_40_CBC,
    eAosEncryptType_IDEA_CBC,
    eAosEncryptType_DES40_CBC,
    eAosEncryptType_DES_CBC,
    eAosEncryptType_3DES_EDE_CBC,
    eAosEncryptType_Max,
} AosEncryptType;

typedef enum
{
	
	MD5_LEN 				= 16,
	SHA1_LEN 				= 20,
	MAX_HASH_LEN			= 20,
	
	RC440_KEY_LEN 			= 8,
	RC4128_KEY_LEN		= 16,
	DES64_KEY_LEN			= 8,
	TRIPLE_DES128_KEY_LEN	= 16,
	
	DES_BLOCK_LEN			= 8,
	TRIPLE_DES_BLOCK_LEN	= 16,

	RSA1024_MODULUS_LEN = 128,
} AosCipherLength;

typedef enum
{
	eAosSslAcceleratorType_Soft = 0,			// pure software ssl cipher algorithms
	eAosSslAcceleratorType_517PM = 1,		// jns ssl PUMI cipher accelerator card
	eAosSslAcceleratorType_CaviumGen = 2,	// cavium ssl general cipher algorithms accelerator card
	eAosSslAcceleratorType_CaviumPro = 4,	// cavium ssl protocol accelerator card
}AosSslAcceleratorType;

struct _X509_CERT_INFO;
struct AosCertChain;
struct aosSslContext
{
	uint8 			SeqNum[SEQ_NUMBER_LENGTH];
	u16 				CipherSelected;

	AosHashType 		hash_type;
	AuthType 		auth_type;
	AosKeyExchange	key_exchange_type;
   	AosEncryptType 	encrypt_type;
	RsaBlockType	 	RSA_block_type;
		
	struct rc4_ctx	client_cipher_ctx; 		
	struct rc4_ctx	server_cipher_ctx; 		

	u8				encrypt_key_len;
	u8				hash_len;
	u8				iv_len;

	// 
	// Chen Ding, 11/08/2005
	// Commented out with Li Qin's permission
	//
	// u8					pad_len;
	
	int 				AppType;
	int 				RecStatus;
	int 				ConfParam;
	uint64 			server_num;
	uint64 			client_num;
	uint8 			ClientRandom[RANDOM_LENGTH];
	uint8 			ServerRandom[RANDOM_LENGTH];
	uint8 			PreMasterSecret[PRE_MASTER_SECRET_LENGTH];
	uint8 			MasterSecret[MASTER_SECRET_LENGTH];
	int 				KeyBlockLength;
	uint8 			KeyBlock[KEY_BLOCK_MAX_LENGTH];
	RSA_PUBLIC_KEY 		PublicKey;
	AosRsaPrivateKey_t	*PrivateKey;
	uint16 				HandshakeLength;
	//uint8 				Handshake[eAosSSL_HandshakeMaxLen];
	uint8 				*Handshake;

	// Chen Ding, 09/09/2005
	int				state;
	int				isClient;
	int				isResumed;
	u16				ServerId;
	char	      	   		*bufferedData;
	u32				bufferedDatalen;
	uint8			SessionId[eAosSSL_SessionIdLen];
	u32				ServerCertLen;

	struct _X509_CERT_INFO		*PeerCert;	// The Server's certificate
	struct _X509_CERT_INFO	  	*self_cert;	// Its own certificate
	struct AosCertChain 		*authorities;		// authorites trusted by server
	asn_data_t 		PeerCert_SerialNumber;
	
	AosSSLStateFunc	stateFunc;
	u16				CipherSuiteDetermined;
	char	   		   	*ClientMacSecret;
	char	   		   	*ServerMacSecret;
	char	   		   	*ClientWriteKey;
	char	   		   	*ServerWriteKey;
	char	   		   	*ClientIV;
	char	   		   	*ServerIV;
	AosSslRecord_t 	record;
	AosSslRecord_t 	record_plain;	// recv plain application data, encrypt it then send out 
	u32				flags;			
	int				sock;

	struct tcp_vs_conn 	*conn;			// used in OCSP callback
	SSL_PROC_CALLBACK	callback; 		// used in OCSP callback
	atomic_t			refcnt;

	// special case
	// app want to send plain data, but ssl is not established
	// so buffer plain data in plain buffer, once ssl is established
	// send this data to peer
	uint8			*plain_buffer;	// app want to send plain data, but ssl is not established
	int				plain_len;

	// cavium special
	uint64			cavium_context;		// when using cavium 
	uint64			cavium_keyhandle; // used by cavium
	SslVersion		ssl_version;
	uint8			*EncryptClientFinishedMessage;
	uint8			*EncryptServerFinishedMessage;
	uint8 			*verify_data;
	//uint8
	
};

struct tcp_vs_conn;

#ifdef __KERNEL__
#include "Ktcpvs/tcp_vs_def.h"
#else
typedef struct tcp_vs_dest {
    __u32 addr;     /* IP address of real server */
    __u16 port;     /* port number of the service */
} tcp_vs_dest_t;

struct tcp_vs_dest;
struct tcp_vs_conn
{
	struct aosSslContext *front_context;
	struct aosSslContext *backend_context;
	uint32 front_ssl_flags;
	uint32 backend_ssl_flags;
    struct tcp_vs_dest      *dest;          // destination server
	int dsock;
	int csock;
	struct _X509_CERT_INFO	*front_cert;		// front ssl certificate 
	struct _X509_CERT_INFO	*backend_cert;		// backend ssl certificate 
	struct AosRsaPrivateKey	*front_prikey;		// fornt ssl PrivateKey;
	struct AosRsaPrivateKey	*backend_prikey;	// backend ssl PrivateKey;
	struct AosCertChain     *front_authorities;        // authorites trusted by server
	unsigned char			isSecureUser;

};
#endif

#endif //!AOS_SSL_HEAD_H

