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
// 08/06/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/StrSplit.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"


AosStrSplit::AosStrSplit()
:
mStrings(0),
mStrSize(0),
mEntries(0)
{
}


AosStrSplit::AosStrSplit(
		const char *contents, 
		const char *sep, 
		OmnString *strs,
		const int max, 
		bool &finished)
:
mStrings(strs),
mStrSize(0),
mEntries(0),
mIdx(0)
{
	aos_assert(splitStr(contents, sep, strs, max, finished) >= 0);
}


AosStrSplit::AosStrSplit(
		const OmnString &contents, 
		const char *sep) 
:
mStrSize(0),
mEntries(0),
mIdx(0)
{
	aos_assert(splitStr(contents.data(), sep) >= 0);
}


AosStrSplit::AosStrSplit(
		const char *contents, 
		const char *sep) 
:
mStrSize(0),
mEntries(0),
mIdx(0)
{
	aos_assert(splitStr(contents, sep) >= 0);
}


int 
AosStrSplit::splitStr(
		const char *orig_str, 
		const char *sep) 
{
	if (!orig_str || strlen(orig_str) == 0) 
	{
		mEntries = 0;
		return 0;
	}

	aos_assert_r(sep, -1);
	mStringsV.clear();

	int idx = 0;
	int start_idx = 0;
	int strl = strlen(orig_str);
	char c;
	char ch = sep[0];
	int sep_len = strlen(sep);
	mEntries = 0;
	while ((c = orig_str[idx]))
	{
		if (c == ch && strncmp(&orig_str[idx], sep, sep_len) == 0)
		{
			// Found one entry
			int len = idx - start_idx; 
			//mStrings[mEntries].assign(&orig_str[start_idx], len);
			mStringsV.push_back(OmnString(&orig_str[start_idx], len));
			if (start_idx >= strl) return mEntries;
			++mEntries;
			idx += sep_len;
			start_idx = idx;
			if (mEntries >= 5000) return mEntries;
			continue;
		}
		idx++;
	}

	int len = idx - start_idx;
	//mStrings[mEntries].assign(&orig_str[start_idx], len);
	mStringsV.push_back(OmnString(&orig_str[start_idx], len));
	return ++mEntries;
}


OmnString
AosStrSplit::nextWord()
{
	if(mIdx >= mEntries) return "";
	return mStringsV[mIdx++];
}


void 
AosStrSplit::reset()
{
	mIdx = 0;
}


bool
AosStrSplit::hasMore()
{
	if (mIdx >= mEntries) return false;
	return true;

}

int
AosStrSplit::splitStr(
		const char *orig_str, 
		const char *sep, 
		OmnString *strs,
		const int max, 
		bool &finished)
{
	finished = false;
	if (!orig_str || strlen(orig_str) == 0) 
	{
		mEntries = 0;
		return 0;
	}

	aos_assert_r(sep, -1);
	aos_assert_r(max > 0, -1);
	aos_assert_r(strs, -1);
	mStrings = strs;
	mStrSize = max;

	int idx = 0;
	int start_idx = 0;
	char c;
	char ch = sep[0];
	int sep_len = strlen(sep);
	mEntries = 0;
	while ((c = orig_str[idx]))
	{
		if (c == ch && strncmp(&orig_str[idx], sep, sep_len) == 0)
		{
			// Found one entry
			int len = idx - start_idx; 
			mStrings[mEntries].assign(&orig_str[start_idx], len);
			if (++mEntries>= mStrSize) return mEntries;
			idx += sep_len;
			start_idx = idx;
			if (mEntries >= max) return mEntries;
			continue;
		}
		idx++;
	}

	finished = true;
	int len = idx - start_idx;
	mStrings[mEntries].assign(&orig_str[start_idx], len);
	return ++mEntries;
}


AosStrSplit::~AosStrSplit() 
{
}


bool
AosStrSplit::deleteEntry(const int idx)
{
	aos_assert_r(idx >= 0 && idx < mEntries && mEntries > 0, false);
	for (int i=idx; i<mEntries-1; i++)
	{
		mStrings[i] = mStrings[i+1];
	}
	mEntries--;
	return true;
}


