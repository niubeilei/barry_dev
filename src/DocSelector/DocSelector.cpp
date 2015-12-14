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
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/DocSelector.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "MultiLang/LangTermIds.h"
#include "Random/RandomRule.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

extern AosDocSelectorPtr	sgSelectors[AosDocSelector::eMax+1];
static OmnMutex				sgLock;

#include "DocSelector/AllSelectors.h"

AosDocSelector::AosDocSelector(
		const OmnString &name,
		const AosDocSelectorType::E type, 
		const bool reg)
:
mType(type),
mLock(OmnNew OmnMutex())
{
	AosDocSelectorPtr thisptr(this, false);
	if (reg) registerSelector(name, thisptr);
}


AosDocSelector::~AosDocSelector()
{
}


bool
AosDocSelector::registerSelector(const OmnString &name, const AosDocSelectorPtr &selector)
{
	AosDocSelectorType::E type = selector->getType();
	if (!AosDocSelectorType::isValid(type))
	{
		OmnAlarm << "Invalid Doc Selector id: " << type << enderr;
		return false;
	}
	mLock->lock();
	if (sgSelectors[type])
	{
		mLock->unlock();
		OmnAlarm << "Doc Selector already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgSelectors[type] = selector;
	AosDocSelectorType::addName(name, type);
	mLock->unlock();
	return true;
}


/*
AosDocSelectorPtr
AosDocSelector::createSelector(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// This function creates a selector. (not sure whether we need it)
	rdata->setError();
	aos_assert_r(def, 0);
	AosDocSelectorType::E type = AosDocSelectorType::toEnum(
		def->getAttrStr(AOSTAG_ZKY_TYPE));
	if (!AosDocSelectorType::isValid(type))
	{
		rdata->setError() << "Invalid doc selector type: " 
			<< def->getAttrStr(AOSTAG_ZKY_TYPE);
		return 0;
	}

	AosDocSelectorPtr selector = sgSelectors[type];
	//AosDocSelector* selector = sgSelectors[type];
	aos_assert_r(selector, 0);
	rdata->setOk();
	return selector->clone(def, rdata);
}
*/


AosDocSelectorPtr
AosDocSelector::getSelectorStatic(
		const AosDocSelectorType::E type, 
		const AosRundataPtr &rdata)
{
	if (!AosDocSelectorType::isValid(type))
	{
		rdata->setError() << "Unrecognized action: " << (int)type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosDocSelectorPtr selector = sgSelectors[type];

	if (!selector)
	{
		rdata->setError() << "Action not defined: " << (int)type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return selector;
}


AosXmlTagPtr
AosDocSelector::selectDocStatic(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This function selects docs. 
	aos_assert_rr(sdoc, rdata, 0);
	AosDocSelectorType::E type = AosDocSelectorType::toEnum(
		sdoc->getAttrStr(AOSTAG_ZKY_TYPE));
	if (!AosDocSelectorType::isValid(type))
	{
		AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sdoc);
		if (jimo)
		{
			bool rslt = jimo->run(rdata, sdoc);
			aos_assert_rr(rslt, rdata, 0);
			return rdata->getWorkingDoc();
		}

		AosSetErrorUser(rdata, "invalid_doc_selector") << ": "
			<< sdoc->toString() << enderr;
		return 0;
	}

	AosDocSelectorPtr selector = getSelectorStatic(type, rdata);
	aos_assert_rr(selector, rdata, 0);
	return selector->selectDoc(sdoc, rdata);
}


AosXmlTagPtr
AosDocSelector::selectDocStatic(
		const AosXmlTagPtr &sdoc, 
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr ss = sdoc->getFirstChild(name);
	if (!ss)
	{
		rdata->setError() << "Selector not found: " << name;
		return 0;
	}

	return selectDocStatic(ss, rdata);
}


/*
AosXmlTagPtr
AosDocSelector::getDocByObjid(const OmnString &objid, const AosRundataPtr &rdata)
{
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, 0);
	return smDocClient->getDocByObjid(objid, rdata);
}


u64
AosDocSelector::getDocidByObjid(const OmnString &objid, const AosRundataPtr &rdata)
{
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_rr(smDocClient, rdata, false);
	return smDocClient->getDocidByObjid(objid, rdata);
}
*/


void
AosDocSelector::addHeader(
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

