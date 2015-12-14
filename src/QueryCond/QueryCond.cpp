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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryCond/QueryCond.h"

#include "Debug/Except.h"
#include "QueryCond/CondArith.h"
#include "QueryCond/CondIds.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static AosQueryCondPtr  sgConds[AosCondId::eMax];
static OmnMutex         sgLock;

AosQueryCond::AosQueryCond(
		const OmnString &name, 
		const AosCondId::E id, 
		const bool regflag)
:
mId(id),
mCondName(name)
{
	if (regflag)
	{
		AosQueryCondPtr thisptr(this, false);
		OmnString errmsg;
		if (!registerCond(thisptr, errmsg))
		{
			OmnExcept e(__FILE__, __LINE__, errmsg);
			throw e;
		}
	}
}


AosQueryCond::~AosQueryCond()
{
}


AosQueryCondPtr
AosQueryCond::parseCondStatic(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	// This function parses the condition. It assumes 'term' is in the 
	// following format:
	// 	<term ...>
	// 		...
	// 		<cond .../>
	// 	</term>
	//
	// If it is a valid condition, it returns the condition.
	aos_assert_rr(term, rdata, 0);
	
	AosXmlTagPtr condtag = term->getFirstChild(AOSTAG_COND);
	if (!condtag)
	{
		rdata->setError() << "Missing condition!";
		return 0;
	}

	OmnString type = condtag->getAttrStr(AOSTAG_TYPE);
	AosQueryCondPtr cond = getCondCloneStatic(type);
	if (!cond)
	{
		rdata->setError() << "Condition not defined: " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	if (cond->parse(condtag, rdata)) return cond;
	return 0;
}


bool
AosQueryCond::registerCond(const AosQueryCondPtr &cond, OmnString &errmsg)
{
	sgLock.lock();
	if (!AosCondId::isValid(cond->mId))
	{
		sgLock.unlock();
		errmsg = "Incorrect Query Condition ID: ";
		errmsg << cond->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgConds[cond->mId])
	{
		sgLock.unlock();
		errmsg = "SeProc already registered: ";
		errmsg << cond->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgConds[cond->mId] = cond;
	bool rslt = AosCondId::addName(cond->mCondName, cond->mId, errmsg);
	sgLock.unlock();
	return rslt;
}


AosQueryCondPtr 
AosQueryCond::getCond(const OmnString &idstr)
{
	sgLock.lock();
	AosCondId::E id = AosCondId::toEnum(idstr);
	if (!AosCondId::isValid(id)) 
	{
		sgLock.unlock();
		return 0;
	}

	AosQueryCondPtr cond = sgConds[id];
	sgLock.unlock();
	return cond;
}


AosQueryCondPtr 
AosQueryCond::getCondCloneStatic(const OmnString &idstr)
{
	AosQueryCondPtr cond = getCond(idstr);
	if (!cond) return 0;
	return cond->clone();
}


AosQueryCondPtr
AosQueryCond::createCond(const AosOpr opr, const u64 &value)
{
	return OmnNew AosCondArith(opr, value);
}
