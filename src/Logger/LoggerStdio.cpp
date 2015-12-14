////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LoggerStdio.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Logger/LoggerStdio.h"

#include "Logger/LogListener.h"

OmnStdioLogger::OmnStdioLogger(const OmnString &name)
:
OmnLogger(OmnLogType::eLoggerStdio, name)
{
}


OmnStdioLogger::~OmnStdioLogger()
{
}


bool
OmnStdioLogger::openLogger()
{
	return true;
}


bool
OmnStdioLogger::closeLogger()
{
	return true;
}


void
OmnStdioLogger::log(const OmnString &contents)
{
	std::cout << contents << std::endl;
}


bool
OmnStdioLogger::config(const OmnXmlItemPtr &conf)
{
	return true;
}
	

#endif