bool	
AosStrSplit::getWords(
		const OmnString &str,
		OmnString *words, 
		int &numWords, 
		const int maxsize, 
		const char *sep)
{
	// It splits the string 'str' into words. Words are splitted
	// by the characters in 'sep'. All characters in 'sep' are 
	// removed from the words. Words are put into 'words', which 
	// is an array of 'maxsize' size. If the maximum size is
	// reached, additional words are ignored. 

	int idx = 0;
	char c;
	int sep_len = strlen(sep);
	const char *orig_str = str.data();

	char map[256];
	memset(map, 0, 256);
	for (int i=0; i<sep_len; i++) map[(u8)sep[i]] = 1;
	numWords = 0;
	int len = str.length();

	// Skip the leading seps
	while ((c = orig_str[idx]) && idx < len && map[(u8)c]) {idx++;}

	int start_idx = idx;
	while ((c = orig_str[idx]) && idx < len)
	{
		if (map[(u8)c])
		{
			// It hits a sep char. 
			if (start_idx != 0)
			{
				words[numWords++].assign(&orig_str[start_idx], idx - start_idx);
			}
			if (numWords >= maxsize || idx >= len) return true;

			// skip all the sep chars
			idx++;
			while (idx < len && map[(u8)orig_str[idx]]) idx++;
			start_idx = idx;
		}
		idx++;
	}

	words[numWords++].assign(&orig_str[start_idx], idx-start_idx);
	return true;
}


int
AosStrSplit::splitStrByChar(
		const char *orig_str, 
		const char sep, 
		vector<OmnString> &substrs, 
		const int max, 
		bool &finished)
{
	char sep_map[256];
	memset(sep_map, 0, 256);
	sep_map[(unsigned char)sep] = 1;
	return splitStr(orig_str, sep_map, substrs, max, finished);
}


int
AosStrSplit::splitStrByChar(
		const char *orig_str, 
		const char *sep, 
		vector<OmnString> &substrs, 
		const int max, 
		bool &finished)
{
	// If splits the string into an array of substrings, separated
	// by characters in 'sep'.
	aos_assert_r(sep, -1);
	char sep_map[256];
	int sep_len = strlen(sep);
	memset(sep_map, 0, 256);
	for (int i=0; i<sep_len; i++)
	{
		sep_map[(unsigned char)sep[i]] = 1;
	}

	return splitStr(orig_str, sep_map, substrs, max, finished);
}


int
AosStrSplit::splitStr(
		const char *orig_str, 
		const char *sep_map, 
		vector<OmnString> &substrs, 
		const int max, 
		bool &finished)
{
	finished = true;
	substrs.clear();
	int str_len = strlen(orig_str);
	if (!orig_str || str_len == 0) 
	{
		return 0;
	}

	int idx = 0;
	int start_idx = 0;
	char c;
	while ((c = orig_str[idx]))
	{
		if (sep_map[(unsigned char)c])
		{
			// Found one entry
			int len = idx - start_idx; 
			OmnString str(&orig_str[start_idx], len);
			substrs.push_back(str);
			if (max > 0 && substrs.size() > (u32)max) 
			{
				finished = false;
				return substrs.size();
			}

			start_idx = idx+1;
			//idx++;
			//while ((c = orig_str[idx]) && sep_map[(unsigned char)c]) idx++;
			//start_idx = idx;
			//continue;
		}
		idx++;
	}

	int len = idx - start_idx;
	OmnString str(&orig_str[start_idx], len);
	substrs.push_back(str);
	return substrs.size();
}


