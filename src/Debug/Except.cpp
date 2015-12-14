////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Except.cpp
// Description:
//    This class defines the exception that all MMSN program throws.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Debug/Except.h"

#include "Debug/Debug.h"
#include "Alarm/Alarm.h"


OmnExcept::OmnExcept(const OmnString &file, const int line, const OmnString &errmsg)
:
mFile(file),
mLine(line),
mErrmsg(errmsg),
mErrid(OmnErrId::eAlarm)
{
	// Ketty RlbTest tmp.
	OmnAlarm << "Exception created: " << file << ":" << line << ":" << errmsg << enderr;
}


OmnExcept::OmnExcept(const OmnString &file,
		const int line,
		const OmnErrId::E errid,
		const OmnString &errMsg)
:
mFile(file),
mLine(line),
mErrmsg(errMsg),
mErrid(errid)
{ 
	// Ketty RlbTest tmp.
	OmnAlarm << mErrmsg << enderr;
}


OmnExcept::~OmnExcept()
{
}


OmnExcept::ExceptRslt
OmnExcept::procException() const
{
	OmnNotImplementedYet;
	return eInvalid;
}
