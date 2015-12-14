////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogType.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Logger/LogType.h"

OmnLogType::E
OmnLogType::toEnum(const OmnString &t)
{
	OmnString type = t;
	type.toLower();
	if (type == "filelogger")
	{
		return eLoggerFile;
	}

	if (type == "dblogger")
	{
		return eLoggerDb;
	}

	if (type == "grouplogger")
	{
		return eLoggerGroup;
	}

	if (type == "stdiologger")
	{
		return eLoggerStdio;
	}

	return eFirstValidEntry;
}

