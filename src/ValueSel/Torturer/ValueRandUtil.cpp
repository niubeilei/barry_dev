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
// 01/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/Torturer/ValueRandUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DbQuery/Query.h"
#include "Random/DbRand.h"
#include "Random/RandomUtil.h"
#include "Random/CommonValues.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelRandObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryObj.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ValueDefs.h"
#include "Util/ValueRslt.h"
#include "ValueSel/ValueSelConst.h"
#include "ValueSel/ValueSelAttr.h"

static AosDocClientObjPtr sgDocClient;
static AosQueryObjPtr sgQueryObj;

OmnString 
AosValueRandUtil::pickValueSel(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnString tname = AosValueSel::getTagnameStatic(tagname, rule);
	aos_assert_r(tname != "", "");
	AosDocSelRandObjPtr docselector = AosDocSelRandObj::getDocSelRand();
	aos_assert_r(docselector, "");

	switch (OmnRandom::nextInt(0, 1))
	{
	case 0:
		 return AosValueSelConst::getXmlStr(tname, level, 
				 AosCommonValues::pickAttrValue(), rdata);

	case 1:
		 return AosValueSelAttr::getXmlStr(tname, level,
				 pickDataType(), 
				 AosCommonValues::pickXpath(),
				 docselector->pickDocSelector("docselector", level-1, rule, rdata), 
				 AosCommonValues::pickAttrValue(),
				 rdata);

	default:
		 break;
	}

	OmnNotImplementedYet;
	return "";
}


OmnString 
AosValueRandUtil::pickLhsSelector(
		const OmnString &tagname, 
		const int level,
		const OmnString &dft_value,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// LHS mostly means to pick an attribute name. 
	return AosValueSelConst::getXmlStr(tagname, level, 
			AosCommonValues::pickAttrName(), rdata);
	return "";
}


OmnString 
AosValueRandUtil::pickRhsSelector(
		const OmnString &tagname, 
		const int level,
		const OmnString &dft_value,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// RHS mostly means to pick an attribute value.
	return AosValueSelConst::getXmlStr(tagname, level, 
			AosCommonValues::pickAttrValue(), rdata);
}


OmnString 
AosValueRandUtil::pickSingleValueSelector(
		const OmnString &tagname,
		const int level,
		const OmnString &dft_value,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// This function creates a value selector that selects a single
	// string value.
	if (OmnRandom::percent(rule, AOSTAG_MATCH_SOURCE, 80))
	{
		// This means that another function has determined the 
		// source, which is stored in rdata's AOSARG_VALUESEL_SOURCE arg.
		OmnString type = rdata->getArg1(AOSARG_VALUESEL_SOURCE);
		if (type == AOSVALUE_USERDOMAIN)
		{
			return getSingleValueFromUserDomain(tagname, level, dft_value, rule, rdata);
		}

		if (type == AOSVALUE_CONTAINER)
		{
			return getSingleValueFromContainer(tagname, level, "", "", dft_value, rule, rdata);
		}
	}

	switch (OmnRandom::percent(30, 30))
	{
	case 0:
		 return getSingleValueFromUserDomain(tagname, level, dft_value, rule, rdata);

	case 1:
	default:
		 return getSingleValueFromContainer(tagname, level, "", "", dft_value, rule, rdata);
	}	

	OmnShouldNeverComeHere;
	return "";
}


OmnString 
AosValueRandUtil::pickMultiValuesSelector(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// This function creates a value selector that lists a number
	// of values.. If AOSARG_CTNR_OBJID is specified, it will generate 
	// valus based on the attribute AOSTAG_SELECTED_ATTR from
	// the docs in that container. 
	switch (OmnRandom::percent(30, 30))
	{
	case 0:
		 return getMulValuesFromUserDomain(tagname, level, rule, rdata);

	case 1:
	default:
		 return getMulValuesFromContainer(tagname, level, "", "", rule, rdata);
	}	

	OmnShouldNeverComeHere;
	return "";
}



OmnString
AosValueRandUtil::getMulValuesFromUserDomain(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// It generates a value listing selector from a container. It picks
	// a number of docs from the container. It then randomly picks 
	// docs from the retrieved docs, and then retrieve the values
	// from the docs.
	OmnString user_domain = AosDbRand::pickUserDomain(rdata);
	aos_assert_r(user_domain != "", "");
	return getMulValuesFromContainer(tagname, level, user_domain, 
			AOSTAG_USERNAME, rule, rdata);
}


