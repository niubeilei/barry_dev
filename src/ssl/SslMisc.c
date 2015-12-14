////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslMisc.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include "asm/current.h"
#include "linux/sched.h"
#endif

#include "aos/aosKernelApi.h"
#include "aosUtil/Types.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"
#include "ssl/SslCommon.h"
#include "ssl/ReturnCode.h"
#include "ssl/random.h"
#include "PKCS/CertMgr.h"
#include "ssl/md5.h"
#include "ssl/sha1.h"
#include "ssl/SslMisc.h"
#include "ssl/rsa.h"
#include "ssl/des.h"
#include "ssl/Ssl.h"
#include "ssl/SslStatemachine.h"
#include "ssl/rc4.h"
#include "ssl/SslCipher.h"
#include "PKCS/x509.h"

#include "Porting/TimeOfDay.h"

extern u32 sgServerCiphersSupported[eAosMaxCipher];
extern  u8 sgNumServerCiphersSupported;

AosCipherNameMap_t aos_cipher_name_map[] =
{
    {"TLS_RSA_WITH_NULL_MD5", 1},
    {"TLS_RSA_WITH_NULL_SHA", 2},
    {"TLS_RSA_EXPORT_WITH_RC4_40_MD5", 3},
    {"TLS_RSA_WITH_RC4_128_MD5", 4},
    {"TLS_RSA_WITH_RC4_128_SHA", 5},
    {"TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5", 6},
    {"TLS_RSA_WITH_IDEA_CBC_SHA", 7},
    {"TLS_RSA_EXPORT_WITH_DES40_CBC_SHA", 8},
    {"TLS_RSA_WITH_DES_CBC_SHA", 9},
    {"TLS_RSA_WITH_3DES_EDE_CBC_SHA", 10},
    {"TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA", 11},
    {"TLS_DH_DSS_WITH_DES_CBC_SHA", 12},
    {"TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA", 13},
    {"TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA", 14},
    {"TLS_DH_RSA_WITH_DES_CBC_SHA", 15},
    {"TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA", 16},
    {"TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA", 17},
    {"TLS_DHE_DSS_WITH_DES_CBC_SHA", 18},
    {"TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA", 19},
    {"TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA", 20},
    {"TLS_DHE_RSA_WITH_DES_CBC_SHA", 21},
    {"TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA", 22},
    {"TLS_DH_anon_EXPORT_WITH_RC4_40_MD5", 23},
    {"TLS_DH_anon_WITH_RC4_128_MD5", 24},
    {"TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA", 25},
    {"TLS_DH_anon_WITH_DES_CBC_SHA", 26},
    {"TLS_DH_anon_WITH_3DES_EDE_CBC_SHA", 27},
    {"TLS_RSA_EXPORT1024_WITH_RC4_56_MD5", 96},
    {"TLS_RSA_EXPORT1024_WITH_RC2_CBC_56_MD5", 97},
    {"TLS_RSA_EXPORT1024_WITH_DES_CBC_SHA", 98},
    {"TLS_DHE_DSS_EXPORT1024_WITH_DES_CBC_SHA", 99},
    {"TLS_RSA_EXPORT1024_WITH_RC4_56_SHA", 100},
    {"TLS_DHE_DSS_WITH_RC2_56_CBC_SHA", 101},
    {"TLS_DHE_DSS_WITH_RC4_128_SHA", 102},
    {"TLS_DHE_DSS_WITH_NULL_SHA", 103},
    {"ssl_CK_RC4", 65664},
    {"ssl_CK_RC4_EXPORT40", 131200},
    {"ssl_CK_RC2", 196736},
    {"ssl_CK_RC2_EXPORT40", 262272},
    {"ssl_CK_IDEA", 327808},
    {"ssl_CK_DES", 393280},
    {"ssl_CK_RC464", 524416},
    {"ssl_CK_3DES", 458944},
    {"TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA", 74},
    {"TLS_ECDH_ECDSA_WITH_RC4_128_SHA", 72},
    {"SSL_RSA_WITH_RC2_CBC_MD5", 65408},
    {"SSL_RSA_WITH_RC4_128_MD5", 4},
    {"TLS_ECDH_ECDSA_WITH_DES_CBC_SHA", 73},
    {"TLS_ECDH_ECDSA_EXPORT_WITH_RC4_56_SHA", 65413},
    {"TLS_ECDH_ECDSA_EXPORT_WITH_RC4_40_SHA", 65412},
	{"JNSTLS_RSA_WITH_NULL_MD5", 0x81},
	{"JNSTLS_RSA_WITH_NULL_SHA", 0x82},
	{"", 0}
};

const static uint8 pad_char[8][9] = 
{
	"A",
	"BB",
	"CCC",
	"DDDD",
	"EEEEE",
	"FFFFFF",
	"GGGGGGG",
	"HHHHHHHH"
};

static unsigned char PAD1[MD5_PAD_LENGTH] = {
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36
};

static unsigned char PAD2[MD5_PAD_LENGTH] = {
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c
};

//
// Description:
//	This function calculate digest with sha1 arth.
// Arguments:
//  Input:
//	InputData:Pointer to the InputData.
//	InLen:Input data length.
//  Output:
//	OutputBuffer:Pointer to the digest.
// Return value:
//	 This function no return value.
//
void SHA_arth(uint8* InputData, int InLen, uint8* OutputBuffer)
{
	SHA(InputData, InLen, OutputBuffer);

	return;
}

//
// Description:
//	This function will convert little-endian type to big-endian type.
// Arguments:
//	Input:
//	 Input:Pointer to the little-endian type data
//	 Length:Input data length
//	Output:
//	 Output:Pointer to the output big-endian type
// Return value:
//	 This function no return value.
//
void LEndianToBEndian(uint8* Input, uint8* Output, uint32 Length)
{
#ifdef INTELX86
	unsigned int i;

	memset(Output, 0, Length);
	for(i=0;i<Length;i++)
	{
		Output[Length-i-1]=Input[i];
	}
#else
#endif
	return;
}

//
// Description:
//	This function calculate digest with md5 arth.
// Arguments:
//  Input:
//	InputData:Pointer to the InputData.
//	InLen:Input data length.
//  Output:
//	OutputBuffer:Pointer to the digest.
// Return value:
//	 This function no return value.
//
void MD5_arth(uint8* InputData, int InLen, uint8* OutputBuffer)
{
	struct MD5Context ctx;

	AosSsl_md5Init((void*)(&ctx));
	AosSsl_md5Update((void*)(&ctx), (uint8*)InputData, (uint32)InLen);
	AosSsl_md5Final((void*)(&ctx), (uint8*)OutputBuffer);

	return;
}

void des_cbc(int data_len, uint8 *input, uint8* output, uint8 key[8], uint8 iv[8], int encrypt)
{
	DES_CBC_CTX ctx;

	DES_CBCInit(&ctx, key, iv, encrypt);
	DES_CBCUpdate(&ctx, output, input, data_len);
	//DES_CBCRestart(ctx);

	return;
}

//
// Description:
//	This function check the ssl version.
// Arguments:
//  Input:
//	msg:Pointer to the Hello message.
// Return value:
//	eAosRc_Error, eAosRc_Success
//
int AosSsl_v3CheckVersion(char *msg, int *pos)
{
	*pos += 2;
	if(msg[0]!=MAJOR_VERSION ||msg[1]!=MINOR_VERSION)
	{
		return eAosRc_Error;
	}
	return eAosRc_Success;
}

//
// Description:
//	This function select compression arth with clientHello message.
//	This function do nothing. The compression method usually is null.
// Arguments:
//   In:
//	context:Pointer to the struct AosSslContext.
//	msg:Pointer to the compression suit in clienthello message.
//	msg_len: the length of the msg
// Return value:
//	eAosRc_Success
//
int AosSsl_selectCompression(struct AosSslContext *context, char *msg, int msg_len)
{
	// to be added
	return 2;
}

