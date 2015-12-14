////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrParser.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/StrParser.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "MultiLang/LangDictMgr.h"
#include "Porting/InetAddr.h"
#include "Rundata/Rundata.h"
#include "Util/IpAddr.h"
#include "Util/StrSplit.h"
#include <limits.h>
#include <vector>

OmnStrParser1::OmnStrParser1(
		const OmnString &str)
:
mString(str),
mData(mString.data()),
mCrt(0)
{
	mString.removeTailWhiteSpace();
	mLen = mString.length();
}


OmnStrParser1::OmnStrParser1(
		const OmnString &str, 
		const OmnString &deli, 
		const bool cdb,
		const bool csg) 
:
mString(str),
mCrt(0),
mCheckDoubleQuote(cdb),
mCheckSingleQuote(csg)
{
	mData = mString.data();
	mLen = mString.length();
	mDelimiters = deli;
	aos_assert(!(cdb && csg));
	aos_assert(resetNextWord(deli.data()));
}


bool
OmnStrParser1::resetNextWord(const char *deli)
{
	aos_assert_r(deli, false);
	memset(mMap, 0, sizeof(mMap));
	int len = strlen(deli);		
	for (int i=0; i<len; i++)
		mMap[(u8)deli[i]] = true;
	return true;
}


OmnStrParser1::~OmnStrParser1()
{
}


void
OmnStrParser1::setData(const OmnString &str)
{
	mString = str;
	mString.removeTailWhiteSpace();
	mData = str.data();
	mLen = mString.length();
	mCrt = 0;
}


void
OmnStrParser1::reset()
{
	mCrt = 0;
}


OmnString
OmnStrParser1::peekWord()
{
	int crt = mCrt;

	OmnString word = nextWord("");
	mCrt = crt;
	return word;
}


// 
// It retrieves a word from the current position. Leading white spaces
// will be skipped. A word contains characters whose ASCII code is 
// greater than 0x32. If no word is found, "dft" is returned.
// If found, it cursor will be right after the 
// delimitor after return. Quoted words are treated as one word.
// Quotes can be single quote, double quote, or both.
//
OmnString	
OmnStrParser1::nextWord(const OmnString &dft) 
{
	// 
	// Skip leading white spaces
	// 
	if (mCrt >= mLen) return dft;
	u8 c = (u8)mData[mCrt];
	while (mCrt < mLen)
	{
		// 
		// Chen Ding, 10/21/2005, To support Chinese, we do not want to 
		// check the max.
		//
		// if (c >= 33 && c <= 126) 
		if (c >= 33 && !mMap[c]) 
		{
			break;
		}
		c = (u8)mData[++mCrt];
	}

	if (mCrt >= mLen)
	{
		// 
		// Didn't find any word
		//
		return dft;
	}

	// 
	// Find the word. 
	//
	int start = mCrt;
	u8 prev = 0;
	bool quoteFound = false;
	bool found;
	bool processed;
	int delta = 0;
	while (mCrt < mLen)
	{
		// 
		// Chen Ding, 10/21/2005, Chinese support
		// if (c < 33 || c > 126)
		if (c < 32)
		{
			// 
			// Found the word
			//
			break;
		}

		// Check whether it is one of the deliminators
		found = false;
		processed = false;
		switch (c)
		{
		case '"':
			 // Check whether it is escaped
			 if (!mCheckDoubleQuote) break;
			 if (prev == '\\')
			 {
				 // It is escaped. 
				 prev = 0;
				 break;
			 }
			 prev = 0;
			 
			 // Not escaped. Check whether it is an opening or 
			 // closing quote
			 if (quoteFound)
			 {
				 // It is the closing quote. This is the end of 
				 // the word. 
				 found = true;
				 mCrt++;
				 delta = 1;
				 break;
			 }
			 quoteFound = true;
			 if (start == mCrt) start++;
			 processed = true;
			 break;

		case '\'':
			 // Check whether it is escaped
			 if (!mCheckSingleQuote) break;
			 if (prev == '\\')
			 {
				 // It is escaped. 
				 prev = 0;
				 break;
			 }
			 prev = 0;

			 // Not escaped. Check whether it is an opening or 
			 // closing quote
			 if (quoteFound)
			 {
				 // It is the closing quote. This is the end of 
				 // the word. 
				 found = true;
				 mCrt++;
				 delta = 1;
				 break;
			 }
			 quoteFound = true;
			 if (start == mCrt) start++;
			 processed = true;
			 break;

		case '\\':
			 // It is an escape. 
			 prev = '\\';
			 break;

		default:
			 break;
		}

		if (found) break;

		// Chen Ding, 07/05/2011
		if (!quoteFound && !processed && mMap[(u8)c]) break;
		// if (!quoteFound && !processed)
		// {
		// 	if (mMap[(u8)c] && strncmp(&mData[mCrt], mDelimiters.data(), mDelimiters.length())==0) 
		// 	{
		// 		break;
		// 	}
		// }
		c = (u8)mData[++mCrt];
	}

	return OmnString(&mData[start], mCrt - start - delta);
}

