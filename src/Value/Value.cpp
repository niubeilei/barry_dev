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
//
// Modification History:
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Value/Value.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/Buff.h"
#include "Value/ValueU64.h"
#include "Value/ValueU32.h"
#include "Value/ValueU16.h"
#include "Value/ValueU8.h"
#include "Value/ValueInt8.h"
#include "Value/ValueInt16.h"
#include "Value/ValueInt.h"
#include "Value/ValueInt64.h"
#include "Value/ValueFloat.h"
#include "Value/ValueDouble.h"
#include "XmlUtil/XmlTag.h"
#include <math.h>

AosValue::AosValue(
		const bool is_null,
		const AosDataType::E type, 
		const AosValueImpPtr &value)
:
mIsNull(is_null),
mDataType(type),
mValue(value),
mValueRaw(mValue.getPtrNoLock())
{
}


AosValue::AosValue(const u64 value)
:
mIsNull(false),
mDataType(AosDataType::eU64)
{
	mValue = OmnNew AosValueU64(value);
	mValueRaw = mValue.getPtrNoLock();
}


AosValue::~AosValue()
{
}


const char *
AosValue::getCharStr(int &len) const
{
	if (mIsNull) 
	{
		len = 0;
		return 0;
	}
	return mValueRaw->getCharStr(len);
}
	

bool 
AosValue::setChar(const char vv)
{
	mIsNull = false;
	return mValueRaw->setChar(vv);
}


bool 
AosValue::setU64(const u64 vv)
{
	mIsNull = false;
	return mValueRaw->setU64(vv);
}


bool 
AosValue::setU32(const u32 vv)
{
	mIsNull = false;
	return mValueRaw->setU32(vv);
}


bool 
AosValue::setU16(const u16 vv)
{
	mIsNull = false;
	return mValueRaw->setU16(vv);
}


bool 
AosValue::setU8(const u8 vv)
{
	mIsNull = false;
	return mValueRaw->setU8(vv);
}


bool 
AosValue::setInt64(const int64_t vv)
{
	mIsNull = false;
	return mValueRaw->setInt64(vv);
}


bool 
AosValue::setInt(const int vv)
{
	mIsNull = false;
	return mValueRaw->setInt(vv);
}


bool 
AosValue::setInt16(const i16 vv)
{
	mIsNull = false;
	return mValueRaw->setInt16(vv);
}


bool 
AosValue::setInt8(const i8 vv)
{
	mIsNull = false;
	return mValueRaw->setInt8(vv);
}


bool 
AosValue::setDouble(const double vv)
{
	mIsNull = false;
	return mValueRaw->setDouble(vv);
}


bool 
AosValue::setFloat(const float vv)
{
	mIsNull = false;
	return mValueRaw->setFloat(vv);
}


bool 
AosValue::setCharStr(const char *data, const int len, const bool copy_flag)
{
	mIsNull = false;
	return mValueRaw->setCharStr(data, len, copy_flag);
}


bool 
AosValue::setXml(const AosXmlTagPtr &vv)
{
	mIsNull = false;
	return mValueRaw->setXml(vv);
}


bool 
AosValue::setBuff(const AosBuffPtr &buff)
{
	mIsNull = false;
	return mValueRaw->setBuff(buff);
}


OmnString 
AosValue::toString() const
{
	return mValueRaw->toString(mIsNull);
}


double 
AosValue::sqrt() const
{
	OmnNotImplementedYet;
	return 0.0;
}


double 
AosValue::power(const double s) const
{
	OmnNotImplementedYet;
	return 0.0;
}


AosValue 
AosValue::operator +(const AosValue &rhs)
{
	AosValueImpPtr value = mValueRaw->clone();
	if (mIsNull && rhs.mIsNull) return AosValue(true, mDataType, value);
	value->add(rhs);
	AosValue vv(false, mDataType, value);
	return vv;
}

	
AosValue 
AosValue::operator -(const AosValue &rhs)
{
	AosValueImpPtr value = mValueRaw->clone();
	if (mIsNull && rhs.mIsNull) return AosValue(true, mDataType, value);
	value->minus(rhs);
	AosValue vv(false, mDataType, value);
	return vv;
}


AosValue 
AosValue::operator *(const AosValue &rhs)
{
	AosValueImpPtr value = mValueRaw->clone();
	if (mIsNull && rhs.mIsNull) return AosValue(true, mDataType, value);
	value->multiply(rhs);
	AosValue vv(false, mDataType, value);
	return vv;
}


AosValue 
AosValue::operator /(const AosValue &rhs)
{
	AosValueImpPtr value = mValueRaw->clone();
	if (mIsNull && rhs.mIsNull) return AosValue(true, mDataType, value);
	value->divide(rhs);
	AosValue vv(false, mDataType, value);
	return vv;
}


bool 
AosValue::operator ==(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkEQ(rhs);
}


bool 
AosValue::operator !=(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkNE(rhs);
}


bool 
AosValue::operator >(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkGT(rhs);
}


bool 
AosValue::operator >=(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkGE(rhs);
}


bool 
AosValue::operator <(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkLT(rhs);
}


bool 
AosValue::operator <=(const AosValue &rhs)
{
	if (mIsNull || rhs.mIsNull) return false;
	return mValueRaw->checkLE(rhs);
}


AosValue &
AosValue::operator = (const AosValue &rhs)
{
	mIsNull = rhs.mIsNull;
	mDataType = rhs.mDataType;
	mValue = rhs.mValue;
	mValueRaw = mValue.getPtrNoLock();
	return *this;
}