OmnString
AosValueRandUtil::getMulValuesFromContainer(
		const OmnString &tagname,
		const int level,
		const OmnString &container_objid,
		const OmnString &attrname,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// It generates a value listing selector from a container. It picks
	// a number of docs from the container. It then randomly picks 
	// docs from the retrieved docs, and then retrieve the values
	// from the docs.
	
	// 1. Determine the container
	OmnString ctnr_objid = container_objid;
	if (ctnr_objid == "") ctnr_objid = AosDbRand::pickContainer(rdata);
	aos_assert_r(ctnr_objid != "", "");

	// 2. Determine the attribute name
	OmnString aname = attrname;
	if (aname == "") aname = AosCommonValues::pickAttrName();
	aos_assert_r(aname != "", "");
	rdata->setArg1(AOSARG_VALUESEL_SOURCE, AOSVALUE_CONTAINER);

	// 3. Retrieve doc's objids
	vector<OmnString> objids;
	if (!sgQueryObj) sgQueryObj = AosQueryObj::getQueryObj();
	aos_assert_r(sgQueryObj, "");
	int total = sgQueryObj->retrieveContainerMembers(ctnr_objid, objids, 
			eMaxDocsPerCtnr, 0, false, rdata);
	if (total <= 0) return "";

	OmnString sep = rule->getEntrySeparator(",");
	aos_assert_r(sep != "", "");
	OmnString values;
	int step = OmnRandom::intByRange(
				1, 5, 50,
				6, 10, 30,
				11, 15, 10,
				16, 30, 5);
	int idx = 0;

	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(sgDocClient, "");

	for (u32 i=0; i<objids.size(); i+=step)
	{
		OmnString objid = objids[i];
		aos_assert_r(objid != "", "");
		AosXmlTagPtr doc = sgDocClient->getDocByObjid(objid, rdata);
		aos_assert_r(doc, "");
		OmnString value = doc->getAttrStr(aname);
		if (value != "")
		{
			if (idx != 0) values << sep;
			values << value;
			idx++;
		}
	}

	// In the current implementations, we only generate AosValueSelConst.
	return AosValueSelConst::getXmlStr(tagname, level, values, rdata);
}


OmnString
AosValueRandUtil::getSingleValueFromUserDomain(
		const OmnString &tagname,
		const int level,
		const OmnString &dft_value,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// It generates a value listing selector from a container. It picks
	// a number of docs from the container. It then randomly picks 
	// docs from the retrieved docs, and then retrieve the values
	// from the docs.
	OmnString user_domain = AosDbRand::pickUserDomain(rdata);
	if (user_domain == "") return "";
	return getSingleValueFromContainer(tagname, level, user_domain, 
			AOSTAG_USERNAME, dft_value, rule, rdata);
}


OmnString
AosValueRandUtil::getSingleValueFromContainer(
		const OmnString &tagname,
		const int level,
		const OmnString &container_objid,
		const OmnString &attrname,
		const OmnString &dft_value,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	// It generates a value listing selector from a container. It picks
	// a number of docs from the container. It then randomly picks 
	// docs from the retrieved docs, and then retrieve the values
	// from the docs.
	
	// 1. Determine the container
	OmnString ctnr_objid = container_objid;
	if (ctnr_objid == "") ctnr_objid = AosDbRand::pickContainer(rdata);
	aos_assert_r(ctnr_objid != "", "");

	// 2. Determine the attribute name
	OmnString aname = attrname;
	if (aname == "") aname = AosCommonValues::pickAttrName();
	aos_assert_r(aname != "", "");
	rdata->setArg1(AOSARG_VALUESEL_SOURCE, AOSVALUE_CONTAINER);

	// 3. Retrieve doc's objids
	vector<OmnString> objids;
	if (!sgQueryObj) sgQueryObj = AosQueryObj::getQueryObj();
	aos_assert_r(sgQueryObj, "");
	int total = sgQueryObj->retrieveContainerMembers(ctnr_objid, objids, 
			eMaxDocsPerCtnr, 0, false, rdata);
	if (total <= 0) return "";

	int idx = OmnRandom::nextInt(0, objids.size()-1);
	OmnString objid = objids[idx];
	aos_assert_r(objid != "", "");

	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(sgDocClient, "");

	AosXmlTagPtr doc = sgDocClient->getDocByObjid(objid, rdata);
	aos_assert_r(doc, "");
	OmnString value = doc->getAttrStr(aname);
	if (value == "")
	{
		value = dft_value;
	}

	// In the current implementations, we only generate AosValueSelConst.
	return AosValueSelConst::getXmlStr(tagname, level, value, rdata);
}

