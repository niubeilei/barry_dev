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
// 2012/12/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupField.h"

#include "Groupby/GroupFieldStr.h"
#include "Groupby/GroupFieldU64.h"


AosGroupField::AosGroupField(const FieldType type)
:
mType(type),
mRawFormat(eRawFormat_Invalid),
mFromDocidIdx(-1),
mSetRecordDocid(false),
mFromFieldIdx(-1),
mToFieldIdx(-1),
mCount(0),
mPrevCount(0)
{
}


AosGroupField::~AosGroupField()
{
}


bool
AosGroupField::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mName = def->getAttrStr(AOSTAG_NAME);
	if(mName == "")
	{
		AosSetErrorU(rdata, "missing_data_field_name:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Parse the raw format
	OmnString ss = def->getAttrStr(AOSTAG_RAW_FORMAT, getDefaultRawFormat());
	if (ss == "u64")
	{
		mRawFormat = eRawFormat_U64;
	}
	else if(ss == "str")
	{
		mRawFormat = eRawFormat_Str;
	}
	else
	{
		AosSetErrorU(rdata, "invalid_raw_format") << ": " << ss << ":" << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mFieldOpr = AosFieldOpr::toEnum(def->getAttrStr(AOSTAG_FIELD_OPR));
	mFromDocidIdx = def->getAttrInt(AOSTAG_FROM_DOCID_IDX, -1);
	mSetRecordDocid = def->getAttrBool(AOSTAG_SET_RECORD_DOCID, false);
	
	mFromFieldIdx = def->getAttrInt(AOSTAG_FROM_FIELD_IDX, -1);
	mToFieldIdx = def->getAttrInt(AOSTAG_TO_FIELD_IDX, -1);
	if (mFromFieldIdx >= 0)
	{
		mSep = def->getAttrStr(AOSTAG_SEPARATOR);
		if (mSep == "")
		{
			AosSetErrorU(rdata, "missing_separator");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	
	return true;
}

	
AosGroupFieldPtr
AosGroupField::createFieldStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	if (ss == "str") return AosGroupFieldStr::create(def, rdata);
	if (ss == "u64") return AosGroupFieldU64::create(def, rdata);

	return 0;
}
