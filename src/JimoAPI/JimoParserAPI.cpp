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
// 2015/04/01 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoParserAPI.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"

static AosJimoParserObjPtr sgJimoParser = 0;
static OmnMutex sgLock;

static bool loadJimoParser(AosRundata *rdata)
{
	sgLock.lock();
	if(sgJimoParser)
	{
		sgLock.unlock();
		return true;
	}
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata,"AosJimoParser",1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "failed_create_jimo_parser") << enderr;
		return false;
	}

	sgJimoParser = dynamic_cast<AosJimoParserObj *>(jimo.getPtr());
	if (!sgJimoParser)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "failed_create_jimo_parser") << enderr;
		return false;
	}
	sgLock.unlock();
	return true;
}


namespace Jimo
{

AosJimoParserObjPtr jimoCreateJimoParser(AosRundata *rdata)
{
	if(!sgJimoParser)	loadJimoParser(rdata);
	aos_assert_rr(sgJimoParser, rdata, 0);
	return sgJimoParser->createJimoParser(rdata);
}

};

