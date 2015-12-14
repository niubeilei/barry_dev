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
//
// Modification History:
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActDeleteIILEntry.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "SEInterfaces/IILClientObj.h"


AosActDeleteIILEntry::AosActDeleteIILEntry(const bool flag)
:
AosSdocAction(AOSACTTYPE_DELETEIILENTRY, AosActionType::eDeleteIILEntry, flag)
{
}


AosActDeleteIILEntry::~AosActDeleteIILEntry()
{
}


AosActionObjPtr
AosActDeleteIILEntry::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDeleteIILEntry(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActDeleteIILEntry::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);

	AosValueRslt iilname_rslt;
	bool rslt = AosValueSel::getValueStatic(
		iilname_rslt, sdoc, AOSTAG_IILNAME, rdata);
	aos_assert_r(rslt, false);

	OmnString iilname = iilname_rslt.getStr();
	if (iilname == "")
	{
		AosSetErrorU(rdata, "missing_iilname:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt iiltype_rslt;
	rslt = AosValueSel::getValueStatic(
		iiltype_rslt, sdoc, AOSTAG_IILTYPE, rdata);
	aos_assert_r(rslt, false);

	OmnString iiltype = iiltype_rslt.getStr();
	if (iiltype != "str" && iiltype != "u64" && iiltype != "hit")
	{
		AosSetErrorU(rdata, "missing_iiltype:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt key_rslt;
	rslt = AosValueSel::getValueStatic(
		key_rslt, sdoc, AOSTAG_KEY, rdata);
	aos_assert_r(rslt, false);

	OmnString key = key_rslt.getStr();
	if (key == "")
	{
		AosSetErrorU(rdata, "missing_key:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt value_rslt;
	rslt = AosValueSel::getValueStatic(
		value_rslt, sdoc, AOSTAG_VALUE, rdata);
	aos_assert_r(rslt, false);

	u64 value = 0;
	rslt = value_rslt.getU64();
	if (!rslt)
	{
		AosSetErrorU(rdata, "missing_value:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	//Modified by Yazong Ma at 2014-1-6
	//Will all use AosIILClient::getSelf () changed to AosIILClientObj::getIILClient()
	
	if (iiltype == "str")
	{
		rslt = AosIILClientObj::getIILClient()->removeStrValueDoc(iilname, key, value, rdata);
	}
	else if (iiltype == "u64")
	{
		u64 u64_key = key.toU64();
		rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname, u64_key, value, rdata);
	}
	else if (iiltype == "hit")
	{
		u64 u64_key = key.toU64();
		rslt = AosIILClientObj::getIILClient()->removeHitDoc(iilname, u64_key, rdata);
	}

	return rslt;
}

