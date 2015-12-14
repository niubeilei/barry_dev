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
// 2014/09/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JDFSTrans_JDFSTrans_h
#define Aos_JDFSTrans_JDFSTrans_h

#include "SEInterfaces/JDFSTransObj.h"


class AosJDFSTrans : public AosJDFSTransObj
{
protected:
	u64						mTransId;
	AosJDFSTransType::E		mTransType;

public:
	AosJDFSTrans(const int version);
};
#endif

