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
#include "ValueSel/ValueSelCounterValue.h"

#include "API/AosApiD.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
//#include "DocClient/DocClient.h"
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
#if 0
AosValueSelCounterValue::AosValueSelCounterValue(const bool reg)
:
AosValueSel(AOSACTOPRID_COUNTERVALUE, AosValueSelType::eCounterValue, reg)
{
}


AosValueSelCounterValue::AosValueSelCounterValue(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_COUNTERVALUE, AosValueSelType::eCounterValue, false)
{
}


AosValueSelCounterValue::~AosValueSelCounterValue()
{
}


bool
AosValueSelCounterValue::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//_zt44_unayst_other_party_carrier_city
	//_zt44_unayst_other_party_first_call
	//_zt44_unayst_other_party_last_call
	//
	//<value zky_datatype="int64|string" idx="xxxx">
	//	<counterid .../>
	//	<level ..../>
	//	<append_bit ..../>
	//	<statType ..../>
	//	<time_gran ..../>
	//	<couterName .../>
	//</value>
	OmnString counter_id;
	AosValueRslt valueIdRslt;
	bool rslt = AosValueSel::getValueStatic(valueIdRslt, sdoc, AOSTAG_COUNTERID, rdata);
	aos_assert_r(rslt, false);

	counter_id = valueIdRslt.getStr();
	aos_assert_r(counter_id != "", false);

	OmnString counterName;
	rslt = AosResolveCounterName::resolveCounterName2(
		sdoc, AOSTAG_COUNTERNAMES, counterName, rdata);
	aos_assert_r(rslt, false);

	OmnString counterName2;
	if (sdoc->getFirstChild("counter_name2"))
	{
		rslt = AosResolveCounterName::resolveCounterName2(
			sdoc, "counter_name2", counterName2, rdata);
		aos_assert_r(rslt, false);
	}

	// gets counter stattype from smart doc. It's optional.
	AosXmlTagPtr statTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERSTATTYPE);
	AosValueRslt statTypeRslt;
	OmnString statTypeStr = "sum";
	rslt = AosValueSel::getValueStatic(statTypeRslt, sdoc, AOSTAG_COUNTERSTATTYPE, rdata);
	if (rslt)
	{
	    statTypeStr = statTypeRslt.getStr();
	}
	AosStatType::E statType = AosStatType::toEnum(statTypeStr);
	aos_assert_r(AosStatType::isOrigStat(statType), false);
	
	AosValueRslt valueCtypeRslt;
	OmnString ctype = "ntm";
	rslt = AosValueSel::getValueStatic(valueCtypeRslt, sdoc, AOSTAG_COUNTERTYPE, rdata);
	if (rslt)
	{
		ctype = valueCtypeRslt.getStr();
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum(ctype);
	aos_assert_r(AosTimeGran::isValid(time_gran), false);

	int level = 0;
	AosValueRslt valueLevelRslt;
	rslt = AosValueSel::getValueStatic(valueLevelRslt, sdoc, "counter_level", rdata);
	if (rslt)
	{
	    level = valueLevelRslt.getI64(); 
	}
	aos_assert_r(level >= 0, false);

	int append_bit = 0;
	AosValueRslt valueAppendBitRslt;
	rslt = AosValueSel::getValueStatic(valueAppendBitRslt, sdoc, "counter_appendbit", rdata);
	if (rslt)
	{
		append_bit = valueAppendBitRslt.getI64();
	}
	aos_assert_r(append_bit >= 0, false);

	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, statType, time_gran);
	OmnString cname = AosCounterUtil::composeTerm2(entry_type, counterName, 0);

	vector<OmnString> cnames;
	cnames.push_back(cname);

	if (counterName2 != "")
	{
		OmnString cname2 = AosCounterUtil::composeTerm2(entry_type, counterName2, 0);
		cnames.push_back(cname2);
	}

	vector<AosStatType::E> stat_types;
	stat_types.push_back(statType);

	AosCounterQueryPtr counter_query;
	AosXmlTagPtr aggregate_tag = sdoc->getFirstChild("aggregate");
	if (aggregate_tag)
	{
		OmnString aggregateType = aggregate_tag->getAttrStr("zky_aggregate_type", "");
		if (aggregateType != "")
		{
			counter_query = OmnNew AosCounterQuery();
			counter_query->setAggregateType(aggregateType);

			i64 fieldindex = aggregate_tag->getAttrInt64("zky_mergefield_index", -1);
			counter_query->setMergeFieldIndex(fieldindex);  
	
			counter_query->setConvertEntriesConfig(aggregate_tag);
		}
	}

	AosQueryRsltObjPtr query_rslt;
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	if (counterName2 == "")
	{
		rslt = AosCounterClt::getSelf()->retrieveCountersPrefix(
			counter_id, cnames, stat_types, counter_query,
			buff, query_rslt, true, "norm", rdata);
	}
	else
	{
		rslt = AosCounterClt::getSelf()->retrieveCounters2(
			counter_id, cnames, stat_types, counter_query,
			buff, query_rslt, true, "norm", rdata);
	}
	aos_assert_r(rslt, false);
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
	buff->reset();
	OmnString key = buff->getOmnStr("");
	int64_t value = buff->getI64(0);
	
	OmnString str_value;
	if (datatype == AosDataType::eString)
	{
		if (key != "")
		{
			str_value = AosCounterUtil::getAllTerm2(key);
			int idx = sdoc->getAttrInt("idx", 0);
			str_value = AosCounterUtil::getTerm2(str_value, idx);
		}
	}
	else if (datatype == AosDataType::eInt64)
	{
		str_value << value;
	}
	valueRslt.setValue(datatype, str_value, rdata.getPtrNoLock());
	return true;
}


OmnString 
AosValueSelCounterValue::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelCounterValue::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelCounterValue(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
