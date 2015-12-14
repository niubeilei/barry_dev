////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysConsole/ConsoleRslts.h"

AosConsoleRslt::AosConsoleRslts()
{
}


AosConsoleRslt::~AosConsoleRslts()
{
}


bool 
AosConsoleRslt::addResults(
		const OmnString &info_id, 
		const OmnString &contents, 
		const bool finished)
{
	mEntries.push_back(info_id, contents, finished);
	return true;
}
#endif
