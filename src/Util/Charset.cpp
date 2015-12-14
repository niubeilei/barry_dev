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
// 05/20/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Charset.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

/*
static char sgAosCharsetDigits[AosCharset::eCharsetSize] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [20, 29]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [30, 39]
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 		// [40, 49]
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 		// [50, 59]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [60, 69]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [70, 79]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [80, 89]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [90, 99]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [100, 109]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [110, 119]
	0, 0, 0, 0, 0, 0, 0, 0  			// [120, 127]
};

static char sgAosCharsetLowerCaseLetters[AosCharset::eCharsetSize] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [20, 29]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [30, 39]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [40, 49]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [50, 59]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [60, 69]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [70, 79]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [80, 89]
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 		// [90, 99]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [100, 109]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [110, 119]
	1, 1, 1, 0, 0, 0, 0, 0  			// [120, 127]
};

static char sgAosCharsetUpperCaseLetters[AosCharset::eCharsetSize] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [20, 29]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [30, 39]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [40, 49]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [50, 59]
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 		// [60, 69]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [70, 79]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [80, 89]
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [90, 99]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [100, 109]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [110, 119]
	0, 0, 0, 0, 0, 0, 0, 0  			// [120, 127]
};


static char sgAosCharsetLetters[AosCharset::eCharsetSize] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [20, 29]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [30, 39]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [40, 49]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [50, 59]
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 		// [60, 69]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [70, 79]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [80, 89]
	1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 		// [90, 99]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [100, 109]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [110, 119]
	1, 1, 1, 0, 0, 0, 0, 0  			// [120, 127]
};

// Valid Chars: Digits and letters
// Terminating Chars: space, tab, \r, \n, eof
static char sgDigitsLetters[AosCharset::eCharsetSize] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [20, 29]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// [30, 39]
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 		// [40, 49]
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 		// [50, 59]
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 		// [60, 69]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [70, 79]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [80, 89]
	1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 		// [90, 99]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [100, 109]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// [110, 119]
	1, 1, 1, 0, 0, 0, 0, 0  			// [120, 127]
};
*/

AosCharset::AosCharset()
{

}

AosCharset::AosCharset(const vector<Entry> &def)
{
	memset(mCharset, 0, eCharsetSize);
	if (!init(def)) OmnThrowException("invalid_charset");
}


bool
AosCharset::init(const vector<Entry> &def)
{
	for (u32 i=0; i<def.size(); i++)
	{
		aos_assert_r(def[i].start >= 0 && def[i].start < eCharsetSize, false);
		aos_assert_r(def[i].end >= 0 && def[i].end < eCharsetSize, false);
		aos_assert_r(def[i].start <= def[i].end, false);
		aos_assert_r(def[i].flag > eInvalidFlag && def[i].flag < eMaxFlag, false);

		memset(&mCharset[def[i].start], def[i].flag, def[i].end-def[i].start+1);
	}
	return true;
}

AosCharset::Flag
AosCharset::getStrToEnum(const OmnString flag)
{
	if(flag == "eAllowFlag")
		return AosCharset::eAllowFlag;
	if(flag == "eFinishFlag")
		return AosCharset::eFinishFlag;
	if(flag == "eNotAllowedFlag")
		return AosCharset::eNotAllowedFlag;
	return AosCharset::eInvalidFlag;	
}

bool
AosCharset::config(AosRundata *rdata, const AosXmlTagPtr &def)
{
	// Its format is:
	//	<charset min_len="xxx" max_len="xxx">
	//		<range start="xxx" end="xxx" flag="xxx"/>
	//		<range start="xxx" end="xxx" flag="xxx"/>
	//		...
	//		<range start="xxx" end="xxx" flag="xxx"/>
	//	</charset>
	mMinLen = def->getAttrInt("min_len",-1);
	mMaxLen = def->getAttrInt("max_len",-1);
	AosXmlTagPtr tag = def->getFirstChild();
	vector<Entry>  vEntries;
	while(tag)
	{
		Entry entry;
		entry.start = tag->getAttrInt("start",-1);
		aos_assert_r(entry.start != -1,false);
		entry.end = tag->getAttrInt("end",-1);
		aos_assert_r(entry.end != -1,false);
		OmnString flag = tag->getAttrStr("flag","");
		entry.flag = getStrToEnum(flag);
		vEntries.push_back(entry);
		tag = def->getNextChild();
	}

	return  init(vEntries);
}

bool
AosCharset::nextWord(
		const char *data, 
		const int len, 
		int &idx, 
		int &start, 
		int &word_len,
		char &invalid_char,
		ErrorCode &errcode) const
{
	// This function skips leading 'eFinishedFlag' characters. 
	// It then scans all the chars that are 'eAllowFla' until
	// it terminates by a 'eFinishFlag' or the end of the string.
	// Upon success, 'start' points to the first valid character
	// of the word and 'word_len' is the length of the word.
	word_len = 0;

	// Skip the leading 'eFinishFlag' chars.
	bool found = false;
	while (idx < len && !found)
	{
		switch ((Flag)mCharset[(u8)data[idx]])
		{
		case eAllowFlag:
			 start = idx++;
			 word_len = 1;
			 found = true;
			 break;

		case eFinishFlag:
			 idx++;
			 break;

		case eNotAllowedFlag:
			 errcode = eInvalidChar;
			 return false;

		default:
			 errcode = eInternalError;
			 return false;
		}
	}
		
	if (!found)
	{
		// No word found
		return true;
	}

	while (idx < len && word_len <= mMaxLen)
	{
		switch ((Flag)mCharset[(u8)data[idx]])
		{
		case eAllowFlag:
			 idx++;
			 word_len++;
			 break;

		case eFinishFlag:
			 if (word_len < mMinLen)
			 {
				 errcode = eTooShort;
				 return false;
			 }
			 return true;

		case eNotAllowedFlag:
			 invalid_char = data[idx];
			 idx++;
			 word_len++;
			 errcode = eInvalidChar;
			 break;

		default:
			 errcode = eInternalError;
			 return false;
		}
	}

	if (word_len >= mMaxLen)
	{
		errcode = eTooLong;
		return false;
	}

	if (mAllowEOF)
	{
		return true;
	}

	errcode = eReachEOF;
	return false;
}


int 
AosCharset::parseWords(
		const char *data, 
		const int len, 
		std::vector<OmnString> &words, 
		ErrorCode &errcode)
{
	words.clear();
	if (len <= 0) return 0;

	int idx = 0;
	char invalid_char;
	int start, word_len;
	while (1)
	{
		if (!nextWord(data, len, idx, start, word_len, invalid_char,errcode)) return -1;
		aos_assert_r(word_len > 0, -1);
		words.push_back(OmnString(&data[start], word_len));
	}

	return words.size();
}