//
// Description:
//	This function select cipher suites with clientHello message.
//	If this function get some cipher suites matched with server supported,
//		store cipher suite to context.
//	Parse selected cipher suite, set hash type, auth arth, aes type to context.
// Arguments:
//  	In:
//	context:Pointer to the struct AosSslContext.
//	msg:Pointer to the cipher suite in clientHello message.
//	msg_len: the length of the msg
// Return value:
//	if return value >=0 , than success, the reaturn value is  the parsed msg length
//   else return value < 0, means error 
//
int AosSsl_checkAndSelectCipher(
		struct AosSslContext *context, 
		char *msg, 
		int msg_len)
{
	uint8 *cipher_spec;
	uint16 cipher_len = 0;
	uint16 temp16;
	int i, j;

	// CipherSuite cipher_suites<2..2^16-1>
	// uint8 CipherSuite[2];
	
	// Get and Check cipher suite length
	aos_assert1(msg_len >= 2);
	cipher_len = ((uint16)msg[0])<<8;
	cipher_len |= msg[1]&0x00ff;
	if ((cipher_len+2) > msg_len)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
			"ciphersuite length %d is larger than msg length %d", 
			cipher_len, msg_len - 2);
		return -eAosRc_CipherErr;
	}	

	cipher_spec = (uint8*)msg + 2;
	for (i = 0; i < cipher_len/2; i++)
	{
		for (j = 0; j < eAosMaxCipher; j++)
		{
			temp16 = ((uint16)cipher_spec[0])<<8;
			temp16 |= cipher_spec[1]&0x00ff;
			if( temp16 == (uint16)sgServerCiphersSupported[j] )
			{
				context->cipher_selected = 
					(uint16)sgServerCiphersSupported[j];
				AosSSL_parseCipher(context);
				return cipher_len+2;
			}
		}
		cipher_spec += 2;
	}
	
	return -eAosRc_CipherErr;
}


//
// Description:
//	This function select cipher suites with clientHello message.
//	If this function get some cipher suites matched with server supported,
//		store cipher suite to context.
//	Parse selected cipher suite, set hash type, auth arth, aes type to context.
// Arguments:
//  	In:
//	context:Pointer to the struct AosSslContext.
//	msg:Pointer to the cipher suite in clientHello message.
//	msg_len: the length of the msg
// Return value:
//	if return value >=0 , than success, the reaturn value is  the parsed msg length
//   else return value < 0, means error 
//
int AosSsl_checkAndSelectCipherV2(
	struct AosSslContext *context, 
	char *cipher_spec, 
	int cipher_len)
{
	uint16 temp16;
	int i, j;

	// CipherSuite cipher_suites<2..2^16-1>
	// uint8 CipherSuite[3];
	
	for (i = 0; i < cipher_len/3; i++)
	{
		if ( cipher_spec[0] )
		{
			aos_trace( "sslv2 cipher 0x%02x%02x%02x", cipher_spec[0], cipher_spec[1], cipher_spec[2] );
			cipher_spec += 3;
			continue;
		}
		for (j = 0; j < eAosMaxCipher; j++)
		{
			temp16 = ((uint16)cipher_spec[1])<<8;
			temp16 |= cipher_spec[2]&0x00ff;
			if( temp16 == (uint16)sgServerCiphersSupported[j] )
			{
				context->cipher_selected = 
					(uint16)sgServerCiphersSupported[j];
				AosSSL_parseCipher(context);
				return cipher_len+3;
			}
		}
		cipher_spec += 3;
	}
	
	return -eAosRc_CipherErr;
}

//
// Description:
//	This function check the sessionid in client hello message
//	If it is not zero, it stands for cliet want to resume this session
// Arguments:
//  Input:
//  context: ssl context, seesionid will be stored in context
//	msg:Pointer to the sessionid parts in clientHello message.
//  msg_len: the length of msg
// Return value:
//	>1: resumed
//  =1: new session, do not resumed
//  <1: error
//
int AosSsl_checkSessionResume(struct AosSslContext *context, char *msg, int msg_len)
{
	int sessionid_len;
	sessionid_len = msg[0];
	aos_assert1(sessionid_len <= (msg_len - 1));
	memcpy(context->session_id, msg+1, sessionid_len);
	return sessionid_len+1;
}

//
// Description:
//	This function parse the cipher code and store cipher type to context.
// Arguments:
//	Input:
//	context:Pointer to the AosSslContext structure.
// Return value:
//	 This function no return value.
//
int AosSSL_parseCipher(struct AosSslContext *context)
{
	int ret = 0;

	switch(context->cipher_selected)
	{
	case TLS_RSA_WITH_NULL_MD5:
	case JNSTLS_RSA_WITH_NULL_MD5:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_Null;
		 context->hash_type 		= eAosHashType_MD5;
		 context->encrypt_key_len 	= 0;
	 	 context->hash_len 			= MD5_LEN;
 	 	 context->iv_len 			= 0;
		 break;

	case TLS_RSA_WITH_NULL_SHA:
	case JNSTLS_RSA_WITH_NULL_SHA:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_Null;
		 context->hash_type 		= eAosHashType_SHA1;
 		 context->encrypt_key_len 	= 0;
 	 	 context->hash_len 			= SHA1_LEN;
  	 	 context->iv_len 			= 0;
		 break;

	case TLS_RSA_WITH_DES_CBC_SHA:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_DES_CBC;
		 context->hash_type			= eAosHashType_SHA1;
 		 context->encrypt_key_len 	= DES64_KEY_LEN;
  	 	 context->hash_len 			= SHA1_LEN;
 	 	 context->iv_len 			= DES_BLOCK_LEN;
		 break;

	case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_3DES_EDE_CBC;
		 context->hash_type			= eAosHashType_SHA1;
  		 context->encrypt_key_len 	= TRIPLE_DES128_KEY_LEN;
  	 	 context->hash_len 			= SHA1_LEN;
  	 	 context->iv_len 			= TRIPLE_DES_BLOCK_LEN;
		 break;

	case TLS_RSA_WITH_RC4_128_MD5:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_RC4_128;
		 context->hash_type 		= eAosHashType_MD5;
  		 context->encrypt_key_len 	= RC4128_KEY_LEN;
 	 	 context->hash_len 			= MD5_LEN;
  	 	 context->iv_len 			= 0;
		 break;

	case TLS_RSA_WITH_RC4_128_SHA:
		 context->auth_type 		= eAosAuthType_RSA;
		 context->key_exchange_type = eAosKeyExchange_RSA;
		 context->encrypt_type 		= eAosEncryptType_RC4_128;
		 context->hash_type 		= eAosHashType_SHA1;
   		 context->encrypt_key_len 	= RC4128_KEY_LEN;
 	 	 context->hash_len 			= SHA1_LEN;
   	 	 context->iv_len 			= 0;
		 break;

	default:
		 ret = -eAosAlarm_CipherSuiteNotSupported; 
		 break;
	}
	
	return ret;
}


// 
// It creates the Finished message. It assumes master_secret has been 
// calculated. 
//
int AosSSL_createFinishedMsg(
				u32 hashLen,
				struct AosSslContext *context, 
				char *finished_msg,
				u32 *length,
				u32 sender)
{
	char *mac;
	char message_hash[FINISH_MESSAGE_LENGTH];
	u32 pad_length;
	u64 *serialNum;
	char *key, *iv, *mac_secret;

	if (sender == eAosSSLClient)
	{
		serialNum = &context->client_num;
		key = context->client_write_key;
		iv = context->client_iv;
		mac_secret = context->client_mac_secret;
	}
	else if (sender == eAosSSLServer)
	{
		serialNum = &context->server_num;
		key = context->server_write_key;
		iv = context->server_iv;
		mac_secret = context->server_mac_secret;
	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Invalid sender: %d", sender);
	}

	// 
	// Create the hash of the accumulated messages
	//
    	AosSsl_constructFinishedMessage(
    		(uint8*)context->handshake,
       	context->handshake_len, 
       	(uint8*)message_hash,
       	context->master_secret, 
       	(AosSSLSender)sender);

	memcpy(&context->handshake[context->handshake_len], message_hash, FINISH_MESSAGE_LENGTH);
	context->handshake_len += FINISH_MESSAGE_LENGTH;

	//
	// Create the MAC of the message hash
	//
    	mac = (char *)aos_malloc_atomic(hashLen);
	aos_assert1(mac);

    AosSsl_calculateMAC(
		(uint8*)message_hash, 
		FINISH_MESSAGE_LENGTH,
        (uint8*)mac, 
        context->hash_type,
        (uint8*)mac_secret, 
        (*serialNum)++,
        eAosContentType_Handshake);
	if (context->encrypt_type == eAosEncryptType_RC4_128 ||
		context->encrypt_type == eAosEncryptType_Null)
	{
		pad_length = 0;
	}
	else
	{	
    	pad_length = context->encrypt_key_len - (FINISH_MESSAGE_LENGTH+hashLen)%context->encrypt_key_len -1;
  	}
	AosSsl_constructFinishedBlock(
		(uint8*)mac, 
		hashLen,
       	(uint8*)message_hash, 
       	pad_length, 
       	(uint8*)finished_msg, 
		(int*)length,
       	key, 
       	iv, 
       	context);
		
	/*if (pad_length)
		*length = FINISH_MESSAGE_LENGTH+hashLen+pad_length+1;
	else
		*length = FINISH_MESSAGE_LENGTH+hashLen;
	*/
	if (context->iv_len)
	{
		memcpy(iv, finished_msg+*length-context->iv_len, context->iv_len);
	}
	
	aos_free(mac);
	return 0;
}

