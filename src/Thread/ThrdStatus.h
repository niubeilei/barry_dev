////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThrdStatus.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_ThrdStatus_h
#define Omn_Thread_ThrdStatus_h

#include "Util/String.h"


class OmnThrdStatus
{
public:
	enum E
	{
		eIdle,
		eStop,
		eStopped,
		eActive,
		eExit,
		eExited
	};
};
#endif
