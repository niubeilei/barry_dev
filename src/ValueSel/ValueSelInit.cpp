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
// 2013/02/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelInit.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelInit::AosValueSelInit()
:
AosValueSel(AOSACTOPRID_INIT, AosValueSelType::eInit, false)
{
}


AosValueSelInit::~AosValueSelInit()
{
}


bool
AosValueSelInit::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosValueSelObjPtr
AosValueSelInit::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelInit();
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

