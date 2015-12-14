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
#include "TimeSelector/TimeEpoch2Str.h"

#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"


AosTimeEpoch2Str::AosTimeEpoch2Str(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_EPOCH2STR, AosTimeSelectorType::eEpoch2Str, reg)
{
}


AosTimeEpoch2Str::~AosTimeEpoch2Str()
{
}


bool  
AosTimeEpoch2Str::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" zky_timesel_type="epoch2str" time_format="yyyymmddhhmmss">
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

	OmnString epoch;
	AosXmlTagPtr timeStrChild = sdoc->getFirstChild("zky_timetag");
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
		epoch = valueRslt.getStr();
	}
	aos_assert_r(epoch!= "", false);

	OmnString time_format = sdoc->getAttrStr("time_format");
	if (time_format == "")
	{
		AosSetErrorUser(rdata, "time_format_incorrect") << time_format << enderr;
		return false;
	}

	OmnString vv;
	// Brian Zhang 08/17/2012
	vv << AosTimeUtil::convertToLocalTime(time_format, epoch.data(), epoch.length());
	value.setStr(vv);
	return true;
}