// 
// It retrieves a word from the current position, which may be enclosed by " or '. 
// A word contains characters whose ASCII code is 
// greater than 0x32. If no word is found, "dft" is returned. The word is 
// delininated by any characters in 'deliminator'. If found, 'deli' will 
// hold the char that ends the word and the pointer points to the first
// char that ends the word.
//
OmnString	
OmnStrParser1::nextWordEx(
		const OmnString &dft, 
		const char * const deliminator) 
{
	int start, st = 0;
	char quote, prev_quote;

	while (mCrt <= mLen)
	{
		unsigned char c = (unsigned char)mData[mCrt];
		switch (st)
		{
		case 0: //start state
			 if (!strchr(deliminator, c)) //skip the white spaces
			 {
				if (c == '"' || c == '\'')
				{
					quote = c;
					prev_quote = 0; //whatever except '\\'
					st = 2;
					start = mCrt + 1;
				} 
				else
				{
					start = mCrt;
					st = 1;
				}
			 }
			 mCrt++;
			 break;
					
		case 1: //no " or ' leading, normal case
			 if (strchr(deliminator, c) || c == 0) 
			 {
				 // Found it.
			 	 return OmnString(&mData[start], mCrt - start);
			 }
			 mCrt++;
			 break;

		case 2: //find " or ' leading, special case
			 if (c == quote && prev_quote != '\\')
			 {
				 // Return the string, not including the quotes.
				 return OmnString (&mData[start], mCrt-start);
			 }
			 prev_quote = c;
			 mCrt++;
			 break;

		default: //error happens
			 return dft;
		}
	}
	
	return dft;
}

OmnString	
OmnStrParser1::nextEngWord(const OmnString &dft)
{
	// 
	// An English word contains only [a..z], [A..Z], '_', and [0..9]
	//
	// Skip leading white spaces
	// 
	while (mCrt < mLen)
	{
		unsigned char c = (unsigned char)mData[mCrt];
		// Chinese 
		// if (c >= 33 && c <= 126) 
		if (c >= 33) 
		{
			break;
		}
		mCrt++;
	}

	if (mCrt >= mLen)
	{
		// 
		// Didn't find any word
		//
		return dft;
	}

	int start = mCrt++;
	while (mCrt < mLen)
	{
		char c = mData[mCrt];
		if (!((c >= '0' && c <= '9') ||
			  (c >= 'a' && c <= 'z') ||
			  (c >= 'A' && c <= 'Z') ||
			  (c == '_')))
		{
			// 
			// Found the word
			//
			break;
		}
		mCrt++;
	}

	return OmnString(&mData[start], mCrt - start);
}


bool			
OmnStrParser1::nextInt(int64_t &value1, const int dft)
{
	u64 value2;
	bool isSigned;
	if (!nextInt(value2, value1, isSigned, dft)) return false;

	if (!isSigned)
	{
		if (value2 > LLONG_MAX)
		{
			value1 = dft;
			return false;
		}

		value1 = (int64_t)value2;
	}
	return true;
}


bool			
OmnStrParser1::nextIntEx(int64_t &value, const int dft)
{
	u64 value2;
	bool isSigned;
	if (!nextIntEx(value2, value, isSigned, dft)) return false;

	if (isSigned)
	{
		if (value2 > LLONG_MAX)
		{
			value = dft;
			return false;
		}

		value = (int64_t)value2;
	}
	return true;
}


bool			
OmnStrParser1::nextIntEx(u64 &value1, 
						int64_t &value2, 
						bool &isSigned, 
						const int dft)
{
	OmnString word = peekWord();
	if (checkMinMax(word, value1, value2, isSigned)) return true;

	isSigned = true;
	word = nextWord("");
	if (word.length() == 0)
	{
		value2 = dft;
		return false;
	}

	const char *d = word.data();
	int index = 0, len = word.length();

	if (d[0] == '+' || d[0] == '-') 
	{
		isSigned = (d[0] == '-')?true:false;
		index++;
	}
	else
	{
		isSigned = false;
	}
	
	while (index < len)
	{
		if (d[index] < '0' || d[index] > '9')
		{
			value2 = dft;
			return false;
		}

		index++;
	}
	
	if (isSigned) value2 = ::atoll(word.data());
	else          value1 = (u64)::atoll(word.data());
	return true;
}


bool			
OmnStrParser1::nextInt(u64 &value1, 
					  int64_t &value2, 
					  bool &isSigned, 
					  const int dft)
{
	// 
	// It retrieves the next word as integer. If the next 
	// word isnot an inteter, it returns false and 'def'
	// is returned. 
	// Note that we also hornor the following:
	// 1. INT_MAX
	// 2. U32_MAX
	// 3. U16_MAX
	// 4. U8_MAX
	// 5. ...
	//
	OmnString word = peekWord();
	word.removeLeadingWhiteSpace();
	word.removeTailWhiteSpace();
	word.removeTailSymbols();
	if (checkMinMax(word, value1, value2, isSigned)) return true;

	isSigned = true;
    word = nextDigits("");
    if (word.length() == 0)
    {
        value2 = dft;
        return false;
    }

    const char *d = word.data();
    int index = 0;
    if (d[0] == '+' || d[0] == '-') 
	{
		isSigned = (d[0] == '-')?true:false;
		index++;
	}
	else
	{
		isSigned = false;
	}

    if (d[index] < '0' || d[index] > '9')
    {
        value2 = dft;
        return false;
    }

	if (isSigned) value2 = ::atoll(word.data());
	else          value1 = (u64)::atoll(word.data());
    return true;
}

bool			
OmnStrParser1::nextAddr(u32 &value, const uint dft)
{
	while (mCrt < mLen && mData[mCrt] == ' ') mCrt++;

	if(mString.findSubString("0 ", mCrt) == mCrt)
	{
		value = 0;
		while (mCrt < mLen && mData[mCrt] != ' ') mCrt++;
		return true;
	}

	if (!OmnIpAddr::isValidAddr(&mData[mCrt]))
	{
		return false;
	}

	bool rslt = true;
	OmnInetAddr(&mData[mCrt], value);
	if (value == 0)
	{
		if(mString.findSubString("0.0.0.0",mCrt) != mCrt)
		{
			value = dft;
			rslt = false;
		}
		while (mCrt < mLen && mData[mCrt] != ' ') mCrt++;
	}
	else
	{
		// 
		// Move the cursor
		//
		while (mCrt < mLen && mData[mCrt] != ' ') mCrt++;
	}
	
	return rslt;
}


