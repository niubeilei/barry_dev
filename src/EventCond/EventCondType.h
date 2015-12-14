////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_EventCond_EventCondType_h 
#define Aos_EventCond_EventCondType_h 

#include "Util/String.h"


class AosEventCondType 
{
public:
	enum E
	{
		eInvalid,

		eEq,

		eMax
	};

	static E toEnum(const OmnString &name);
	static bool isValid(E e) {return e>eInvalid && e<eMax;}
};
#endif

