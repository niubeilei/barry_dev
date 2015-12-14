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
// This value selectr composes a value.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelCompose.h"

#include "API/AosApi.h"
#include "Actions/ActUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelCompose::AosValueSelCompose(const bool reg)
:
AosValueSel(AOSACTOPRID_COMPOSE, AosValueSelType::eCompose, reg)
{
}


AosValueSelCompose::AosValueSelCompose(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_COMPOSE, AosValueSelType::eCompose, false)
{
}


AosValueSelCompose::~AosValueSelCompose()
{
}


bool
AosValueSelCompose::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// The XML format is:
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_COMPOSE AOSTAG_SEPARATOR="xxx">
	// 		<valuesel .../>
	// 		<valuesel .../>
	// 		...
	// 	</valuesel>
	valueRslt.reset();
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	rdata->setArg1(AOSARG_CALC_VALUE, "");

	// 1. Retrieve the separator
	OmnString sep = sdoc->getAttrStr(AOSTAG_SEPARATOR);
	AosConvertAsciiBinary(sep);

	// 2. Retrieve the value selectors
	AosXmlTagPtr value_tag = sdoc->getFirstChild();
	int guard = eMaxSegments;
	OmnString value;
	bool first = true;
	while (guard-- && value_tag)
	{
		AosValueRslt vv;
		bool rslt = AosValueSel::getValueStatic(vv, value_tag, rdata);
		aos_assert_r(rslt, false);
		OmnString str = vv.getValueStr("", rslt);
		if (!rslt)
		{
			AosSetErrorUser(rdata, "valuesel_compose_001") << sdoc->toString() << enderr;
			return false;
		}
		if (str == "")
		{
			OmnAlarm << "str is empty str" << enderr;
		}

		if (first)
		{
			first = false;
		}
		else if (sep != "")
		{
			value << sep;
		}

		value << str;
		value_tag = sdoc->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetErrorUser(rdata, "valuesel_compose_002") << sdoc->toString() << enderr;
		return false;
	}

	rdata->setArg1(AOSARG_CALC_VALUE, value);
	valueRslt.setStr(value);
	return true;
}


OmnString
AosValueSelCompose::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &selectors,
		const OmnString &sep, 
		const AosRundataPtr &rdata)
{
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_COMPOSE AOSTAG_SEPARATOR="xxx">
	aos_assert_r(tagname != "", "");
	OmnString docstr = "<";
	docstr << tagname << " " << AOSTAG_VALUE_TYPE 
		<< "=\"" << AOSACTOPRID_COMPOSE << "\"";
	if (sep != "")
	{
		docstr << " " << AOSTAG_SEPARATOR << "=\"" << sep << "\">";
	}

	docstr << selectors << "</" << tagname << ">";
	return docstr;
}


OmnString 
AosValueSelCompose::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelCompose::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelCompose(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif 
