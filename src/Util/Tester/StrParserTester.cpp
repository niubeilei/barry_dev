////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringTester.cpp
// Description:
//   
//
// Modification History:
// 05/19/2007 Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/StrParserTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "Util/StrType.h"
#include "Util1/Time.h"



bool OmnStrParserTester::start()
{
	cout << "    Start OmnString Tester ...";

	// testGetNameValuePair(100);
	testGetSubstr(10000000);
	return true;
}


bool
OmnStrParserTester::testGetNameValuePair(const u32 tries)
{
	OmnString str = "[name1, value1], [name2, value2], [name3, value3]";
	OmnStrParser1 parser(str);

	parser.reset();
	OmnString name, value;
	int index = 1;
	while (parser.hasMore())
	{
		OmnString n = "name";
		n << index;

		OmnString v = "value";
		v << index++;

		OmnTCTrue(parser.getValuePair(name, value, '[', ',')) << endtc;
		OmnTCTrue(n == name) << "n = " << n << " name = " << name << endtc;
		OmnTCTrue(v == value) << "v = " << v << " value = " << value << endtc;
	}

	return true;
}


bool
OmnStrParserTester::testGetSubstr(const u32 tries)
{
	u32 idx = 0;
	// int repeat = 0;
	while (idx < tries)
	{
		if (idx != 0 && idx%100000 == 0)
		{
			cout << "Tries: " << idx << endl;
		}

		OmnString leading = OmnRandom::digitStr(0, 30);
		OmnString pattern = OmnRandom::digitStr(1, 20);
		OmnString tail = OmnRandom::digitStr(0, 40);
		OmnString prefix = OmnRandom::digitStr(0, 40);
		OmnString postfix = OmnRandom::digitStr(0, 40);

		// 
		// Make sure prefix + pattern does not contain the pattern until
		// the end of the 'prefix'.
		//
		OmnString pp = prefix;
		pp << leading;
		OmnStrParser1 parser1(pp);
		if (parser1.findWord(0, leading) != prefix.length()) 
		{
			continue;
		}

		// 
		// Similarly, make sure 'pattern + tail' does not contain the
		// 'tail' until the end of 'pattern'.
		//
		OmnString qq = pattern;
		qq << tail;
		OmnStrParser1 parser2(qq);
		if (parser2.findWord(0, tail) != pattern.length()) 
		{
			continue;
		}

		// 
		// Now construct the string: prefix + leading + pattern + tail + postfix
		// We know that this string should match: leading + $$$ + tail and the
		// matched part must be 'pattern'.
		//
		OmnString str = prefix;
		str << leading << pattern << tail << postfix;

		OmnStrParser1 parser3(str);
		OmnString thePattern = leading;
		thePattern << "$$$" << tail;
		OmnString patternFound = parser3.getSubstr(thePattern, "");
		if (leading == "")
		{
			OmnString ttt = prefix;
			ttt << pattern;
			OmnTCTrue(patternFound == ttt)
				<< "\nString: " << str
				<< "\nLeading: " << leading
				<< "\nPattern: " << pattern
				<< "\nThePattern: " << thePattern
				<< "\nPatternFound: " << patternFound 
				<< "\nTail: " << tail << endtc;
		}
		else if (tail == "")
		{
			OmnString ttt = tail;
			ttt << postfix;
			OmnTCTrue(patternFound == ttt)
				<< "\nString: " << str
				<< "\nLeading: " << leading
				<< "\nPattern: " << pattern
				<< "\nThePattern: " << thePattern
				<< "\nPatternFound: " << patternFound 
				<< "\nTail: " << tail << endtc;
		}
		else
		{
			OmnTCTrue(pattern == patternFound) 
				<< "\nString: " << str
				<< "\nLeading: " << leading
				<< "\nPattern: " << pattern
				<< "\nThePattern: " << thePattern
				<< "\nPatternFound: " << patternFound 
				<< "\nTail: " << tail << endtc;
		}
		idx++;
	}

	return true;
}



