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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryCond/CondArith.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryCond/CondNames.h"
#include "QueryCond/CondIds.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "UtilTime/TimeUtil.h"
#include "Util/DataTypes.h"
#include "UtilTime/TimeInfo.h"
#include "XmlUtil/XmlTag.h"


AosCondArith::AosCondArith(const bool regflag)
:
AosQueryCond(AOSCONDTYPE_ARITH, AosCondId::eArith, regflag),
mOpr(eAosOpr_Invalid)
{
}


AosCondArith::AosCondArith(const AosOpr opr, const u64 &value)
:
AosQueryCond(AOSCONDTYPE_ARITH, AosCondId::eArith, false),
mOpr(opr)
{
	mValue << value;
}


AosCondArith::~AosCondArith()
{
}


bool
AosCondArith::parse(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// This function parses the condition. It assumes 'def' is in the 
	// following format:
	//
	// 	<cond type=AOSCONDTYPE_ARITH zky_opr="xxx" zky_datatype="xxx"><![CDATA[...]]></cond> 
	//
	// where opr can be one of the values defined in AosOpr.
	//
	// 'datatype' can be:
	// 		date			Date: yyyy.mm.dd.hh.mm.ss
	//
	// If not specified, the value is taken as it is.
	
	aos_assert_rr(def, rdata, false);
	
	OmnString opr = def->getAttrStr(AOSTAG_OPR);
	OmnString datatype = def->getAttrStr(AOSTAG_DATATYPE);
	mOpr = AosOpr_toEnum(opr);
	if (!AosOpr_valid(mOpr))
	{
		rdata->setError() << "Invalid condition operator: " << opr;
		return false;
	}

	mValue = def->getNodeText();

	if (datatype == AOSDATATYPE_DATE)
	{
		// The value is yyyy.mm.dd.hh.mm.ss. Need to convert.
		OmnString errmsg;
		bool rslt = AosTimeUtil::toU32(mValue, mValue, errmsg);
		if (!rslt)
		{
			rdata->setError() << errmsg;
			return false;
		}
	}
	return true;
}


bool
AosCondArith::toString(OmnString &str)
{
	str << "<cond type=\"" << AosOpr_toStr(mOpr) << "\"><![CDATA["
		<< mValue << "]]></cond>";
	return true;
}

AosQueryCondPtr	
AosCondArith::clone() const
{
	OmnNotImplementedYet;
	return 0;
}

