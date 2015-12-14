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
// 2015/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoJQLParer_h
#define Aos_JimoAPI_JimoJQLParer_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
//#include "IDO/Ptrs.h"

class AosRundata;

const int AOS_JQLPARSER_OLD_VERSION = 1;
const int AOS_JQLPARSER_NEW_VERSION = 2;
namespace Jimo
{
	extern AosJQLParserObj *jimoGetJQLParser(AosRundata *rdata);
	extern int jimoGetJQLParserVersion();
};
#endif

