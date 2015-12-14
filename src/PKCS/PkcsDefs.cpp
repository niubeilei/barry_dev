////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PkcsDefs.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/PkcsDefs.h"

#include "ASN1/ObjId.h"


int aosPkcsAlg_init(struct aosPkcsAlg *self)
{
	self->mParms = 0;
	return aosAsnObjId_init(&self->mAlgId);
}


