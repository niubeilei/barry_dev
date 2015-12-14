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
// 2013/11/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelCounterKey.h"

#include "API/AosApiD.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterClt/CounterClt.h"

AosValueSelCounterKey::AosValueSelCounterKey(const bool reg)
:
AosValueSel(AOSACTOPRID_COUNTERKEY, AosValueSelType::eCounterKey, reg)
{
}


AosValueSelCounterKey::AosValueSelCounterKey(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_COUNTERKEY, AosValueSelType::eCounterKey, false)
{
}


AosValueSelCounterKey::~AosValueSelCounterKey()
{
}


bool
AosValueSelCounterKey::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//<value>
	//	<level ..../>
	//	<append_bit ..../>
	//	<statType ..../>
	//	<time_gran ..../>
	//	<couterName .../>
	//</value>
	OmnString counterName;
	bool rslt = AosResolveCounterName::resolveCounterName2(
		sdoc, AOSTAG_COUNTERNAMES, counterName, rdata);
	aos_assert_r(rslt, false);

	AosValueRslt statTypeRslt;
	OmnString statTypeStr = "sum";
	rslt = AosValueSel::getValueStatic(
		statTypeRslt, sdoc, AOSTAG_COUNTERSTATTYPE, rdata);
	if (rslt)
	{
	    statTypeStr = statTypeRslt.getStr();
	}
	AosStatType::E statType = AosStatType::toEnum(statTypeStr);
	aos_assert_r(AosStatType::isOrigStat(statType), false);
	
	OmnString ctype = "ntm";
	AosValueRslt valueCtypeRslt;
	rslt = AosValueSel::getValueStatic(
		valueCtypeRslt, sdoc, AOSTAG_COUNTERTYPE, rdata);
	if (rslt)
	{
		ctype = valueCtypeRslt.getStr();
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum(ctype);
	aos_assert_r(AosTimeGran::isValid(time_gran), false);

	int level = 0;
	AosValueRslt valueLevelRslt;
	rslt = AosValueSel::getValueStatic(
		valueLevelRslt, sdoc, "counter_level", rdata);
	if (rslt)
	{
	    level = valueLevelRslt.getI64(); 
	}
	aos_assert_r(level >= 0, false);

	int append_bit = 0;
	AosValueRslt valueAppendBitRslt;
	rslt = AosValueSel::getValueStatic(
		valueAppendBitRslt, sdoc, "counter_appendbit", rdata);
	if (rslt)
	{
		append_bit = valueAppendBitRslt.getI64();
	}
	aos_assert_r(append_bit >= 0, false);

	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, statType, time_gran);
	OmnString cname = AosCounterUtil::composeTerm2(entry_type, counterName, 0);
	valueRslt.setStr(cname);
	return true;
}


OmnString 
AosValueSelCounterKey::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelCounterKey::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelCounterKey(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}







