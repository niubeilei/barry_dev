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
#include "Util/Tester2/ValueRsltTester.h"
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>


AosValueRsltTester::AosValueRsltTester()
{
}

AosValueRsltTester::~AosValueRsltTester()
{
}

bool
AosValueRsltTester::start()
{
	dateTimeTest();
	return true;

	AosValueRslt rhs_v, lhs_v, value_v;
	while(1)
	{
		int rslt = random();
		if (rslt == 1)
		{
			rslt = random()%5;
			double value, lhs, rhs;
			rhs = getDoubleValue();
			rhs_v.setDouble(rhs);
			if (rslt == 0){
				lhs_v.setDouble(rhs);
			}else if (rslt == 1) {
				lhs_v.setDouble(getDoubleValue());
			} else if (rslt == 2) {
				rslt = random()%2;
				if (rslt){
					lhs_v.setDouble(getI64Value());
				} else {
					lhs_v.setI64(rhs);
				}
			} else if (rslt == 3) {
				rslt = random()%2;
				if (rslt){
					lhs_v.setDouble(getU64Value());
				} else {
					lhs_v.setU64(rhs);
				}
			}
			lhs = lhs_v.getDouble();
			value = rhs + lhs;
			value_v == rhs_v + lhs_v;
			aos_assert_r(value == value_v.getDouble(), false);

			value = rhs - lhs;
			value_v == rhs_v - lhs_v;
			aos_assert_r(value == value_v.getDouble(), false);

			value = rhs * lhs;
			value_v == rhs_v * lhs_v;
			aos_assert_r(value == value_v.getDouble(), false);

			value = rhs / lhs;
			value_v == rhs_v / lhs_v;
			aos_assert_r(value == value_v.getDouble(), false);
			if (rhs > lhs) aos_assert_r(rhs_v > lhs_v, false);
			if (rhs < lhs) aos_assert_r(rhs_v < lhs_v, false);
			if (rhs <= lhs) aos_assert_r(rhs_v <= lhs_v, false);
			if (rhs >= lhs) aos_assert_r(rhs_v >= lhs_v, false);
			if (rhs != lhs) aos_assert_r(rhs_v != lhs_v, false);
			if (rhs == lhs) aos_assert_r(rhs_v == lhs_v, false);
			continue;
		} else if (rslt == 1) {
			rslt = random()%5;
			i64 value, lhs, rhs;
			rhs = getI64Value();
			rhs_v.setI64(rhs);
			if (rslt == 0) {
				lhs_v.setI64(rhs);
			} else if (rslt == 1) {
				rslt = random()%2;
				if (rslt){
					lhs_v.setI64(getDoubleValue());
				} else {
					lhs_v.setDouble(rhs);
				}
			} else if (rslt == 2) {
				lhs_v.setI64(getI64Value());
			} else if (rslt == 3) {
				rslt = random()%2;
				if (rslt){
					lhs_v.setI64(getU64Value());
				} else {
					lhs_v.setU64(rhs);
				}
			}
			lhs = lhs_v.getI64();
			value = rhs + lhs;
			value_v == rhs_v + lhs_v;
			aos_assert_r(value == value_v.getI64(), false);

			value = rhs - lhs;
			value_v == rhs_v - lhs_v;
			aos_assert_r(value == value_v.getI64(), false);

			value = rhs * lhs;
			value_v == rhs_v * lhs_v;
			aos_assert_r(value == value_v.getI64(), false);

			value = rhs / lhs;
			value_v == rhs_v / lhs_v;
			aos_assert_r(value == value_v.getI64(), false);
			if (rhs > lhs) aos_assert_r(rhs_v > lhs_v, false);
			if (rhs < lhs) aos_assert_r(rhs_v < lhs_v, false);
			if (rhs <= lhs) aos_assert_r(rhs_v <= lhs_v, false);
			if (rhs >= lhs) aos_assert_r(rhs_v >= lhs_v, false);
			if (rhs != lhs) aos_assert_r(rhs_v != lhs_v, false);
			if (rhs == lhs) aos_assert_r(rhs_v == lhs_v, false);
			continue;
		} else if (rslt == 2) {
			rslt = random()%5;
			u64 value, lhs, rhs;
			rhs = getU64Value();
			rhs_v.setU64(rhs);
			if (rslt == 0) {
				lhs_v.setU64(rhs);
			} else if (rslt == 1) {
				lhs_v.setU64(getDoubleValue());
			} else if (rslt == 2) {
				lhs_v.setU64(getI64Value());
			} else if (rslt == 3) {
				lhs_v.setU64(getU64Value());
			}
			lhs = lhs_v.getU64();
			value = rhs + lhs;
			value_v == rhs_v + lhs_v;
			aos_assert_r(value == value_v.getU64(), false);

			value = rhs - lhs;
			value_v == rhs_v - lhs_v;
			aos_assert_r(value == value_v.getU64(), false);

			value = rhs * lhs;
			value_v == rhs_v * lhs_v;
			aos_assert_r(value == value_v.getU64(), false);

			value = rhs / lhs;
			value_v == rhs_v / lhs_v;
			aos_assert_r(value == value_v.getU64(), false);
			if (rhs > lhs) aos_assert_r(rhs_v > lhs_v, false);
			if (rhs < lhs) aos_assert_r(rhs_v < lhs_v, false);
			if (rhs <= lhs) aos_assert_r(rhs_v <= lhs_v, false);
			if (rhs >= lhs) aos_assert_r(rhs_v >= lhs_v, false);
			if (rhs != lhs) aos_assert_r(rhs_v != lhs_v, false);
			if (rhs == lhs) aos_assert_r(rhs_v == lhs_v, false);
			continue;
		} else {
			rslt = random()%5;
			OmnString value, lhs, rhs;
			rhs = getStringValue();
			rhs_v.setStr(rhs);
			if (rslt == 0){
				lhs_v.setStr(rhs);
			} else if (rslt == 1) {
				lhs_v.setDouble(getDoubleValue());
			} else if (rslt == 2) {
				lhs_v.setI64(getI64Value());
			} else if (rslt == 3) {
				lhs_v.setU64(getU64Value());
			} else {
				lhs_v.setStr(getStringValue());
			}

 			lhs = lhs_v.getStr();
			if (rhs > lhs)
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v > lhs_v), false);
			}
			if (rhs < lhs) 
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v < lhs_v), false);
			}
			if (rhs <= lhs) 
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v <= lhs_v), false);
			}
			if (rhs >= lhs) 
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v >= lhs_v), false);
			}
			if (rhs != lhs) 
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v != lhs_v), false);
			}
			if (rhs == lhs) 
			{
				if (rslt != 0 && rslt != 4)
					aos_assert_r(!(rhs_v == lhs_v), false);
			}
			continue;
		}
	}
	return true;
}

u64
AosValueRsltTester::getU64Value()
{
	boost::random_device rd;
	u64 value = rd();
	int rslt = random()%2;
	if (rslt)
		value = (value << 32) | rd();
	return value; 
}

i64
AosValueRsltTester::getI64Value()
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
AosValueRsltTester::getDoubleValue()
{
	return (double)getI64Value();
}

OmnString
AosValueRsltTester::getStringValue()
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

bool
AosValueRsltTester::dateTimeTest()
{
	int num = 1000;
	string year = "1980", month, day, hour, minutes, seconds, str_date;
	while (num--)
	{
		year += random()%100;
		month = random()%12 + 1;
		day = random()%30 + 1;
		hour = random()%24;
		minutes = random()%60;
		seconds = random()%60;
		str_date = yaer + "-" + month + "-" + day + " " + hour + ":" ;
		AosDateTime dt(str_date, "");
	}
}

