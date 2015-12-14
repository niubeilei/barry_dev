////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspMsg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_OCSP_OcspMsg_h
#define Omn_OCSP_OcspMsg_h

#include "ASN1/ObjId.h"
#include "PKCS/PkcsDefs.h"
#include "PKCS/Certificate.h"
#include "PKCS/Signature.h"


enum
{
	eOcspReqNameMaxLen = 30,
	eOcspMaxRequests = 5,
	eOcspIssuerNameHashLen = 128,
	eOcspIssuerKeyHashLen = 128,
	eOcspCertSerialNumLen = 64,
};


struct aosOcspTbsRequest
{
	struct aosPkcsAlg	mAlgId;
	char				mIssuerNameHash[eOcspIssuerNameHashLen];
	int					mIssuerNameHashLen;
	char				mIssuerKeyHash[eOcspIssuerKeyHashLen];
	int					mIssuerKeyHashLen;
	int					mCertSerialNum;
};


struct aosOcspRequest
{
	unsigned char				mVersion;
	char						mReqName[eOcspReqNameMaxLen];
	struct aosOcspTbsRequest	mRequests[eOcspMaxRequests];
	int							mNumRequests;
	struct aosPkcsAlg			mSignatureAlg;
	char						mSignature[ePkcsSignatureLen];
	unsigned int				mSignatureBitLen;
	char						mCertificate[ePkcsCertificateMaxLen];	// Only one certificate allowed

};



struct aosOcspResponse
{
};

#endif