bool			
OmnStrParser1::nextMask(uint &value, const uint dft)
{
	return true;
}


bool
OmnStrParser1::nextBlock(OmnString &value)
{
	// 
	// It retrieves a block from the current location until 
	// it reads a line "...\n"
	// 
	int start = mCrt++;
	while (mCrt < mLen)
	{
		char c = mData[mCrt];
		if (c == '.' && mCrt + 3 < mLen &&
			mData[mCrt+1] == '.' &&
			mData[mCrt+2] == '.' &&
			mData[mCrt+3] == '\n')
		{
			// 
			// Found the end
			//
			value.assign(&mData[start], mCrt-start);
			mCrt += 4;
			return true;
		}

		mCrt++;
	}

	return false;
}


OmnString
OmnStrParser1::nextDigits(const OmnString &dft)
{
	// 
	// Skip leading white spaces
	// 
	while (mCrt < mLen)
	{
		unsigned char c = (unsigned char) mData[mCrt];
		// 
		// Chen Ding, 10/21/2005, To support Chinese, we do not want to 
		// check the max.
		//
		// if (c >= 33 && c <= 126) 
		if (c >= 33) 
		{
			break;
		}
		mCrt++;
	}

	if (mCrt >= mLen)
	{
		// 
		// Didn't find any word
		//
		return dft;
	}

	int start = mCrt;
	while (mCrt < mLen)
	{
		unsigned char c = (unsigned char)mData[mCrt];
		if ((c != '-') && (c != '+') && (c < '0' || c > '9'))
		{
			// 
			// Found the digit string 
			//
			break;
		}
		mCrt++;
	}

	return OmnString(&mData[start], mCrt - start);
}

// 
// Chen Ding, 03/21/2007
//
OmnString
OmnStrParser1::getValue(const OmnString &name, const OmnString &dflt)
{
	// 
	// It looks for the pattern "<key> = <value>". If found, it returns
	// true and the value is returned through "value".
	//
	if (name == "")
	{
		OmnAlarm << "Name is null" << enderr;
		return dflt;
	}

	int pos = 0;
	while (pos < mLen)
	{
		if ((pos = findWord(pos, name)) < 0)
		{
			// 
			// Didn't find it. 
			//
			return dflt;
		}

		// 
		// Found the name. Check whether the following is a '='.
		//
		mCrt = pos + name.length();
		int ss;
		if ((ss = nextChar(mCrt, '=', true)) >= 0)
		{
			// 
			// Found the pair. 
			//
			mCrt = ss+1;
			return nextWord(dflt);
		}

		pos += name.length();
	}

	return dflt;
}


int
OmnStrParser1::findWord(u32 startPos, const OmnString &word)
{
	// 
	// It finds the word "word" from the start position 'startPos'. 
	// If found, it returns the starting index of the word. Otherwise,
	// it returns -1.
	//
	if (startPos >= (u32)mLen)
	{
		return -1;
	}

	if (word.length() == 0) return startPos;

	const char *data = word.data();
	const char *str = strstr(&mData[startPos], data);
	if (str)
	{
		// 
		// Found it. 
		//
		return str - mData;
	}

	// 
	// Did not find it. 
	//
	return -1;
}


int
OmnStrParser1::nextChar(const u32 start, const char c, const bool leading)
{
	// 
	// This function searches for the first occurance of the 
	// character 'c'. If found, it returns the position. 
	// Otherwise, it returns -1.
	//
	// If 'leading' is true, 'c' must be the first none white space
	// character. 
	//
	u32 pos = start;	
	while (pos < (u32)mLen)
	{
		char ccc = mData[pos];
		if (ccc == ' ' || ccc == '\t')
		{
			pos++;
			continue;
		}

		if (c == ccc) return pos;

		if (leading) return -1;

		pos++;
	}
	
	return -1;
}


int
OmnStrParser1::skipWhiteSpace()
{
	// 
	// A white space is anything whose code is less than 33. 
	// It returns the number of bytes skipped.
	//
	int skipped = 0;
	while (mCrt < mLen)
	{
		unsigned char c = (unsigned char) mData[mCrt];
		// 
		// To support Chinese, we do not want to check the max.
		//
		if (c >= 33) 
		{
			break;
		}
		mCrt++;
		skipped++;
	}

	return skipped;
}


void
OmnStrParser1::dump() const
{
	cout << "mLen:     " << mLen << endl;
	cout << "mCrt:     " << mCrt << endl;
	cout << "mConnets: " << mString.data() << endl;
}


