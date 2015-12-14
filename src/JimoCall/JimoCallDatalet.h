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
//
// Modification History:
// 2014/12/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_DataletJimoCall_h
#define Aos_JimoCall_DataletJimoCall_h

#include "JimoCall/JimoCall.h"


class AosDataletJimoCall : public AosJimoCall
{
	OmnDefineRCObject;

private:

public:
	AosDataletJimoCall(AosRundata *rdata, const int package, const int func);
};
#endif

