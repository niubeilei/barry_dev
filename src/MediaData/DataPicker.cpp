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
// 12/24/2010	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "MediaData/DataPicker.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MediaData/Ptrs.h"
#include "MediaData/DataPickerIds.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


extern AosDataPickerPtr	sgPickers[AosDataPickerId::eMax];
static OmnMutex			sgLock;

AosDataPicker::AosDataPicker(
		const OmnString &name,
		const AosDataPickerId::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosDataPickerPtr thisptr(this, false);
		registerDataPicker(thisptr, name);
	}
}


bool
AosDataPicker::registerDataPicker(const AosDataPickerPtr &picker, const OmnString &name)
{
    aos_assert_r(AosDataPickerId::isValid(picker->mType), false);

	sgLock.lock();
    if (sgPickers[picker->mType])
	{
		OmnAlarm << "Access already registered: " << picker->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgPickers[picker->mType] = picker;
	sgLock.unlock();
	return true;
}


bool 
AosDataPicker::pickDataStatic(
		const OmnString &algorithm_id,
		const OmnString &data, 
		OmnString &rslts, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(algorithm_id != "", rdata, false);
	AosDataPickerId::E code = AosDataPickerId::toEnum(algorithm_id);
	if (!AosDataPickerId::isValid(code))
	{
		AosSetError(rdata, AOSLT_UNRECOGNIZED_DATA_PICKER_ID);
		OmnAlarm << rdata->getErrmsg() << ": " << algorithm_id << enderr;
		return false;
	}

	AosDataPickerPtr picker = sgPickers[code];
	if (!picker)
	{
		AosSetError(rdata, AOSLT_PICKER_NOT_FOUND);
		OmnAlarm << rdata->getErrmsg() << ": " << algorithm_id << enderr;
		return false;
	}

	return picker->pickData(data, rslts, sdoc, rdata);
}

bool 
AosDataPicker::pickDataStatic2(
		const OmnString &algorithm_id,
		const OmnString &data, 
		AosXmlTagPtr &rslts, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(algorithm_id != "", rdata, false);
	AosDataPickerId::E code = AosDataPickerId::toEnum(algorithm_id);
	if (!AosDataPickerId::isValid(code))
	{
		AosSetError(rdata, AOSLT_UNRECOGNIZED_DATA_PICKER_ID);
		OmnAlarm << rdata->getErrmsg() << ": " << algorithm_id << enderr;
		return false;
	}

	AosDataPickerPtr picker = sgPickers[code];
	if (!picker)
	{
		AosSetError(rdata, AOSLT_PICKER_NOT_FOUND);
		OmnAlarm << rdata->getErrmsg() << ": " << algorithm_id << enderr;
		return false;
	}

	return picker->pickData2(data, rslts, sdoc, rdata);

}

