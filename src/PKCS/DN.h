////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DN.h
// Description:
//	Distinguished Name (DN).   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_DN_h
#define Omn_PKCS_DN_h

#include "PKCS/PkcsDefs.h"


struct aosDnNameValuePair
{
	struct aosAsnObjId	mType;
	char				mValue[ePkcsMaxDnValueLen];
};

struct aosPkcsDN
{
	struct aosDnNameValuePair	mComps[ePkcsMaxDnPairs];
	int							mNumComps;
};

static inline int aosPkcsDN_init(struct aosPkcsDN *self)
{
	self->mNumComps = 0;
	return 0;
}

#endif

