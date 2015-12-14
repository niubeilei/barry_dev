////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemObj.h
// Description:
//   This class defines an interface. Any class that maintains large 
//   memory must implement this interface and must register with 
//   OmnMemMgr so that when the system runs out of memory, their 
//   member functions can be called to release up some space.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_MemObj_h
#define Omn_Util1_MemObj_h

#include "Util/RCObject.h"

class OmnMemObj : public OmnRCObject
{
public:
	//
	// Memory manager captures a call that the application is running out of
	// memory. It is important for the called to release some memory immediately.
	// The called should return an uint indicating how many bytes it released.
	//
	virtual uint	releaseMemory() = 0;
};
#endif