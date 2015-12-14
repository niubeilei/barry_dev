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
// 04/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelByCond.h"

#include "Actions/ActUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Conds/Condition.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosValueSelByCond::AosValueSelByCond(const bool reg)
:
AosValueSel(AOSACTOPRID_BYCOND, AosValueSelType::eByCond, reg)
{
}


AosValueSelByCond::AosValueSelByCond(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_BYCOND, AosValueSelType::eByCond, false)
{
}


AosValueSelByCond::~AosValueSelByCond()
{
}


bool
AosValueSelByCond::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// The XML format is:
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_BYCOND>
	// 		<AOSTAG_CONDITION .../>
	// 	</valuesel>
	valueRslt.reset();
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);

	// 1. Retrieve the conditions 
	AosXmlTagPtr cond_tag = sdoc->getFirstChild(AOSTAG_CONDITION);
	if (!cond_tag)
	{
		return true;
	}

	bool rslt = AosCondition::evalCondStatic(cond_tag, rdata);
	aos_assert_rr(rslt, rdata, false);

	bool exist = false;
	OmnString vv = rdata->getArg1(AOSARG_CALC_VALUE, exist);
	if (exist) valueRslt.setStr(vv);
	return true;
}


AosValueSelObjPtr
AosValueSelByCond::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelByCond(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

