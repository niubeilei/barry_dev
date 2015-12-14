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
// 2013/08/05	Created by ken Lee
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelIILValue.h"

#include "API/AosApiD.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/IILClientObj.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"


AosValueSelIILValue::AosValueSelIILValue(const bool reg)
:
AosValueSel(AOSACTOPRID_IILVALUE, AosValueSelType::eIILValue, reg)
{
}


AosValueSelIILValue::AosValueSelIILValue(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_IILVALUE, AosValueSelType::eIILValue, false)
{
}


AosValueSelIILValue::~AosValueSelIILValue()
{
}


bool
AosValueSelIILValue::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosValueRslt value_rslt;
	bool rslt = AosValueSel::getValueStatic(value_rslt, sdoc, AOSTAG_IILNAME, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname = value_rslt.getStr();
	aos_assert_r(iilname != "", false);

	AosValueRslt value_rslt2;
	rslt = AosValueSel::getValueStatic(value_rslt2, sdoc, AOSTAG_IILKEY, rdata);
	aos_assert_r(rslt, false);

	OmnString key = value_rslt2.getStr();
	aos_assert_r(key != "", false);

	u64 value = 0;
	rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, value, rdata);
	aos_assert_r(rslt, false);

	valueRslt.setU64(value);
	return true;
}


OmnString 
AosValueSelIILValue::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelIILValue::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelIILValue(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

