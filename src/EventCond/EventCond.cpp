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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EventCond/EventCond.h"

#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

static bool				sgEventCondInited = false;
static AosEventCond*	sgEventCond[AosEventCondType::eMax+1];
static OmnMutex 		sgLock;

AosEventCond::AosEventCond(const AosEventCondType::E type, const bool reg)
:
mType(type),
mLock(OmnNew OmnMutex())
{
	if (reg) registerEventCond(this);
}

AosEventCond::AosEventCond()
{
}

AosEventCond::~AosEventCond()
{
}


bool
AosEventCond::registerEventCond(AosEventCond*  cond)
{
	AosEventCondType::E type = cond->getType();
	if (type <= AosEventCondType::eInvalid || type >= AosEventCondType::eMax)
	{
		OmnAlarm << "Invalid cond id: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (!sgEventCondInited)
	{
		for (int i=0; i<AosEventCondType::eMax+1; i++)
		{
			sgEventCond[i] = 0;
		}
		sgEventCondInited = true;
	}

	if (!sgEventCond[type])
	{
		sgEventCond[type] = cond;
	}
	sgLock.unlock();
	return true;
}


bool 
AosEventCond::evalConds(
		const AosXmlTagPtr &conds, 
		bool &rslt,
		const AosRundataPtr &rdata)
{
	// This function evaluates the conditions 'conds', which is
	// a linked list. If all conditions are evaluated true, 
	// 'rslt' is set to true. Otherwise, it is false. 
	//
	// IMPORTANT: The function return value is NOT used as the
	// result of the condition evaluation. It is used to indicate
	// whether errors occured during the evaluation.
	
	rslt = false;
	AosXmlTagPtr crtcond = conds;
	AosEventCond *cond;
	int guard = AosSmartDoc::eMaxConds;
	while (crtcond && guard--)
	{
		AosEventCondType::E cond_id = AosEventCondType::toEnum(
				crtcond->getAttrStr(AOSTAG_CONDITIONID));
		if (!AosEventCondType::isValid(cond_id))
		{
			rdata->setError() << "Condition unrecognized: "
				<< crtcond->toString();
			return false;
		}
		cond = sgEventCond[cond_id];
		if (!cond)
		{
			rdata->setError() << "Condition not registered: "
				<< crtcond->toString();
			return false;
		}

		if (!cond->evalCond(crtcond, rslt, rdata))
		{
			return false;
		}

		crtcond = crtcond->getNext();
	}

	if (guard <= 0)
	{
		OmnAlarm << "Internal error!" << enderr;
		rdata->setError() << "Internal Error";
		return false;
	}

	return true;

	/*
	OmnString logic = conds->getAttrStr("logic");
	AosXmlTagPtr subxml = conds->getNextChild();

	OmnString tag = subxml->getTagname();
	if(tag == "Condition")
	{
		AosEventCondType::E type = AosEventCondType::toEnum(subxml->getAttrStr(AOSTAG_OPR));
		aos_assert_r(type > AosEventCondType::eInvalid && type < AosEventCondType::eMax, 0);
		AosEventCond* cond = sgEventCond[type];
		aos_assert_r(cond, 0);
		rslt = cond->evalCond(subxml, rdata);
	}
	else if(tag == "Conditons")
	{
		evalConds(subxml, rslt, rdata);
	}

	subxml = conds->getNextChild();
	while(subxml)
	{
		OmnString tag = subxml->getTagname();
		if(tag == "Condition")
		{
			AosEventCondType::E type = AosEventCondType::toEnum(subxml->getAttrStr(AOSTAG_OPR));
			aos_assert_r(type > AosEventCondType::eInvalid && type < AosEventCondType::eMax, 0);
			AosEventCond* cond = sgEventCond[type];
			aos_assert_r(cond, 0);
			bool rslt1 = cond->evalCond(subxml, rdata);
			rslt = logicOpr(logic, rslt, rslt1);
		}
		else if(tag == "Conditions")
		{
			bool rslt1;
			evalConds(subxml, rslt1, rdata);
			rslt = logicOpr(logic, rslt, rslt1);
		}
		subxml = conds->getNextChild();
	}
	return rslt;
	*/
}


bool
AosEventCond::logicOpr(
		const OmnString &logic,
		const bool &b1,
		const bool &b2)
{
	const char *logc = logic.data();
	switch(logc[0])
	{
		case 'a':
			if (logic == "and") return (b1 && b2);

		case 'o':
			if(logic == "or")   return (b1 || b2);

		default:
			OmnAlarm << "have something wrong!" << enderr;
			break;
	}

	OmnAlarm << "have something wrong!" << enderr;
	return false;
}
