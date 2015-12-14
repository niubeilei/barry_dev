////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//  This file contains a group of random functions and a random class. 
//  The OmnRandom class is obselete. You are encouraged to use the 
//  random functions.  
//
// Modification History:
// 05/18/2007 Moved from Util/ by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomUtil_h
#define Omn_Random_RandomUtil_h

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Debug/Debug.h"
#include "aosUtil/Types.h"
#include "Random/Ptrs.h"
#include "Random/RandomTypes.h"
#include "Util/String.h"
#include "Util/Opr.h"
#include "Util/IpAddr.h"
#include "Util/MacAddr.h"
#include "Util/StrType.h"


class OmnRandom
{
public:
	enum
	{
		eMaxStrLen = 1000000
	};

public:
	static char	mAlphanum[65];
	static int mAlphanumLen;

public:
	static void setSeed(const int seed){srand(seed);}
	static void init();
	static void init(u32 seed);

	// Integers
	inline static int nextInt() {return rand();}
	// inline static int nextInt(const int mask) {return (rand() & mask);}
	// inline static int nextInt(const int max) {if (max <= 0) return 0; return rand() % max;}
	static int nextInt1(int min, int max);
	static i64 nextInt64(i64 min,i64 max);
	static int nextU32(u32 min, u32 max)
	{
		if (min > max) 
		{
			u32 vv = min;
			min = max;
			max = vv;
		}

		if (min == max) return min;
	
		return rand() % (max-min) + min;
	}

	static int nextInt(int min, int max, int min2, int max2);
	static int intByRange(int *min, int *max, int *weights, const int len);
	static int intByRange(
						int min1, int max1, int weight1, 
						int min2, int max2, int weight2);
	static int intByRange(
						int min1, int max1, int weight1, 
						int min2, int max2, int weight2,
						int min3, int max3, int weight3);
	static int intByRange(
						int min1, int max1, int weight1, 
						int min2, int max2, int weight2,
						int min3, int max3, int weight3,
						int min4, int max4, int weight4);
	static int intByRange(
						int min1, int max1, int weight1, 
						int min2, int max2, int weight2,
						int min3, int max3, int weight3,
						int min4, int max4, int weight4,
						int min5, int max5, int weight5);
	static int intByRange(
						int min1, int max1, int weight1, 
						int min2, int max2, int weight2,
						int min3, int max3, int weight3,
						int min4, int max4, int weight4,
						int min5, int max5, int weight5,
						int min6, int max6, int weight6);

	// Strings
	static OmnString asciiStr(int min, int max);
	static OmnString printableStr(int min, int max, bool);
	static OmnString noSpaceStr(int minlen, int maxlen);
	static OmnString binaryStr(int minlen, int maxlen);
	static OmnString lowcaseStr(int minlen, int maxlen);
	static OmnString capitalStr(int minlen, int maxlen);
	static OmnString letterStr(int minlen, int maxlen);
	static OmnString dateStr(const int min_year, const int max_year);
	// Chen Ding, 2013/02/15
	static int letterDigitStr(
						char *buff,
						int minlen, 
						int maxlen, 
						const bool firstDigitAllowed = false);
	static OmnString letterDigitStr(
						int minlen, 
						int maxlen, 
						const bool firstDigitAllowed = false);
	static OmnString varName(
						int minlen, 
						int maxlen, 
						const bool firstUnderscoreAllowed);
    static OmnString mixedString(int minLen, int maxLen);
	inline static OmnString word(int minlen, int maxlen)
	{
		return letterDigitDashUnderscore(minlen, maxlen);
	}
	inline static OmnString word(int len)
	{
		return letterDigitDashUnderscore(len, len);
	}
	inline static OmnString word30()
	{
		int len = intByRange(
					1, 5, 70,
					6, 10, 20,
					11, 20, 8,
					21, 30, 2);
		return word(len);
	}

	inline static OmnString word50()
	{
		int len = intByRange(
					1, 5, 70,
					6, 10, 20,
					11, 20, 5,
					21, 30, 3, 
					31, 50, 2);
		return word(len);
	}