//
// Description:
//	This function parse the cipher code and store cipher type to context.
// Arguments:
//	Input:
//	context:Pointer to the AosSslContext structure.
//	sender: server or client
//	msg: the whole encrypted finish message, include mac and padding
//    msg_len: the length of msg
// Return value:
//	0: successful
//	<0: error
//
int AosSSL_verifyFinished(
	struct AosSslContext *context, 
	AosSSLSender sender,
	uint8* msg,
	int msg_len)
{
	uint8 msg_plain[64], *key = NULL, *iv = NULL;
	u64 *serialNum = NULL;
	uint8  mac[MAX_HASH_LEN], *mac_secret = NULL;
	uint8  msg_plain_expected[FINISH_MESSAGE_LENGTH];
	struct rc4_ctx *ctx = NULL;
	int msg_plain_len;
	
	//
	// Decrypt finished message
	//
	if (sender == eAosSSLClient)
	{
		serialNum = &context->client_num;
		key = (uint8*)context->client_write_key;
		iv  = (uint8*)context->client_iv;
		mac_secret = (uint8*)context->client_mac_secret;
		ctx = &context->client_cipher_ctx;
	}
	else if (sender == eAosSSLServer)
	{
		serialNum = &context->server_num;
		key = (uint8*)context->server_write_key;
		iv  = (uint8*)context->server_iv;
		mac_secret = (uint8*)context->server_mac_secret;
		ctx = &context->server_cipher_ctx;
	}

	if (context->encrypt_type == eAosEncryptType_RC4_128 ||
		context->encrypt_type == eAosEncryptType_Null)
	{
		//aos_assert1(msg_len == 60); 
		aos_ssl_cipher[context->encrypt_type].setkey((void*)ctx, key, context->encrypt_key_len);
		aos_ssl_cipher[context->encrypt_type].decrypt(
			(void*)ctx,
			msg,
			msg_len,
			msg_plain,
			&msg_plain_len,
			(uint8*)key,
			context->encrypt_key_len);
	}
	else if (context->encrypt_type == eAosEncryptType_DES_CBC) 
	{
		aos_assert1(msg_len == 64); 
		aos_ssl_cipher[context->encrypt_type].decrypt(
			(void*)iv,
			msg,
			msg_len,
			msg_plain,
			&msg_plain_len,
			(uint8*)key,
			context->encrypt_key_len);
	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, 
				"encrypt_type not supported: %d", context->encrypt_type);
	}
	
	// construct finished  message and do comparation with received 
	AosSsl_constructFinishedMessage(
		(uint8*)context->handshake,
		context->handshake_len, 
		msg_plain_expected,
       	context->master_secret, 
       	sender);
	if ( memcmp(msg_plain, msg_plain_expected, FINISH_MESSAGE_LENGTH) != 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
				"%s finished message verify failed", sender==eAosSSLServer? "server":"client");
	}

	//
	// Create the MAC of the message hash
	//
    AosSsl_calculateMAC(
		msg_plain_expected, 
		FINISH_MESSAGE_LENGTH,
        	mac, 
        	context->hash_type,
		mac_secret,
		(*serialNum)++,
        	eAosContentType_Handshake);

	// mac comparation
	if ( memcmp(msg_plain+FINISH_MESSAGE_LENGTH, mac, context->hash_len) != 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
				"%s finished message verify failed, bad mac", sender==eAosSSLServer? "server":"client");
	}

	// store the received plain finished message into context
	memcpy(&context->handshake[context->handshake_len], msg_plain_expected, FINISH_MESSAGE_LENGTH);
	context->handshake_len += FINISH_MESSAGE_LENGTH;
	
	if (context->iv_len)
	{
		// the last cipher block is the next iv, so we should store to into context
		memcpy(iv, msg+msg_len-context->iv_len, context->iv_len);
	}	
	return 0;
}

//
// Description:
//	This function decode the received der format certificate, retrived pub key and der cert
// Arguments:
//	Input:
//	msg: the certificate handshake msg body
//   msg_len: the length of the msg
//   Output:
//	cert: the decoded information about received certificate
// Return value:
//	0: successful
//	<0: failed
//
int AosSsl_decodeCertMsg(uint8 *msg, int msg_len, struct _X509_CERT_INFO **cert)
{	
	uint32 total_len, cert_len;	
	int guard, first, consumed;	
	int pos = 0, ret; 	
	
	aos_trace("AosSSL_DecodeCertMsg entry");	
	aos_assert1(cert);	
	aos_assert1(msg_len >= 3);	
	//
	// msg[0..2]		The total length of all certificates (3 bytes)	
	// 	
	// For each certificate, it is:	
	// msg[pos..+2]	The length of the certificate ( 3 bytes)	
	// msg[...]		The certificate	
	//
	total_len = ((uint32)msg[pos] << 16) + ((uint32)msg[pos+1] << 8) + msg[pos+2];	
	pos += 3;
	aos_assert1((int)total_len == msg_len - 3);
	guard = 0;	
	first = 1;	
	consumed = 3;	
	while (guard++ < 5)	
	{		
		cert_len = ((uint32)msg[pos] << 16) + ((uint32)msg[pos+1] << 8) + msg[pos+2];		
		pos += 3;
		consumed += cert_len + 3;		
		if (consumed > msg_len)		
		{			
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
					"Buffer out of bound: %d, %d", consumed, msg_len);		
		}		
		if (first)		
		{			
			if ((ret = AosCertMgr_decode((char*)msg+pos, cert_len, cert)))
			{				
				//
				// Should not happen
				//				
				return aos_alarm(eAosMD_SSL, eAosAlarm_VerifyCertFailed,
							 		"Failed to verify certificate: %d", ret);
			}			
			first = 0;
		}		
		pos += cert_len;
		if (consumed >= msg_len)
		{			
			break;		
		}	
	}	
	
	return eAosRc_Success;
}

