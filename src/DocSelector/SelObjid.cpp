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
#include "DocSelector/SelObjid.h"

#include "SEInterfaces/DocClientObj.h"
#include "Random/CommonValues.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/Torturer/ValueRandUtil.h"


AosDocSelObjid::AosDocSelObjid(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_BY_OBJID, AosDocSelectorType::eObjid, reg)
{
}


AosDocSelObjid::~AosDocSelObjid()
{
}


AosXmlTagPtr 
AosDocSelObjid::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the doc identified by the objid specified in 'sdoc'.
	// 'sdoc' should be in the form:
	// 	<selector AOSTAG_ZKY_TYPE=AOSDOCSELTYPE_BY_OBJID
	// 		AOSTAG_DOC_OBJID="xxx">
	// 		<AOSTAG_VALUEDEF .../>
	// 	</selector>
	aos_assert_r(false, NULL);
	return NULL;
#if 0
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnString objid = sdoc->getAttrStr(AOSTAG_DOC_OBJID);
	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing siteid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	if (objid == "") 
	{
		// Retrieve the value
		AosXmlTagPtr value_def = sdoc->xpathGetChild(AOSTAG_VALUEDEF);
		if (!value_def)
		{
			rdata->setError() << "Missing value definition";
			return 0;
		}

		AosValueRslt value;
		if (!AosValueSel::getValueStatic(value, value_def, rdata))
		{
			return 0;
		}

		// The data type cannot be XML DOC
		if (!value.isValid())
		{
			rdata->setError() << "Value is invalid";
			return 0;
		}
	
		bool rslt;
		objid = value.getValueStr("", rslt);
		if (!rslt || objid == "")
		{
			rdata->setError() << "Missing objid!";
			return 0;
		}
	}
		
	rdata->setOk();
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
#endif
}


OmnString
AosDocSelObjid::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	// 	<selector AOSTAG_ZKY_TYPE=AOSDOCSELTYPE_BY_OBJID
	// 		AOSTAG_DOC_OBJID="xxx">
	// 		<AOSTAG_VALUEDEF .../>
	// 	</selector>
	aos_assert_rr(tagname != "", rdata, "");
	OmnString str = "<";
	addHeader(str, tagname, AOSDOCSELTYPE_BY_OBJID, rule);

	OmnString objid;
	if (rule)
	{
		objid = rule->pickObjid();
	}
	else
	{
		objid = AosCommonValues::pickObjid();
	}

	if (OmnRandom::percent(50))
	{
		str << AOSTAG_DOC_OBJID << "=\"" << objid << "\">";
	}
	else
	{
		//str << AosValueRandUtil::pickSingleValueSelector(
		//		AOSTAG_VALUEDEF, level-1, objid, rule, rdata);
	}
	str << "</" << tagname << ">";
	return str;
}

