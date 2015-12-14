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
// 2014/12/15 Created by Jiang Arvin
////////////////////////////////////////////////////////////////////////////
/*
#ifndef AOS_Value_ValueTemplate_h
#define AOS_Value_ValueTemplate_h
#include<iostream>
using namespace std;
#include<typeinfo>
#define AINT8	(typeid(i8).name())
#define AINT16	(typeid(i16).name())
#define AINT	(typeid(int).name())
#define AINT64	(typeid(i64).name())
#define AFLOAT	(typeid(float).name())
#define ADOUBLE (typeid(double).name())
#define AU8		(typeid(u8).name())
#define AU16	(typeid(u16).name())
#define AU32	(typeid(u32).name())
#define AU64	(typeid(u64).name())
// template<typename T1, T2>
template<typename T1>
class AosValueTemplate
{
private:
	T1 mValue;

public:
	AosValueTemplate(const T1 value)
	:
	mValue(value)
	{
	}

	virtual ~AosValueTemplate();

	//return the datatype of this class
	virtual int getDateType()const;

	virtual T2 		getValue() const;
	virtual bool 	setValue(T2 value);

	//operators on AosValue
	virtual AosValue& operator +(const AosValue &rhs);
	virtual AosValue& operator -(const AosValue &rhs);
	virtual AosValue& operator *(const AosValue &rhs);
	virtual AosValue& operator /(const AosValue &rhs);
	virtual bool operator ==(const AosValue &rhs);
	virtual bool operator !=(const AosValue &rhs);
	virtual bool operator >(const AosValue &rhs);
	virtual bool operator >=(const AosValue &rhs);
	virtual bool operator <(const AosValue &rhs);
	virtual bool operator <=(const AosValue &rhs);

	//operations on other values
	virtual AosValue& operator +(const T2 &rhs);
	virtual AosValue& operator -(const T2 &rhs);
	virtual AosValue& operator *(const T2 &rhs);
	virtual AosValue& operator /(const T2 &rhs);
	virtual bool operator ==(const T2 &rhs);
	virtual bool operator !=(const T2 &rhs);
	virtual bool operator >(const T2 &rhs);
	virtual bool operator >=(const T2 &rhs);
	virtual bool operator <(const T2 &rhs);
	virtual bool operator <=(const T2 &rhs);

};

#endif
*/
