////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ExceptId.h
// Description:
//    This class defines the Exception IDs that MMSN uses.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_ExceptId_h
#define Omn_Debug_ExceptId_h


#include "Util/String.h"



class OmnExceptId
{
public:
	enum E
	{
		eInvalidExceptionId,
		eGeneralException,

		eIndexOutofBound,
		eIncorrectAccessSwitchId,				// Each access switch must have a unique switch ID.
		eMissingProperty
	};

	static OmnString getName(const E e);
};
#endif
