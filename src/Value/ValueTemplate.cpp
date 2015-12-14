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
// 2014/12/15 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
/*
#include "Value/ValueTemplate.h"

//return the datatype of this class
template<typename T1, T2>
char*
AosValueTemplate::getDateType()const
{
	return typeid(T1).name();
}

template<typename T1, T2>
T1 		
AosValueTemplate::getValue() const
{
	T1 value = mValue;
	return value;
}

template<typename T1, T2>
bool 	
AosValueTemplate::setValue(T2 value)
{
	mValue = value;
}

//operators on AosValue
AosValue&
AosValueTemplate::operator +(const AosValue &rhs)
{
	const char* lType = this.getDataType();
	const char* rType = rhs.getDataType();
	if((lType = AINT8||lType = AINT16||lType = AINT||lType = AINT64||lType = AFLOAT||
			lType = ADOUBLE||lType = AU8||lType = AU16||lType = AU32||lType = AU6i4)
			&&(rType = AINT8||rType = AINT16||rType = AINT||rType = AINT64||rType = AFLOAT||
			rType = ADOUBLE||rType = AU8||rType = AU16||rType = AU32||rType = AU64))
	{
		double value = (double)mValue + (double)rhs.getValue();
		AosValue *rslt = getAosValue(value);
		return *rslt;
	}
}
AosValue&
AosValueTemplate::operator -(const AosValueTemplate &rhs)
{
	double value = (double)mValue - (double)rhs.getValue();
	AosValue *rslt = getAosValue(value);
	return *rslt;
}
AosValue&
AosValueTemplate::operator *(const AosValueTemplate &rhs)
{
	double value = (double)mValue * (double)rhs.getValue();
	AosValue *rslt = getAosValue(value);
	return *rslt;
}

AosValue&
AosValueTemplate::operator /(const AosValueTemplate &rhs)
{
	double value = (double)mValue / (double)rhs.getValue();
	AosValue *rslt = getAosValue(value);
	return *rslt;
}

bool 
AosValueTemplate::operator ==(const AosValueTemplate &rhs)
{

}
bool 
AosValueTemplate::operator !=(const AosValueTemplate &rhs)
{

}
bool 
AosValueTemplate::operator >(const AosValueTemplate &rhs)
bool 
AosValueTemplate::operator >=(const AosValueTemplate &rhs)
bool 
AosValueTemplate::operator <(const AosValueTemplate &rhs)
bool 
AosValueTemplate::operator <=(const AosValueTemplate &rhs)

//operations on other values
AosValueTemplate 
operator +(const T2 &rhs)
{
	switch (typeid(mValue + rhs)
	{
		case eInt32:
			return (new AosValueTemplate(mValue + rhs)); 

		default:
			break;
	}

	return NULL;

}

AosValueTemplate operator -(const T2 &rhs);
AosValueTemplate operator *(const T2 &rhs);
AosValueTemplate operator /(const T2 &rhs);
bool operator ==(const T2 &rhs);
bool operator !=(const T2 &rhs);
bool operator >(const T2 &rhs);
bool operator >=(const T2 &rhs);
bool operator <(const T2 &rhs);
bool operator <=(const T2 &rhs);
*/
