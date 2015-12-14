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
// 2013/03/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Logger_LogEntryDatalet_h
#define Aos_Logger_LogEntryDatalet_h

#include "Logger/LogEntry.h"
#include "Util/RCObjImp.h"

class AosLogEntryDatalet : public AosLogEntry
{
	OmnDefineRCObject;

private:

public:
	AosLogEntryDatalet(const int version);
	virtual ~AosLogEntryDatalet();

};
#endif
