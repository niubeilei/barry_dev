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
// 2015/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoJQLParser.h"

#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JQLParserObj.h"


static AosJQLParserObjPtr sgJQLParser;
static AosJQLParserObj * sgJQLParserRaw = 0;
static OmnMutex sgLock;
static int sgJQLParserVersion = AOS_JQLPARSER_OLD_VERSION;

static bool loadJimoParser(AosRundata *rdata)
{
	sgLock.lock();
	if(sgJQLParserRaw)
	{
		sgLock.unlock();
		return true;
	}
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata,"AosJQLParserWrapper", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "failed_create_jql_parser") << enderr;
		return false;
	}

	sgJQLParser = dynamic_cast<AosJQLParserObj *>(jimo.getPtr());
	if (!sgJQLParser)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "failed_create_jimo_parser") << enderr;
		return false;
	}
	sgJQLParserRaw = sgJQLParser.getPtr();
	sgLock.unlock();
	return true;
}


namespace Jimo
{

AosJQLParserObj *jimoGetJQLParser(AosRundata *rdata)
{
	if(!sgJQLParserRaw)	loadJimoParser(rdata);
	aos_assert_rr(sgJQLParserRaw, rdata, 0)
	return sgJQLParserRaw;
}

int jimoGetJQLParserVersion() 
{
	return sgJQLParserVersion;
}

};

