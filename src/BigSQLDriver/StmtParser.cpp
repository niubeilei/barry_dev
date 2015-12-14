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
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigSQLDriver/StmtParser.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"

static OmnMutex sgLock;
static AosSqlStmtParserPtr	sgParsers[AosSqlStmtType::eMaxInvalid];


AosSqlStmtParser::AosSqlStmtParser(
	const OmnString &name, 
	const AosSqlStmtType::E type, 
	const bool regflag)
{
	mType = type;
	if (regflag) 
	{
		AosSqlStmtParserPtr thisptr(this, false);
		registerStmt(thisptr, name);
	}
}


AosSqlStmtParser::~AosSqlStmtParser()
{
}


bool
AosSqlStmtParser::registerStmt(
		const AosSqlStmtParserPtr &parser, 
		const OmnString &name)
{
	AosSqlStmtType::E type = parser->getType();
	aos_assert_r(type > AosSqlStmtType::eMinInvalid && type < AosSqlStmtType::eMaxInvalid, false);

	sgLock.lock();
	if (sgParsers[type])
	{
		sgLock.unlock();
		OmnAlarm << "Parser already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgParsers[type] = parser;
	OmnString errmsg;
	bool rslt = AosSqlStmtType::addName(name, type, errmsg);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add name: " << errmsg << enderr;
	}
	return rslt;
}


AosSqlStmtParserPtr
AosSqlStmtParser::getParser(
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	AosSqlStmtType::E type = AosSqlStmtType::toEnum(name);
	if (!AosSqlStmtType::isValid(type)) return false;
	return sgParsers[type];
}

