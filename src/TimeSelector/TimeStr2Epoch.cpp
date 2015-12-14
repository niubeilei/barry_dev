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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TimeSelector/TimeStr2Epoch.h"

#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "UtilTime/TimeInfo.h"
#include "XmlUtil/XmlTag.h"


AosTimeStr2Epoch::AosTimeStr2Epoch(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_STR2EPOCH, AosTimeSelectorType::eStr2Epoch, reg)
{
}


AosTimeStr2Epoch::~AosTimeStr2Epoch()
{
}


bool  
AosTimeStr2Epoch::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" data_unit ="d|m|y|H|M|S" zky_timesel_type="str2epoch" time_format="yyyymmddhhmmss">
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
		return false;
	}

	OmnString timeStr;
	AosXmlTagPtr timeStrChild = sdoc->getFirstChild("zky_timestr");
	if (timeStrChild)
	{
		AosValueRslt valueRslt;
		if (!AosValueSel::getValueStatic(valueRslt, timeStrChild, rdata))
		{
			// Failed retrieving the value.
			AosSetErrorUser(rdata, "time_failed_002") << sdoc->toString() << enderr;
			return false;
		}
		bool correct;
		timeStr = valueRslt.getStr();
	}
	aos_assert_r(timeStr != "", false);

	OmnString time_format = sdoc->getAttrStr("time_format");
	AosTimeFormat::E format = AosTimeFormat::toEnum(time_format);
	if (!AosTimeFormat::isValid(format))
	{
		AosSetErrorUser(rdata, "data_format_incorrect") << time_format << enderr;
		return false;
	}

	OmnString timegran = sdoc->getAttrStr("zky_timegran", "sec");
	AosTimeGran::E time_gran = AosTimeGran::toEnum(timegran);
	if (!AosTimeGran::isValid(time_gran))
	{
		AosSetErrorUser(rdata, "time_gran_incorrect") << time_format << enderr;
		return false;
	}

	OmnString vv;
	vv << AosTime::getUniTime(timeStr, format, time_gran);
	value.setStr(vv);
	return true;
}

