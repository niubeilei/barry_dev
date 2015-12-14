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
// 03/26/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/StatTypes.h"

#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

bool AosStatType::smInited = false;
char AosStatType::smMap[AosStatType::eMaxEntry];
static OmnMutex sgLock;

bool
AosStatType::init()
{
	sgLock.lock();
	if (smInited)
	{
		sgLock.unlock();
		return true;
	}

	smMap[eInvalid] 			= 'a';
	smMap[eOrigStatStart]		= 'b';
	smMap[eCount]				= 'c';
	smMap[eValue]				= 'd';
	smMap[eValueCount]			= 'e';
	smMap[eSum]					= 'f';
	smMap[eTimeSum]				= 'g';
	smMap[eOrigStatEnd]			= 'h';
	smMap[eMin]					= 'i';
	smMap[eMax]					= 'j';
	smMap[eAverage]				= 'k';
	smMap[eMean]				= 'l';
	smMap[eStandardError]		= 'm';
	smMap[eMaxEntry]			= 'n';
	
	smInited = true;
	sgLock.unlock();
	return true;
}


bool
AosStatType::retrieveStatTypes(vector<E> &stattypes, const AosXmlTagPtr &def)
{
	if (def->getAttrBool(AOSSTATTYPE_SUM))
		stattypes.push_back(toEnum(AOSSTATTYPE_SUM));
	if (def->getAttrBool(AOSSTATTYPE_AVERAGE))
	{
		//avg = sum/count;
		if (stattypes.size() == 0)stattypes.push_back(toEnum(AOSSTATTYPE_SUM));
		stattypes.push_back(toEnum(AOSSTATTYPE_COUNT));
	}
	if (def->getAttrBool(AOSSTATTYPE_MAX))
		stattypes.push_back(toEnum(AOSSTATTYPE_MAX));
	if (def->getAttrBool(AOSSTATTYPE_MIN))
		stattypes.push_back(toEnum(AOSSTATTYPE_MIN));
	if (stattypes.size() == 0) 
		stattypes.push_back(toEnum(AOSSTATTYPE_SUM));
	return true;
}

