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
#if 0
#include "ValueSel/ValueSelValueSel.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosValueSelValueSel::AosValueSelValueSel(const bool reg)
:
AosValueSel(AosValueSelType::eValueSel, reg)
{
}


AosValueSelValueSel::~AosValueSelValueSel()
{
}


bool
AosValueSelValueSel::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//<name AOSTAG_VALUE_TYPE="const" AOSTAG_VALUE="read|modify|..." AOSTAG_DATA_TYPE="xxx"></name>
	// If AOSTAG_DATA_TYPE is not specified, it defaults to string.
	aos_assert_r(rdata, false);
	aos_assert_rr(item, rdata, false);
	AosValueRslt::Type datatype = AosValueRslt::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosValueRslt::isValid(datatype))
	{
		datatype = AosValueRslt::eString;
	}
	aos_assert_rr(datatype == AosValueRslt::eString, rdata, false);
	OmnString vv = item->getAttrStr(AOSTAG_VALUE, "");
	return valueRslt.setValue(datatype, vv, rdata);
}


AosValueSelPtr
AosValueSelValueSel::clone()
{
	AosValueSel *pp = (AosValueSel*)OmnNew AosValueSelValueSel(false);
	return pp;
}

OmnString 
AosValueSelValueSel::getXml(const AosRandomRulePtr &rule)
{
	mRule = rule;

	OmnString xml = "<";
	xml << rule->mNodeName << " ";

	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		if (rand()%10 < 8)
			xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	
	xml << "/>";
	return  xml;
}


bool 
AosValueSelValueSel::getValue(
		AosValueRslt &valueRslt, 
		const AosXmlTagPtr &obj,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosValueSelPtr 
AosValueSelValueSel::createValueSelector(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}
#endif
