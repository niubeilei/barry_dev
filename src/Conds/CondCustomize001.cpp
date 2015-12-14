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
// This condition tests the requester belongs to one of the user
// roles specified by this condition.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondCustomize001.h"

#include "API/AosApi.h"
#include "DocSelector/DocSelector.h"
#include "Random/CommonValues.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondCustomize001::AosCondCustomize001(const bool reg)
:
AosCondition(AOSCONDTYPE_CUSTOMIZE001, AosCondType::eCustomize001, reg)
{
}


AosCondCustomize001::~AosCondCustomize001()
{
}


bool
AosCondCustomize001::evalCond(const AosXmlTagPtr &cfg, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCondCustomize001::evalCond(const char *record, const int len, const AosRundataPtr &rdata)
{
	// This is a customized condition. It does the following:
	// key is in the form:
	// 		cpn|$$|upn|$$|cpncity2|$$|upncity|$$|upntown
	
	// 1. Retrieve the key
	int key_len;
	char *key = rdata->getCharPtr(0, key_len);
	if (!key || key_len <= 0)
	{
		AosSetErrorU(rdata, "failed_retrieve_key:") << key;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Retrieve upncity
	const char *vv = 0;
	int vv_len = 0;
	bool rslt = AosGetField(vv, vv_len, key, key_len, 3, "$", rdata.getPtr());

	// Chen Ding, 06/15/2012
	// if (!rslt || !vv || vv_len <= 0)
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_retrieve_value:") << key;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Chen Ding, 06/15/2012
	int upncity = 0;
	if (vv_len > 0)
	{
		if (!vv)
		{
			AosSetErrorU(rdata, "failed_retrieve_valuei:") << key;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		upncity = atoi(vv);
		if (upncity < 0)
		{
			AosSetErrorU(rdata, "invalid_data:") << key;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	// Chen Ding, 06/18/2012
	// We will check the prefix table. If their city codes match, return true.
	// Otherwise, check their CDR's city codes.
	int64_t cpncity = rdata->getU64Value(1, 0);
	
	if (cpncity == upncity) return true;
	// if (cpncity != 0)
	// {
	//		if (cpncity != upncity) return false;
	//		return true;
	// }

	// There is no matching cpn. Need to use cpncity2
	rslt = AosGetField(vv, vv_len, key, key_len, 2, "$", rdata.getPtr());
	
	// Chen Ding, 06/15/2012
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_retrieve_value:") << key;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int ccity = 0;
	if (vv_len > 0)
	{
		if (!vv)
		{
			AosSetErrorU(rdata, "failed_retrieve_value:") << key;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		ccity = atoi(vv);
		if (ccity < 0)
		{
			AosSetErrorU(rdata, "invalid_data:") << key;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	if (ccity != upncity) return false;
	return true;
}


AosConditionObjPtr
AosCondCustomize001::clone()
{
	try
	{
		return OmnNew AosCondCustomize001(false);
	}

	catch (...)
	{
		OmnAlarm << "Failed to clone Cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

