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
// 2013/11/29 Created by ken Lee
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelIILEntryMap.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILEntryMap/IILEntryMapMgr.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#if 0

AosValueSelIILEntryMap::AosValueSelIILEntryMap(const bool reg)
:
AosValueSel(AOSACTOPRID_IILENTRYMAP, AosValueSelType::eIILEntryMap, reg)
{
}


AosValueSelIILEntryMap::AosValueSelIILEntryMap(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_IILENTRYMAP, AosValueSelType::eIILEntryMap, false)
{
}


AosValueSelIILEntryMap::~AosValueSelIILEntryMap()
{
}


bool
AosValueSelIILEntryMap::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "valsel_attr_001") << enderr;
		return false;
	}

	AosValueRslt value_rslt;
	bool rslt = AosValueSel::getValueStatic(value_rslt, sdoc, AOSTAG_IILNAME, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname = value_rslt.getStr();
	aos_assert_r(iilname != "", false);

	bool need_split = sdoc->getAttrBool(AOSTAG_NEEDSPLIT, false);
	OmnString sep = sdoc->getAttrStr(AOSTAG_SEPARATOR);
	bool need_swap = sdoc->getAttrBool(AOSTAG_NEEDSWAP, false);
	bool use_key_as_value = sdoc->getAttrBool(AOSTAG_USE_KEY_AS_VALUE, false);

	AosIILEntryMapPtr iilmap = AosIILEntryMapMgr::retrieveIILEntryMap(
		iilname, need_split, sep, need_swap, use_key_as_value, rdata);
	aos_assert_r(iilmap, false);

	AosValueRslt value_rslt2;
	rslt = AosValueSel::getValueStatic(value_rslt2, sdoc, AOSTAG_KEY, rdata);
	aos_assert_r(rslt, false);

	OmnString key = value_rslt2.getStr();
	aos_assert_r(key != "", false);

	OmnString dft = sdoc->getNodeText("default");
	if (dft == "") dft = "0";
	
	OmnString map_value = dft;
	iilmap->lock();
	map<OmnString, OmnString>::iterator itr = iilmap->find(key);
	if(itr != iilmap->end())
	{
		map_value = itr->second;
	}
	iilmap->unlock();

	valueRslt.setValue(datatype, map_value, rdata.getPtrNoLock());
	return true;
}


OmnString 
AosValueSelIILEntryMap::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelIILEntryMap::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelIILEntryMap(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
