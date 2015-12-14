////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Certificate.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
#include "PKCS/Certificate.h"


#include "aos/aosKernelAlarm.h"
#include "ASN1/AsnCodec.h"
#include "KernelUtil/KernelMem.h"
#include "PKCS/PkcsDefs.h"


static int AosCertificate_constructor(struct AosCertificate **certificate)
{
	struct AosCertificate *cert;
	*certificate = 0;
	
	cert = (struct AosCertificate*)OmnKernelAlloc(sizeof(struct AosCertificate));
	aosKernelAssert(cert, eAosRc_MemErr);

	cert->mVersion = 0;		// Default to be Version 1 (0)
	cert->mSerialNum = 0;
	aosPkcsAlg_init(&cert->mSignatureAlg);
	aosPkcsDN_init(&cert->mIssuer);
	cert->mNotBefore[0] = 0;
	cert->mNotAfter[0] = 0;
	aosPkcsDN_init(&cert->mSubject);
	aosPkcsAlg_init(&cert->mPubKeyAlg);
	cert->mPubKeyBitLen = 0;
	cert->mIssuerIdBitLen = 0;
	cert->mSubIdBitLen = 0;
	cert->mSignature[0] = 0;
	cert->mSignatureBitLen = 0;

	*certificate = cert;
	return 0;
}


static int AosCertificate_decodeIssuerId(char *buffer, 
								  unsigned int buflen, 
								  unsigned int *cursor, 
								  struct AosCertificate *cert)
{
	int ret;
	unsigned int tagid, len;
	unsigned char theclass, primitive;

	if (cert->mVersion == 0)
	{
		return aosAlarmStr(eAosRc_DecodeFailed, "Certificate version 1 but has Issuer ID");
	}

	ret |= aosAsn1Codec_getLength(buffer, cursor, &len);
	ret |= aosAsn1Codec_bitstr_decode(&tagid, &theclass, &primitive, 
				cert->mIssuerId, ePkcsMaxIdLen, &cert->mIssuerIdBitLen, 
				buffer, *cursor + len, cursor);

	return ret;
}


static int AosCertificate_decodeSubId(char *buffer, 
								  unsigned int buflen, 
								  unsigned int *cursor, 
								  struct AosCertificate *cert)
{
	int ret;
	unsigned int tagid, len;
	unsigned char theclass, primitive;

	if (cert->mVersion == 0)
	{
		return aosAlarmStr(eAosRc_DecodeFailed, "Certificate version 1 but has Subject ID");
	}

	ret |= aosAsn1Codec_getLength(buffer, cursor, &len);
	ret |= aosAsn1Codec_bitstr_decode(&tagid, &theclass, &primitive, 
				cert->mSubId, ePkcsMaxIdLen, &cert->mSubIdBitLen, 
				buffer, *cursor + len, cursor);

	return ret;
}


static int AosCertificate_decodeExtension(char *buffer, 
								  unsigned int buflen, 
								  unsigned int *cursor, 
								  struct AosCertificate *cert)
{
	int ret;
	unsigned int len;

	// 
	// Only Version 3 can have extensions
	//
	if (cert->mVersion != 2)
	{
		return aosAlarmStr(eAosRc_DecodeFailed, "Certificate not version 3 but has extensions");
	}

	// 
	// Currently we do not process any extension. Simply pass over the extensions.
	//
	ret |= aosAsn1Codec_getLength(buffer, cursor, &len);
	if (ret)
	{
		return aosAlarmStr(ret, "Failed to retrieve extension length");
	}

	(*cursor) += len;
	return 0;
}


int AosCertificate_decodeTbsCertificate(char *buffer, 
							unsigned int buflen, 
							unsigned int *cursor,
							struct AosCertificate *cert)
{
	// 
	// Version	[0] EXPLICIT Version DEFAULT v1,
	// serialNumber		CertificateSerialNumber,
	//
	int ret;
	unsigned int tbslen, len;
	unsigned int tagid;
	unsigned char theclass, primitive;
	unsigned int certFinish;
	ret = aosAsn1Codec_getSeqLen(buffer, cursor, &tbslen);
	aosKernelAssert(ret == 0, eAosRc_DecodeFailed);

	certFinish = tbslen + *cursor;

	if (buffer[*cursor] == 0xa0)
	{
		// 
		// There is Version 
		//
		ret = aosAsn1Codec_getLength(buffer, cursor, &len);
		ret = aosAsn1Codec_integer_decode(buffer, buflen, cursor, &cert->mVersion);
	}

