////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerObj.h
// Description:
//	If an object wants OmnTimer to call back, it must implement this 
//  interface.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_TimerObj_h
#define Omn_Util1_TimerObj_h

#include "Util/RCObject.h"
#include "Util/String.h"


class OmnTimerObj : public virtual OmnRCObject
{
public:
	virtual void		timeout(const int timerId, 
								const OmnString &timerName,
								void *parm) = 0;
};
#endif