//
// Description:
//	This function generate client CertificateVerify v3 message body
//	This message include md5_hash[16] and sha_hash[20]
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//    cert_veri_len: the length of cert_veri output buffer
//  Output:
//   cert_veri: the output buffer of CertificateVerify message body
// Return value:
//	>=0: Success
//    <0: error returned
//
int AosSsl_createCltCertVeriMsg(
			struct AosSslContext *context, 
			uint8 *cert_veri, 
			int cert_veri_len)
{
	struct MD5Context ctx;
	uint8 pad1[MD5_PAD_LENGTH];
	uint8 pad2[MD5_PAD_LENGTH];
	uint8 md5_out[MD5_LEN];
	uint8 *sha_buffer;
	uint8 sha_out[SHA1_LEN];
	uint8 *ptr;

	aos_assert1(cert_veri_len == MD5_LEN + SHA1_LEN);
	
	// 1.md5_hash[16] = MD5(master_secret+pad2+MD5(handshake_messages+master_secret+pad1));
	// 2.sha_hash[20] = SHA-1(master_secret+pad2+SHA-1(handshake_messages+master_secret+pad1));

	// md5_hash[16] = MD5(master_secret+pad2+MD5(handshake_messages+master_secret+pad1));	
	AosSsl_md5Init(&ctx);
	AosSsl_md5Update(&ctx, context->handshake, context->handshake_len);
	AosSsl_md5Update(&ctx, context->master_secret, MASTER_SECRET_LENGTH);
	memset(pad1, PAD_1, MD5_PAD_LENGTH);
	AosSsl_md5Update(&ctx, pad1, MD5_PAD_LENGTH);
	AosSsl_md5Final(&ctx, md5_out);

	AosSsl_md5Init(&ctx);
	AosSsl_md5Update(&ctx, context->master_secret, MASTER_SECRET_LENGTH);
	memset(pad2, PAD_2, MD5_PAD_LENGTH);
	AosSsl_md5Update(&ctx, pad2, MD5_PAD_LENGTH);
	AosSsl_md5Update(&ctx, md5_out, MD5_LEN);
	AosSsl_md5Final(&ctx, md5_out);

	memcpy(cert_veri, md5_out, MD5_LEN);
	
	// 2.sha_hash[20] = SHA-1(master_secret+pad2+SHA-1(handshake_messages+master_secret+pad1));
	sha_buffer = (uint8*)aos_malloc_atomic(context->handshake_len+MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH);
	if (!sha_buffer)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Create CertificateVerify failed, failed to alloc sha buffer");
	}	
	ptr = sha_buffer;
	memcpy(ptr, context->handshake, context->handshake_len);
	ptr += context->handshake_len;
	memcpy(ptr, context->master_secret, MASTER_SECRET_LENGTH);
	ptr += MASTER_SECRET_LENGTH;
	memset(ptr, PAD_1, SHA1_PAD_LENGTH);
	ptr += SHA1_PAD_LENGTH;
	SHA(sha_buffer, ptr-sha_buffer, sha_out);
	aos_free(sha_buffer);

	sha_buffer = (uint8*)aos_malloc_atomic(MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH+SHA1_LEN);
	if (!sha_buffer)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Create CertificateVerify failed, failed to alloc sha buffer");
	}
	ptr = sha_buffer;
	memcpy(ptr, context->master_secret, MASTER_SECRET_LENGTH);
	ptr += MASTER_SECRET_LENGTH;
	memset(ptr, PAD_2, SHA1_PAD_LENGTH);
	ptr += SHA1_PAD_LENGTH;
	memcpy(ptr, sha_out, SHA1_LEN);
	ptr += SHA1_LEN;
	SHA(sha_buffer, ptr-sha_buffer, sha_out);
	aos_free(sha_buffer);

	memcpy(cert_veri+MD5_LEN, sha_out, SHA1_LEN);
	
	return eAosRc_Success;
}

//
// Description:
//  This function decrypt application data and verify the mac
// Arguments:
//  Input:
//  context:Point to the struct AosSslContext.
//    inout: the encrypted application data
//    inout_len: the length of the application data
//  Output:
//   inout: the decrypted application data, do not include mac
//   inout_len: return the length of decrypted app data
// Return value:
//  >=0: Success
//    <0: error returned
//
int AosSsl_DecAppData(
    struct AosSslContext *context,
    uint8 *inout,
    int *inout_len)
{
    int appdata_len;
    int pad_len;
    uint8 mac_expected[SHA1_LEN];
    
    //aos_trace("AosSsl_DecAndVeriAppData entry");
   // Decryption
    if (context->encrypt_type == eAosEncryptType_DES_CBC)
	{
        aos_assert1(*inout_len%8 == 0);

        aos_ssl_cipher[context->encrypt_type].decrypt(
            (void*)(context->is_client?context->server_iv:context->client_iv),
            inout,
            *inout_len,
            inout,
            &appdata_len,
            (uint8*)(context->is_client?context->server_write_key:context->client_write_key),
            context->encrypt_key_len);

        pad_len = inout[*inout_len-1];
        aos_assert1(pad_len <= 7);
        aos_assert1(appdata_len>(context->hash_len+pad_len+1));
        appdata_len = appdata_len - context->hash_len - pad_len - 1 ;
    }
    else if (context->encrypt_type == eAosEncryptType_RC4_128 ||
             context->encrypt_type == eAosEncryptType_Null)
    {
        //aos_trace("ssl dec before %u", AosGetUsec());
        aos_ssl_cipher[context->encrypt_type].decrypt(
            (void*)(context->is_client?&context->server_cipher_ctx:&context->client_cipher_ctx),
            inout,
            *inout_len,
            inout,
            &appdata_len,
            (uint8*)(context->is_client?context->server_write_key:context->client_write_key),
            context->encrypt_key_len);

        aos_assert1(appdata_len>(context->hash_len));
        appdata_len = appdata_len - context->hash_len;
        //aos_trace("ssl dec after %u", AosGetUsec());
    }
    else
    {
        return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"unsupported encryption type %d", context->encrypt_type);
    }

    //aos_trace("mac before %u", AosGetUsec());
    // Mac check
    AosSsl_calculateMAC(
        inout,
        appdata_len,
        mac_expected,
        context->hash_type,
        (uint8*)(context->is_client?context->server_mac_secret:context->client_mac_secret),
        context->is_client?context->server_num:context->client_num,
        eAosContentType_AppData);
    //aos_trace("mac end %u", AosGetUsec());
    if  (memcmp(inout+appdata_len, mac_expected, context->hash_len) != 0 )
    {
        return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "application data mac check failed");
    }

    // Increase the sequence number
    context->is_client?context->server_num++:context->client_num++;

    if (context->iv_len)
    {
        memcpy(
            context->is_client?context->server_iv:context->client_iv,
            inout+*inout_len-context->iv_len,
            context->iv_len);
    }

    *inout_len = appdata_len;
    return eAosRc_Success;
}

//
// Description:
//	This function decrypt application data and verify the mac
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//    enc: the encrypted application data
//	enc_len: the length of the application data
//    plain_len: the length of the plain buffer, should not little than encrypted_len
//    server: which side call this func, 1 is server and 0 is client
//  Output:
//   plain: the decrypted application data, do not include mac
//   plain_len: return the length of decrypted app data
// Return value:
//	>=0: Success
//    <0: error returned
//
int AosSsl_DecAppData1(
	struct AosSslContext *context, 
	uint8 *enc,
	int enc_len, 
	uint8 *plain, 
	int *plain_len)
{
	int appdata_len;
	int pad_len;
	uint8 mac_expected[SHA1_LEN];
	
	//aos_trace("AosSsl_DecAndVeriAppData entry");

	// Decryption
	if (context->encrypt_type == eAosEncryptType_DES_CBC)
	{
		aos_assert1(enc_len%8 == 0);
		
		aos_ssl_cipher[context->encrypt_type].decrypt(
			(void*)(context->is_client?context->server_iv:context->client_iv),
			enc,
			enc_len,
			plain,
			&appdata_len,
			(uint8*)(context->is_client?context->server_write_key:context->client_write_key), 
			context->encrypt_key_len);
		
		pad_len = plain[enc_len-1];
		aos_assert1(pad_len <= 7); 
		aos_assert1(appdata_len>(context->hash_len+pad_len+1));
		appdata_len = appdata_len - context->hash_len - pad_len - 1 ; 
	}
	else if (context->encrypt_type == eAosEncryptType_RC4_128 ||
			 context->encrypt_type == eAosEncryptType_Null)
	{
		//aos_trace("ssl dec before %u", AosGetUsec());
		aos_ssl_cipher[context->encrypt_type].decrypt(
			(void*)(context->is_client?&context->server_cipher_ctx:&context->client_cipher_ctx),
			enc,
			enc_len,
			plain,
			&appdata_len,
			(uint8*)(context->is_client?context->server_write_key:context->client_write_key), 
			context->encrypt_key_len);

		aos_assert1(appdata_len>(context->hash_len));
		appdata_len = appdata_len - context->hash_len; 
		//aos_trace("ssl dec after %u", AosGetUsec());
	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
					"unsupported encryption type %d", context->encrypt_type);
	}
	
	//aos_trace("mac before %u", AosGetUsec());
	// Mac check
	AosSsl_calculateMAC(
		plain, 
		appdata_len,
		mac_expected, 
		context->hash_type,
		(uint8*)(context->is_client?context->server_mac_secret:context->client_mac_secret),
		context->is_client?context->server_num:context->client_num,
		eAosContentType_AppData);		
	//aos_trace("mac end %u", AosGetUsec());
	if  (memcmp(plain+appdata_len, mac_expected, context->hash_len) != 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "application data mac check failed");
	}

	// Increase the sequence number
	context->is_client?context->server_num++:context->client_num++;
	*plain_len = appdata_len;

	if (context->iv_len)
	{
		memcpy(
			context->is_client?context->server_iv:context->client_iv, 
			enc+enc_len-context->iv_len, 
			context->iv_len);
	}
	
	return eAosRc_Success;
}

