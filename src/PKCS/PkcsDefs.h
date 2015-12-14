////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PkcsDefs.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_PkcsDefs_h
#define Omn_PKCS_PkcsDefs_h

#include "ASN1/ObjId.h"

enum
{
	ePkcsSignatureLen = 512,		// ????
	ePkcsMaxCertsPerSignature = 3,
	ePkcsMaxDnValueLen = 50,
	ePkcsMaxDnPairs = 20,
	ePkcsMaxTimeLength = 20,
	ePkcsTimeType_UTC = 0,
	ePkcsTimeType_General = 1,
	ePkcsPubKeyMaxLen = 256,		// ???
	ePkcsMaxIdLen = 30,
	ePkcsUctTimeMaxLen = 20,
	ePkcsCertificateMaxLen = 1000,
};


struct aosPkcsAlg
{
	struct aosAsnObjId		mAlgId;
	void *					mParms;
};

extern int aosPkcsAlg_init(struct aosPkcsAlg *self);

#endif

