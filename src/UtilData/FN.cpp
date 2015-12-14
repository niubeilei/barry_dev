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
//
// Modification History:
// 2015/03/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilData/FN.h"

#include "Util/String.h"


OmnString 
AosFN::toStr(const E code)
{
	OmnString ss;
	ss << code;
	return ss;
}

