////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlParser.h
//
// Modification History:
// 09/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
// Moved to API
#if 0
#include "XmlParser/XpUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"


bool AosNextWordChar(
		char *data, 
		const int datalen,
		char &ch,
		int &idx);

// This function retrieves the string that is quoted string. The string
// may be quoted by either the single quote or double quote. If it is 
// quoted by single quote, double quotes may appear in the string, and
// vice vesa. The same quote may be escaped inside the string. 
// 'idx' should point right after the quote.
// 
// If found, 'len' is the length of the string. Note that the length
// does not include the opening and closing quotes. 
// 
// Returns:
//	false:	if the string is not end-quoted properly or one of the
//			'exclude' chars is found. In this case, 'start_idx' points
//			to the first found 'exclude' char. 
//	true:	If the quoted string is found. 'str_start' points right
//			after the found string. 'start_idx' points right after
//			the quote.
bool AosNextQuotedStr(const u8 * const data, 
		const int datalen,
		const unsigned char quote, 
		int &crtIdx, 
		int &str_start,
		int &str_len, 
		const u8 *exclude, 
		const int exclen)
{
	char map[256];
	memset(map, 1, 128);
	if (exclude)
	{
		for (int i=0; i<exclen; i++)
			map[exclude[i]] = 1;
	}

	int theidx = crtIdx;
	str_start = crtIdx;
	int prev = -1;
	u8 c;
	while (theidx < datalen)
	{
		c = data[theidx];
		aos_assert_r(((c >= 32 && c <= 126 && map[c]) || 
			c == 10 || c == 11 ||
			c >= 128), false);

		if (c == quote)
		{
			// Check whether it is escaped
			if (prev == -1 || prev != '\\')
			{
				// It is not escaped. It is the end of the string
				str_len = theidx - crtIdx;
				crtIdx = theidx+1;
				return true;	
			}

			// It is escaped
		}	
		
		prev = c;
		theidx++;
	}

	// This means it runs out of the data but still the ending quote
	// not found. This is an error. 
	OmnAlarm << "Failed to find the string: "
		<< &data[crtIdx] << enderr;
	return false;
}


// This function retrieves the next word, starting from 'start_idx', 
// skipping the leading white spaces, terminating at 
// a white space, or ']]>'. 
int AosNextCdataWord(const u8 * const data, 
		const int datalen,
		const int start_idx, 
		int &word_start,
		int &len, 
		bool &closed)
{
	// Removing the white spaces
	char c;
	int idx = start_idx;
	closed = false;
	while (idx < datalen)
	{
		c = data[idx];
		if (c > 32 && c < 127) break;
		idx++;
	}

	word_start = idx;
	// Search for the end of the word, which should stop at either
	// one of the white spaces or one of the 'delis'
	while (idx < datalen)
	{
		c = data[idx];
		if (c <= 32 || c >= 127) break;
		if (idx + 2 < datalen && c == ']' && 
			data[idx+1] == ']' && data[idx+2] == '>')
		{
			len = idx - word_start;
			closed = true;
			idx += 3;
			return idx;
		}
		idx++;
	}

	len = idx - word_start;
	return idx;
}


// Chen Ding, 2014/12/06
// Moved to ApsApiN.h
//
// This function retrieves the next English word, starting from 'start_idx', 
// skipping the leading white spaces, terminating
// at one of 'delis', a white space, or the end of the data. 
// The following characters 
// are not allowed, i.e., will terminate the word:
//	space, !, ", ', (, ), comma, ., /, <, >, \, `(96)
/*
int AosNextEnWord(const u8 * const data, 
		const u8 *theMap,
		const unsigned char *delis,
		const int delis_len,
		const int start_idx, 
		const int datalen,
		int &word_start,
		int &len)
{
	unsigned char map[256];
	if (delis)
	{
		memcpy(map, theMap, 256);
		for (int i=0; i<delis_len; i++)
			map[delis[i]] = 0;
		theMap = map;
	}
	
	// Removing the white spaces
	u8 c;
	int idx = start_idx;
	while (idx < datalen)
	{
		if (data[idx] > 32) break;
		idx++;
	}

	word_start = idx;
	// Search for the end of the word, which should stop at either
	// one of the white spaces or one of the 'delis'
	while (idx < datalen)
	{
		c = data[idx];
		if (c <= 127 && theMap[c] == 0) break;
		idx++;
	}

	len = idx - word_start;
	return idx;
}
*/


