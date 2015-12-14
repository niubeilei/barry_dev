////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogType.h
// Description:
//	A system has many logs. Each log has a specific type (we probably 
//  should call this class OmnLogType). This class defines an enum
//  for log IDs.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Logger_LogType_h
#define Omn_Logger_LogType_h

#include "Util/String.h"

class OmnLogType
{
public:
	enum E 
	{
		eFirstValidEntry,

		eLoggerFile,
		eLoggerDb,
		eLoggerStdio,
		eLoggerGroup,

		eLastValidEntry
	};

	static E toEnum(const OmnString &s);
	static bool isValid(const E c)
	{
		return (c > eFirstValidEntry && c < eLastValidEntry);
	}
};
#endif