int
AosStrSplit::splitStrBySubstr(
		const char *orig_str, 
		const char *sep, 
		vector<OmnString> &substrs, 
		const int max, 
		bool &finished)
{
	// If splits the string into an array of substrings, separated
	// by characters in 'sep'.
	finished = true;
	substrs.clear();
	int str_len = strlen(orig_str);
	if (!orig_str || str_len == 0) 
	{
		return 0;
	}

	aos_assert_r(sep, -1);

	int idx = 0;
	int start_idx = 0;
	char c;
	int sep_len = strlen(sep);
	char sepc = sep[0];
	bool found = true;

	while (idx < str_len)
	{
		while ((c = orig_str[idx]) && c != sepc) idx++;
		if (idx >= str_len) break;
		
		found = true;
		for (int i=1; i<sep_len; i++)
		{
			if (sep[i] != orig_str[idx+i]) 
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			// Found one entry
			int len = idx - start_idx; 
			OmnString str(&orig_str[start_idx], len);
			substrs.push_back(str);
			if (max > 0 && substrs.size() >= (u32)max) 
			{
				finished = false;
				return substrs.size();
			}

			idx += sep_len;
			start_idx = idx;
			continue;
		}
		idx++;
	}
	int len = idx - start_idx;
	OmnString str(&orig_str[start_idx], len);
	substrs.push_back(str);
	return substrs.size();
	/*
	finished = true;
	char *ptr = 0;
	char *startptr = (char *)orig_str;
	int nn = 0;
	int sep_len = strlen(sep);
	while ((ptr = strstr(startptr, sep)))
	{
		// Found one. 'ptr' points to the beginnng of 'sep'. 
		int len = ptr - startptr;
		substrs.push_back(OmnString(startptr, len));
		if (++nn >= max) 
		{
			finished = false;
			return nn;
		}
		startptr = ptr + sep_len;
	}
	substrs.push_back(OmnString(startptr));
	nn++;
	return nn;
	*/
}


// Chen Ding, 04/03/2012
bool 
AosStrSplit::parseStrRanges(
		const OmnString &data, 
		const OmnString &fieldSep, 
		const OmnString &entrySep, 
		vector<AosStrRange> &ranges, 
		const int max)
{
	ranges.clear();
	vector<OmnString> entries;
	bool finished;
	splitStrBySubstr(data.data(), entrySep.data(), entries, max, finished);
	if (entries.size() <= 0) return true;

	vector<OmnString> pair;
	for (u32 i=0; i<entries.size(); i++)
	{
		splitStrBySubstr(entries[i].data(), fieldSep.data(), pair, 2, finished);
		aos_assert_r(pair.size() == 2, false);
		
		const char *data1 = pair[0].data();
		int len = pair[0].length();
		aos_assert_r(len > 0, false);
		bool inclusive_left = false;
		switch (data1[0])
		{
		case '(': inclusive_left = false;
		case '[': inclusive_left = true;
		default:
		     OmnAlarm << "Invalid range: " << data << enderr;
			 return false;
		}

		const char *data2 = pair[1].data();
		len = pair[1].length();
		aos_assert_r(len > 0, false);
		bool inclusive_right = false;
		switch (data2[0])
		{
		case ')': inclusive_right = false;
		case ']': inclusive_right = true;
		default:
		     OmnAlarm << "Invalid range: " << data << enderr;
			 return false;
		}

		ranges.push_back(AosStrRange(inclusive_left, &data1[1], inclusive_right, &data2[1]));
	}

	return true;
}


bool 
AosStrSplit::parseU64Ranges(
		const OmnString &data, 
		const OmnString &fieldSep, 
		const OmnString &entrySep, 
		vector<AosU64Range> &ranges, 
		const int max)
{
	ranges.clear();
	vector<OmnString> entries;
	bool finished;
	splitStrBySubstr(data.data(), entrySep.data(), entries, max, finished);
	if (entries.size() <= 0) return true;

	vector<OmnString> pair;
	for (u32 i=0; i<entries.size(); i++)
	{
		splitStrBySubstr(entries[i].data(), fieldSep.data(), pair, 2, finished);
		aos_assert_r(pair.size() == 2, false);
		
		const char *data1 = pair[0].data();
		int len = pair[0].length();
		aos_assert_r(len > 0, false);
		bool inclusive_left = false;
		switch (data1[0])
		{
		case '(': inclusive_left = false;
		case '[': inclusive_left = true;
		default:
		     OmnAlarm << "Invalid range: " << data << enderr;
			 return false;
		}

		const char *data2 = pair[1].data();
		len = pair[1].length();
		aos_assert_r(len > 0, false);
		bool inclusive_right = false;
		switch (data2[0])
		{
		case ')': inclusive_right = false;
		case ']': inclusive_right = true;
		default:
		     OmnAlarm << "Invalid range: " << data << enderr;
			 return false;
		}

		char *pp;
		u64 left = strtoull(&data1[1], &pp, 10);
		u64 right = strtoull(&data2[1], &pp, 10);
		ranges.push_back(AosU64Range(inclusive_left, left, inclusive_right, right));
	}

	return true;
}


