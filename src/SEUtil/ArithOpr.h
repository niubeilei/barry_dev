////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/24/2015 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_ArithOpr_h
#define Omn_SEUtil_ArithOpr_h

#include "Util/String.h"


struct ArithOpr
{
	enum E 
	{
		eInvalid,

		eAdd,
		eSub,
		eMul,
		eDiv, 
		eMod,

		eMax
	};

};


#endif
