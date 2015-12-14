////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Certificate.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_Certificate_h
#define Omn_PKCS_Certificate_h

#include "aosUtil/Types.h"
#include "ASN1/ObjId.h"
#include "PKCS/DN.h"
#include "PKCS/PkcsDefs.h"


typedef struct AosCertificate
{
	int						mVersion;
	OmnUint64				mSerialNum;
	struct aosPkcsAlg		mSignatureAlg;
	struct aosPkcsDN		mIssuer;
	char					mNotBefore[ePkcsUctTimeMaxLen];
	char					mNotAfter[ePkcsUctTimeMaxLen];
	struct aosPkcsDN		mSubject;
	struct aosPkcsAlg		mPubKeyAlg;
	char					mPubKey[ePkcsPubKeyMaxLen];
	unsigned int			mPubKeyBitLen;
	char					mIssuerId[ePkcsMaxIdLen];
	unsigned int			mIssuerIdBitLen;
	char					mSubId[ePkcsMaxIdLen];
	unsigned int			mSubIdBitLen;
	char					mSignature[ePkcsSignatureLen];
	unsigned int			mSignatureBitLen;
} AosCertificate_t;


extern int AosCertificate_decode(char *buffer, 
						  unsigned int buflen,
						  struct AosCertificate **certificate);
extern int AosCert_retrieve(const AosCertificate_t *cert, 
						const char *fieldName, 
						char *value, 
						const u32 valueLen);
extern int AosCert_isValidFieldName(const char *fieldName);

#endif

