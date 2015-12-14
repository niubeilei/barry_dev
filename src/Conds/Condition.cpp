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
// This is a utility to select docs.
//
// Modification History:
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/Condition.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Conds/AllConds.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"

static AosConditionObjPtr	sgConditions[AosCondType::eMax+1];
static OmnMutex 		sgLock;
static bool				sgInited = false;

AosCondition::AosCondition(
		const OmnString &name, 
		const AosCondType::E type, 
		const bool reg)
{
	mType = type;
	AosConditionObjPtr thisptr(this, false);
	if (reg) registerCondition(name, thisptr);
}


AosCondition::~AosCondition()
{
}


bool
AosCondition::initConditions()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	sgInited = true;
	AosInitConditions();
	sgLock.unlock();
	return true;
}


bool
AosCondition::registerCondition(const OmnString &name, const AosConditionObjPtr &cond)
{
if (name == AOSCONDTYPE_ARITH) 
	OmnMark;
	AosCondType::E type = cond->getType();
	if (!AosCondType::isValid(type))
	{
		sgLock.unlock();
		OmnAlarm << "Invalid condition: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (sgConditions[type])
	{
		sgLock.unlock();
		OmnScreen << "Condition already registered: " << name << ":" << type << endl;
		return true;
	}

	sgConditions[type] = cond;
	AosCondType::addName(name, type);
	sgLock.unlock();
	return true;
}


AosConditionObjPtr
AosCondition::getCondition(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
{
	if (!sgInited)
	{
		sgInited = true;
		AosInitConditions();
	}

	// Chen Ding, 2013/07/14
	OmnString typestr = conf->getAttrStr(AOSTAG_TYPE);
	if (typestr == "")
	{
		// Backward compatibility
		typestr = conf->getAttrStr(AOSTAG_ZKY_TYPE);
	}

	// Chen Ding, 2013/07/14
	// AosCondType::E type = AosCondType::toEnum(conf->getAttrStr(AOSTAG_ZKY_TYPE));
	AosCondType::E type = AosCondType::toEnum(typestr);
	AosConditionObjPtr cond = getCondition(type, rdata);
	if (!cond) return 0;
	bool rslt = cond->config(conf, rdata);
	aos_assert_r(rslt, 0);
	return cond;
}


AosConditionObjPtr
AosCondition::getCondition(
		const AosCondType::E type, 
		const AosRundataPtr &rdata)
{
	if (!sgInited)
	{
		sgInited = true;
		AosInitConditions();
	}

	if (!AosCondType::isValid(type))
	{
		rdata->setError() << "Unrecognized condition: " << (int)type ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosConditionObjPtr cond = sgConditions[type];
	if (!cond)
	{
		rdata->setError() << "Action not defined: " << (int)type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return cond->clone();
}


bool
AosCondition::evalCondsStatic(
		const AosXmlTagPtr &conds, 
		const int max,
		const AosRundataPtr &rdata)
{
	// This function evaluates a group of conditions. The conditions are 
	// defined by 'conds', which is in the following format:
	// 	<conds ...>
	// 		<cond .../>
	// 		<cond .../>
	// 		...
	// 	</conds>
	// These conditions are ANDed. It returns as soon as one condition
	// evaluates false. 
	
	if (!sgInited)
	{
		sgInited = true;
		AosInitConditions();
	}

	if (!conds)
	{
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr cond_tag = conds->getFirstChild();
	int guard = max;
	while (guard-- && cond_tag)
	{
		AosCondType::E type = AosCondType::toEnum(cond_tag->getAttrStr(AOSTAG_ZKY_TYPE));
		if (!AosCondType::isValid(type))
		{
			AosSetError(rdata, "invalid_cond_type:") << cond_tag->getAttrStr(AOSTAG_ZKY_TYPE);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosConditionObjPtr cond = getCondition(type, rdata);
		if (!cond)
		{
			AosSetError(rdata, "internal_error:") << cond_tag->getAttrStr(AOSTAG_ZKY_TYPE);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (!cond->evalCond(cond_tag, rdata)) return false;
		cond_tag = conds->getNextChild();
	}

	aos_assert_rr(guard, rdata, false);
	return true;
}


bool
AosCondition::evalCondStatic(
		const AosXmlTagPtr &cond_tag, 
		const AosRundataPtr &rdata)
{
	if (!sgInited)
	{
		sgInited = true;
		AosInitConditions();
	}

	// This function selects docs. 
	if (!cond_tag)
	{
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosCondType::E type = AosCondType::toEnum(cond_tag->getAttrStr(AOSTAG_ZKY_TYPE));
	if (!AosCondType::isValid(type))
	{
		AosSetError(rdata, "invalid_cond_type:") << cond_tag->getAttrStr(AOSTAG_ZKY_TYPE);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosConditionObjPtr cond = getCondition(type, rdata);
	if (!cond)
	{
		AosSetError(rdata, "internal_error:") << cond_tag->getAttrStr(AOSTAG_ZKY_TYPE);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return cond->evalCond(cond_tag, rdata);
}


bool 
AosCondition::evalCondStatic(
		const AosXmlTagPtr &sdoc, 
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	if (!sgInited)
	{
		sgInited = true;
		AosInitConditions();
	}

	AosXmlTagPtr ss = sdoc->getFirstChild(name);
	if (!ss)
	{
		rdata->setError() << "Condition not found: " << name;
		return false;
	}

	return evalCondStatic(ss, rdata);
}


/*
OmnString 
AosCondition::getRandXml(
		const OmnString &tagname,
		AosRandomRulePtr &rule, 
		const int idx, 
		const AosRundataPtr &rdata)
{
	int i = idx;
	if(idx<1 || idx >4)
		i = rand()%(AosCondType::eMax-1)+1;

	rule = OmnNew AosRandomRule(AosCondType::toStr(sgConditions[i]->mType));
	return sgConditions[i]->getXmlStr(tagname, rule, rdata);

}
*/


OmnString
AosCondition::getTagname(
		const OmnString &tagname,
		const AosRandomRulePtr &rule)
{
	if (tagname != "") return tagname;
	if (rule && rule->mNodeName != "") return rule->mNodeName;
	return "cond";
}


void
AosCondition::addHeader(
		OmnString &str,
		const OmnString &tagname,
		const OmnString &type,
		const AosRandomRulePtr &rule)
{
	str << tagname << " " << AOSTAG_ZKY_TYPE << "=\"" << type << "\" ";
	if (rule)
	{
		XmlGenRuleValueIter itr;
		for (itr=rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
		{
			str << itr->first << "=\"" << (itr->second).getRandomValue()<<"\" ";
		}

		for(itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
		{
			if(rand()%10<8)
			{
				str << itr->first <<"=\"" << (itr->second).getRandomValue() <<"\" ";
			}
		}
	}
}


void
AosCondition::addThenElse(
		OmnString &str,
		const OmnString &then_part,
		const OmnString &else_part, 
		const OmnString &tagname)
{
	if (then_part != "")
	{
		str << then_part;
	}

	if (else_part != "")
	{
		str << else_part;
	}

	str << "</" << tagname << ">";
}


OmnString 
AosCondition::getXmlStr(
		const OmnString &tagname,
		const OmnString &then_part,
		const OmnString &else_part,
		const OmnString &type,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<"; 
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, type, rule);
	str << ">";
	str << "</" << tname << ">";
	return str;
}


OmnString 
AosCondition::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule,
	 	const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return "";
}


OmnString 
AosCondition::generateCommand(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return "";
}


bool
AosCondition::evalCond(const char *record, const int len, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosCondition::evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosCondition::evalCond(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosCondition::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

