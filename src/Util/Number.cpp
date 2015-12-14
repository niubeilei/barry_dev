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
//	12/06/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Util/Number.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

#include <cmath>


const AosNumber sgInvalidValue;

AosNumber::AosNumber()
{
	init();
}


AosNumber::AosNumber(u32 precision, int scale)
{
	init();
	mPrecision = precision;
	mScale = scale;
}


AosNumber::AosNumber(const double value, const u32 precision, const int scale)
{
	mValue = value;
	mPrecision = precision;
	mScale = scale;
}


AosNumber::AosNumber(const AosNumber &rhs)
{
	mPrecision = rhs.mPrecision;
	mScale = rhs.mScale;
	mValue = rhs.mValue;
}


AosNumber::~AosNumber()
{
}


void
AosNumber::init()
{
	mPrecision = eDftPrecisionValue;
	mScale = eDftScaleValue;
	mValue = 0.0;
}


bool
AosNumber::setValue(const double &vv)
{
	mValue = vv;
	return true;
}

double
AosNumber::getValue() const
{
	return mValue;
}


OmnString
AosNumber::toString() const
{
	if (mPrecision <= 0 || mValue > pow(10, mPrecision+1)) return "0";

	OmnString strval = "";
	if (mScale > 0) 
	{
		if ((u32)mScale >= mPrecision)
		{
			if (mValue > 0) return "0";
			double n = mValue - (u64)mValue;
			if (n > n*pow(0.1, mScale-mPrecision)) return "0";
		}

		char val[400];
		OmnString format = "";
		format << "%.0" << mScale << "f";
		sprintf(val, format.data(), mValue);
		return OmnString(val); 
	}
	if (mScale == 0) 
	{
		int64_t val = (int64_t)round(mValue); //round(mValue / pow(10, abs(mScale))) * pow(10, abs(mScale));	
		strval << val;
	}
	else
	{
		int64_t val = round(mValue / pow(10, abs(mScale))) * pow(10, abs(mScale));
		strval << val;
	}

	return strval;
}


AosNumber
AosNumber::operator + (const AosNumber &rhs)
{
	u32 prec = (mPrecision > rhs.mPrecision) ? mPrecision : rhs.mPrecision;
	int scale = (mScale > rhs.mScale) ? mScale : rhs.mScale;
	double val = mValue + rhs.mValue;

	AosNumber num(prec, scale);
	num.setValue(val);
	return num;
}


AosNumber
AosNumber::operator - (const AosNumber &rhs)
{
	u32 prec = (mPrecision > rhs.mPrecision) ? mPrecision : rhs.mPrecision;
	int scale = (mScale > rhs.mScale) ? mScale : rhs.mScale;
	double val = mValue - rhs.mValue;

	AosNumber num(prec, scale);
	num.setValue(val);
	return num;
}


AosNumber
AosNumber::operator * (const AosNumber &rhs)
{
	u32 prec = (mPrecision > rhs.mPrecision) ? mPrecision : rhs.mPrecision;
	int scale = (mScale > rhs.mScale) ? mScale : rhs.mScale;
	double val = mValue * rhs.mValue;

	AosNumber num(prec, scale);
	num.setValue(val);
	return num;
}


AosNumber
AosNumber::operator / (const AosNumber &rhs)
{
	if (rhs.mValue == 0)
	{
		OmnAlarm << "Divided by 0" << enderr;
		return sgInvalidValue;
	}

	u32 prec = (mPrecision > rhs.mPrecision) ? mPrecision : rhs.mPrecision;
	int scale = (mScale > rhs.mScale) ? mScale : rhs.mScale;
	double val = mValue / rhs.mValue;

	AosNumber num(prec, scale);
	num.setValue(val);
	return num;
}

AosNumber&
AosNumber::operator = (const AosNumber &rhs)
{
	mPrecision = rhs.mPrecision;
	mScale = rhs.mScale;
	mValue = rhs.getValue();
	return *this;
}


bool 
AosNumber::operator == (const AosNumber &rhs)
{
	if (abs(mValue - rhs.mValue) < pow(0.1, 6)) return true;

	return false;
}


bool 
AosNumber::operator != (const AosNumber &rhs)
{
	return !(*this == rhs);
}


bool 
AosNumber::operator <  (const AosNumber &rhs)
{
	return mValue < rhs.mValue;
}


bool 
AosNumber::operator <= (const AosNumber &rhs)
{
	return !(*this > rhs);
}


bool 
AosNumber::operator >  (const AosNumber &rhs)
{
	return mValue >	rhs.mValue;
}


bool 
AosNumber::operator >= (const AosNumber &rhs)
{
	return !(*this < rhs);
}


AosNumber
AosNumber::operator + (const i64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue + (double)value);
	return number;
}


AosNumber
AosNumber::operator - (const i64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue - (double)value);
	return number;
}


AosNumber
AosNumber::operator * (const i64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue * (double)value);
	return number;
}


AosNumber
AosNumber::operator / (const i64 &value)
{
	if (value == 0) 
	{
		OmnAlarm << "Divided by 0" << enderr;
		return sgInvalidValue;
	}

	AosNumber number(*this);
	number.setValue(mValue / value);
	return number;
}


AosNumber
AosNumber::operator + (const u64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue + (double)value);
	return number;
}


AosNumber
AosNumber::operator - (const u64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue - (double)value);
	return number;
}


AosNumber
AosNumber::operator * (const u64 &value)
{
	AosNumber number(*this);
	number.setValue(mValue * (double)value);
	return number;
}


AosNumber
AosNumber::operator / (const u64 &value)
{
	if (value == 0) 
	{
		OmnAlarm << "Divided by 0" << enderr;
		return sgInvalidValue;
	}

	AosNumber number(*this);
	number.setValue(mValue / value);
	return number;
}


AosNumber
AosNumber::operator + (const double &value)
{
	AosNumber number(*this);
	number.setValue(mValue + value);
	return number;
}


AosNumber
AosNumber::operator - (const double &value)
{
	AosNumber number(*this);
	number.setValue(mValue - value);
	return number;
}


AosNumber
AosNumber::operator * (const double &value)
{
	AosNumber number(*this);
	number.setValue(mValue * value);
	return number;
}


AosNumber
AosNumber::operator / (const double &value)
{
	if (value == 0)
	{
		OmnAlarm << "Divided by 0" << enderr;
		return sgInvalidValue;
	}

	AosNumber number(*this);
	number.setValue(mValue / value);
	return number;
}
