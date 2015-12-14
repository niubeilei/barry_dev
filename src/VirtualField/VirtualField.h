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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_VirtualField_VirtualField_h
#define Aos_VirtualField_VirtualField_h

#include "SEInterfaces/VirtualFieldObj.h"



class AosVirtualField : public AosVirtualFieldObj
{
protected:

public:
	AosVirtualField(const int version);
	~AosVirtualField();
};

#endif
