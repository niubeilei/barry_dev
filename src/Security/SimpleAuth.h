////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SimpleAuth.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_SimpleAuth_h
#define Omn_Security_SimpleAuth_h

#include "Security/Auth.h"
#include "Util/RCObjImp.h"


class OmnSimpleAuth : public OmnAuth
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxChallengeLen = 128 
	};

	int			mChallengeLen;

public:
	OmnSimpleAuth();
	~OmnSimpleAuth();

	virtual OmnString	getChallenge();
	virtual void		setChallengeLen(const int l);
};
#endif