// Chen Ding, 04/03/2012
int
AosStrSplit::splitStrBySubstr(
		const OmnString &data,
		const char *sep, 
		vector<OmnString> &containers, 
		vector<OmnString> &members, 
		const int max)
{
	// If splits the string into two arrays, one called 'containers' and 
	// the other 'members'. 
	//
	// Example:
	// 		aaa.bbb.ccc.ddd.eee
	//
	// Will be split into:
	// 		Containers			Members
	// 		aaa.bbb.ccc.ddd		eee
	// 		aaa.bbb.ccc			ddd
	// 		aaa.bbb				ccc
	// 		aaa					bbb
	containers.clear();
	members.clear();

	// The following is a simple implementation of this split. It
	// splits the whole string first, and then assemble them.
	vector <OmnString> substrs;
	//splitStrBySubstr(data, sep, members, max);
	splitStrBySubstr(data, sep, substrs, max);

	u32 size = substrs.size();
	if (size < 2) 
	{
		members.clear();
		return 0;
	}

	for (u32 i=0; i<size; i++)
	{
		// Assembler the remaining
		/*
		if (i == size-2)
		{
			containers.push_back(members[i+1]);
			members.pop_back();
			return members.size();
		}

		OmnString ss;
		for (u32 k=i+1; k<size; k++)
		{
			if (ss != "") ss << sep;
			ss << members[k];
		}
		containers.push_back(ss);
		*/
		if (i == size -1) 
		{
			return members.size();
		}
		OmnString ss;
		u32 k;
		for (k = 0; k < size-i-1; k++)
		{
			if (ss != "") ss << sep;
			ss << substrs[k];
		}
		aos_assert_r(k < size, 0);
		members.push_back(substrs[k]);
		containers.push_back(ss);
	}

	OmnShouldNeverComeHere;
	return -1;
}


int
AosStrSplit::splitStrBySubstr(
		const OmnString &data,
		const char *sep, 
		vector<OmnString> &containers, 
		vector<OmnString> &members, 
		vector<int> &numstrs, 
		const int max)
{
	// If splits the string into two arrays, one called 'containers' and 
	// the other 'members'. 
	//
	// Example:
	// 		aaa.bbb.ccc.ddd.eee
	//
	// Will be split into:
	// 		Containers			Members
	// 		aaa.bbb.ccc.ddd		eee
	// 		aaa.bbb.ccc			ddd
	// 		aaa.bbb				ccc
	// 		aaa					bbb
	containers.clear();
	members.clear();

	// The following is a simple implementation of this split. It
	// splits the whole string first, and then assemble them.
	vector <OmnString> substrs;
	splitStrBySubstr(data, sep, substrs, max);

	u32 size = substrs.size();
	if (size < 2) 
	{
		members.clear();
		return 0;
	}

	for (u32 i=0; i<size; i++)
	{
		if (i == size -1) 
		{
			return members.size();
		}
		OmnString ss;
		u32 k;
		for (k = 0; k < size-i-1; k++)
		{
			if (ss != "") ss << sep;
			ss << substrs[k];
		}
		aos_assert_r(k < size, 0);
		members.push_back(substrs[k]);
		containers.push_back(ss);
		numstrs.push_back(k);	
	}

	OmnShouldNeverComeHere;
	return -1;
}

