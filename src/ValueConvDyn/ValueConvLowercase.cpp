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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueConvDyn/ValueConvLowercase.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TransBasic/Trans.h"
#include "Util/DirDesc.h"
#include "XmlUtil/SeXmlParser.h"


AosValueConvLowercase::AosValueConvLowercase()
{
}


AosValueConvLowercase::~AosValueConvLowercase()
{
}


bool
AosValueConvLowercase::run(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosValueConverterPtr AosValueConvLowercaseFunc(const AosRundataPtr &rdata) 
{
	// AosXmlTagPtr sdoc = rdata->getSdoc();
	// aos_assert_rr(sdoc, rdata, false);

	return OmnNew AosValueConvLowercase();
}

