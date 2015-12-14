////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Signature.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_Signature_h
#define Omn_PKCS_Signature_h

#include "ASN1/ObjId.h"
#include "PKCS/PkcsDefs.h"

struct aosCertificate;

struct aosSignature
{
	struct aosAsnObjId		mAlgId;
	char					mSignature[ePkcsSignatureLen];
	struct aosCertificate *	mCertificates[ePkcsMaxCertsPerSignature];
	int						mNumCerts;
};

static inline int aosSignature_isValid(struct aosSignature *self)
{
	return aosAsnObjId_isValid(&self->mAlgId);
}

#endif