	// 
	// Decode Serial Number
	//
	int sn;
	ret = aosAsn1Codec_integer_decode(buffer, tbslen + *cursor, cursor, &sn);
	aosKernelAssert(ret == 0, eAosRc_DecodeFailed);
	cert->mSerialNum = sn;

	//
	// Decode Algorithm ID
	//
	ret |= aosAsn1Codec_decodeAlgId(buffer, tbslen, cursor, &cert->mSignatureAlg);
	aosKernelAssert(ret == 0, eAosRc_DecodeFailed);

	// Decode Issuer
	ret |= aosAsn1Codec_decodeDN(buffer, tbslen, cursor, &cert->mIssuer);

	// Decode Vadility
	unsigned int vallen;
	ret |= aosAsn1Codec_getSeqLen(buffer, cursor, &vallen);

	ret |= aosAsn1Codec_utctime_decode(&tagid, &theclass, &primitive, cert->mNotBefore, 
		ePkcsUctTimeMaxLen, buffer, buflen, cursor);
	ret |= aosAsn1Codec_utctime_decode(&tagid, &theclass, &primitive, cert->mNotAfter, 
		ePkcsUctTimeMaxLen, buffer, buflen, cursor);

	// Decode Subject
	ret |= aosAsn1Codec_decodeDN(buffer, tbslen, cursor, &cert->mSubject);

	// Decode Public Key
	ret |= aosAsn1Codec_getSeqLen(buffer, cursor, &len);
	ret |= aosAsn1Codec_decodeAlgId(buffer, tbslen, cursor, &cert->mPubKeyAlg);
	ret |= aosAsn1Codec_bitstr_decode(&tagid, &theclass, &primitive, 
		cert->mPubKey, ePkcsPubKeyMaxLen, &cert->mPubKeyBitLen, buffer, certFinish, cursor);

	// 
	// Decode the optional fields
	//
	while (*cursor < certFinish)
	{
		switch (buffer[(*cursor)++])
		{
		case 0xa1:
			 // Issuer ID
			 if (cert->mIssuerIdBitLen > 0)
			 {
				 return aosAlarmStr(eAosRc_DecodeFailed, "Multiple Issuer ID Found");
			 }
			 AosCertificate_decodeIssuerId(buffer, certFinish, cursor, cert);
			 break;

		case 0xa2:
			 if (cert->mSubIdBitLen > 0)
			 {
				 return aosAlarmStr(eAosRc_DecodeFailed, "Multiple Subject ID found");
			 }
			 AosCertificate_decodeSubId(buffer, certFinish, cursor, cert);
			 break;

		case 0xa3:
			 AosCertificate_decodeExtension(buffer, certFinish, cursor, cert);
			 break;

		default:
			 return aosAlarmStr(eAosRc_DecodeFailed, "Unrecognized tag id");
		}
	}

	return 0;
}


int AosCertificate_decodeAlgId(char *buffer, 
							unsigned int buflen, 
							unsigned int *cursor,
							struct AosCertificate *cert)
{
	// Decode the algorithm ID
	struct aosPkcsAlg alg;
	int ret = aosAsn1Codec_decodeAlgId(buffer, buflen, cursor, &alg);
	return ret;
}


int AosCertificate_decodeSignature(char *buffer, 
							unsigned int buflen, 
							unsigned int *cursor,
							struct AosCertificate *cert)
{
	// Decode the signature
	unsigned int tagid;
	unsigned char theclass, primitive;
	int ret = aosAsn1Codec_bitstr_decode(&tagid, &theclass, &primitive, 
		cert->mSignature, ePkcsSignatureLen, &cert->mSignatureBitLen, 
		buffer, buflen, cursor);
	return ret;
}


int AosCertificate_decode(char *buffer, 
						  unsigned int buflen,
						  struct AosCertificate **certificate)
{
	unsigned int cursor = 0;
	int ret;
	struct AosCertificate *cert = 0;

	// 
	// The total length of the certificate
	//
	unsigned int totalLength; 
	ret = aosAsn1Codec_getSeqLen(buffer, &cursor, &totalLength);
	if (ret != 0 || totalLength + cursor > buflen)
	{
		return aosAlarmStr(eAosAlarm, "Failed to retrieve certificate length");
	}

	ret = AosCertificate_constructor(&cert);
	aosKernelAssert(cert, ret);

	// 
	// Decode the TBSCertificate component.
	//
	if ((ret = AosCertificate_decodeTbsCertificate(buffer, buflen, &cursor, cert)) ||
		(ret = AosCertificate_decodeAlgId(buffer, buflen, &cursor, cert)) ||
		(ret = AosCertificate_decodeSignature(buffer, buflen, &cursor, cert)))
	{
		return ret;
	}

	*certificate = cert;
	return 0;
}

*/

