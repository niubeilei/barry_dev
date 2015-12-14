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
#include "TimeSelector/TimeGetEpoch.h"

#include "Rundata/Rundata.h"
#include "QueryUtil/QrUtil.h"
#include "XmlUtil/XmlTag.h"


AosTimeGetEpoch::AosTimeGetEpoch(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_GETEPOCH, AosTimeSelectorType::eGetEpoch, reg)
{
}


AosTimeGetEpoch::~AosTimeGetEpoch()
{
}


bool  
AosTimeGetEpoch::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" data_unit="d|m|y|H|M" type="getepoch">
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

	OmnString date_unit = sdoc->getAttrStr("data_unit");

	// get Current time 
	OmnString  vv;
	i64 crt_epoch, cal_epoch;
	if (date_unit == "d" || date_unit == "m" || date_unit == "y" ||
			date_unit == "H" || date_unit == "M")
	{
		AosQrUtil::calculateTimeToEpoch(-1, date_unit, crt_epoch, cal_epoch, rdata);
		vv << cal_epoch;
	}
	else
	{
		vv << OmnGetSecond();
	}

	value.setStr(vv);
	return true;
}

