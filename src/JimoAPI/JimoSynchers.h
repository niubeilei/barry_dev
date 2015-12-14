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
// 2015/09/16 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoSynchers_h
#define Aos_JimoAPI_JimoSynchers_h

#include "SEInterfaces/Ptrs.h"
#include "JimoCall/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "IDO/Ptrs.h"

class AosRundata;

namespace Jimo
{
	extern AosJimoCallPtr jimoSendSyncher(AosRundata *rdata, AosSyncherObj *syncher, AosJimoCallerPtr caller = NULL);
};
#endif

