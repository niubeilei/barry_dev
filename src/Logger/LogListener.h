////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_LogListener_LogListener_h
#define Omn_LogListener_LogListener_h

#include "Util/RCObject.h"
#include "Util/String.h"


class OmnLogListener : public OmnRCObject
{
public:
	// virtual void	entryAdded(const OmnLogEntryPtr &entry) = 0;
	virtual void	entryAdded(const OmnString &entry) = 0;
	virtual void	logFlushed() = 0;
};
#endif

