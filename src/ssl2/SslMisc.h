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
#include "ssl2/ReturnCode.h"
#include "ssl2/SslRecord.h"


struct aosSslContext;
extern int AosSsl_releaseContext(struct aosSslContext *context);
extern int AosSsl_isCipherSupported(u32 cipher);
extern char *AosSsl_getCipherName(u32 cipher);
extern int AosSsl_getCipher(const char *name, u32 *cipher);
extern int AosSSL_verifyFinished(
	struct aosSslContext *context, 
	AosSSLSender sender,
	uint8* msg,
	int msg_len);
extern int AosSSL_parseCipher(struct aosSslContext *context);
extern int AosSSL_createFinishedMsg(
	u32 hashLen,
	struct aosSslContext *context, 
	char *finishedMsg,
	u32 *length,
	u32 sender);

static inline int AosSSL_addHandshake(
				struct aosSslContext *context, 
				struct AosSslRecord *record)
{
	char *msg = AosSslRecord_getMsg(record);
	if (context->HandshakeLength + record->msg_body_len + 4 
			> eAosSSL_HandshakeMaxLen)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Handshake message too long: %d, %d", 
			context->HandshakeLength, record->msg_body_len);
	}

	memcpy(&context->Handshake[context->HandshakeLength], msg, 
		record->msg_body_len + 4);
	context->HandshakeLength += record->msg_body_len + 4;
	return 0;
}

static inline int AosSsl_mallocFinishedMessage(struct aosSslContext *context)
{
	context->EncryptClientFinishedMessage = (uint8*)aos_malloc(FINISH_MESSAGE_LENGTH+context->hash_len);
	if(!context->EncryptClientFinishedMessage)
	{
		return eAosAlarm_SSLSynErr;
	}
	context->EncryptServerFinishedMessage = (uint8*)aos_malloc(FINISH_MESSAGE_LENGTH+context->hash_len);
	if(!context->EncryptServerFinishedMessage)
	{
		aos_free(context->EncryptClientFinishedMessage);
		context->EncryptClientFinishedMessage = NULL;
		return eAosAlarm_SSLSynErr;
	}
	return eAosRc_Success;
}

extern int AosSsl_v3CheckVersion(char *msg, int *pos);

extern int AosSsl_selectCompression(struct aosSslContext *context, char *msg, int msg_len);

extern int AosSsl_checkAndSelectCipher(struct aosSslContext *context, char *msg, int msg_len);

extern int AosSsl_checkSessionResume(struct aosSslContext *context, char *msg, int msg_len);

extern int AosSsl_decodeCertMsg(uint8 *msg, int msg_len, struct _X509_CERT_INFO **cert);

extern int AosSsl_createCltCertVeriMsg(struct aosSslContext *context, uint8 *cert_veri, int cert_veri_len);

extern int AosSsl_EncAppData(
	struct aosSslContext *context, 
	uint8 *inout,
	int *inout_len, 
	AosContentType msg_type);

extern int AosSsl_DecAppData(
	struct aosSslContext *context, 
	uint8 *inout,
	int *inout_len);

extern void AosSsl_getMasterSecret(
	uint8* PreMasterSecret,
    uint32 PreMasterSecretLength,
    uint8* MasterSecret,
    uint8* ClientRandom,
    uint8* ServerRandom);

extern void AosSsl_constructFinishedMessage(
	uint8* Handshake,
    uint32 HandshakeLength,
    uint8* Finished,
    uint8* MasterSecret,
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
    struct aosSslContext *context);

extern void AosSsl_calculateKeyBlock(
	uint8* MasterSecret, 
    uint8* ServerRandom,
    uint8* ClientRandom,
    struct aosSslContext *context,
    u32 hash_len,
    uint32 KeyBlockLength);

extern int AosSsl_calculateMAC(
	uint8* msg,
	uint32 msg_len,
	uint8* msg_mac,
	AosHashType hash_type,
	uint8* ServerWriteMACSecret,
	uint64 SeqNum,
	AosContentType ContentType);

struct _X509_CERT_INFO;
extern int AosSsl_createCertificateMsg(
	struct aosSslContext *context, 
	AosSslRecord_t *record, 
	struct _X509_CERT_INFO *certificate);

extern int AosSsl_createAlertMsg(
	struct aosSslContext *context, 
	uint8 *msg, 
	int *msg_len, 
	int errcode);

extern int AosSsl_createAndSendAlertRecord(
	struct aosSslContext *context, 
	int errcode);

extern int AosSsl_preEstablished(struct aosSslContext *context);

extern int AosSsl_getFinishMsgLen(
	struct aosSslContext *context, 
	int *finished_msg_len);

extern int AosSsl_csp1DecAppData(
	struct aosSslContext *context, 
	uint8 *inout, 
	int *inout_len);

extern int AosSsl_csp1EncAppData(
	struct aosSslContext *context, 
	uint8 *inout, 
	int *inout_len, 
	AosContentType msg_type);

void MD5_arth(uint8* InputData, int InLen, uint8* OutputBuffer);
extern int AosSsl_showSupportedCiphers(
    char* rsltBuff,
    unsigned int *rsltIndex,
    int length,
    char *errmsg,
    const int errlen);

#endif

