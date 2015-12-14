////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspMsg.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "OCSP/OcspMsg.h"

#include "aos/aosKernelAlarm.h"
#include "ASN1/AsnCodec.h"
#include "ASN1/AsnDefs.h"



// OCSPRequest     ::=     SEQUENCE {
//		tbsRequest                  TBSRequest,
//		optionalSignature   [0]     EXPLICIT Signature OPTIONAL }
//
// TBSRequest      ::=     SEQUENCE {
//		version             [0]     EXPLICIT Version DEFAULT v1,
//		requestorName       [1]     EXPLICIT GeneralName OPTIONAL,
//		requestList                 SEQUENCE OF Request,
//		requestExtensions   [2]     EXPLICIT Extensions OPTIONAL }
//
// Signature       ::=     SEQUENCE {
//		signatureAlgorithm      AlgorithmIdentifier,
//		signature               BIT STRING,
//		certs               [0] EXPLICIT SEQUENCE OF Certificate OPTIONAL}
//
// Version         ::=             INTEGER  {  v1(0) }
//
// Request         ::=     SEQUENCE {
//		reqCert                     CertID,
//		singleRequestExtensions     [0] EXPLICIT Extensions OPTIONAL }
//
// CertID          ::=     SEQUENCE {
//		hashAlgorithm       AlgorithmIdentifier,
//		issuerNameHash      OCTET STRING, -- Hash of Issuer's DN
//		issuerKeyHash       OCTET STRING, -- Hash of Issuers public key
//		serialNumber        CertificateSerialNumber }
//


static int aosOcspReq_encodeReqName(struct aosOcspRequest *self, 
									char *buffer, 
									unsigned int buflen,
									unsigned int *cursor)
{
	return aosAlarmStr(eAosRc_NotImplementedYet, "Not implemented yet");
}


static int aosOcspReq_encodeRequest(struct aosOcspTbsRequest *self, 
									char *buffer, 
									unsigned int buflen,
									unsigned int *cursor)
{
	// 
	// 0x10
	// length
	// The contents
	// 
	// The syntax defintions:
	// Request         ::=     SEQUENCE {
	//		reqCert                     CertID,
	//		singleRequestExtensions     [0] EXPLICIT Extensions OPTIONAL }
	//
	// CertID          ::=     SEQUENCE {
	//		hashAlgorithm       AlgorithmIdentifier,
	//		issuerNameHash      OCTET STRING, -- Hash of Issuer's DN
	//		issuerKeyHash       OCTET STRING, -- Hash of Issuers public key
	//		serialNumber        CertificateSerialNumber }
	//
	// CertificateSerialNumber ::= INTEGER
	//
	int ret;
	unsigned int reqLenPos, certIdLenPos;
	aosKernelAssert(*cursor + 3 < buflen, eAosRc_BufferTooShort);

	buffer[(*cursor)++] = 0x10;
	reqLenPos = (*cursor)++;

	buffer[(*cursor)++] = 0x10;
	certIdLenPos = (*cursor)++;

	// Encode the algorithm ID
	ret = aosAsn1Codec_encodeAlgId(buffer, buflen, cursor, &self->mAlgId);

	// Encode issuerNameHash
	ret |= aosAsn1Codec_string_encode(4, 0, 1, self->mIssuerNameHash, 
			self->mIssuerNameHashLen, buffer, buflen, cursor);

	// Encode issuerKeyHash
	ret |= aosAsn1Codec_string_encode(4, 0, 1, self->mIssuerKeyHash, 
			self->mIssuerKeyHashLen, buffer, buflen, cursor);

	// Encode serialNumber
	ret |= aosAsn1Codec_integer_encode(2, 0, 1, self->mCertSerialNum, buffer, buflen, cursor);

	return ret;
}


static int aosOcspReq_encodeRequests(struct aosOcspRequest *self, 
									char *buffer, 
									unsigned int buflen,
									unsigned int *cursor)
{
	// 
	// 0x10 
	// length
	// Individual request encoding
	//
	unsigned int lengthPos;
	int ret;

	aosKernelAssert(*cursor + 3 < buflen, eAosRc_BufferTooShort);

	buffer[(*cursor)++] = 0x10;
	lengthPos = (*cursor)++;

	for (int i=0; i<self->mNumRequests; i++)
	{
		ret |= aosOcspReq_encodeRequest(&self->mRequests[i], buffer, buflen, cursor);
		if (ret)
		{
			return ret;
		}
	}

	ret |= aosAsn1Codec_adjustLength(buffer, buflen, cursor, lengthPos, *cursor - lengthPos + 1);

	return ret;
}


static int aosOcspReq_encodeSignature(struct aosOcspRequest *self, 
									char *buffer, 
									unsigned int buflen,
									unsigned int *cursor)
{
	return 0;
}


int aosOcspReq_encode(struct aosOcspRequest *req, 
					  char *buffer, 
					  unsigned int buflen,
					  unsigned int *cursor)
{
	int ret;
	unsigned int ocspReqStart, tbsReqStart;

	// 
	// Encode the identifier (should be 00010000)
	//
	ret = aosAsn1Codec_setid(16, eAsnTypeClass_Universal, 1, buffer, buflen, cursor);
	aosKernelAssert(ret == 0, ret);

	// 
	// We do not know the length of the structure yet. We will reserve one byte
	// for the length. If the result length is less than 128, everything will
	// be just fine. Otherwise, we will have to make room for the length. 
	// This is something stupid about ASN.1. It did not consider the encoding
	// efficiency.
	//
	ocspReqStart = *cursor;
	(*cursor)++;

	// 
	// Encode TBSRequest ::= SEQUENCE {...}
	//
	ret = aosAsn1Codec_setid(16, eAsnTypeClass_Universal, 1, buffer, buflen, cursor);
	aosKernelAssert(ret == 0, ret);
	tbsReqStart = *cursor;
	(*cursor)++;

	// 
	// Since version is default to v1 and v1 is the only allowed version, DER 
	// requires version shall not be encoded into the string. 

	// 
	// Encode requestorName:
	//	requestName [1] EXPLICIT GeneralName OPTIONAL
	// Note that this is an optional component. It should not be encoded unless
	// it is set.
	//
	if (req->mReqName[0] > 0)
	{
		ret = aosOcspReq_encodeReqName(req, buffer, buflen, cursor);
		aosKernelAssert(ret == 0, ret);
	}

	// 
	// Encode requests:
	//	requestList SEQUENCE Of Request
	// 
	ret = aosOcspReq_encodeRequests(req, buffer, buflen, cursor);
	aosKernelAssert(ret == 0, ret);

	// 
	// Encode the extensions. 
	// In the current implementation, no extensions are allowed.
	//

	// 
	// Encode the signature:
	//	optionalSignature [0] EXPLICIT Signature OPTIONAL
	// The signature is present if and only if its algorithm ID is valid.
	//
	if (req->mSignatureBitLen > 0)
	{
		ret = aosOcspReq_encodeSignature(req, buffer, buflen, cursor);
		aosKernelAssert(ret == 0, ret);
	}

	// 
	// Check the length
	//
	       
	ret |= aosAsn1Codec_adjustLength(buffer, buflen, cursor, tbsReqStart, *cursor - tbsReqStart + 1);
	ret |= aosAsn1Codec_adjustLength(buffer, buflen, cursor, ocspReqStart, *cursor - ocspReqStart + 1);

	return 0;
}

