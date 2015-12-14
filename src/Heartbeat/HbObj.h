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
//	This interface is used to call back when the heartbeat connection
//	detects a remote peer fails.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Heartbeat_HbObj_h
#define Omn_Heartbeat_HbObj_h

#include "Util/RCObject.h"


class AosHbObj : virtual public OmnRCObject
{
public:
	virtual void	heartbeatRestored(const u64 &monitorId) = 0;
	virtual void	heartbeatFailed(const u64 &monitorId) = 0; 
};

#endif
