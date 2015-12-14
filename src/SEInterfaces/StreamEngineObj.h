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
// 2015/01/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StreamEngineObj_h
#define Aos_SEInterfaces_StreamEngineObj_h

#include "Jimo/Jimo.h"

class AosStreamEngineObj : public AosJimo
{
protected:

public:
	AosStreamEngineObj(const int version);
	virtual ~AosStreamEngineObj();

};
#endif

