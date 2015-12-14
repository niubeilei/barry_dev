////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IdMgr.h
// Description:
//	This class manages a collection of IDs. It ensures the uniqueness
//  of these IDs. This is a utility.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_IdMgr_h
#define Snt_Util_IdMgr_h

#include "aosUtil/Types.h"
#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"

class OmnIdMgr 
{
public:
	static uint		getUintId();
	static int64_t	getLLId();
};
#endif

