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
// It defines a new data type: Trivalue. The value can take three
// values:
// 	true
// 	false
// 	unknown
//
// Modification History:
//	Created: 11/26/2007 by Chen Ding 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Trivalue_h
#define Omn_Util_Trivalue_h

#include "Util/BasicTypes.h"
#include "Util/String.h"

enum AosTrivalue
{
	eAosTrivalue_false,
	eAosTrivalue_true,
	eAosTrivalue_unknown
};

#endif