bool		
OmnStrParser1::getValuePair(OmnString &name, 
						   OmnString &value, 
						   const char bracket,
						   const char delimiter)
{
	// 
	// This function retrieves a name-value pair from the 
	// current position 'mCrt'. The pair
	// is bracketed by 'bracket' and separated by 'delimiter'. 
	// If 'bracket' is '[' and 'delimiter' is ',', for instance, 
	// it parses the following:
	// 		"[name, value], [name, value], ..., [name, value]"
	//
	skipWhiteSpace();	
	
	// 
	// Retrieve the opening bracket. Note that if the bracket
	// is a space, there is actually no bracket.
	//
	if (mCrt >= mLen)
	{
		// 
		// Did not find the pair. 
		//
		return false;
	}

	if (bracket != ' ')
	{
		if (mData[mCrt] != bracket)
		{
			OmnAlarm << "Expecting the bracket: " << bracket 
				<< ". " << &mData[mCrt] << enderr;
			return false;
		}

		mCrt++;
	}

	if (mCrt >= mLen)
	{
		OmnAlarm << "Expecting open bracket: " << &mData[mCrt] << enderr;
		return false;
	}

	// 
	// Retrieve the name
	//
	OmnString delimiterStr;
	delimiterStr << delimiter;
	name = nextWordEx("", delimiterStr.data());
	if (name == "")
	{
		OmnAlarm << "Expecting the name: " << &mData[mCrt] << enderr;
		return false;
	}

	mCrt++;
	if (mCrt >= mLen)
	{
		OmnAlarm << "Expecting the value: " << &mData[mCrt] << enderr;
		return false;
	}

	skipWhiteSpace();

	// 
	// Retrieving the value
	//
	delimiterStr = "";
	switch (bracket)
	{
	case '[':
		 delimiterStr = "]";
		 break;
	
	case '(':
		 delimiterStr = ")";
		 break;
	
	case '{':
		 delimiterStr = "}";
		 break;
	
	default:
		 delimiterStr = "";
		 delimiterStr << bracket;
		 break;
	}
		
	value = nextWordEx("", delimiterStr.data());
	if (value == "")
	{
		OmnAlarm << "Expecting the value: " << &mData[mCrt] << enderr;
		return false;
	}

	// 
	// Retrieving the closing bracket
	//
	if (delimiter == ' ') return true;
	skipWhiteSpace();
	if (mCrt >= mLen) 
	{
		OmnAlarm << "Expecting the closing delimiter: " << delimiter
			<< ". " << &mData[mCrt] << enderr;
		return false;
	}

	if ((bracket == '[' && mData[mCrt] != ']') ||
		(bracket == '(' && mData[mCrt] != ')') ||
		(bracket == '{' && mData[mCrt] != '}') ||
		(bracket != '[' && bracket != '(' && bracket != '{' &&
		 bracket != mData[mCrt]))
	{
		OmnAlarm << "Failed to retrieve the closing bracket: " 
			<< bracket << ". " << mData << enderr;
		return false;
	}

	mCrt++;
	skipWhiteSpace();
	if (delimiter == ' ' || mCrt >= mLen) return true;

	if (delimiter != mData[mCrt])
	{
		OmnAlarm << "Failed to find the pair delimiter: " 
			<< mData << enderr;
		return false;
	}

	mCrt++;
	skipWhiteSpace();
	return true;
}


// 
// This function determines whether the 'word' is a max/min. If yes, 
// it returns true. Otherwise, it returns false. 
// When the function returns true, 'isSigned' indicates whether the
// value is signed (true) or not signed (false). If it is signed, 
// the value is returned through 'value1'. Otherwise, it is returned
// through 'value2'. 
//
bool
OmnStrParser1::checkMinMax(const OmnString &word, 
						  u64 &value1, 
						  int64_t &value2, 
						  bool &isSigned)
{
	const char *data = word.data();
	if ((data[0] >= '0' && data[0] <= '9') || 
		(data[0] == '+' || data[0] == '-'))
	{
		return false;
	}

	if (word == "INT_MAX") 
	{
		nextEngWord(""); 
		value2 = INT_MAX; 
		isSigned = true;
		return true;
	}

	if (word == "INT_MIN") 
	{
		nextEngWord(""); 
		value2 = INT_MIN; 
		isSigned = true;
		return true;
	}

	if (word == "LONG_MAX") 
	{
		nextEngWord(""); 
		value2 = LONG_MAX; 
		return true;
	}

	if (word == "LONG_MIN") 
	{
		nextEngWord(""); 
		value2 = LONG_MIN; 
		return true;
	}

	if (word == "LLONG_MAX") 
	{
		nextEngWord(""); 
		value2 = LLONG_MAX; 
		return true;
	}

	if (word == "LLONG_MIN") 
	{
		nextEngWord(""); 
		value2 = LLONG_MIN; 
		return true;
	}

	if (word == "SHORT_MAX") 
	{
		nextEngWord(""); 
		value2 = SHRT_MAX; 
		return true;
	}

	if (word == "SHORT_MIN") 
	{
		nextEngWord(""); 
		value1 = SHRT_MIN; 
		return true;
	}

	if (word == "CHAR_MAX") 
	{
		nextEngWord(""); 
		value1 = CHAR_MAX; 
		return true;
	}

	if (word == "CHAR_MIN") 
	{
		nextEngWord(""); 
		value1 = CHAR_MIN; 
		return true;
	}

	isSigned = false;
	if (word == "U64_MAX") 
	{
		nextEngWord(""); 
		value1 = ULLONG_MAX; 
		return true;
	}

	if (word == "U32_MAX") 
	{
		nextEngWord(""); 
		value1 = UINT_MAX; 
		return true;
	}

	if (word == "U16_MAX") 
	{
		nextEngWord(""); 
		value1 = USHRT_MAX; 
		return true;
	}

	if (word == "U8_MAX") 
	{
		nextEngWord(""); 
		value1 = UCHAR_MAX; 
		return true;
	}

	return false;
}


