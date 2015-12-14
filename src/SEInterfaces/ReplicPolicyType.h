////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	
// Modification History:
// 09/21/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_ReplicPolicyType_h
#define AOS_SEInterfaces_ReplicPolicyType_h

#include "Util/Ptrs.h"
#include "Util/String.h"

#define	 AOSPOLICY_ONEPLUSONE	"oneplusone"
#define	 AOSPOLICY_ONEPLUSTWO	"oneplustwo"
#define	 AOSPOLICY_NOBKP		"nobkp"

class AosReplicPolicyType 
{

public:
	enum E 
	{
		eInvalid,

		eOnePlusOne,	
		eOnePlusTwo,
		eTwoPlusOne,
		replic_policy,
		mReplicPolicy,
		eNoBkp,
		eThree,
		eMax,

		eMaxEntries
	};

	static bool isValid(const E tp)
	{
		return tp > eInvalid && tp < eMaxEntries;
	}

	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E code);

};
#endif
