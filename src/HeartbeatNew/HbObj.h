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
#ifndef Omn_HeartbeatNew_HbObj_h
#define Omn_HeartbeatNew_HbObj_h



class AosHbObj
{
public:
	virtual void	heartbeatFailed(const u64 &monitorId) = 0; 
};

#endif
