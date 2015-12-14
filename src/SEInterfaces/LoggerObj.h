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
#ifndef Aos_SEInterfaces_LoggerObj_h
#define Aos_SEInterfaces_LoggerObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/LogEntryObj.h"
#include "SEInterfaces/Ptrs.h"
#include <hash_map>

using namespace std;

class AosEndLogEntry
{
};

class AosBSON;

class AosLoggerObj : public AosJimo
{
	typedef hash_map<const OmnString, AosLoggerObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosLoggerObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	static map_t		smLoggers;

public:

	AosLoggerObj(const int version);
	virtual ~AosLoggerObj();

	virtual AosLogEntryObj & getLogEntry(AosRundata *rdata) = 0;
	virtual bool closeEntry(AosRundata *rdata, AosLogEntryObj *entry) = 0;
	virtual bool closeLogEntry(AosRundata *rdata, const AosBSON *bson) = 0;
	
	static AosLoggerObj *getLogger(AosRundata *rdata, const OmnString &logname);

	static AosLoggerObjPtr createLogger(
							AosRundata *rdata, 
							const OmnString &classname, 
							const OmnString &logname, 
							const int version);

private:
	static bool init(AosRundata *rdata);
};

#define AOSLOGGER_SYSLOG 					"_syslog"
#define AOSLOGGER_JIMOCALL 					"_jimocalls"

#endif
