////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThrdShellParm.h
// Description:
//	This class is used to pass ThreadShell Processor parameters.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_ThrdShellParm_h
#define Omn_Thread_ThrdShellParm_h

#include "Util/RCObject.h"

class OmnThrdShellParm : virtual public OmnRCOBject
{
	OmnDefineRCObject;

public:
	int		getParmId() const = 0;
};
#endif

