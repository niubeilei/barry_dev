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
// 03/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_CommOwner_h
#define Omn_UtilComm_CommOwner_h

#include "Util/RCObject.h"


class AosCommOwner : public virtual OmnRCObject
{
public:
	enum E
	{
		eInvalid,

		eMax
	};

public:
	virtual E getOwnerId() const = 0;
};
#endif

