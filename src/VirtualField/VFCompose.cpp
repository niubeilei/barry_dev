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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VirtualField/VFCompose.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosVFCompose_1(
 		const AosRundataPtr &rdata,
		const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosVFCompose(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosVFCompose::AosVFCompose(const int version)
:
AosVirtualField(version)
{
}


AosVFCompose::~AosVFCompose()
{
}


AosJimoPtr
AosVFCompose::cloneJimo() const
{
	return OmnNew AosVFCompose(*this);
}


bool
AosVFCompose::getFieldValue(
		AosRundata *rdata, 
		AosDataRecordObj *record, 
		AosValueRslt &value)
{
	value.setNullValue();

	// Check whether it is filtered.
	AosValueRslt vv;
	if (mFilterRaw)
	{
		bool rslt = mFilterRaw->getValue(rdata, record, vv);
		if (!rslt) return false;
		if (!vv.getValueBool())
		{
			// It is filtered. 
			return true;
		}
	}

	// It composes the field values.
	u32 size = mFields.size();
	OmnString value_str;
	for(u32 i=0; i<size; i++)
	{
		bool rslt = record->getFieldValue(mFields[i].mFieldIdx, vv, false, rdata);
		if (!rslt) return false;

		if (vv.isNull())
		{
			if (mFields[i].mNullDefault.isNull()) 
			{
				// It is ignored.
				return true;
			}

			vv = mFields[i].mNullDefault;
		}

		if (mFields[i].mNeedTrim) vv.normalizeWhiteSpace();
		if (vv.isEmpty())
		{
			if (mFields[i].mEmptyDefault.isNull())
			{
				// Empty field is ignored.
				return true;
			}
			vv = mFields[i].mEmptyDefault;
		}

		OmnString ss = vv.getValueStr1();
		value_str << mFields[i].mSep << ss;
	}

	if (mRsltFilterRaw)
	{
		AosValueRslt v_rslt = value_str;
		bool rslt = mRsltFilterRaw->getValue(rdata, v_rslt, vv);
		if (!rslt) return false;
		if (!vv.getValueBool()) 
		{
			// Filtered
			return true;
		}
	}

	value = value_str;
	return true;
}

