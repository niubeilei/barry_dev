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
// 2015/04/01 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoParserAPI_h
#define Aos_JimoAPI_JimoParserAPI_h

#include "SEInterfaces/JimoParserObj.h"
#include "Rundata/Rundata.h"

namespace Jimo
{

extern AosJimoParserObjPtr jimoCreateJimoParser(AosRundata *rdata);

};

#endif