OmnString
OmnStrParser1::getSubstr(const OmnString &pattern, const OmnString &dft)
{
	// 
	// The function retrieves a substring that matches the 
	// pattern: 'pattern', where 'pattern' is in the form of
	// 'xxxx$$$yyyy', where 'xxxx' and 'yyyy' must be an exact
	// match and '$$$' can be anything. The match will start from
	// the current position. If a match is found, it returns '$$$'
	// portion. If not found, the position is not changed and 
	// 'dft' is returned.
	//
	
	// 
	// Find the 'xxxx' and 'yyyy' parts.
	//
	const char *data = pattern.data();

	OmnStrParser1 parser(pattern);
	int idx = parser.findWord(0, "$$$");
	if (idx < 0)
	{
		OmnAlarm << "Pattern is invalid. The '$$$' is not found: " 
			<< pattern << enderr;
		return dft;
	}
	OmnString leadingPart(pattern.data(), idx);

	if (idx + 3 >= pattern.length())
	{
		OmnAlarm << "Incorrect pattern: Too Short: " 
			<< idx << ": " << pattern << enderr;
		return dft;
	}

	OmnString tailPart(&data[idx+3]);

	int crtPos = mCrt;
	int len = mLen - leadingPart.length() - tailPart.length();
	while (crtPos < len)
	{
		// 
		// Find	the leading portion
		//
		int startIdx = findWord(crtPos, leadingPart);
		if (startIdx < 0)
		{
			// 
			// The pattern is not found. 
			//
			return dft;
		}

		// 
		// Found the leading position. 'startIdx' is the beginning of 
		// the string right after the leading portion.
		//
		int endIdx;
		for (int i=startIdx+leadingPart.length(); i<mLen - tailPart.length(); i++)
		{
			if ((endIdx = findWord(i, tailPart)) >= 0)
			{
				// 
				// Found the tailPart. 
				//
				startIdx += leadingPart.length();
				OmnString str(&mData[startIdx], endIdx - startIdx);
				return str;
			}
		}

		crtPos++;
	}

	// 
	// Did not find it.
	//
	return dft;
}


OmnString
OmnStrParser1::getLastWord(const OmnString &str, const char sep)
{
	// This function retrieves the last word separated by 'sep'. 
	const char *data = str.data();
	int len = str.length();
	for (int i=len-1; i>=0; i--)
	{
		if (data[i] == sep)
		{
			if (i == len-1)
			{
				return "";
			}
			return OmnString(&data[i]);
		}
	}
	return str;
}


//felicia 2011/09/02 for border
OmnString
OmnStrParser1::convertBorder(OmnString &str, const OmnString &browser)
{
	//this function is to change the div's width,height,x,y
	//1. the div must have the attribute isIE
	//2. the div must have style
	//3. the div style must have border or border-width
	//4. if((isIE="true" && browser != "IE")|| (isIE=="true" && browser == "IE"))
	int divstart = str.findSubString("<div ", 0);
	int divend = str.findSubString(">", divstart+4);
	while(divstart != -1 && divend != -1)
	{
		OmnString divcont = str.substr(divstart, divend);
		OmnString newdivcont = divcont;
		int isIEt = divcont.findSubString(" isIE=\"true\"", 0);
		if(isIEt == -1)
		{
			isIEt = divcont.findSubString("\"isIE=\"true\"", 0);
		}
		if(isIEt == -1)
		{
			isIEt = divcont.findSubString(" isie=\"true\"", 0);
		}
		if(isIEt == -1)
		{
			isIEt = divcont.findSubString("\"isie=\"true\"", 0);
		}
		int isIEf = divcont.findSubString(" isIE=\"false\"", 0);
		if(isIEf == -1)
		{
			isIEf = divcont.findSubString("\"isIE=\"false\"", 0);
		}
		if(isIEf == -1)
		{
			isIEf = divcont.findSubString(" isie=\"false\"", 0);
		}
		if(isIEf == -1)
		{
			isIEf = divcont.findSubString("\"isie=\"false\"", 0);
		}
		if(isIEt != -1 && browser != "IE")
		{
			//firefox to IE
			IEchangediv(newdivcont);
		}
		else if(isIEf != -1 && browser == "IE")
		{
			//IE to firefox
			changediv(newdivcont);	
		}
		str.replace(divcont, newdivcont, true);
		divstart = str.findSubString("<div ", divend+1);
		divend = str.findSubString(">", divstart+4);
	}
	return "";
}

void
OmnStrParser1::setBorder(OmnString &stylecont, int &bstart, OmnString &value)
{
	int bend, nums;
	OmnString bordercont;
	OmnString border[20];
	bool finished;
	AosStrSplit split;
	bend = stylecont.findSubString(";", bstart);
	bordercont = stylecont.substr(bstart, bend);
	nums = split.splitStr(bordercont.data(), " ", border, 20, finished);
	if(nums >= 1)
	{
		int val = atoi(border[3].data()) + atoi(value.data());
		value = "";
		value << val;
	}
}

void
OmnStrParser1::IEchangediv(OmnString &divcont)
{
	int stylestart = divcont.findSubString(" style=\"", 0);
	int styleend = divcont.findSubString("\"", stylestart+9);
	int divlen = divcont.length();
	if(stylestart != -1 && styleend < divlen)
	{
		OmnString stylecont = divcont.substr(stylestart, styleend);
		OmnString oldstylecont = stylecont;
		OmnString width, height;
		int bstart = stylecont.findSubString(" border-left:", 0);
		if(bstart == -1)
		{
			bstart = stylecont.findSubString("\"border-left:", 0);
		}
		if(bstart != -1)
		{
			setBorder(stylecont, bstart, width);
		}
		bstart = stylecont.findSubString(" border-right:", 0);
		if(bstart == -1)
		{
			bstart = stylecont.findSubString("\"border-right:", 0);
		}
		if(bstart != -1)
		{
			setBorder(stylecont, bstart, width);
		}
		bstart = stylecont.findSubString(" border-bottom:", 0);
		if(bstart == -1)
		{
			bstart = stylecont.findSubString("\"border-bottom:", 0);
		}
		if(bstart != -1)
		{
			setBorder(stylecont, bstart, height);
		}
		bstart = stylecont.findSubString(" border-top:", 0);
		if(bstart == -1)
		{
			bstart = stylecont.findSubString("\"border-top:", 0);
		}
		if(bstart != -1)
		{
			setBorder(stylecont, bstart, height);
		}
		int widthstart = stylecont.findSubString(" width:", 0);
		if(widthstart == -1)
		{
			widthstart= stylecont.findSubString("\"width:", 0);
		}
		if(widthstart != -1)
		{
			int widthend = stylecont.findSubString(";", widthstart);
			OmnString widthstr = stylecont.substr(widthstart, widthend);
			widthstart = widthstr.findSubString(":", 0);
			if(widthstart != -1)
			{
				widthend = widthstr.findSubString(";", widthstart);
				widthstr = widthstr.substr(widthstart+1, widthend-1);
				OmnString newwidth = " ";
				newwidth << atoi(width.data()) + atoi(widthstr.data()) << "px;";
				stylecont.replace(widthstr, newwidth, true);
			}
		}
		
		int heightstart = stylecont.findSubString(" height:", 0);
		if(heightstart == -1)
		{
			heightstart = stylecont.findSubString("\"height:", 0);
		}
		if(heightstart != -1)
		{
			int heightend = stylecont.findSubString(";", heightstart);
			OmnString heightstr = stylecont.substr(heightstart, heightend);
			heightstart = heightstr.findSubString(":", 0);
			if(heightstart != -1)
			{
				heightend = heightstr.findSubString(";", heightstart);
				heightstr = heightstr.substr(heightstart+1, heightend-1);
				OmnString newheight = " ";
				newheight << atoi(height.data()) + atoi(heightstr.data()) << "px;";
				stylecont.replace(heightstr, newheight, true);
			}
		}
		divcont.replace(oldstylecont, stylecont, true);
	}
}


