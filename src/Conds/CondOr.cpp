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
// 12/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondOr.h"

#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

AosCondOr::AosCondOr(const bool reg)
:
AosCondition(AOSCONDTYPE_OR, AosCondType::eOr, reg),
mLock(OmnNew OmnMutex())
{
}


AosCondOr::~AosCondOr()
{
}


bool
AosCondOr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// The condition is in the form:
	// 	<cond AOSTAG_ZKY_TYPE="or" ...>
	// 		<condition ...>
	// 		<condition ...>
	// 		...
	// 	</and>
	aos_assert_rr(def, rdata, false);
	
	AosXmlTagPtr cond_tag = def->getFirstChild();
	while (cond_tag)
	{
		try
		{
			AosCondType::E type = AosCondType::toEnum(cond_tag->getAttrStr(AOSTAG_ZKY_TYPE));
			AosConditionObjPtr cond = getCondition(type, rdata);
			if (!cond)
			{
				OmnAlarm << "Failed creating condition: " << def->toString() << enderr;
				return false;
			}
			mConds.push_back(cond);
			cond_tag = def->getNextChild();
		}

		catch (...)
		{
			OmnAlarm << "Failed creating condition: " << def->toString() << enderr;
			return false;
		}

	}

	rdata->setOk();
	return true;
}


bool 
AosCondOr::evalCond(const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mConds.size() > 0, mLock, false);
	for (u32 i=0; i<mConds.size(); i++)
	{
		aos_assert_rl(mConds[i], mLock, false);
		if (mConds[i]->evalCond(rdata))
		{
			mLock->unlock();
			return true;
		}
	}
	mLock->unlock();
	return false;
}


bool 
AosCondOr::evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		rdata->setError() << ":" << __LINE__ << ": null pointer";
		return false;
	}

	AosXmlTagPtr cond = sdoc->getFirstChild();
	if (!cond)
	{
		rdata->setOk();
		return true;
	}

	while (cond)
	{
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			rdata->setOk();
			return true;
		}
		cond = sdoc->getNextChild();
	}

	rdata->setOk();
	return false;
}


bool
AosCondOr::evalCond(const char *data, int len, const AosRundataPtr &rdata)
{
	aos_assert_r(mConds.size() > 0, false);
	for (u32 i=0; i<mConds.size(); i++)
	{
		aos_assert_r(mConds[i], false);
		if (mConds[i]->evalCond(data, len, rdata))
		{
			return true;
		}
	}
	return false;
}


AosConditionObjPtr
AosCondOr::clone()
{
	try
	{
		AosCondOr * cond = OmnNew AosCondOr(false);
		for(u32 i=0; i<mConds.size(); i++)
		{
			cond->mConds.push_back(mConds[i]->clone());
		}
		return cond;
	}

	catch (...)
	{
		OmnAlarm << "Failed to clone cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondOr::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString cond_str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(cond_str, tname, AOSCONDTYPE_OR, rule);
	cond_str << ">";

	AosConditionObjPtr cond;
	mConds.clear();
	if (level > 0)
	{
		int num = rand()%5;
		for(int i=0; i<num; i++)
		{
			//OmnString str = AosCondRandUtil::pickCondition("condition", level-1, cond, rule, rdata);
			OmnString str;
			aos_assert_r(str != "", "");
			aos_assert_r(cond, "");
			cond_str << str;
			mConds.push_back(cond);
			cond = 0;
		}
	}

	addThenElse(cond_str, then_part, else_part, tname);
	return cond_str;
}


OmnString
AosCondOr::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

