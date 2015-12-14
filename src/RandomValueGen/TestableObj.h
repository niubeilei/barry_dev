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
// 11/16/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Parms_TestableObj_h
#define Aos_Parms_TestableObj_h

#include "Util/RCObject.h"


class AosTestableObj : virtual public OmnRCObject
{

	
public:
	AosTestableObj();
	~AosTestableObj();

	bool checkInvariants();
};
#endif



