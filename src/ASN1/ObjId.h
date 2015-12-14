////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ASN1_ObjId_h
#define Omn_ASN1_ObjId_h

#include "ASN1/AsnDefs.h"



struct aosAsnObjId
{
	int	mComps[eAsnMaxObjComps];
};


static inline int aosAsnObjId_isValid(struct aosAsnObjId *self)
{
	return self->mComps[0] > 0;
}

extern int aosAsnObjId_init(struct aosAsnObjId *self);

#endif

