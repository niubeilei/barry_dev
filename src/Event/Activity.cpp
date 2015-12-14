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
//
// Modification History:
// 12/10/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "Event/Activity.h"

#include "Util/a_string.h"


std::string		
AosActivity::toString()
{
	std::string str;
	str << "Activity: " 
		<< "\n    ActivityId:   " << mActivityId
		<< "\n    Filename:     " << mFilename
		<< "\n    Lineno:       " << mLineno
		<< "\n    Instance:     " << mInst;
	return str;
}