//
// Description:
//  This function decrypt application data and verify the mac
// Arguments:
//  Input:
//  context:Point to the struct AosSslContext.
//   inout: the plain application data, the inout buffer size shoud be large than inout_len+hash_len
//   inout_len:  the length of app data
//   msg_type: the type of the msssage, it may be application or alert type
//    server: which side call this func, 1 is server and 0 is client
//  Output:
//   inout: the encrypted application data, include mac and padding
//   inout_len: return the length of the encrypted application data
// Return value:
//  >=0: Success
//    <0: error returned
//
int AosSsl_EncAppData(
    struct AosSslContext *context,
    uint8 *inout,
    int *inout_len,
    AosContentType msg_type)
{
	int pad_len;
	uint8 mac[SHA1_LEN];
	int pos;

	//aos_trace("AosSsl_EncAppData entry");

	AosSsl_calculateMAC(
	    inout,
	    *inout_len,
	    mac,
	    context->hash_type,
	    (uint8*)(context->is_client?context->client_mac_secret:context->server_mac_secret),
	    context->is_client?context->client_num:context->server_num,
	    msg_type);//eAosContentType_AppData or eAosContentType_Alert;       

	pos = *inout_len;
	memcpy(inout+pos, mac, context->hash_len);
	pos += context->hash_len;

	if (context->encrypt_type == eAosEncryptType_DES_CBC)
	{
	    pad_len = (context->encrypt_key_len - ((pos+1)&(context->encrypt_key_len-1)))&(context->encrypt_key_len-1);
	    memset(inout+pos, pad_len, pad_len+1);
	    pos +=pad_len+1;

	    *inout_len = pos;
	    aos_ssl_cipher[context->encrypt_type].encrypt(
	        (void*)(context->is_client?context->client_iv:context->server_iv),
	        inout,
	        *inout_len,
	        inout,
	        inout_len,
	        (uint8*)(context->is_client?context->client_write_key:context->server_write_key),
	        context->encrypt_key_len);
	}
	else if (context->encrypt_type == eAosEncryptType_RC4_128 ||
	         context->encrypt_type == eAosEncryptType_Null)
	{
	    *inout_len = pos;

	    aos_ssl_cipher[context->encrypt_type].encrypt(
	        (void*)(context->is_client?&context->client_cipher_ctx:&context->server_cipher_ctx),
	        inout, 
	        *inout_len,
	        inout,
	        inout_len,
	        (uint8*)(context->is_client?context->client_write_key:context->server_write_key),
	        context->encrypt_key_len);

	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
	        "Encryption algorith not supported: %d", context->encrypt_type);
	}

	context->is_client?context->client_num++:context->server_num++;

	// the last cipher block is the next iv, so we should store to into context
	if (context->iv_len)
	{
	    memcpy(
	        (uint8*)(context->is_client?context->client_iv:context->server_iv),
	        inout+*inout_len-context->iv_len,
	        context->iv_len);
	}

	return eAosRc_Success;
}

//
// Description:
//	This function decrypt application data and verify the mac
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//   plain: the plain application data
//   plain_len:  the length of app data
//   msg_type: the type of the msssage, it may be application or alert type
//	enc_len: the length of the encryption data buffer
//    server: which side call this func, 1 is server and 0 is client
//  Output:
//    enc: the encrypted application data, include mac and padding
//	enc_len: return the length of the encrypted application data
// Return value:
//	>=0: Success
//    <0: error returned
//
int AosSsl_EncAppData1(
	struct AosSslContext *context, 
	uint8 *plain,
	int plain_len, 
	AosContentType msg_type,
	uint8 *enc, 
	int *enc_len)
{
	int pad_len;
	uint8 mac[SHA1_LEN];
	int pos;
	//uint8 *plain_input = NULL;

	//aos_trace("AosSsl_EnciAppData entry");

	AosSsl_calculateMAC(
		plain, 
		plain_len,
		mac, 
		context->hash_type,
		(uint8*)(context->is_client?context->client_mac_secret:context->server_mac_secret),
		context->is_client?context->client_num:context->server_num,
		msg_type);//eAosContentType_AppData or eAosContentType_Alert;		

	//plain_input = (uint8*) aos_malloc_atomic(*enc_len);
	//aos_assert1(plain_input);

	aos_assert1(*enc_len >=(plain_len+context->hash_len));
	memcpy(enc, plain, plain_len);
	//memcpy(plain_input, plain, plain_len);
	pos = plain_len;
	memcpy(enc+pos, mac, context->hash_len);
	//memcpy(plain_input+pos, mac, context->hash_len);
	pos += context->hash_len;
	
	if (context->encrypt_type == eAosEncryptType_DES_CBC)
	{
		pad_len = (context->encrypt_key_len - ((pos+1)&(context->encrypt_key_len-1)))&(context->encrypt_key_len-1);
		memset(enc+pos, pad_len, pad_len+1);
		//memset(plain_input+pos, pad_len, pad_len+1);
		pos +=pad_len+1;
		
		aos_assert1(*enc_len >= pos);
		*enc_len = pos;
		aos_ssl_cipher[context->encrypt_type].encrypt(
			(void*)(context->is_client?context->client_iv:context->server_iv),
			enc,//plain_input
			*enc_len, 
			enc,
			enc_len,
			(uint8*)(context->is_client?context->client_write_key:context->server_write_key), 
			context->encrypt_key_len);
	}
	else if (context->encrypt_type == eAosEncryptType_RC4_128 ||
			 context->encrypt_type == eAosEncryptType_Null)
	{
		aos_assert1(*enc_len >= pos);
		*enc_len = pos;
			
		aos_ssl_cipher[context->encrypt_type].encrypt(
			(void*)(context->is_client?&context->client_cipher_ctx:&context->server_cipher_ctx),
			enc,//plain_input, 
			*enc_len, 
			enc,
			enc_len,
			(uint8*)(context->is_client?context->client_write_key:context->server_write_key), 
			context->encrypt_key_len);
		
	}
	else
	{
		//aos_free(plain_input);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Encryption algorith not supported: %d", context->encrypt_type);
	}
	//aos_free(plain_input);

	context->is_client?context->client_num++:context->server_num++;
	
	// the last cipher block is the next iv, so we should store to into context
	if (context->iv_len)
	{
		memcpy(
			(uint8*)(context->is_client?context->client_iv:context->server_iv), 
			enc+*enc_len-context->iv_len, 
			context->iv_len);
	}

	return eAosRc_Success;
}

