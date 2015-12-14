////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslMisc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_SslMisc_h
#define Aos_ssl_SslMisc_h

#include "aosUtil/Alarm.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslRecord.h"

struct AosSslContext;

#define IS_SSL_ESTABLISHED(context) ((context)->state == eAosSSLState_Established)

extern int AosSsl_releaseContext(struct AosSslContext *context);
extern int AosSsl_isCipherSupported(u32 cipher);
extern char *AosSsl_getCipherName(u32 cipher);
extern int AosSsl_getCipher(const char *name, u32 *cipher);
extern int AosSSL_verifyFinished(
	struct AosSslContext *context, 
	AosSSLSender sender,
	uint8* msg,
	int msg_len);
extern int AosSSL_parseCipher(struct AosSslContext *context);
extern int AosSSL_createFinishedMsg(
	u32 hashLen,
	struct AosSslContext *context, 
	char *finishedMsg,
	u32 *length,
	u32 sender);

static inline int AosSSL_addhandshake(
				struct AosSslContext *context, 
				struct AosSslRecord *record)
{
	char *msg = (char*)AosSslRecord_getMsg(record);
	if ( (context->handshake_len + record->msg_cur_len)	> eAosSSL_HandshakeMaxLen )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"handshake message too long: %d, %d", 
			context->handshake_len, record->msg_cur_len);
	}

	memcpy( &context->handshake[context->handshake_len], msg, record->msg_cur_len );
	context->handshake_len += record->msg_cur_len;
	return 0;
}

static inline int AosSsl_mallocFinishedMessage(struct AosSslContext *context)
{
	context->enc_clt_fin_msg = (uint8*)aos_malloc_atomic(FINISH_MESSAGE_LENGTH+context->hash_len);
	if(!context->enc_clt_fin_msg)
	{
		return eAosAlarm_SSLSynErr;
	}
	context->enc_svr_fin_msg = (uint8*)aos_malloc_atomic(FINISH_MESSAGE_LENGTH+context->hash_len);
	if(!context->enc_svr_fin_msg)
	{
		aos_free(context->enc_clt_fin_msg);
		context->enc_clt_fin_msg = NULL;
		return eAosAlarm_SSLSynErr;
	}
	return eAosRc_Success;
}

extern int AosSsl_v3CheckVersion(char *msg, int *pos);

extern int AosSsl_selectCompression(struct AosSslContext *context, char *msg, int msg_len);

extern int AosSsl_checkAndSelectCipher(struct AosSslContext *context, char *msg, int msg_len);

extern int AosSsl_checkAndSelectCipherV2(struct AosSslContext *context, char *cipher_spec, 	int cipher_len);

extern int AosSsl_checkSessionResume(struct AosSslContext *context, char *msg, int msg_len);

extern int AosSsl_decodeCertMsg(uint8 *msg, int msg_len, struct _X509_CERT_INFO **cert);

extern int AosSsl_createCltCertVeriMsg(struct AosSslContext *context, uint8 *cert_veri, int cert_veri_len);

extern int AosSsl_EncAppData(
	struct AosSslContext *context, 
	uint8 *inout,
	int *inout_len, 
	AosContentType msg_type);

extern int AosSsl_DecAppData(
	struct AosSslContext *context, 
	uint8 *inout,
	int *inout_len);

extern void AosSsl_getmaster_secret(
	uint8* pre_master_secret,
    uint32 pre_master_secretLength,
    uint8* master_secret,
    uint8* client_random,
    uint8* server_random);

extern void AosSsl_constructFinishedMessage(
	uint8* handshake,
    uint32 handshake_len,
    uint8* Finished,
    uint8* master_secret,
    AosSSLSender Sender);

extern int AosSsl_constructFinishedBlock(
	uint8* FinishedMac,
    uint32 HashLength,
    uint8* Finished,
    uint32 PadLength,
    uint8* BlockCipher,
	int *BlockCipher_len,
    char *key,
    char *iv,
    struct AosSslContext *context);

extern void AosSsl_calculateKeyBlock(
	uint8* master_secret, 
    uint8* server_random,
    uint8* client_random,
    struct AosSslContext *context,
    u32 hash_len,
    uint32 key_block_len);

extern int AosSsl_calculateMAC(
	uint8* msg,
	uint32 msg_len,
	uint8* msg_mac,
	AosHashType hash_type,
	uint8* ServerWriteMACSecret,
	uint64 seq_num,
	AosContentType ContentType);

struct _X509_CERT_INFO;
extern int AosSsl_createCertificateMsg(
	struct AosSslContext *context, 
	AosSslRecord_t *record, 
	struct _X509_CERT_INFO *certificate);

extern int AosSsl_createAlertMsg(
	struct AosSslContext *context, 
	uint8 *msg, 
	int *msg_len, 
	int errcode);

extern int AosSsl_createAndSendAlertRecord(
	struct AosSslContext *context, 
	int errcode);

extern int AosSsl_preEstablished(struct AosSslContext *context);

extern int AosSsl_getFinishMsgLen(
	struct AosSslContext *context, 
	int *finished_msg_len);

extern int AosSsl_csp1RecordDec(
	struct AosSslContext *context, 
	struct AosSslRecord *record);

extern int AosSsl_csp1RecordEnc(
	struct AosSslContext *context, 
	struct AosSslRecord *record,
	AosContentType msg_type);

extern void MD5_arth(uint8* InputData, int InLen, uint8* OutputBuffer);

#endif

