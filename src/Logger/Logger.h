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
// 2013/06/15 Recreated by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Logger_Logger_h
#define Omn_Logger_Logger_h

#include "SEInterfaces/LoggerObj.h"

class AosLogger : virtual public AosLoggerObj
{
protected:

public:
	AosLogger();
	virtual ~AosLogger();
};
#endif

