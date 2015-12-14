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
// 06/03/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CustomizedValueSel/Unicom/UnicomSubstr.h"

#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
//#include "IILClient/IILClient.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosUnicomSubstr::AosUnicomSubstr(const bool reg)
:
AosValueSel(AOSACTOPRID_UNICOMSUBSTR, AosValueSelType::eUnicomSubstr, reg)
{
}


AosUnicomSubstr::AosUnicomSubstr(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_UNICOMSUBSTR, AosValueSelType::eUnicomSubstr, false)
{
	if (!config(sdoc, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosUnicomSubstr::~AosUnicomSubstr()
{
}


bool
AosUnicomSubstr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosUnicomSubstr::run(
		AosValueRslt &value,
		const char *record, 
		const int record_len, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosUnicomSubstr::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosValueSelObjPtr
AosUnicomSubstr::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosUnicomSubstr(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj");
		AosLogError(rdata);
		return 0;
	}
}

