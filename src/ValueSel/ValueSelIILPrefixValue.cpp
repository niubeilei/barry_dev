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
#include "ValueSel/ValueSelIILPrefixValue.h"

#include "API/AosApiD.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterClt/CounterClt.h"

#include <map>

const u64 MAXNUMBER = 2*100*1000;
const char KEYSEP = '\a';
static map<OmnString, OmnString> sgIILPrefixValueMap;
static OmnMutexPtr sgLock = OmnNew OmnMutex();


AosValueSelIILPrefixValue::AosValueSelIILPrefixValue(const bool reg)
:
AosValueSel(AOSACTOPRID_IILPREFIXVALUE, AosValueSelType::eIILPrefixValue, reg)
{
}


AosValueSelIILPrefixValue::AosValueSelIILPrefixValue(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_IILPREFIXVALUE, AosValueSelType::eIILPrefixValue, false)
{
}


AosValueSelIILPrefixValue::~AosValueSelIILPrefixValue()
{
}


bool
AosValueSelIILPrefixValue::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//_zt44_unayst_other_party_carrier_city
	//_zt44_unayst_other_party_first_call
	//_zt44_unayst_other_party_last_call
	//
	//<value zky_datatype="int64|string">
	//	<prefix  ...>
	//	<iilname ...>
	//</value>
	
	aos_assert_r(sdoc, false);

	AosValueRslt value_rslt;
	bool rslt = AosValueSel::getValueStatic(value_rslt, sdoc, AOSTAG_IILNAME, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname = value_rslt.getStr();
	aos_assert_r(iilname != "", false);

	AosValueRslt value_rslt2;
	rslt = AosValueSel::getValueStatic(value_rslt2, sdoc, AOSTAG_PREFIX, rdata);                           
	aos_assert_r(rslt, false);

	OmnString prefix = value_rslt2.getStr();
	aos_assert_r(prefix != "", false);

	OmnString data_type = sdoc->getAttrStr(AOSTAG_DATA_TYPE);
	aos_assert_r(data_type != "", false);

	AosDataType::E datatype = AosDataType::toEnum(data_type);
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)                                             
	{
		AosSetErrorUser(rdata, "valsel_attr_001") << enderr;
		return false;
	}

	OmnString key;
	key << iilname << KEYSEP << prefix << KEYSEP << data_type; 

	sgLock->lock();
	if (sgIILPrefixValueMap.find(key) != sgIILPrefixValueMap.end())
	{
		OmnString value = sgIILPrefixValueMap[key];
		sgLock->unlock();
		valueRslt.setStr(value);
		return true;
	}
	sgLock->unlock();

	u64 docid;
	OmnString vv;
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_prefix);
	query_context->setStrValue(prefix);
	rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, false);
	rslt = query_rslt->nextDocidValue(docid, vv, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(vv != "", false);

	int idx = strncmp(prefix.data(), vv.data(), prefix.length()); 
	aos_assert_r(idx == 0, false);
	vv = vv.substr(prefix.length());
	
	OmnString str_value;
	if (datatype == AosDataType::eString)
	{
		str_value << vv;
	}
	else if (datatype == AosDataType::eInt64)
	{
		str_value << docid;
	}
	valueRslt.setStr(str_value);

	sgLock->lock();
	if (sgIILPrefixValueMap.size() > MAXNUMBER)
	{
		sgIILPrefixValueMap.clear();
	}
	sgIILPrefixValueMap[key] = str_value;
	sgLock->unlock();

	return true;
}


OmnString 
AosValueSelIILPrefixValue::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelIILPrefixValue::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelIILPrefixValue(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

