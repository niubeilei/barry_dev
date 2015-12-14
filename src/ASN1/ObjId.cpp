////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjId.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ASN1/ObjId.h"


int aosAsnObjId_init(struct aosAsnObjId *self)
{
	for (int i=0; i<eAsnMaxObjComps; i++)
	{
		self->mComps[i] = -1;
	}

	return 0;
}
