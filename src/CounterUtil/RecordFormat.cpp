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
// 02/28/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/RecordFormat.h"

#include "CounterUtil/Ptrs.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/RecordOneDime.h"
#include "CounterUtil/RecordTwoDime.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


AosRecordFormat::AosRecordFormat()
:
mTime(0),
mFactor(0.0),
mDftValue(0),
mFillingFlag(false)
{
}


AosRecordFormat::~AosRecordFormat()
{
}


AosRecordFormatPtr
AosRecordFormat::createFormat(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	OmnString type = term->getAttrStr("zky_type");
	aos_assert_r(type != "", 0);
	char c = type.data()[0];
	switch(c)
	{
		case 'o':
			 if (type == "onedime")
			 {
				 return OmnNew AosOneDime(term, rdata);
			 }
			 break;
		case 't':
			 if (type == "twodime")
			 {
				 return OmnNew AosTwoDime(term, rdata);
			 }
			 break;
		default:
			 break;
	}
	AosSetErrorU(rdata, "unrecognized_format_type") << ": " << type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return 0;
}


OmnString
AosRecordFormat::splitName(const OmnString &cname)
{
	aos_assert_r(cname != "", "");
	OmnString record;
	vector<OmnString> str;
	AosCounterUtil::splitTerm2(cname, str);
	for (u32 i = 0; i< str.size(); i++)
	{
		record << AOSTAG_CNAME << i << "=\"" << str[i] << "\" ";
	}
	return record;
}


bool
AosRecordFormat::filling(
		OmnString &cname, 
		int64_t &value, 
		const AosRundataPtr &rdata)
{
	if (!mFillingFlag) return false;

	if (cname == "") return fillingFinished(cname, value, rdata);
	aos_assert_r(mFilling.type == "time", false);

	aos_assert_r(mTime, false);
	OmnString timestr = AosCounterUtil::getTerm2(AosCounterUtil::getAllTerm2(cname), mFilling.filling_field);
	aos_assert_r(timestr != "", false);

	u64 tt = timestr.parseU64(0);
	aos_assert_r(tt > 0, false);

	u64 end_time = mTime->convertUniTime(mTime->getEndTime());
	if (mFilling.crt_time > tt || mFilling.crt_time > end_time) 
	{
		cname = "";
		return true;
	}

	if (mFilling.template_cname == "") mFilling.template_cname = cname;
	OmnString str;
	str << mFilling.crt_time;

	if (mFilling.crt_time < tt)
	{
		cname = AosCounterUtil::decomposeReplaceTerm(
				mFilling.template_cname, mFilling.filling_field, str);
		value = mDftValue;
	}
	else
	{
		aos_assert_r(mFilling.crt_time == tt, false);
	}
	mFilling.crt_time = mTime->nextTime(mFilling.crt_time);
	return true;
}

bool
AosRecordFormat::fillingFinished(
		OmnString &cname, 
		int64_t &value, 
		const AosRundataPtr &rdata)
{
	if (!mFillingFlag) return false;
	aos_assert_r(mFilling.type == "time", false);

	u64 end_time = mTime->convertUniTime(mTime->getEndTime());
	if (mFilling.crt_time > end_time) return false;

	OmnString str;
	str << mFilling.crt_time;
	if (mFilling.template_cname == "")
	{
		u16 entry_type;
		OmnString key((char *)&entry_type, sizeof(u16));
		cname << key << str;
		mFilling.template_cname = cname;
	}
	cname = AosCounterUtil::decomposeReplaceTerm(mFilling.template_cname, mFilling.filling_field, str);

	aos_assert_r(cname != "", false);
	mFilling.crt_time = mTime->nextTime(mFilling.crt_time);
	return true;
}


OmnString
AosRecordFormat::convertDataType(
		const OmnString &name,
		const int64_t &v1,
		const AosRundataPtr &rdata)
{
	// double
	OmnString record;
	if (mFactor != 0.0)
	{
		double vv1 = v1 * mFactor;
		record << name << "=\"" << vv1 << "\" ";
	}
	else
	{
		record << name << "=\"" << v1 << "\" ";
	}
	return record;
}

