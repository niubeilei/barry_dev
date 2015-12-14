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
// 	Created: 01/13/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_UtUtil_h
#define Omn_Util_UtUtil_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "SEUtil/SeTypes.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include <list>
#include <map>
#include <ext/hash_map>

using namespace std;



inline u64 AosStrHashFunc(const char *word, const int wordlen)
{
	
    u64 hashkey = 5381;
	char c;
	for (int i=0; i<wordlen; i++)
	{
    	c = word[i];
    	hashkey = ((hashkey << 5) + hashkey) + c; // i.e., hash * 33 + c 
	}
	return hashkey;
}

inline u32 AosStrHashFunc2(const char *word, const int wordlen)
{
	u32 value = AosStrHashFunc(word, wordlen);
	return (value >> 16) + value;
}


inline u32 AosStrHashFunc3(const char *word, const int wordlen)
{
    u32 hashkey = 0;
	char c;
	for (int i=0; i<wordlen; i++)
	{
    	hashkey += c;
	}
	return hashkey;
}


inline u32 AosStrHashFunc(u32 &hashkey, const char *word, const int wordlen)
{
	char c;
	for (int i=0; i<wordlen; i++)
	{
    	c = word[i];
    	hashkey = ((hashkey << 5) + hashkey) + c;
	}
	return hashkey;
}

extern OmnString AosNextWord(const int maxlen);
extern OmnString AosNextWord(
					const char *map, 
					const int mapMin,
					const int mapMax,
					const char **excludes, 
					const int numExcludes, 
					const int *lenWeights, 
					const int lenWeightSize);
extern OmnString AosNextWord(
					const char *map, 
					const int mapMin,
					const int mapMax,
					const char **excludes, 
					const int numExcludes, 
					const int len);

// Chen Ding, 07/20/2010
struct Aosltstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

extern OmnString AosGeneratePasswd(int length = 10);

const char AosLetterMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

inline void AosRandomLetterStr(const int len, char *data)
{
	int i = 0;
	while (i<len)
	{
		u32 idx = rand();
		data[i++] = AosLetterMap[idx % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>8) % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>16) % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>24) % 0x3f];
	}
	data[len] = 0;
}


// Chen Ding, 05/31/2012
const char AosLetterOnlyMap[] = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzABCDEFGHIJKL";

inline void AosRandomLetterOnlyStr(const int len, char *data)
{
	int i = 0;
	while (i<len)
	{
		u32 idx = rand();
		data[i++] = AosLetterOnlyMap[idx % 0x3f];
		if (i < len) data[i++] = AosLetterOnlyMap[(idx>>8) % 0x3f];
		if (i < len) data[i++] = AosLetterOnlyMap[(idx>>16) % 0x3f];
		if (i < len) data[i++] = AosLetterOnlyMap[(idx>>24) % 0x3f];
	}
	data[len] = 0;
}


inline bool AosRandomLetterStr(
		const int min, 
		const int max, 
		int &len,
		char *data)
{
	aos_assert_r(max > min, false);
	len = rand() % (max-min) + min;
	int i = 0;
	while (i<len)
	{
		u32 idx = rand();
		data[i++] = AosLetterMap[idx % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>8) % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>16) % 0x3f];
		if (i < len) data[i++] = AosLetterMap[(idx>>24) % 0x3f];
	}
	data[len] = 0;
	return true;
}

//John 27/12/2010
extern bool AosGetStrDiff(
		const OmnString &str1, 
		const OmnString &str2,
		const OmnString &seps,
		list<OmnString> &list1,
		list<OmnString> &list2,
		list<OmnString> &list3);

//Zky2789
extern bool AosCalHashkey(
		const char *word,
		const int len,
		u32 &hashkey,
		const bool init);
//Zky2789
extern OmnString AosCalHashkey(const u64 &value1, const u64 &value2, 
			const u64 &value3, const OmnString &key);

inline bool AosIsValidHashBitmap(const u64 &bitmap)
{
	u32 tsize = 1;
	for (int i=0; i<64; i++)
	{
		tsize = (tsize << 1) + 1;
		if (tsize == bitmap) return true;
		if (tsize > bitmap) return false;
	}
	return false;
}

// Chen Ding, 03/02/2012
extern OmnString AosComposeByWords(
		const OmnString &str1, 
		const OmnString &str2, 
		const OmnString &sep); 

extern int AosFindFirstIndex(
		const char **mValues, 
		const u32 num_values, 
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha);

extern int AosFindFirstIndexReverse(
		const char **mValues, 
		const u32 num_values, 
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha);

inline int AosFindFirstIndex(
		const char **values, 
		const u32 num_values, 
		const AosOpr opr, 
		const OmnString &value, 
		const bool reverse,
		const bool isNumAlpha)
{
	if (reverse) return AosFindFirstIndexReverse(values, num_values, opr, value, isNumAlpha);
	return AosFindFirstIndex(values, num_values, opr, value, isNumAlpha);
}

extern bool AosUnicomConvertUpnPrefix();
extern bool AosUnicomConvertCpnPrefix();
extern OmnString AosNumber2String(const int64_t &size);

#endif