void
OmnStrParser1::changediv(OmnString &divcont)
{
	int stylestart = divcont.findSubString(" style=\"", 0);
	int styleend = divcont.findSubString("\"", stylestart+9);
	int divlen = divcont.length();
	if(stylestart != -1 && styleend < divlen)
	{
		OmnString stylecont = divcont.substr(stylestart, styleend);
		OmnString oldstylecont = stylecont;
		int bstart = stylecont.findSubString(" border:", 0);
		int bend, nums;
		OmnString bordercont, width, height;
		OmnString border[20];
		bool finished;
		AosStrSplit split;
		if(bstart != -1)
		{
			bend = stylecont.findSubString(";", bstart);
			bordercont = stylecont.substr(bstart, bend);
			finished = false;
			nums = split.splitStr(bordercont.data(), " ", border, 20, finished);
			if(nums >= 1)
			{
				width << 2*atoi(border[2].data()) << "px";
				height << 2*atoi(border[2].data()) << "px";
			}
		}
		else
		{
			bstart = stylecont.findSubString(" border-width:", 0);
			if(bstart == -1)
			{
				bstart = stylecont.findSubString("\"border-width:", 0);
			}
			if(bstart != -1)
			{
				bend = stylecont.findSubString(";", bstart);
				bordercont = stylecont.substr(bstart, bend);
				finished = false;
				nums = split.splitStr(bordercont.data(), " ", border, 20, finished);
				if(nums >= 1)
				{
					width << atoi(border[3].data()) + atoi(border[5].data()) << "px";
					height << atoi(border[2].data()) + atoi(border[4].data()) << "px";
				}
			}
		}

		int widthstart = stylecont.findSubString(" width:", 0);
		if(widthstart == -1)
		{
			widthstart = stylecont.findSubString("\"width:", 0);
		}
		if(widthstart != -1)
		{
			int widthend = stylecont.findSubString(";", widthstart);
			OmnString widthstr = stylecont.substr(widthstart, widthend);
			widthstart = widthstr.findSubString(":", 0);
			if(widthstart != -1)
			{
				widthend = widthstr.findSubString(";", widthstart);
				widthstr = widthstr.substr(widthstart+1, widthend-1);
				OmnString newwidth = " ";
				newwidth << atoi(width.data()) + atoi(widthstr.data()) << "px;";
				stylecont.replace(widthstr, newwidth, true);
			}
		}
		
		int heightstart = stylecont.findSubString(" height:", 0);
		if(heightstart == -1)
		{
			heightstart = stylecont.findSubString("\"height:", 0);
		}
		if(heightstart != -1)
		{
			int heightend = stylecont.findSubString(";", heightstart);
			OmnString heightstr = stylecont.substr(heightstart, heightend);
			heightstart = heightstr.findSubString(":", 0);
			if(heightstart != -1)
			{
				heightend = heightstr.findSubString(";", heightstart);
				heightstr = heightstr.substr(heightstart+1, heightend-1);
				OmnString newheight = " ";
				newheight << atoi(height.data()) + atoi(heightstr.data()) << "px;";
				stylecont.replace(heightstr, newheight, true);
			}
		}
		divcont.replace(oldstylecont, stylecont, true);
	}
}



//felicia 2011/04/27 for read more
OmnString
OmnStrParser1::subHtml(OmnString &str)
{
	int p1 = str.findSubString("subdiv=\"true\"",0);
	int p2 = str.findSubString("subdiv=\"true\"",p1+1);
	while(p1 != -1)
	{
		OmnString cont; 
		if(p2 == -1)
		{
			p2 = str.length() - 1;
		}
		cont = str.substr(p1-5,p2-6);
		int len = cont.length();
		int p3 = cont.findSubString("onreadmore",0);
		OmnString params,values;
		if(p3 != -1)
		{
			int p4 = cont.findSubString("\'",p3+1);
			int p5 = cont.findSubString("\'",p4+1);
			if(p4 != -1 && p5 != -1)
			{
				params = cont.substr(p4+1,p5-1);
				p3 = cont.findSubString("\'",p5+1);
				p4 = cont.findSubString("\'",p3+1);
				if(p3 != -1 && p4 != -1)
				{
					values = cont.substr(p3+1,p4-1);
				}
			}
			int cnum = 0xffffffab;
			OmnString rmname,act;
			if(params !="" && values !="")
			{
				OmnStrParser1 parser(params," ");
				OmnString param;
				param = parser.nextWord();
				OmnStrParser1 value(values," ");
				while(param != "")
				{
					if(param == "cnum" || param == "cnum\\")
					{
						cnum = atoi(value.nextWord().data());
					}
					else if(param == "rmname" || param == "rmname\\")
					{
						rmname = value.nextWord();
					}
					else
					{
						act = value.nextWord();
					}
					param = parser.nextWord();
				}
			}
			if(cnum != (int)0xffffffab)
			{
				trimHtml(cont, cnum);
			}
			if(rmname == "")
			{
				rmname = "... Read More";
			}
			OmnString readmore = "";
			readmore <<"<a onclick=\"gAosHtmlHandler.mouseEvent(this.parentNode,\\' act\\',\\' "<<
				act<<"\\')\">"<<rmname<<"</a>";
			cont.insert(readmore,cont.length()-6);
			str.replace(p1-5,len,cont);
		}
		p1 = str.findSubString("subdiv=\"true\"",p1+1);
		p2 = str.findSubString("subdiv=\"true\"",p1+1);
	}
	return "";
}


