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
// 08/19/2015	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester2/ValueRsltPerformances.h"
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>


AosValueRsltPerformances::AosValueRsltPerformances()
{
}

AosValueRsltPerformances::~AosValueRsltPerformances()
{
}

bool
AosValueRsltPerformances::start()
{
	OmnString value("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
//	= getStringValue();
	u64 t1 = OmnGetTimestamp();
	for (u32 i = 0; i < 100000000; i++)
	{
		AosValueRslt value_rslt(value);
	}
	u64 t2 = OmnGetTimestamp();
OmnScreen << "Performances : Alloc String ValueRslt. " 
		  << " Length is " << value.length() << "."
		  << " Time is " << (t2-t1)/1000 << "ms." << endl;

	u64 t3 = OmnGetTimestamp();
	AosValueRslt value_rslt;
	for (u32 i = 0; i < 100000000; i++)
	{
		value_rslt.setStr(value);
	}
	u64 t4 = OmnGetTimestamp();

OmnScreen << "Performances : Call setStr. " 
		  << " Length is " << value.length() << "."
		  << " Time is " << (t4-t3)/1000<< "ms." << endl;

	u64 t5 = OmnGetTimestamp();
	AosValueRslt lhs, rhs;
	lhs.setStr(value);
	rhs.setStr(value);
	for (u32 i = 0; i < 100000000; i++)
	{
		lhs < rhs;
	}
	u64 t6 = OmnGetTimestamp();
OmnScreen << "Performances : Call compare. " 
		  << " Length is " << value.length() << "."
		  << " Time is " << (t6-t5)/1000 << "ms." << endl;
	exit(0);
	return true;
}



u64
AosValueRsltPerformances::getU64Value()
{
	boost::random_device rd;
	u64 value = rd();
	int rslt = random()%2;
	if (rslt)
		value = (value << 32) | rd();
	return value; 
}

i64
AosValueRsltPerformances::getI64Value()
{
	boost::random_device rd;
	i64 value = rd();
	int rslt = random()%5;
	if (rslt == 0) {
		value = (value << 31) | rd();
	 } else if (rslt == 1){
		value = (value << 32) | rd();
	 } else if (rslt == 2){
		value = (value >> 16) *-1;
	 } else if (rslt == 3){
		value = value >> 16;
	 }
	return value; 
}

double
AosValueRsltPerformances::getDoubleValue()
{
	return (double)getI64Value();
}

OmnString
AosValueRsltPerformances::getStringValue()
{
	OmnString vv;
	std::string chars(
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890"
			"!@#$%^&*()"
			"`~-_=+[{]}\\|;:'\",<.>/? ");
	boost::random::random_device rng;
	boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
	int len = random()%100000;
	len++;
	for(int i = 0; i < len; ++i) {
		vv << chars[index_dist(rng)];
	} 
	return vv;
}