/*
// This function retrieves the next word, starting from 'start_idx', 
// skipping the leading and trailing white spaces, terminating
// at one of 'delis' or the end of the data. 
//
// The retrieved word is stored in 'buff', whose maximum length
// is 'len'. If the word is too long than 'len', it is an error.
//
// Upon return, 'len' holds the length of the word. The null 
// character is added into 'buff' upon return.
bool AosNextWord(
		char *data, 
		const char *delis,
		const int delis_len,
		const int start_idx, 
		const int datalen,
		int &word_start,
		char *buff,
		u32 &len, 
		int &wordlen)
{
	// Removing the white spaces
	char c;
	int idx = start_idx;
	const int maxlen = len;
	len = 0;
	while (idx < datalen)
	{
		c = data[idx];
		if (c > 32) break;
		idx++;
	}

	if (idx >= datalen)
	{
		len = 0;
		buff[0] = 0;
		return true;
	}

	wordlen = 0;

	// Search for the end of the word, which should stop at either
	// one of the white spaces or one of the 'delis'
	bool stop = false;
	while (idx < datalen)
	{
		c = data[idx];
		if (c <= 32 || c >= 127) break;
		if (c == '&') 
		{
			aos_assert_r(AosNextWordChar(data, datalen, c, idx), false);
		}

		// Check whether 'c' is one of the 'delis'. If yes, it needs
		// to stop the search.
		if (delis)
		{
			int ii = 0;
			while (ii < delis_len)
			{
				if (c == delis[ii])
				{
					stop = true;
					break;
				}
			}
		}

		if (stop) break;

		// It is a valid char for the word
		aos_assert_r(wordlen < maxlen, false);
		buff[wordlen++] = c;
		idx++;
	}

	len = idx - word_start;
	buff[wordlen] = 0;
	return true;
}
*/


