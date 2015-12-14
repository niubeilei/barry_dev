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
#include "Conds/CondAnd.h"

#include "Conds/Condition.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"



AosCondAnd::AosCondAnd(const bool reg)
:
AosCondition(AOSCONDTYPE_AND, AosCondType::eAnd, reg),
mLock(OmnNew OmnMutex())
{
}


AosCondAnd::~AosCondAnd()
{
}


bool
AosCondAnd::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The condition is in the form:
	// 	<cond AOSTAG_ZKY_TYPE="and" ...>
	// 		<condition ...>
	// 		<condition ...>
	// 		...
	// 	</and>
	aos_assert_r(def, false);
	
	AosXmlTagPtr cond_tag = def->getFirstChild(true);
	while (cond_tag)
	{
		try
		{
			AosCondType::E type = AosCondType::toEnum(cond_tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				AosConditionObjPtr cond = getCondition(cond_tag, rdata);
				if (!cond)
				{
					OmnAlarm << "Failed creating condition: " << def->toString() << enderr;
					return false;
				}
				mConds.push_back(cond);
			}
			cond_tag = def->getNextChild();
		}

		catch (...)
		{
			OmnAlarm << "Failed creating condition: " << def->toString() << enderr;
			return false;
		}

	}

	return true;
}


bool 
AosCondAnd::evalCond(const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mConds.size() > 0, mLock, false);
	for (u32 i=0; i<mConds.size(); i++)
	{
		aos_assert_rl(mConds[i], mLock, false);
		if (!mConds[i]->evalCond(rdata))
		{
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();
	return true;
}


bool 
AosCondAnd::evalCond(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The condition is in the form:
	// 	<cond AOSTAG_ZKY_TYPE="and" ...>
	// 		<condition ...>
	// 		<condition ...>
	// 		...
	// 	</and>
	if (!def)
	{
		rdata->setError() << __FILE__ << ":" << __LINE__ << ": null pointer";
		return false;
	}

	AosXmlTagPtr cond = def->getFirstChild(true);
	if (!cond)
	{
		rdata->setOk();
		return true;
	}

	while (cond)
	{
		if (!AosCondition::evalCondStatic(cond, rdata)) return false;

		cond = def->getNextChild();
	}

	rdata->setOk();
	return true;
}


AosConditionObjPtr
AosCondAnd::clone()
{
	try
	{
		AosCondAnd * cond = OmnNew AosCondAnd(false);
		for(u32 i=0; i<mConds.size(); i++)
		{
			cond->mConds.push_back(mConds[i]->clone());
		}
		return cond;
	}

	catch (...)
	{
		OmnAlarm << "Failed to clone valueset: " << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondAnd::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString cond_str = "<"; 
	OmnString tname = getTagname(tagname, rule);
	addHeader(cond_str, tname, AOSCONDTYPE_AND, rule);
	cond_str << ">";

	mConds.clear();
	AosConditionObjPtr cond;
	if (level > 0)
	{
		int num = rand()%5;
		for(int i=0; i<num; i++)
		{
			//OmnString str = AosCondRandUtil::pickCondition("condition", level-1, cond, rule, rdata);
			OmnString str;
			aos_assert_r(str != "", "");
			aos_assert_r(cond, "");
			mConds.push_back(cond);
			cond = 0;
			cond_str << str;
		}
	}

	addThenElse(cond_str, then_part, else_part, tname);
	return cond_str;
}


OmnString
AosCondAnd::generateCommand(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


bool
AosCondAnd::evalCond(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	//aos_assert_r(mConds.size() > 0, false);

	for (u32 i=0; i<mConds.size(); i++)
	{
		aos_assert_r(mConds[i], false);

		if (!mConds[i]->evalCond(record, rdata))
		{
			return false;
		}
	}
	return true;
}

