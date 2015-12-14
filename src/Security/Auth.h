////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Auth.h
// Description:
//	This is an interface defining Authencator. There may be multiple
//  authenticator implementations, all derived from this class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_Auth_h
#define Omn_Security_Auth_h

#include "Util/String.h"
#include "Util/RCObject.h"



class OmnAuth : public OmnRCObject
{
public:
	virtual OmnString	getChallenge() = 0;
	virtual void		setChallengeLen(const int l) = 0;
};
#endif
