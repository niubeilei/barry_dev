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
//
// Modification History:
// 2009/10/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/UtUtil.h"
// #include "Random/RandomUtil.h"

#include "Alarm/Alarm.h"
#include "IILUtil/IILUtil.h"
#include "Porting/TimeOfDay.h"
#include "Util/Opr.h"
#include "Util/File.h"
#include "Util/StrSplit.h"
#include "Util/StrParser.h"
#include "Util/HashUtil.h"

static char *sgDftmap[128];
static bool  sgAosNextWordInitFlag = false;

static char sgMap[64];
static bool  sgAosMapInitFlag = false;

void AosNextWordInit()
{
	memset(sgDftmap, 0, 128);
	memset(&sgDftmap[33], 1, 127-33);
	sgAosNextWordInitFlag = true;
}

void AosMapInit()
{
	for (int i=0; i<=9; i++)
	{
		sgMap[i] = (char)(i+48);
	}
	
	for (int i = 10; i<=35; i++)
	{
		sgMap[i] = (char)(55+i);
	}

	for (int i = 36; i<= 61; i++)
	{
		sgMap[i] = (char)(61+i);
	}
	sgMap[62] = '+';
	sgMap[63] = '-';
	sgAosMapInitFlag = true;
}


OmnString 
AosNextWord(
	const char *map, 
	const int mapMin,
	const int mapMax,
	const char **excludes, 
	const int numExcludes, 
	const int *lenWeights, 
	const int lenWeightSize)
{

	// This function randomly generates a word that contains
	// the characters defined by 'map', but not one of the
	// 'exclude'. The word length is determined randomly
	// by 'lenWeights'.
	const int lcMaxLen = 15;

	int len;
	if (lenWeights)
	{
		int idx = rand() % lenWeightSize;
		int range = lenWeights[idx];
		len = (rand() % range) + 1;
	}
	else
	{
		len = rand() % lcMaxLen;
	}

	return AosNextWord(map, mapMin, mapMax, excludes, numExcludes, len);
}


