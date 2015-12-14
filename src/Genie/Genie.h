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
// 11/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Genie_Genie_h
#define Aos_Genie_Genie_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosGenie : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:

public:
	AosGenie();
	~AosGenie();
};
#endif
