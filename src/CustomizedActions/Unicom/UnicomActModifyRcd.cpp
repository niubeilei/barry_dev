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
#include "CustomizedActions/Unicom/UnicomActModifyRcd.h"

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

AosUnicomActModifyRcd::AosUnicomActModifyRcd(const bool reg)
:
AosSdocAction(AOSACTTYPE_UNICOM_MODIFYRCD, AosActionType::eUnicomModifyRcd, reg)
{
}


AosUnicomActModifyRcd::AosUnicomActModifyRcd(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_UNICOM_MODIFYRCD, AosActionType::eUnicomModifyRcd, false)
{
	if (!config(sdoc, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosUnicomActModifyRcd::~AosUnicomActModifyRcd()
{
}


bool
AosUnicomActModifyRcd::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosUnicomActModifyRcd::run(const AosXmlTagPtr &def, const AosRundataPtr &rundata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosUnicomActModifyRcd::run(
		const char *record, 
		const int record_len, 
		const AosRundataPtr &rdata)
{
	// Position 31 is a u64. If its value is 0, set 31+8 to 'f'. Otherwise, 
	// set it to 't'.
	u64 value = *(u64*)&record[31];
	((char*)record)[31+8] = (value)?'t':'f';
	return true;
}


AosActionObjPtr
AosUnicomActModifyRcd::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosUnicomActModifyRcd(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj");
		AosLogError(rdata);
		return 0;
	}
}


bool 
AosUnicomActModifyRcd::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosUnicomActModifyRcd::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	return true;
}

