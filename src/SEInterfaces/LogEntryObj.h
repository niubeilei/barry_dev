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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_LogEntryObj_h
#define Aos_SEInterfaces_LogEntryObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/FieldName.h"

class AosEndLogEntry;

class AosLogEntryObj : public AosJimo
{
public:
	AosLogEntryObj(const int version);
	virtual ~AosLogEntryObj();

	virtual AosLogEntryObj & operator << (const AosFieldName::E name) = 0;
	virtual AosLogEntryObj & operator << (const u64 value) = 0;
	virtual AosLogEntryObj & operator << (const u32 value) = 0;
	virtual AosLogEntryObj & operator << (const u8 value) = 0;
	virtual AosLogEntryObj & operator << (const i64 value) = 0;
	virtual AosLogEntryObj & operator << (const i32 value) = 0;
	virtual AosLogEntryObj & operator << (const i8 value) = 0;
	virtual AosLogEntryObj & operator << (const float value) = 0;
	virtual AosLogEntryObj & operator << (const double value) = 0;
	virtual AosLogEntryObj & operator << (const OmnString &value) = 0;
	virtual AosLogEntryObj & operator << (const AosXmlTagPtr &value) = 0;
	virtual void operator << (const AosEndLogEntry &) = 0;
};
#endif