//
// Description:
//	This function calculate the master secret with pre master secret.
// Arguments:
//  Input:
//	pre_master_secret:Pointer to the pre master secret.
//	pre_master_secretLength:The pre master secret buffer length.
//	client_random:Pointer to the client random.
//	server_random:Pointer to the server random.
//  Output:
//	master_secret:Pointer to the master secret.
// Return value:
//	 This function no return value.
//
void AosSsl_getmaster_secret(uint8* pre_master_secret,
					uint32 pre_master_secretLength,
					uint8* master_secret,
					uint8* client_random,
					uint8* server_random)
{
	uint8* T_Buffer[5];
	int i;

	T_Buffer[0]=(uint8*)aos_malloc_atomic(MD5_LEN*3);
	T_Buffer[1]=(uint8*)aos_malloc_atomic(MD5_LEN);
	T_Buffer[2]=(uint8*)aos_malloc_atomic(SHA1_LEN);
	T_Buffer[3]=(uint8*)aos_malloc_atomic(pre_master_secretLength+SHA1_LEN);
	for(i=0;i<3;i++)
	{
		T_Buffer[4]=(uint8*)aos_malloc_atomic(i+1+pre_master_secretLength+RANDOM_LENGTH*2);
		memcpy(T_Buffer[4], pad_char[i], i+1);
		memcpy(T_Buffer[4]+i+1, pre_master_secret, pre_master_secretLength);
		memcpy(T_Buffer[4]+i+1+pre_master_secretLength, client_random, RANDOM_LENGTH);
		memcpy(T_Buffer[4]+i+1+pre_master_secretLength+RANDOM_LENGTH, server_random, RANDOM_LENGTH);
		//PrintData(T_Buffer[4], i+1+pre_master_secretLength+RANDOM_LENGTH*2, "T_BUFF[4]");
		SHA_arth(T_Buffer[4], i+1+pre_master_secretLength+RANDOM_LENGTH*2, T_Buffer[2]);
//		PrintData(T_Buffer[2], SHA1_LEN, "SHA OUT[20]");
		memcpy(T_Buffer[3], pre_master_secret, pre_master_secretLength);
		memcpy(T_Buffer[3]+pre_master_secretLength, T_Buffer[2], SHA1_LEN);
		MD5_arth(T_Buffer[3], pre_master_secretLength+SHA1_LEN, T_Buffer[1]);
//		PrintData(T_Buffer[1], MD5_LEN, "MD5 OUT[16]");
		memcpy(T_Buffer[0]+i*MD5_LEN, T_Buffer[1], MD5_LEN);
		aos_free(T_Buffer[4]);
		memset(T_Buffer[1], 0, MD5_LEN);
		memset(T_Buffer[2], 0, SHA1_LEN);
		memset(T_Buffer[3], 0, pre_master_secretLength+SHA1_LEN);
	}
	memcpy(master_secret, T_Buffer[0], MASTER_SECRET_LENGTH);

	aos_free(T_Buffer[0]);
	aos_free(T_Buffer[1]);
	aos_free(T_Buffer[2]);
	aos_free(T_Buffer[3]);

//	PrintData(master_secret, MASTER_SECRET_LENGTH, "master_secret");
	return;
}

//
// Description:
//	This function construct finished message.
// Arguments:
//  Input:
//	handshake:Pointer to all message buffer.
//	handshake_len:The length of all message.
//	master_secret:Pointer to the master secret.
//	Sender:The sender data, server is 0x53525652(only server).
//  Output:
//	Finished:Pointer to the finished message.
// Return value:
//	 This function no return value.
//
void AosSsl_constructFinishedMessage(uint8* handshake,
							uint32 handshake_len,
							uint8* Finished,
							uint8* master_secret,
							AosSSLSender Sender)
{
	uint8* T_Buffer[5];
	uint8 ctemp16[2];
	uint16 temp16=FINISH_MESSAGE_LENGTH-4;
	uint32 temp32;
	uint8 ctemp32[4];
	//struct finished_head{
	//	uint8 msg_type;
	//	uint8 0x00;
	//	uint8 msg_length[2];
	//}
	
	Finished[0]=(uint8)FINISHED;
	Finished[1]=0x00;
	LEndianToBEndian((uint8*)&temp16, (uint8*)ctemp16, sizeof(uint16));
	memcpy(Finished+2, ctemp16, sizeof(uint16));
	//struct finished_msg{
	//	uint8 md5_hash[MD5_LEN];
	//	uint8 sha1_hash[SHA1_LEN];
	//}
	//md5_hash=MD5(master_secret+pad2+MD5(handshake_messages+Sender+master_secret+pad1))
	//sha1_hash=SHA(master_secret+pad2+SHA(handshake_messages+Sender+master_secret+pad1))
	T_Buffer[0]=(uint8*)aos_malloc_atomic(handshake_len+4+MASTER_SECRET_LENGTH+MD5_PAD_LENGTH);
	T_Buffer[1]=(uint8*)aos_malloc_atomic(MASTER_SECRET_LENGTH+MD5_PAD_LENGTH+MD5_LEN);
	T_Buffer[2]=(uint8*)aos_malloc_atomic(MD5_LEN);
	memcpy(T_Buffer[0], handshake, handshake_len);
	temp32=Sender;
	//memcpy(ctemp32, (uint8*)&temp32, sizeof(uint32));
	LEndianToBEndian((uint8*)&temp32, ctemp32, sizeof(uint32));
	
	memcpy(T_Buffer[0]+handshake_len,
					ctemp32,
					sizeof(uint32));
	memcpy(T_Buffer[0]+handshake_len+sizeof(uint32),
					master_secret,
					MASTER_SECRET_LENGTH);
	memset(T_Buffer[0]+handshake_len+4+MASTER_SECRET_LENGTH,
					PAD_1,
					MD5_PAD_LENGTH);
	MD5_arth(T_Buffer[0], handshake_len+sizeof(uint32)+MASTER_SECRET_LENGTH+MD5_PAD_LENGTH, T_Buffer[2]);
	memcpy(T_Buffer[1], master_secret, MASTER_SECRET_LENGTH);
	memset(T_Buffer[1]+MASTER_SECRET_LENGTH, PAD_2, MD5_PAD_LENGTH);
	memcpy(T_Buffer[1]+MASTER_SECRET_LENGTH+MD5_PAD_LENGTH, T_Buffer[2], MD5_LEN);
	memset(T_Buffer[2], 0, MD5_LEN);
	MD5_arth(T_Buffer[1], MASTER_SECRET_LENGTH+MD5_PAD_LENGTH+MD5_LEN, T_Buffer[2]);

	memcpy(Finished+4, T_Buffer[2], MD5_LEN);
	aos_free(T_Buffer[0]);
	aos_free(T_Buffer[1]);
	aos_free(T_Buffer[2]);

	T_Buffer[0]=(uint8*)aos_malloc_atomic(handshake_len+sizeof(uint32)+MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH);
	T_Buffer[1]=(uint8*)aos_malloc_atomic(MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH+SHA1_LEN);
	T_Buffer[2]=(uint8*)aos_malloc_atomic(SHA1_LEN);
	memcpy(T_Buffer[0], handshake, handshake_len);
	memcpy(T_Buffer[0]+handshake_len,
					ctemp32,
					sizeof(uint32));
	memcpy(T_Buffer[0]+handshake_len+sizeof(uint32),
					master_secret,
					MASTER_SECRET_LENGTH);
	memset(T_Buffer[0]+handshake_len+sizeof(uint32)+MASTER_SECRET_LENGTH,
					PAD_1,
					SHA1_PAD_LENGTH);
	SHA_arth(T_Buffer[0], handshake_len+sizeof(uint32)+MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH, T_Buffer[2]);
	memcpy(T_Buffer[1], 
					master_secret,
					MASTER_SECRET_LENGTH);
	memset(T_Buffer[1]+MASTER_SECRET_LENGTH,
					PAD_2,
					SHA1_PAD_LENGTH);
	memcpy(T_Buffer[1]+MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH,
					T_Buffer[2],
					SHA1_LEN);
	memset(T_Buffer[2], 0, SHA1_LEN);
	SHA_arth(T_Buffer[1], MASTER_SECRET_LENGTH+SHA1_PAD_LENGTH+SHA1_LEN, T_Buffer[2]);
	memcpy(Finished+4+MD5_LEN, T_Buffer[2], SHA1_LEN);
	aos_free(T_Buffer[0]);
	aos_free(T_Buffer[1]);
	aos_free(T_Buffer[2]);

	return;
}

//
// Description:
//	This function construct finished message block.
// Arguments:
//  Input:
//	FinishedMac:Pointer to the finished message digest.
//	HashLength:MD5 digest length or SHA1 digest length.
//	PadLength:The length of pad data.
 // Output:
