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
#include "Actions/ActAddIILEntry.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILClientObj.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"


AosActAddIILEntry::AosActAddIILEntry(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDIILENTRY, AosActionType::eAddIILEntry, flag)
{
}


AosActAddIILEntry::~AosActAddIILEntry()
{
}


AosActionObjPtr
AosActAddIILEntry::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddIILEntry(false);
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
AosActAddIILEntry::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
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
	vector<OmnString> t_keys, keys;
	AosSplitStr(key, ",", t_keys, 1000);
	for (size_t i=0; i<t_keys.size(); i++)
	{
		if (t_keys[i] != "")
		{
			keys.push_back(t_keys[i]);
		}
	}

	if (keys.size() == 0 && iiltype != "hit")
	{
		AosSetErrorU(rdata, "missing_keys:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 default_value = sdoc->getAttrU64(AOSTAG_DEFAULT_VALUE, 0);

	AosValueRslt value_rslt;
	rslt = AosValueSel::getValueStatic(
		value_rslt, sdoc, AOSTAG_VALUE, rdata);
	aos_assert_r(rslt, false);

	OmnString value_str = value_rslt.getStr();
	vector<OmnString> t_values;
	vector<u64> values;
	AosSplitStr(value_str, ",", t_values, 1000);
	for (size_t i=0; i<t_values.size(); i++)
	{
		if (t_values[i] != "")
		{
			values.push_back(t_values[i].toU64());
		}
	}

	while (values.size() < keys.size())
	{
		values.push_back(default_value);
	}

	if (values.size() == 0)
	{
		AosSetErrorU(rdata, "missing_values:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool key_unique = sdoc->getAttrBool("zky_key_unique", true);
	bool value_unique = sdoc->getAttrBool("zky_value_unique", true);

	if (iiltype == "str")
	{
		for (size_t i=0; i<keys.size(); i++)
		{
			//rslt = AosIILClient::getSelf()->addStrValueDoc(
			//	iilname, keys[i], values[i], key_unique, value_unique, rdata);
			rslt = AosIILClientObj::getIILClient()->addStrValueDoc(
				iilname, keys[i], values[i], key_unique, value_unique, rdata);
		}
	}
	else if (iiltype == "u64")
	{
		for (size_t i=0; i<keys.size(); i++)
		{
			u64 kk = keys[i].toU64();
			//rslt = AosIILClient::getSelf()->addU64ValueDoc(
			//	iilname, kk, values[i], key_unique, value_unique, rdata);
			rslt = AosIILClientObj::getIILClient()->addU64ValueDoc(
				iilname, kk, values[i], key_unique, value_unique, rdata);
		}
	}
	else
	{		
		//rslt = AosIILClient::getSelf()->HitBatchAdd(
		//	iilname, values, 0, 0, rdata);
		rslt = AosIILClientObj::getIILClient()->HitBatchAdd(
			iilname, values, 0, 0, rdata);
	}

	return rslt;
}