// Chen Ding, 04/26/2011
int	
OmnStrParser1::trimHtml(OmnString &str, const int max_len)
{
	// The input 'str' is an HTML doc. This function trims
	// its contents to 'max_len'. It goes over the doc, 
	// ignoring any tags (i.e., <xxx ...>), counting on 
	// the text length. When it is longer than the max, 
	// trim it.
	char *data = (char *)str.data();
	int idx = 0;
	int str_len = str.length();
	char c;
	int trim_start = -1;
	int trim_end = -1;
	int data_len = 0;
	while (idx < str_len)
	{
		c = data[idx];
		switch (c)
		{
		case '<':
			 // The beginning of a tag. Scan all the way to '>'. 
			 idx++;
			 while (idx < str_len)
			 {
				 c = data[idx];
				 if (c == '>')
				 {
					 // It is the end of the tag. 
					 idx++;
					 break;
				 }
				 idx++;
			 }
			 break;

		default:
			 // It is a text. Check whether it is CDATA
			 if (idx + 9 < str_len &&
				 data[idx+1] == '<' &&
				 data[idx+2] == '!' &&
				 data[idx+3] == '[' &&
				 data[idx+4] == 'C' &&
				 data[idx+5] == 'D' &&
				 data[idx+6] == 'A' &&
				 data[idx+7] == 'T' &&
				 data[idx+8] == 'A')
			 {
				 trimCdata(data, idx, str_len, data_len, max_len);
			 }

			 // It is not a CDATA. Scan all the way to '<'. 
			 trim_start = -1;
			 trim_end = -1;
			 int charset_count = 0;
			 while (idx < str_len)
			 {
				 c = data[idx];
				 if (c == '<')
				 {
					 // It is the end of the current text.
					 if (trim_start < 0) break;

					 // Need to trim
					 trim_end = idx-1;
					 break;
				 }

				 // It is text. 
				 if (data_len < max_len)
				 {
					 if((unsigned char)c > 128)
					 {
						charset_count++;	
						if(charset_count == 3)  //UTF-8 
						{
							data_len++;
							charset_count = 0;
						}
					 }
					 else
					 {
					 	data_len++;
					 }
				 }
				 else
				 {
					 if (trim_start == -1) trim_start = idx;
				 }
				 idx++;
			 }

			 if (trim_start > 0)
			 {
				 if (trim_end < 0) trim_end = str_len-1;
				 int delta = trim_end - trim_start + 1;
				 int len  = str_len - trim_end - 1; //- delta;
				 memmove(&data[trim_start], &data[trim_end+1], len);
				 str_len -= delta;
				 idx = trim_start;
				 //idx -= delta;
				 //int len = trim_start+delta;
				 if (str_len < str.length())
				 {
					 str.setLength(str_len);
				 }
			 }
			 break;
		}
	}
/*
	if (str_len < str.length())
	{
		str.assign(data, data_len);
	}
*/
	return data_len;
}


bool
OmnStrParser1::trimCdata(
		char *data, 
		int &idx, 
		int &str_len, 
		int &data_len, 
		const int max_len)
{
	// It is CDATA. Scan texts all the way to ']]>'. 
	idx += 9;
	int trim_start = -1;
	int trim_end = -1;
	char c;
	while (idx < str_len)
	{
		c = data[idx];
		if (c == ']' && idx + 3 < str_len &&
			data[idx+1] == ']' && data[idx+2] == '>')
		{
			// It is the end of the CDATA.
			trim_end = idx-1;
			idx += 3;
			break;
		}

		if (data_len >= max_len)
		{
			if (trim_start == -1) trim_start = idx;
		}
		else
		{
			data_len++;
		}
		idx++;
	}

	if (trim_start > 0)
	{
		// This means that some of the contents need to be
		// trimmed: [trim_start, trim_end]
		aos_assert_r(trim_end > trim_start, -1);
		int delta = trim_end - trim_start + 1;
		memmove(&data[trim_start], &data[trim_end+1], delta);
		str_len -= delta;
		idx -= delta;
	}
	return true;
}