//	BlockCipher:Pointer to the finished block cipher buffer.
// Return value:
//	 This function no return value.
//
int AosSsl_constructFinishedBlock(uint8* FinishedMac,
						uint32 HashLength,
						uint8* Finished,
						uint32 PadLength,
						uint8* BlockCipher,
						int	*BlockCipher_len,
						char *key,
						char *iv,
						struct AosSslContext *context)
{
	uint8* plain;
	int pos = 0;
	int ret = 0;
	//aos_trace("function ConstructFinishedBlock begin\n");
	
	plain = (uint8*)aos_malloc_atomic(FINISH_MESSAGE_LENGTH+HashLength+PadLength+1);
	memcpy(plain, Finished, FINISH_MESSAGE_LENGTH);
	pos += FINISH_MESSAGE_LENGTH;
	memcpy(plain+pos, FinishedMac, HashLength);
	pos += HashLength;
	if (PadLength)
	{
		memset(plain+pos, (uint8)PadLength, PadLength+1);
		pos += PadLength + 1;
	}

	switch (context->encrypt_type)
	{
	case eAosEncryptType_DES_CBC:
		 aos_ssl_cipher[context->encrypt_type].encrypt(
		 	(void*)iv,
		 	plain,
		 	pos,
		 	BlockCipher,
		 	&pos,
		 	(uint8*)key,
		 	context->encrypt_key_len);
		 
		 break;
	case eAosEncryptType_Null:	
	case eAosEncryptType_RC4_128:
		 //aos_trace("rc4 encryption in construction finished message");
		aos_ssl_cipher[context->encrypt_type].setkey(
			(void*)(context->is_client?&context->client_cipher_ctx:&context->server_cipher_ctx),
			(uint8*)key, 
			context->encrypt_key_len);
		 aos_ssl_cipher[context->encrypt_type].encrypt(
			(void*)(context->is_client?&context->client_cipher_ctx:&context->server_cipher_ctx),
			plain, 
			pos, 
			BlockCipher, 
			&pos,
			(uint8*)key, 
			context->encrypt_key_len);
		 break;

	default:
		 ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"unknow encryption type %d", context->encrypt_type);
		 break;	
	}
	*BlockCipher_len = pos;	
	aos_free(plain);
	
	return ret;
}

//
// Description:
//	This function calculate the key block.
// Arguments:
//  Input:
//	master_secret:Pointer to the master secret.
//	client_random:Pointer to the client random.
//	server_random:Pointer to the server random.
//	key_block_len:The length of key block.
//  Output:
//	key_block:Pointer to the key block.
// Return value:
//	 This function no return value.
//
void AosSsl_calculateKeyBlock(uint8* master_secret, 
					uint8* server_random, 
					uint8* client_random, 
					struct AosSslContext *context, 
					u32 hash_len,
					uint32 key_block_len)
{
	uint8* T_Buffer[5];
	int i;
	
	T_Buffer[0]=(uint8*)aos_malloc_atomic(8*MD5_LEN);
	T_Buffer[1]=(uint8*)aos_malloc_atomic(MD5_LEN);
	T_Buffer[2]=(uint8*)aos_malloc_atomic(SHA1_LEN);
	T_Buffer[3]=(uint8*)aos_malloc_atomic(MASTER_SECRET_LENGTH+SHA1_LEN);


	//aos_trace_hex("master_secret",master_secret,MASTER_SECRET_LENGTH);
	//aos_trace_hex("crandom",client_random,RANDOM_LENGTH);
	//aos_trace_hex("srandom",server_random,RANDOM_LENGTH);

	for(i=0;i<8;i++)
	{
		T_Buffer[4]=(uint8*)aos_malloc_atomic(i+1+MASTER_SECRET_LENGTH+RANDOM_LENGTH*2);
		memcpy(T_Buffer[4], pad_char[i], i+1);
		memcpy(T_Buffer[4]+i+1, master_secret, MASTER_SECRET_LENGTH);
		memcpy(T_Buffer[4]+i+1+MASTER_SECRET_LENGTH, server_random, RANDOM_LENGTH);
		memcpy(T_Buffer[4]+1+i+MASTER_SECRET_LENGTH+RANDOM_LENGTH, client_random, RANDOM_LENGTH);
		SHA_arth(T_Buffer[4], i+1+MASTER_SECRET_LENGTH+RANDOM_LENGTH*2, T_Buffer[2]);
		memcpy(T_Buffer[3], master_secret, MASTER_SECRET_LENGTH);
		memcpy(T_Buffer[3]+MASTER_SECRET_LENGTH, T_Buffer[2], SHA1_LEN);
		MD5_arth(T_Buffer[3], MASTER_SECRET_LENGTH+SHA1_LEN, T_Buffer[1]);
		memcpy(T_Buffer[0]+i*MD5_LEN, T_Buffer[1], MD5_LEN);
		aos_free(T_Buffer[4]);
	}
	
	memcpy(context->key_block, T_Buffer[0], key_block_len);

    	context->client_mac_secret = (char *)context->key_block;
    	context->server_mac_secret = (char *)context->key_block + hash_len;
    	context->client_write_key  = (char *)context->key_block + hash_len*2;
    	context->server_write_key  = (char *)context->key_block + hash_len*2 + context->encrypt_key_len;
/*
	aos_trace_hex("clientmaxsecret",context->client_mac_secret,hash_len);
	aos_trace_hex("servermaxsecret",context->server_mac_secret,hash_len);
	aos_trace_hex("clientwritekey",context->client_write_key,context->encrypt_key_len);
	aos_trace_hex("serverwritekey",context->server_write_key,context->encrypt_key_len);
*/
	if (context->iv_len)
   	{
    	context->client_iv = 
			(char *)context->key_block + 
				  	hash_len*2 + 
				  	context->encrypt_key_len*2;

	   	context->server_iv = 
			(char *)context->key_block + 
					hash_len*2 + 
					context->encrypt_key_len*2 + 
					context->iv_len;
	}
	else
	{
		context->client_iv = NULL;
		context->server_iv = NULL;
	}

	aos_free(T_Buffer[0]);
	aos_free(T_Buffer[1]);
	aos_free(T_Buffer[2]);
	aos_free(T_Buffer[3]);
	
	return;
}

//
// Description:
//	This function calculate the finished message digest.
// Arguments:
//  Input:
//	msg:Pointer to the message.
//	msg_len:The length of message.
//	hash_type:The hash type with selected cipher suite.
//	ServerWriteMACSecret:Pointer to the server mac secret on the key block.
//	seq_num:Pointer to the session ID in the context.
//  Output:
//	msg_mac:Pointer to the digest of message.
// Return value:
//	 This function no return value.
//	
int AosSsl_calculateMAC(
	uint8* msg,
	uint32 msg_len,
	uint8* msg_mac,
	AosHashType hash_type,
	uint8* ServerWriteMACSecret,					
	uint64 seq_num,
	AosContentType ContentType)
{
	void *ctx;
	
	aos_assert1(hash_type > eAosHashType_None && hash_type < eAosHashType_MAX);
	
	ctx = aos_malloc_atomic(aos_ssl_hashcipher[hash_type].ctx_size);	
	aos_assert1(ctx);

	//     hash(MAC_write_secret + pad_2 +
    //      hash(MAC_write_secret + pad_1 + seq_num +
    //           SSLCompressed.type + SSLCompressed.length +
    //           SSLCompressed.fragment));
	//	seq_num length is 64 bit
	aos_ssl_hashcipher[hash_type].dia_init(ctx);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, ServerWriteMACSecret, aos_ssl_hashcipher[hash_type].hash_len);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, PAD1, aos_ssl_hashcipher[hash_type].pad_len);
	LEndianToBEndian((uint8*)&seq_num, (uint8*)msg_mac, sizeof(uint64));
	msg_mac[8] = ContentType;
	msg_mac[9] = (msg_len>>8)&0xff;
	msg_mac[10]= msg_len&0xff;
	aos_ssl_hashcipher[hash_type].dia_update(ctx, msg_mac, 11);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, msg, msg_len);
	aos_ssl_hashcipher[hash_type].dia_final(ctx, msg_mac);

	aos_ssl_hashcipher[hash_type].dia_init(ctx);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, ServerWriteMACSecret, aos_ssl_hashcipher[hash_type].hash_len);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, PAD2, aos_ssl_hashcipher[hash_type].pad_len);
	aos_ssl_hashcipher[hash_type].dia_update(ctx, msg_mac, aos_ssl_hashcipher[hash_type].hash_len);
	aos_ssl_hashcipher[hash_type].dia_final(ctx, msg_mac);

	aos_free(ctx);

	return eAosRc_Success;
}


