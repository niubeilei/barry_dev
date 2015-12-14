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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallDialer_h
#define Aos_JimoCall_JimoCallDialer_h

#include "JimoCall/JimoCallDialer.h"


class AosReliableDialer : public AosJimoCallDialer
{
public:
	bool makeJimoCall(AosRundata *rdata, const AosJimoCallPtr &call);
};

#endif