OmnString 
AosNextWord(
	const char *map, 
	const int mapMin,
	const int mapMax,
	const char **excludes, 
	const int numExcludes, 
	const int len)
{
	OmnString str(len, ' ', true);
	char *data = (char *)str.data();
	int mapSize = mapMax - mapMin + 1;
	while (1)
	{
		// Construct the word
		for (int i=0; i<len; i++)
		{
			// Generate the next char
			while (1)
			{
				int c = mapMin + (rand() % mapSize);
				if (map[c]) 
				{
					data[i] = c;
					break;
				}
			}
		}
		data[len] = 0;

		if (!excludes) return str;

		// Check whether the word is in one of the 'excludes'
		bool found = false;
		for (int i=0; i<numExcludes; i++)
		{
			if (strcmp(excludes[i], data) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found) return str;
	}

	OmnShouldNeverComeHere;
	return "";
}


OmnString AosNextWord(const int maxlen)
{
	int len;
	if (maxlen <= 0) len = 10;
	len = rand() % maxlen;

	if (!sgAosNextWordInitFlag) AosNextWordInit();

	return AosNextWord((const char *)sgDftmap, 33, 126, (const char **)0, 0, len);
}


// Chen Ding, 2014/12/06
OmnString AosGeneratePasswd(int length)
{
	/*
	const int lsMaxPasswdLen = 64;
	char str[lsMaxPasswdLen+1];

	int len = (length < lsMaxPasswdLen)?length:lsMaxPasswdLen;
	int secs;
	for (int i=0; i<len; i++)
	{
		secs =(int)	OmnGetTimestamp();
		OmnRandom::setSeed(secs);
		str[i] = (char) OmnRandom::nextInt(48,57);
	}
	str[len] = 0;
	return OmnString(str);
	*/
	return "";
}

// Chen Ding, 03/02/2012
OmnString AosComposeByWords(
		const OmnString &str1, 
		const OmnString &str2, 
		const OmnString &sep)
{
	// This function creates a new string that contains all the words
	// contained in both 'str1' and 'str2', but redudant words are
	// removed.
	list<OmnString> list1, list2, list3;
	bool rslt = AosGetStrDiff(str1, str2, sep, list1, list2, list3);
	aos_assert_r(rslt, "");

	OmnString str;
	while (list1.size() > 0)
	{
		if (str != "") str << sep;
		str << list1.front();
		list1.pop_front();
	}

	while (list2.size() > 0)
	{
		if (str != "") str << sep;
		str << list2.front();
		list2.pop_front();
	}

	while (list3.size() > 0)
	{
		if (str != "") str << sep;
		str << list3.front();
		list3.pop_front();
	}
	return str;
}

	
bool AosGetStrDiff(
		const OmnString &str1, 
		const OmnString &str2,
		const OmnString &seps,
		list<OmnString> &list1,
		list<OmnString> &list2,
		list<OmnString> &list3)    
{
	// Given two strings: 'str1' and 'str2', whose contents are words
	// separated by 'seps', this function returns three lists: 
	//	list1: words contained in 'str1' but not 'str2'.
	//	list2: words contained in 'str2' but not 'str1'.
	//	list3: words contained in both 'str1' and 'str2'.
	// For example:
	// 	str1 = "cat,dog,house,street"
	// 	str2 = "street,flower,cat,cloud"
	// 	Then:
	// 		list1: dog,house
	// 		list2: flower,cloud
	// 		list3: cat,street
	//
	// Algorithm:
	// It uses a map<int> using word as the key. The lower two
	// bytes are used to count the number of occurances of 
	// the corresponding word in str1. The higher two bytes are used
	// to count the number of occurances of the corresponding word
	// in str2. 
	//
	// After adding all the words in str1 and str2 into the map:
	// 	For each word in the map, if its lower two bytes are not
	// 	0, that means the word is in str1. If its higher two 
	// 	bytes are not 0, that means the word is in str2. 
	aos_assert_r(seps != "", false);

	list1.clear();
	list2.clear();
	list3.clear();
	OmnStrParser1 parser1(str1, seps.data(), false, false);            
	OmnStrParser1 parser2(str2, seps.data(), false, false);            
	int num;
	hash_map<OmnString, int, Omn_Str_hash, compare_str> map_word;
	hash_map<OmnString, int, Omn_Str_hash, compare_str>::iterator iter;

	OmnString word;
	while((word = parser1.nextWord()) != "")
	{
		//iter = map_word.find(word);
		num = map_word.count(word);

		//if(iter!=map_word.end())
		if(num != 0)
		{
			map_word[word]++;
		}
		else
		{
			map_word[word] =1; 
		}
	}
	
	while((word = parser2.nextWord()) != "")
	{
		//iter = map_word.find(word);
		num = map_word.count(word);

		//if(iter == map_word.end())
		if(num == 0)
		{
			map_word[word] = (1<<16);
		}
		else
		{
			int nn = (map_word[word] >> 16);
			nn++;
			map_word[word] = (nn<<16)|map_word[word];
			//map_word[word] = (nn<<16);
		}
	}

	iter = map_word.begin();
	for (; iter != map_word.end(); iter++)
	{
		int counter = iter->second;
		int count1 = counter & 0xff;
		int count2 = (counter >> 16);
		if (count1)
		{
			if (count2)
			{
				// The word appears in both strings
				list3.push_back(iter->first);
			}
			else
			{
				// The word appears in 'str1' but not in 'str2'. 
				list1.push_back(iter->first);
			}
		}
		else
		{
			if (count2)
			{
				// The word appears in 'str2' but not in 'str1'. 
				list2.push_back(iter->first);
			}
			else
			{
				// This should never happen
				OmnAlarm << "Incorrect: !" << enderr;
			}
		}
	}
	
	return true;
}

//Zky2789
OmnString AosCalHashkey(const u64 &value1, const u64 &value2, 
			const u64 &value3, const OmnString &key)
{
	u32 hashkey = 0;
	AosCalHashkey((char *)&value1, sizeof(u64), hashkey, true);
	AosCalHashkey((char *)&value2, sizeof(u64), hashkey, false);
	AosCalHashkey((char *)&value3, sizeof(u64), hashkey, false);
	AosCalHashkey(key.data(), key.length(), hashkey, false);

	if (!sgAosMapInitFlag) AosMapInit();
	char data[10];
	data[0] = sgMap[(u8)hashkey & 0x3f]; hashkey = (hashkey >> 6);
	data[1] = sgMap[(u8)hashkey & 0x3f]; hashkey = (hashkey >> 6);
	data[2] = sgMap[(u8)hashkey & 0x3f]; hashkey = (hashkey >> 6);
	data[3] = sgMap[(u8)hashkey & 0x3f]; hashkey = (hashkey >> 6);
	data[4] = sgMap[(u8)hashkey & 0x3f]; hashkey = (hashkey >> 6);
	data[5] = 0;
	return data;
}


bool AosCalHashkey(
		const char *word,
		const int len,
		u32 &hashkey,
		const bool init)
{
	if (init) hashkey = 5381;
	char c;	
	for (int i=0; i<len; i++)
	{
		c = word[i];
		hashkey = ((hashkey << 5) + hashkey) + c; // i.e., hash * 33 + c
	}
	return true;
}


int AosFindFirstIndex(
		const char **values, 
		const u32 num_values, 
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha)
{
	// This function retrieves the index of the first entry that meets the condition:
	// 		values opr value
	// If found, the index is returned. Otherwise, -5 is returned.
	
	if (num_values == 0) return -5;
	aos_assert_r(values, -5);
	if(isNumAlpha)
	{
		aos_assert_r(opr != eAosOpr_prefix, -5);
	}

	// u32 start_idx = (start_idx < 0) ? 0 : (u32)sidx;
	u32 start_idx = 0;
	if (start_idx > num_values) return -5;

	// Check the first entry. 
	int rslt = AosIILUtil::valueMatch(values[start_idx], value, isNumAlpha);
	switch (opr)
	{
	case eAosOpr_gt:
		 if (rslt > 0) 
		 {
			 // values[start_idx] > value, it is a match
			 return start_idx;
		 }
		 // values[start_idx] <= value, need to search
		 break;
	 
	case eAosOpr_ge:
		 if (rslt >= 0) 
		 {
			 // values[start_idx] >= value, it is a match
			 return start_idx;
		 }
		 // values[start_idx] < value, need to search
		 break;

	case eAosOpr_lt:
		 if (rslt < 0)
		 {
			 // values[start_idx] < value, it is a match.
			 return start_idx;
		 }

		 // values[start_idx] >= value, not possible
		 return -5;

	case eAosOpr_le:
		 if (rslt <= 0)
		 {
			 // values[start_idx] <= value, it is a match.
			 return start_idx;
		 }

		 // values[start_idx] > value, not possible
		 return -5;

	case eAosOpr_eq:
		 if (rslt == 0) return start_idx;
		 break;

	case eAosOpr_ne:
		 if (rslt != 0) return start_idx;
		 break;

	default:
		 OmnAlarm << "Invalid opr: " << opr << enderr;
		 return -5;
	}

	// When it comes to this point:
	// 	1. opr is (eAosOpr_gt || eAosOpr_ge || eAosOpr_ne || eAosOpr_eq)
	// 	2. start_idx < num_values - 1
	// Need to search [start_idx, num_values-1] for the first one that meets the condition.

	int left = start_idx; 
	int right = num_values-1;
	while (left <= right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(values[nn], -5);
		int rslt = AosIILUtil::valueMatch(values[nn], value, isNumAlpha);

		switch (opr)
		{
		case eAosOpr_gt:
			 if (rslt > 0)
			 {
				 // It is a match: values[nn] > value. 
				 if (left == right) return nn;
				 if (left + 1 == right) 
				 {
					 // This means that values[left] > value. This is the first match.
					 return nn;
				 }

				 // Need to move left. 
				 right = nn;
			 }
			 else
			 {
				// It is not a match: values[nn] <= value. 
				if (left == right) 
				{
					// This means that there is no match. 
					return -5;
				}
				else if (left + 1 == right) 
				{
					left = right;
				}
				else left = nn+1;
			 }
			 break;

		case eAosOpr_ge:
			 if (rslt >= 0)
			 {
				 // It is a match: values[nn] >= value. 
				 if (left == right) return nn;
				 if (left + 1 == right) 
				 {
					 // This means that values[left] >= value. This is the first match.
					 return nn;
				 }

				 // Need to move left. 
				 right = nn;
			 }
			 else
			 {
				// It is not a match: values[nn] < value. 
				if (left == right) 
				{
					// This means that there is no match. 
					return -5;
				}
				else if (left + 1 == right) 
				{
					left = right;
				}
				else 
				{
					left = nn+1;
				}
			 }
			 break;

		case eAosOpr_eq:
			 if (rslt == 0)
			 {
				// value == values[nn], met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
			 	right = nn;
			 }
			 else if (rslt < 0)
			 {
				// values[nn] < value
				if (left == right) return -5;
				else if (left + 1 == right) 
				{
					left = right;
				}
				else
				{
					left = nn+1;
				}
			 }
			 else
			 {
				// values[nn] > value
				if (left == right) return -5;
				else if (left + 1 == right) 
				{
					right = left;
				}
				else
				{
					right = nn-1;
				}
			 }
			 break;

		case eAosOpr_ne:
			 if (rslt > 0)
			 {
				// value != values[nn], met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
			 	right = nn;
			 }
			 else if (rslt < 0)
			 {
				if (left == right || left + 1 == right) return nn;
			 	right = nn;
			 }
			 else
			 {
				// vlaue == values[nn]
				if (left == right) return -5;
				if (left + 1 == right) 
				{
					left = right;
				}
				else
				{
					left = nn+1;
				}
			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized opr: " << opr << enderr;
			 return -5;
		}
	}

	OmnShouldNeverComeHere;
	return -5;
}


int AosFindFirstIndexReverse(
		const char **values, 
		const u32 num_values, 
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha)
{
	// This function is the same as 'AosFindFirstIndex(...)' except that it
	// searches from the end.
	if (num_values == 0) return -5;
	aos_assert_r(values, -5);
	if(isNumAlpha)
	{
		aos_assert_r(opr != eAosOpr_prefix, -5);
	}

	u32 start_idx = num_values-1;

	int rslt = AosIILUtil::valueMatch(values[start_idx], value, isNumAlpha);
	switch (opr)
	{
	case eAosOpr_lt:
		 // It is to search the first (backwards) index:
		 // 		values[index] < value
		 if (rslt < 0) return start_idx;
		 break;
	 
	case eAosOpr_le:
		 if (rslt <= 0) return start_idx;
		 break;

	case eAosOpr_gt:
		 // It is to search the first (backwards) index:
		 // 		values[nn] > value
		 // There is no need to do binary search. Check the last one.
		 if (rslt > 0) return start_idx;
		 return -5;

	case eAosOpr_ge:
		 if (rslt >= 0) return start_idx;
		 return -5;

	case eAosOpr_eq:
		 if (rslt == 0) return start_idx;
		 break;

	case eAosOpr_ne:
		 if (rslt != 0) return start_idx;
		 break;

	default:
		 OmnAlarm << "Invalid opr: " << opr << enderr;
		 return -5;
	}

	int left = 0; 
	int right = num_values-1;
	while (left <= right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(values[nn], -5);
		int rslt = AosIILUtil::valueMatch(values[nn], value, isNumAlpha);

		switch (opr)
		{
		case eAosOpr_gt:
			 // It needs to find the first index (backwards):
			 // 		values[nn] value
			 // There is no need to use binary search. 
			 if (rslt > 0)
			 {
				 // It is a match: values[nn] > value. 
				 if (left == right) return nn;
				 if (left + 1 == right) 
				 {
					 // This means that values[left] > value. This is the first match.
					 return nn;
				 }

				 // Need to move rigtht. 
				 left = nn;
			 }
			 else
			 {
				// It is not a match: values[nn] <= value. 
				if (left == right) 
				{
					// This means that there is no match. 
					return -5;
				}
				else if (left + 1 == right) 
				{
					left = right;
				}
				else left = nn+1;
			 }
			 break;

		case eAosOpr_ge:
			 // It needs to find the first index (backwards):
			 // 		values[nn] value
			 // There is no need to use binary search. 
			 if (rslt >= 0)
			 {
				 // It is a match: values[nn] > value. 
				 if (left == right) return nn;
				 if (left + 1 == right) 
				 {
					 // This means that values[left] > value. This is the first match.
					 return nn;
				 }

				 // Need to move rigtht. 
				 left = nn;
			 }
			 else
			 {
				// It is not a match: values[nn] <= value. 
				if (left == right) 
				{
					// This means that there is no match. 
					return -5;
				}
				else if (left + 1 == right) 
				{
					left = right;
				}
				else left = nn+1;
			 }
			 break;

		case eAosOpr_eq:
			 if (rslt == 0)
			 {
				// value == values[nn], met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
			 	left = nn;
			 }
			 else if (rslt < 0)
			 {
				// values[nn] < value
				if (left == right) return -5;

				if (left + 1 == right) 
				{
					left = right;
				}
				else
				{
					left = nn+1;
				}
			 }
			 else
			 {
				// values[nn] > value
				if (left == right) return -5;

				if (left + 1 == right) 
				{
					right = left;
				}
				else
				{
					right = nn-1;
				}
			 }
			 break;

		case eAosOpr_ne:
			 if (rslt < 0)
			 {
				// value != values[nn], met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
			 	left = nn;
			 }
			 else if (rslt > 0)
			 {
				// value != values[nn], met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
			 	left = nn;
			 }
			 else
			 {
				if (left == right) return -5;
				if (left + 1 == right) 
				{
					right = left;
				}
				else
				{
					right = nn-1;
				}
			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized opr: " << opr << enderr;
			 return -5;
		}
	}

	OmnShouldNeverComeHere;
	return -5;
}


// Chen Ding, 06/15/2012
bool AosUnicomConvertUpnPrefix()
{
	OmnFile cpn_prefix("UpnPrefix.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(cpn_prefix.isGood(), false);
	u64 length = cpn_prefix.getLength();
	char *buff = OmnNew char[length+10];
	bool rslt = cpn_prefix.readToBuff(0, length, buff);
	aos_assert_r(rslt, false);

	OmnFile cpn_new("UpnPrefixConverted.txt", OmnFile::eCreate AosMemoryCheckerArgs);

	u32 idx = 0;
	char record[20];
	cout << __FILE__ << ":" << __LINE__ << ": To convert UPN Prefix ..." << endl;
	char ucity1, ucity2;
	while (idx < length)
	{
		// Original data are in the form:
		// 	number, ucity, utown\n
		// Need to convert ucity into:
		// 		01,555 
		// 		02,552
		// 		03,556
		// 		04,553 
		// 		05,564 
		// 		06,551
		// 		07,558
		// 		08,554
		// 		09,562
		// 		10,565
		// 		11,563
		// 		12,550
		// 		13,557
		// 		14,561
		// 		15,559
		// 		16,566
		// 		17,5582
		//
		u32 rcd_idx = 0;
		while (idx < length && buff[idx] != ',') 
		{
			record[rcd_idx++] = buff[idx];
			idx++;
		}
		if (idx >= length) break;

		if (rcd_idx > 11)
		{
			// Phone numbers should be 11 characters long
			OmnAlarm << "incorrect: " << idx << enderr;
			OmnDelete [] buff;
			return false;
		}
		else if (rcd_idx < 11)
		{
			while (rcd_idx != 11) record[rcd_idx++] = ' ';
		}

		idx++;
		if (buff[idx+2] != ',')
		{
			OmnAlarm << "incorrect: " << idx << enderr;
			OmnDelete [] buff;
			return false;
		}

		// record[rcd_idx++] = buff[idx++];
		// record[rcd_idx++] = buff[idx++];
		ucity1 = buff[idx++];
		ucity2 = buff[idx++];

		idx++;
		if (buff[idx+2] != ' ' ||
		    buff[idx+3] != ' ' ||
		    buff[idx+4] != '\n')
		{
			OmnAlarm << "incorrect: " << idx << enderr;
			OmnDelete [] buff;
			return false;
		}

		// add utown
		record[rcd_idx++] = buff[idx++];
		record[rcd_idx++] = buff[idx++];

		// Convert the ucity
		switch (ucity1)
		{
		case '0':
			 switch (ucity2)
			 {
			 case '1':			// 01: 555
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  break;

			 case '2':			// 02: 552
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '2';
				  break;

			 case '3':			// 03: 556
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  break;

			 case '4':			// 04: 553
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '3';
				  break;

			 case '5':			// 05: 564
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '4';
				  break;

			 case '6':			// 06: 551
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '1';
				  break;

			 case '7':			// 07: 558
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '8';
				  break;

			 case '8':			// 08: 554
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '4';
				  break;

			 case '9':			// 08: 562
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '2';
				  break;

			 default:
				  OmnAlarm << "Invalid: " << &buff[rcd_idx-1] << enderr;
				  break;
			 }
			 break;

		case '1':
			 switch (ucity2)
			 {
			 case '0':			// 10: 565
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '5';
				  break;

			 case '1':			// 11: 563
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '3';
				  break;

			 case '2':			// 12: 550
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '0';
				  break;

			 case '3':			// 13: 557
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '7';
				  break;

			 case '4':			// 14: 561
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '1';
				  break;

			 case '5':			// 15: 559
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '9';
				  break;

			 case '6':			// 16: 566
				  record[rcd_idx++] = '0';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '6';
				  record[rcd_idx++] = '6';
				  break;

			 case '7':			// 17: 5582
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '5';
				  record[rcd_idx++] = '8';
				  record[rcd_idx++] = '2';
				  break;

			 default:
				  OmnAlarm << "Invalid" << enderr;
			 }
		}

		// record[rcd_idx++] = '0';
		// record[rcd_idx++] = '0';
		// record[rcd_idx++] = buff[idx++];
		// record[rcd_idx++] = buff[idx++];
		cpn_new.append(record, 17, false);
		idx++;
		idx++;
		idx++;
	}
	cout << __FILE__ << ":" << __LINE__ << ": To convert UPN Prefix ... Finished" << endl;
	OmnDelete [] buff;
	return true;
}


bool AosUnicomConvertCpnPrefix()
{
	OmnFile cpn_prefix("CpnPrefix.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(cpn_prefix.isGood(), false);
	u64 length = cpn_prefix.getLength();
	char *buff = OmnNew char[length+10];
	bool rslt = cpn_prefix.readToBuff(0, length, buff);
	aos_assert_r(rslt, false);

	OmnFile cpn_new("CpnPrefixConverted.txt", OmnFile::eCreate AosMemoryCheckerArgs);

	u32 idx = 0;
	char record[200];
	cout << __FILE__ << ":" << __LINE__ << ": To convert CPN Prefix ..." << endl;
	vector<OmnString> elems;
	while (idx < length)
	{
		// Original data are in the form:
		// 		Prefix:             1809036 ,
		// 		LONG_CODE:          833 ,
		// 		LONG_CODE_LOC:      833 ,
		// 		TYPE:               2,
		// 		CHANGE_DATE:        2011-07-01-00.00.00.000000,
		// 		LONG_CODE_NEW:      833 ,
		// 		LONG_CODE_LOC_NEW:  833 ,
		// 		TYPE_NEW:           2,
		// 		BEGIN_DATE:         2011-07-01-00.00.00.000000,
		// 		END_DATE:           2050-01-01-00.00.00.000000,
		// 		TIME_TYPE:          0,
		// 		NOTE:               0,
		// 		AUDIT_FLAG:         0
		u32 start = idx;
		while (idx < length && buff[idx] != '\n') idx++;
		if (idx >= length) break;

		OmnString str(&buff[start], idx - start);
		AosStrSplit::splitStrByChar(str, ",", elems, 13);
		if (elems[0].length() > 8)
		{
			OmnAlarm << "incorrect: " << idx << enderr;
			OmnDelete [] buff;
			return false;
		}

		if (elems[5].length() > 4)
		{
			OmnAlarm << "incorrect: " << idx << enderr;
			OmnDelete [] buff;
			return false;
		}

		memset(record, ' ', 120);
		strncpy(record, elems[0].data(), elems[0].length());

		int len = elems[5].length();
		const char *data = elems[5].data();
		if (len == 1)
		{
			record[31] = ' ';
			record[32] = ' ';
			record[33] = ' ';
			record[34] = data[0];
		}
		else if (len == 2)
		{
			record[31] = ' ';
			record[32] = ' ';
			record[33] = data[0];
			record[34] = data[1];
		}
		else if (len == 3)
		{
			record[31] = ' ';
			record[32] = data[0];
			record[33] = data[1];
			record[34] = data[2];
		}
		else 
		{
			record[31] = data[0];
			record[32] = data[1];
			record[33] = data[2];
			record[34] = data[3];
		}

		cpn_new.append(record, 120, false);
		idx++;
	}
	cout << __FILE__ << ":" << __LINE__ << ": To convert CPN Prefix ... Finished" << endl;
	OmnDelete [] buff;
	return true;
}


OmnString AosNumber2String(const int64_t &size)
{
	// this function converts 'size' into:
	// 		xxxB		if size < 1,000,000
	// 		xxxM		if size is [1,000,000, 1,000,000,000)
	// 		xxx.xxG		if size > 1,000,000,000
	double tmp_size = size;
	char size_unit = 'B';
	const int64_t one_g = 1000000000;
	const int64_t one_m = 1000000;
	const int64_t ten_m = one_m * 10;

	if (size >= one_g)
	{
		tmp_size = ((int64_t)tmp_size) / ten_m;
		tmp_size = tmp_size / 100;
		size_unit = 'G';
	}
	else if (size >= 1000000)
	{
		tmp_size = ((int64_t)tmp_size) / one_m;
		size_unit = 'M';
	}

	OmnString ss;
	ss << tmp_size << size_unit;
	return ss;
}

