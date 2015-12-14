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
// 2013/09/16	Created by ken Lee
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelStrMap.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosValueSelStrMap::AosValueSelStrMap(const bool reg)
:
AosValueSel(AOSACTOPRID_STRMAP, AosValueSelType::eStrMap, reg)
{
}


AosValueSelStrMap::AosValueSelStrMap(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_STRMAP, AosValueSelType::eStrMap, false)
{
}


AosValueSelStrMap::~AosValueSelStrMap()
{
}


bool
AosValueSelStrMap::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosValueRslt value_rslt;
	bool rslt = AosValueSel::getValueStatic(value_rslt, sdoc, AOSTAG_KEY, rdata);
	aos_assert_r(rslt, false);

	OmnString key = value_rslt.getStr();
	aos_assert_r(key != "", false);

	OmnString value = sdoc->getNodeText("default");

	AosXmlTagPtr entry;
	AosXmlTagPtr entries = sdoc->getFirstChild("entries");
	if (entries && (entry = entries->getFirstChild()))
	{
		while (entry)
		{
			if (key == entry->getAttrStr(AOSTAG_KEY))
			{
				value = entry->getNodeText();
				break;
			}
			entry = entries->getNextChild();
		}
	}

	valueRslt.setStr(value);
	return true;
}


OmnString 
AosValueSelStrMap::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelStrMap::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelStrMap(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

