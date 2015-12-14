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
#include "ValueSel/ValueSelEpoch.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "TimeSelector/TimeAdditive.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelEpoch::AosValueSelEpoch(const bool reg)
:
AosValueSel(AOSACTOPRID_EPOCH, AosValueSelType::eEpoch, reg)
{
}


AosValueSelEpoch::AosValueSelEpoch(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_EPOCH, AosValueSelType::eEpoch, false)
{
}


AosValueSelEpoch::~AosValueSelEpoch()
{
}


bool
AosValueSelEpoch::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//<name AOSTAG_VALUE_TYPE="epoch" timezone="xxx" AOSTAG_DATA_TYPE="xxx"/>
	return AosTimeSelector::getTimeStatic(valueRslt, item, rdata);
}


OmnString 
AosValueSelEpoch::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelEpoch::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelEpoch(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

