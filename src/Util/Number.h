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
// Created by Young	2014/12/15
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_Number_h
#define Omn_Util_Number_h

#include "aosUtil/Types.h"
#include "Util/DataTypes.h"
#include "Util/String.h"

class AosNumber
{
private:
	enum
	{
		eInvalid = 0,

		eDftScaleValue = 2,
		eDftPrecisionValue = 18,

		eMax
	};

private:
	double 		mValue;		// the real value

public:
	u32			mPrecision;	// total number of digits
	int			mScale;		// number of digits to the right of the decimal point
	
public:
	AosNumber();
	AosNumber(const u32 precision, const int scale);
	AosNumber(const double value, const u32 precision, const int scale);
	AosNumber(const AosNumber &rhs);
	~AosNumber();


	bool			setValue(const double &vv);
	double 			getValue() const;
	u32				getTotalDigits() { return mPrecision; }
	int				getSmallDigits() { return mScale; }
	void			setTotalDigits(u32 digits) { mPrecision = digits; }
	void			setSmallDigits(int digits) { mScale = digits; }
	OmnString	 	toString() const;

	AosNumber 		operator + (const AosNumber &rhs);
	AosNumber 		operator - (const AosNumber &rhs);
	AosNumber		operator * (const AosNumber &rhs);
	AosNumber		operator / (const AosNumber &rhs);
	AosNumber&		operator = (const AosNumber &rhs);
	
	bool 	   		operator == (const AosNumber &rhs);
	bool 	   		operator != (const AosNumber &rhs);
	bool 	   		operator <  (const AosNumber &rhs);
	bool 	   		operator <= (const AosNumber &rhs);
	bool 	   		operator >  (const AosNumber &rhs);
	bool 	   		operator >= (const AosNumber &rhs);

	AosNumber 		operator + (const i64 &value);
	AosNumber 		operator - (const i64 &value);
	AosNumber 		operator * (const i64 &value);
	AosNumber 		operator / (const i64 &value);

	AosNumber 		operator + (const u64 &value);
	AosNumber 		operator - (const u64 &value);
	AosNumber 		operator * (const u64 &value);
	AosNumber 		operator / (const u64 &value);

	AosNumber 		operator + (const double &value);
	AosNumber 		operator - (const double &value);
	AosNumber 		operator * (const double &value);
	AosNumber 		operator / (const double &value);


private:
	void 			init();
};

#endif // Omn_Util_Number_h