/*
// It skips the white spaces (i.e., any characters whose 
// code is less than 32 (space). 
int AosSkipWhitespaces(const u8 * const data, 
		const int datalen, 
		int idx)
{
	// Chen Ding, 04/22/2010. Characters whose values are 
	// larger than 127 are Unicodes.
	// while (idx < datalen && (data[idx] <= 32 || data[idx] >= 127)) idx++;
	while (idx < datalen && data[idx] <= 32) idx++;
	return idx;
}


bool AosNextWordChar(
		char *data, 
		const int datalen,
		char &ch,
		int &idx)
{
	// The caller should have checked data[idx] == '&'
	// Skip special escape sequences:
	// 		&quot;     	return " (double quote)
	// 		&apos;      return ' (single quote)
	// 		&lt;        return '<'
	// 		&gt;        return '>'
	// 		&amp;       return '&' 
	// 		&#dd        Will convert it into a char
	// 		&#xXX       Will convert it into a char
	//
	// Upon return, 'idx' points right after the character it returns.
	if (data[idx] != '&') return data[idx++];
	
	switch (data[idx+1])
	{
	case 'q':
		 if (datalen - idx >= 6 &&
			 data[idx+2] == 'u' &&
			 data[idx+3] == 'o' &&
			 data[idx+4] == 't' &&
			 data[idx+5] == ';')
		 {
			 // It is '&quot;'. 
			 idx += 6;
			 ch = '"';
			 return true;
		 }
		 ch = data[idx++];
		 return true;

	case 'a':
		 if (datalen - idx >= 6 &&
			 data[idx+2] == 'p' &&
			 data[idx+3] == 'o' &&
			 data[idx+4] == 's' &&
			 data[idx+5] == ';')
		 {
			 // It is '&apos;' (single quote). 
			 idx += 6;
			 ch = '\'';
			 return true;
		 }

		 // Check '&amp;'
		 if (datalen - idx >= 5 &&
			 data[idx+2] == 'm' &&
			 data[idx+3] == 'p' &&
			 data[idx+4] == ';')
		 {
			 // It is '&amp;' (&). 
			 idx += 5;
			 ch = '&';
			 return true;
		 }
		 ch = data[idx++];
		 return true;

	case 'l':
		 if (datalen - idx >= 4 &&
			 data[idx+2] == 't' &&
			 data[idx+3] == ';')
		 {
			 // It is '<'
			 idx += 4;
			 ch = '<';
			 return true;
		 }
		 ch = data[idx++];
		 return true;

	case 'g':
		 if (datalen - idx >= 4 &&
			 data[idx+2] == 't' &&
			 data[idx+3] == ';')
		 {
			 // It is '>'
			 idx += 4;
			 ch = '>';
			 return true;
		 }
		 ch = data[idx++];
		 return true;

	case '#':
		 // It can be one of:
		 // 		&#dd;        Will convert it into a char
		 // 		&#xXX;       Will convert it into a char
		 if (datalen - idx >= 3)
		 {
			 char c = data[idx+2];
			 if (c == 'x')
			 {
				 // It should be the form '&#xXX;'
				 c = data[idx+3];
				 int code = 0;
				 if (c >= '0' && c <= '9')
				 {
					 code = '9' - c;
				 }
				 else if (c >= 'a' && c <= 'f')
				 {
					 code = 'f' - 'a' + 10;
				 }
				 else if (c >= 'A' && c <= 'F')
				 {
					 code = 'F' - 'A' + 10;
				 }
				 else
				 {
					 // Not a valid escape sequence
					 OmnAlarm << "Invalid escape sequence: " << data[idx] << enderr;
					 data[idx] = 'x';
					 ch = data[idx++];
					 return false;
				 }

				 c = data[idx+4];
				 if (datalen - idx >= 5 && c == ';')
				 {
					 // It is the case '&#xX;'
					 ch = (char)code;
					 idx += 5;
					 return true;
				 }

				 // Check the case '&#xXX;'
				 code *= 16;
				 if (c >= '0' && c <= '9')
				 {
					 code += '9' - c;
				 }
				 else if (c >= 'a' && c <= 'f')
				 {
					 code += 'f' - 'a' + 10;
				 }
				 else if (c >= 'A' && c <= 'F')
				 {
					 code += 'F' - 'A' + 10;
				 }
				 else
				 {
					 // Not a valid escape sequence
					 OmnAlarm << "Invalid escape sequence: " << &data[idx-10] << enderr;
					 data[idx] = 'x';
					 ch = data[idx++];
					 return true;
				 }

				 c = data[idx+5];
				 if (datalen - idx >= 6 && c == ';')
				 {
					 // It is the case '&#xXX;'
					 ch = (char)code;
					 idx += 6;
					 return true;
				 }

				 // Not a valid escape sequence
				 data[idx] = 'x';
				 ch = data[idx++];
				 OmnAlarm << "Invalid escape sequence: " << &data[idx-10] << enderr;
				 return false;
			 }

			 if (c >= '0' && c <= '9')
			 {
				 int code = '9' - c;
				 c = data[idx+3];
				 if (datalen - idx >= 4 && c == ';') 
				 {
					 // It is the case '&#d;'
					 idx += 4;
					 ch = (char)code;
					 return true;
				 }

				 if (datalen - idx < 4 || c < '0' || c > '9')
				 {
					 // It is not a valid XML escape sequence. 
					 OmnAlarm << "Incorrect xml escape: " << &data[idx] << enderr;
					 data[idx] = 'x';
					 ch = data[idx++];
					 return false;
				 }

				 code = code * 10 + ('9' - c);
				 c = data[idx+4];
				 if (datalen - idx >= 4 && c == ';')
				 {
					 // It is the case '&#dd;'
					 idx += 5;
					 ch = (char)code;
					 return true;
				 }

				 if (datalen - idx < 5 || c < '0' || c > '9')
				 {
					 // It is not a valid XML escape sequence. 
					 OmnAlarm << "Incorrect xml escape: " << &data[idx] << enderr;
					 data[idx] = 'x';
					 ch = data[idx++];
					 return false;
				 }

				 // Currently, we only allow maximum three digits
				 // which means that the next character must be ';'
				 code = code * 10 + ('9' - c);
				 c = data[idx+5];
				 if (datalen - idx >= 5 && c == ';')
				 {
					 // It is the case '&#ddd;'
					 idx += 6;
					 ch = (char)code;
					 return true;
				 }

				 // It is not a valid XML escape sequence. 
				 OmnAlarm << "Incorrect xml escape: " << &data[idx] << enderr;
				 data[idx] = 'x';
				 ch = data[idx++];
				 return false;
			 }
		 }
		 ch = data[idx++];
		 return true;

	default:
		 ch = data[idx++];
		 return true;
	}

	OmnShouldNeverComeHere;
	return false;
}
*/
#endif