	// Digit Strings
	static OmnString digitStr(int minlen, int maxlen);
	static OmnString digitStr(
						int total_len, 
						bool left_align, 
						char padding, 
						const bool unique = false, 
						const bool reset_unique = false);

	static OmnString cellPhoneNumber(
						const int total_length, 
						const int phone_num_len, 
						const bool with_country_code,
						const bool left_align, 
						const char padding);

	inline static OmnString digitStr(u32 len)
	{
		return digitStr(len, len);
	}
	inline static OmnString digitStr10()
	{
		int len = intByRange(
				1, 5, 80, 
				6, 8, 15,
				9, 10,5);
		return digitStr(len);
	}

	// Bools
	static bool nextBool();
	static int nextBool(int trueWeight);

	// Others
	static OmnString strIP();
	static OmnString netMac();
	u32 ipAddr(uint32_t netAddr, uint32_t mask);

	// Percents
	static bool percent(int p1);
	static bool percent(const AosRandomRulePtr &rule,
					const OmnString &name, 
					const int pct);
	static int percent(int p1, int p2);
	static int percent(int p1, int p2, int p3);
	static int percent(int p1, int p2, int p3, int p4);
	static int percent(int p1, int p2, int p3, int p4, int p5);
	static int percent(int p1, int p2, int p3, int p4, int p5, int p6);

	// Utilities
	static int getLength(int minlen, int maxlen)
	{
		int min = minlen;
		int max = maxlen;
		if (min < 0) min = 0;
		if (max < 0) max = 0;
		if (max > eMaxStrLen) max = eMaxStrLen;
		if (min > max) 
		{
			min = maxlen;
			max = minlen;
		}

		int len = min;
		if (min == max) return len;
	
		len = rand() % (max-min) + min;
		return len;
	}
	static OmnString strByType(const AosStrType type, int min, int max);
	static OmnString getRandomStr(
				        const OmnString &origStr,
						const int lenremained,
						const int lennew);
	static int ipAddr(uint32_t *addr, uint32_t netAddr, uint32_t mask);
	static AosOpr nextOpr();
	static OmnString letterDigitStr(
						const u32 total_length, 
						const u32 min_length, 
						const bool left_align,
						const char padding);
	static OmnString nextLetterDigitStr(
						const u32 minlen, 
						const u32 maxlen, 
						const bool firstDigitAllowed = false);
	static bool nextLetterDigitStr(char *str, 
						const int minlen, 
						const int maxlen, 
						const bool firstDigitAllowed);
	static OmnString letterDigitDashUnderscore(int minlen, int maxlen);
	inline static OmnString letterDigitDashUnderscore(
			int min1, int max1, int weight1,
			int min2, int max2, int weight2)
	{
		int len = OmnRandom::intByRange(min1, max1, weight1, min2, max2, weight2);
		return letterDigitDashUnderscore(len, len);
	}
	static u64 nextU64();

	inline static OmnString letterDigitDashUnderscore(
			int min1, int max1, int weight1,
			int min2, int max2, int weight2,
			int min3, int max3, int weight3)
	{
		int len = OmnRandom::intByRange(
				min1, max1, weight1, 
				min2, max2, weight2,
				min3, max3, weight3);
		return letterDigitDashUnderscore(len, len);
	}

	static OmnString pickDomainName();
	static OmnString pickWordSeparator();
	static OmnString pickEntrySeparator();
	static OmnString pickValueDataType();
	static OmnString generateValue(const OmnString &min_value, const OmnString &max_value);

	static OmnString generateNextKey(
				u64 &crt_value, 
				const OmnString &prefix,
				const OmnString &seed,
				const OmnString &separator,
				const int prefix_len,
				const char leading_char,
				const u32 bitmask);

	// Chen Ding, 2013/03/19
	// bool generateUniqueNumbers(
	// 			const int min, 
	// 			const int max, 
	// 			vector<int> &numbers);
};
#endif