int AosSsl_getCipher(const char *name, u32 *cipher)
{
	int index = 0;
	while (aos_cipher_name_map[index].name[0])
	{
		if (strcmp(aos_cipher_name_map[index].name, name) == 0)
		{
			*cipher = aos_cipher_name_map[index].value;
			return 0;
		}

		index++;
	}

	return -eAosRc_CipherNotFound;
}


char *AosSsl_getCipherName(u32 cipher)
{
	int index = 0;
	static char local[30];

	while (aos_cipher_name_map[index].name[0])
	{
		if (aos_cipher_name_map[index].value == cipher)
		{
			return aos_cipher_name_map[index].name;
		}

		index++;
	}

	sprintf(local, "Not Defined: %u", (unsigned int)cipher);
	return local;
}

int AosSsl_isCipherSupported(u32 cipher)
{
	struct AosSslContext *context;
	context = (struct AosSslContext*)aos_malloc_atomic(sizeof(struct AosSslContext));
	aos_assert1(context);
	context->cipher_selected = cipher;
	if (AosSSL_parseCipher(context) == 0)
	{
		aos_free(context);
		return 1;
	}
	aos_free(context);
	return 0;
}

//
// Description:
//	This function create the alert record.
// Arguments:
//  Input:
//	msg:Pointer to the record buffer.
//	msg_len:The length of buffer.
//	errcode:alert error code.
//  Output:
//  	msg: the created alert record
//	msg_len: the length of output
// Return:
//   0: success
//   <0: failed
// 
int AosSsl_createAlertMsg(struct AosSslContext *context, uint8 *msg, int *msg_len, int errcode)
{
	//uint8 plain[2];

	//if (context->state != eAosSSLState_Established)
	{	
		msg[0] = errcode >> 8;
		msg[1] = errcode;
		aos_trace("errcode = %d", errcode);
		aos_trace_hex("alertmsg", (char*)msg, 2);
		*msg_len = 2;
		return eAosRc_Success;
	}

	//plain[0] = errcode >> 8;
	//plain[1] = errcode;
/*
	msg[0] = errcode >> 8;
	msg[1] = errcode;
	*msg_len = 2;

	// if ssl established, we should do encryption
	return AosSsl_EncAppData(context, msg, msg_len, eAosContentType_Alert);
*/	
}


//
// Description:
//	This function generate certificate message.
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//  Output:
//   record: the output buffer of certificate record
// Return value:
//	eAosRc_Success: success
//    <0: error returned
//
int AosSsl_createCertificateMsg(
			struct AosSslContext *context, 
			AosSslRecord_t *record, 
			X509_CERT_INFO *certificate)
{
	uint8* data = (uint8*)AosSslRecord_getMsgBody(record);
	uint32 cert_len, cert_total_len;
	int pos = 0;

	//Certificate
	//	ASN.1Cert certificate_list<1..2^24-1>
	//

	aos_assert1(certificate);
	cert_len = certificate->cert_der_len;
	cert_total_len = cert_len +3;
	data[pos++] = (cert_total_len&0x00ff0000) >> 16;
	data[pos++] = (cert_total_len&0x0000ff00) >> 8;
	data[pos++] = (cert_total_len&0x000000ff);
	data[pos++] = (cert_len&0x00ff0000) >> 16;
	data[pos++] = (cert_len&0x0000ff00) >> 8;
	data[pos++] = (cert_len&0x000000ff);
	memcpy(data+pos, certificate->cert_der, cert_len);
	pos += cert_len;

	//
	// Store  server Certificate message into context
	// create record
	//
	AosSslRecord_createHandshake(record, eAosSSLMsg_Certificate, pos);

	//
	// store ServerHello message into context
	//
	AosSSL_addhandshake(context, record);

	return eAosRc_Success;
}

//
// Description:
//	This function generate and send alert record
//	if ssl is established, the record body will be encrypted
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//  Output:
// Return value:
//	>=0: Success
//    <0: error returned
//

int AosSsl_createAndSendAlertRecord(struct AosSslContext *context, int errcode)
{
	struct AosSslRecord *record;
	uint8 *msg;
	int msg_len;
	int ret;
	
	record = AosSslRecord_create( NULL, eAosSSL_AlertMaxLen, eAosSslRecord_Output );
	aos_assert1( record );
	msg = (uint8*)AosSslRecord_getMsg( record );
	msg_len = AosSslRecord_getRemainBufferLen( record );
	
	AosSsl_createAlertMsg( context, msg, &msg_len, errcode );
	AosSslRecord_createAlertRecord( record, msg_len );

	if ( ( ret = AosSslRecord_send( record, context ) ) < 0 )
	{
		AosSslRecord_release( record );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send alert message, %d", ret);
	}
	
	AosSslRecord_release( record );
	
	return eAosRc_Success;
}

//
// Description:
//  This function process application data buffered in context
//  before ssl is established, call this function once ssl is
//  established
// Arguments:
//  Input:
//  context:Point to the struct AosSslContext.
//  Output:
// Return value:
//  >=0: Success
//    <0: error returned
//

int AosSsl_preEstablished(struct AosSslContext *context)
{
	return AosSslStm_skbOutput(
			context,
			NULL,
			context->callback);
}

int AosSsl_getFinishMsgLen(struct AosSslContext *context, int *finished_msg_len)
{
	aos_assert1(context);
	if (context->encrypt_type == eAosEncryptType_RC4_128 ||
		context->encrypt_type == eAosEncryptType_Null)
	{
		*finished_msg_len = FINISH_MESSAGE_LENGTH + context->hash_len;
	}
	else
	{
		*finished_msg_len = FINISH_MESSAGE_LENGTH + context->hash_len;
    		*finished_msg_len += context->encrypt_key_len - (FINISH_MESSAGE_LENGTH+context->hash_len)%context->encrypt_key_len;
  	}
	return eAosRc_Success;
}

//
// Description:
//  This function decrypt application data and verify the mac
// Arguments:
//  Input:
//  context:Point to the struct AosSslContext.
//    inout: the encrypted application data
//    inout_len: the length of the application data
//  Output:
//   inout: the decrypted application data, do not include mac
//   inout_len: return the length of decrypted app data
// Return value:
//  >=0: Success
//    <0: error returned
//
int AosSsl_csp1RecordDec(
	struct AosSslContext *context,
	struct AosSslRecord *record)
{
	uint32 request_id;
	int ret;
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
	case eAosEncryptType_Null:
		ret = Kernel_Csp1DecryptRecordRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				(SslPartyType)context->is_client,
				APP_DATA,
				AosSslRecord_getMsgLen( record ),
				(uint8*)AosSslRecord_getMsg( record ), 
				(uint8*)AosSslRecord_getMsg( record ), 
				&request_id,
				AosSslStm_cardCallback,
				(void*)record);
		break;
	default:
	        return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Decrypt application data error, unsupported encryption type %d", context->encrypt_type);
	}
	if ( ret )
	{
	    aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1 decrypt record data error");
		return ret;
	}
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	return ret;
}

int AosSsl_csp1RecordEnc(
	struct AosSslContext *context,
	struct AosSslRecord *record,
	AosContentType msg_type)
{
	uint32 request_id;
	int ret;
	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		ret = Kernel_Csp1EncryptRecordRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				(SslPartyType)context->is_client,
				APP_DATA,
				AosSslRecord_getMsgLen( record ),
				(uint8*)AosSslRecord_getMsg( record ), 
				(uint8*)AosSslRecord_getMsg( record ), 
				&request_id,
				AosSslStm_cardCallback,
				(void*)record);
		break;
	default:
	        return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Encrypt application data error, unsupported encryption type %d", context->encrypt_type);
	}
	if ( ret )
	{
	    aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1 encrypt record data error");
		return ret;
	}
	AosSslContext_hold( record->context );
	atomic_inc( &context->out_to_card_cnt );
	return ret;
}


