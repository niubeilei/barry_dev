////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CRL.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_CRL_h
#define Omn_PKCS_CRL_h

#include "ASN1/ObjId.h"
#include "PKCS/DN.h"
#include "PKCS/PkcsDefs.h"
#include "Porting/LongTypes.h"


struct aosCRL
{
	int					version;
	struct aosPkcsAlg	signature_alg;
	struct aosPkcsDN	issuer;
	char				this_update[ePkcsUctTimeMaxLen];
	char				next_update[ePkcsUctTimeMaxLen];
	struct AosHashInt  *hash_table;
};


extern int aosCRL_decode(char *buffer, 
						 unsigned int buflen,
						 struct aosCRL **CRL);
#endif

