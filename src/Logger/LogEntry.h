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
// 2013/03/17 Re-written by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Logger_LogEntry_h
#define Omn_Logger_LogEntry_h

#include "Logger/Ptrs.h"
#include "SEInterfaces/LogEntryObj.h"
#include "Util/RCObject.h"
#include "Util/String.h"

class AosLogEntry : public AosLogEntryObj
{
protected:

public:
	AosLogEntry(const int version); 
	virtual ~AosLogEntry();
};
#endif