bool
OmnStrParser1::nextExcelEntry(int &idx, OmnString &entry)
{
	// This function retrieves the next Excel entry. Excel entries are
	// comma delimited. Its rules are:
	// If an entry starts with '"', the entire entry is double
	// quoted. Any double quotes in the contents are escaped 
	// by prepending another double quote. Otherwise, the entry
	// is terminated by a comma. 
	//
	// This function starts the parsing from 'idx'. Upon finishing,
	// 'idx' points to the comma
	//	int len = mString.length();
	//	const char *data = mString.data();
	// entry = aaa, bbb, ccc, ddd
	
	// 'idx' points to the beginning of the entry. 
	const int len = mString.length();
	aos_assert_r(idx >= 0 && idx < len, false);

	// Skip leading white spaces
	if (mData[idx] == ',' || mData[idx] == '\r' || mData[idx] == '\n')
	{
		// This is an empty entry. 
		entry = "";
		idx++;
		return true;
	}
	
	if (mData[idx] == '"')
	{
		// It is a double quoted entry. Need to go all the way to the
		// closing double quote. 
		idx++;
		int start = idx;
		while (idx < len)
		{
			if (mData[idx] == '"')
			{
				if (idx == len-1 || mData[idx+1] == '\r' || mData[idx+1] == '\n')
				{
					// Found the last entry
					int end = idx-1;
					entry.assign(&mData[start], end-start+1);
					entry.replace("\"\"", "\\\"", true);
					idx = -1;
					return true;
				}

				if (mData[idx+1] == '"')
				{
					idx++;
				}
				else
				{
					int end = idx-1;
					idx++;
					char c = mData[idx];
					switch (c)
					{
					case ',':
						 idx++;
						 entry.assign(&mData[start], end-start+1);
						 entry.replace("\"\"", "\\\"", true);
						 return true;

					case '\r':
					case '\n':
						 entry.assign(&mData[start], end-start+1);
						 entry.replace("\"\"", "\\\"", true);
						 idx = -1;
						 return true;

					default:
						 OmnAlarm << "Format incorrect: " << mString << enderr;
						 return false;
					}
				}
			}
			idx++;
		}

		OmnAlarm << "Format incorrect: " << mString << enderr;
		return false;
	}
	else
	{
		int start = idx;
		idx ++;
		while (idx < len)
		{
			char c = mData[idx];
			int end;
			switch (c)
			{
			case ',':
				 end = idx-1;
				 entry.assign(&mData[start], end-start+1);
				 entry.replace("\"", "\\\"", true);
				 idx++;
				 return true;

			case '\r':
			case '\n':
				 end = idx-1;
				 idx = -1;
				 entry.assign(&mData[start], end-start+1);
				 entry.replace("\"", "\\\"", true);
				 return true;

			default:
				 break;
			}
			idx++;
		}
		int end = idx-1;
		entry.assign(&mData[start], end-start+1);
		entry.replace("\"", "\\\"", true);
		idx = -1;
		return true;
	}

	return true;
}


bool
OmnStrParser1::parserEntry(OmnString &value)
{
	// This function is used to export data in Excel form. 
	int idx = 0;
	const int len = value.length();
	aos_assert_r(len >= idx, false);

	// Find the comma
	int pos = findWord(0, ",");

	// If there is no comma and 
	// if (pos == -1 && mData[0] == '\\' && mData[1] == '"')
	if ((pos == -1 && mData[0] == '\\' && mData[1] == '"') || (pos >=0 && pos < len))
	{
		value.replace("\\\"", "\"\"", true);
		OmnString str = "\"";	
		str << value << "\"";
		value = str; 
		return true;
	}
	
	value.replace("\\\"", "\"", true);
	return true;
}


bool 
OmnStrParser1::containWord(
		const OmnString &words, 
		const OmnString &word, 
		const OmnString &seps)
{
	// This function checks whether 'words' contain the word 'word'. 
	// Words are separated by 'seps'. 
	OmnStrParser1 parser(words, seps.data());

	OmnString ww;
	while ((ww = parser.nextWord()) != "")
	{
		if (ww == word) return true;
	}
	return false;
}


bool 
OmnStrParser1::skipParenthesis(
		const char quote, 
		AosRundata *rdata)
{
	// This function assumes the parser points to the following pattern:
	// 	" (xxx)"
	// There may be leading white spaces. It skips the contents until
	// the next parenthesis. Parentheses that are inside quoted strings
	// are ignored. Quotes are defined by 'quote'.
	skipWhiteSpace();

	if (mCrt >= mLen)
	{
		rdata->setErrmsg(AOSDICTERM("missing_data", rdata));
		return false;
	}

	if (mData[mCrt] != '(')
	{
		rdata->setErrmsg(AOSDICTERM("expect_parenthesis", rdata));
		return false;
	}
	mCrt++;

	char c;
	bool is_in_quote = false;
	while (mCrt++ < mLen)
	{
		c = mData[mCrt]; 
		if (c == quote)
		{
			if (is_in_quote)
			{
				// It is already in quote. Need to check whether it is 
				// escaped. 
				if (mCrt < mLen-1 && mData[mCrt+1] == quote)
				{
					// Double quotes are used to escape. 
					mCrt++;
					continue;
				}
				
				rdata->setErrmsg(AOSDICTERM("str_contain_quotes", rdata));
				return false;
			}
			else
			{
				is_in_quote = true;
			}
		}
		
		if (c == ')') 
		{
			if (is_in_quote)
			{
				// It is in quote. Ignore it.
				continue;
			}

			// This is the end of the parenthesis. 
			return true;
		}
	}

	rdata->setErrmsg(AOSDICTERM("missing_closing_parenthesis", rdata));
	return false;
}


bool 
OmnStrParser1::getValueList(
		vector<OmnString> &names, 
		const char *delimitors, 
		const char terminator, 
		const int max,
		AosRundata *rdata)
{
	names.clear();

	int count = max;
	while (count--)
	{
		skipWhiteSpace();

		if (mCrt >= mLen)
		{
			rdata->setErrmsg(AOSDICTERM("missing_closing_parenthesis", rdata));
			return false;
		}

		if (mData[mCrt] == ')')
		{
			// This is the end of the data. 
			mCrt++;
			return true;
		}

		OmnString word = nextWordEx("", delimitors);
		if (word.length() <= 0)
		{
			rdata->setErrmsg(AOSDICTERM("invalid_data_format", rdata));
			return false;
		}

		names.push_back(word);
	}

	rdata->setErrmsg(AOSDICTERM("too_many_values", rdata));
	return false;
}
