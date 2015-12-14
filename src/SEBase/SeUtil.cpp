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
// 	Created: 10/15/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEBase/SeUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Base64/Base64.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
// #include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"
// #include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "Util/StrSplit.h"
#include <openssl/md5.h>
#include <openssl/sha.h>


// Chen Ding, 11/24/2012
// Moved to XmlUtil/SeXmlUtil.cpp
/*
const char sgAttrMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 0 
};

// This map is used to determine the separator characters that 
// separate words. Below are the list:
// 		0-31:	
// 		32: 	space
//
// Type 0:
// These are not word separators.
//
// Type 1:
// These characters are considered white spaces, which
// means that they are word separators and they are not part of
// the words they separated. 
//
// Type 2:
// This type identify the characters that are word separators
// but they are not white spaces. After such character is encountered, 
// the character should be skipped. '!' is such an example. 
//
// Type 3:
// They are word separators and they may be part of the 
// words. Below are some of the examples:
// 		&lt;
// 		&gt;
// 		&amp;
// 		&#dd;
// 		&#xXX;
//
const char sgWordDeli[128] = 
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 	// 10: line feed, 
									// 11: vertical tab,
									// 12: form feed
									// 13: carriage return
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 2, 2, 0, 0, 0, 0, 2, 	// 32: space
									// 33: '!', 	type 2
									// 34: '"', 	type 2
									// 39: ', 		type 2
	2, 2, 0, 2, 2, 0, 2, 2, 0, 0, 	// 40: (, 		type 2
									// 41: ), 		type 2
									// 43: +,		type 2 (shall we?)
									// 44: , 		type 2
									// 46: .		type 2
									// 47: /		type 2
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 	// 58: :		type 2
									// 59: ;		type 2
	2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 	// 60: <		type 2
									// 61: = 		type 2
									// 62: >		type 2
									// 63: ?		type 2
									// 64: @		type 2
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 2, 2, 2, 2, 0, 2, 0, 0, 0, 	// 91: [		type 2
									// 92: \		type 2
									// 93: ]		type 2
									// 94: ^		type 2
									// 96: `		type 2
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 2, 2, 2, 2, 1 			// 123: {		type 2
									// 124: | 		type 2
									// 125: }		type 2
									// 126: ~		type 2
									// 127: 		type 1
};

const char sgDataidMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 0 
};



bool AosCheckDataid(const OmnString &dataid)
{
	const char *data = dataid.data();
	int len = dataid.length();
	aos_assert_r(len <= 8, false);
	for (int i=0; i<len; i++)
	{
		int c = data[i];
		aos_assert_r(c>32 && c<123 && sgDataidMap[c], false);
	}
	return true;
}


OmnString AosGetXmlAttr(
		const AosXmlTagPtr &cmd, 
		const AosXmlTagPtr &doc,
		const OmnString &attrname)
{
	bool exist;
	OmnString value;
	if (cmd) value = cmd->getAttrStr1(attrname, exist, "");
	if (value == "")
	{
		value = doc->getAttrStr1(attrname, exist, "");
	}
	
	if (value == "") return value;

	// The following characters are not allowed in attribute values:
	// 	Control characters (<32)
	// 	", %, &, ', <, >, 
	//
	// Chen Ding, 03/24/2010
	// Currently we will allow attribute values contain single quotes. 
	// But when parsing an XML, we need to be extremely careful that
	// if it uses a single quote, the value should not contain 
	// single quotes. If the attribute value uses double quotes, 
	// its values hould not contain double quotes. The XML parser 
	// currently enforces this.
	//
	// Allowed control characters (<32): '\n' (10), '\r' (12)
	char *data = (char *)value.data();
	int len = value.length();
	for (int i=0; i<len; i++)
	{
		int c = data[i];
		if (c>0 && c<127)
		{
			if (!sgAttrMap[c])
			{
				OmnAlarm << "Invalid char: " << (int)c << enderr;
				return "";
			}
		}
	}

	return value;
}


// This function retrieves the next word, starting from 'start_idx', 
// skipping the leading and trailing white spaces, terminating
// at one of 'delis' or the end of the data. 
//
// The retrieved word is stored in 'buff', whose maximum length
// is 'len'. If the word is too long than 'len', it is an error.
//
// Upon return, 'len' holds the length of the word. The null 
// character is added into 'buff' upon return.
bool AosNextXmlWord(
		char *data, 
		const char *delis,
		const int delis_len,
		const int start_idx, 
		const int datalen,
		char *buff,
		u32 &len, 
		int &wordlen)
{
	// Removing the white spaces
	int c;
	int idx = start_idx;
	const int maxlen = len;
	len = 0;
	while (idx < datalen)
	{
		c = data[idx];
		//james
		if (c > 32 || c < 0) break;
		idx++;
	}

	if (idx >= datalen)
	{
		len = 0;
		buff[0] = 0;
		wordlen = 0;
		return true;
	}

	wordlen = 0;

	// Search for the end of the word, which should stop at either
	// one of the white spaces or one of the 'delis'
	bool stop = false;
	while (idx < datalen)
	{
		c = data[idx];
		//james
		if ((c <= 32 && c > 0) || c >= 127) break;
		// if (c == '&') 
		// {
		// 	aos_assert_r(AosNextXmlWordChar(data, datalen, c, idx), false);
		// }

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

		// Process the separators

		if (c >= 0)
		{
			int tmplen;
			int tmpidx;
			switch (sgWordDeli[c])
			{
			case 0: 
				 break;

			case 1:
				 // It is a separator and the separator is a white space.
				 stop = true;
				 break;

			case 2:
				 // It is a separator and the separator is not a white space
				 // and it is not part of the word.
				 stop = true;
				 idx++;
				 break;

			case 3:
				 // It is ';'. Check whether it is one of:
				 // 		&quot; 
				 // 		&apos;
				 // 		&lt; 
				 // 		&gt;
				 // 		&amp; 
				 // 		&#dd; 
				 // 		&#xXX; 
				 tmplen = 1;
				 tmpidx = idx-1;
				 while (tmplen < 7 && tmpidx >= start_idx && data[tmpidx] != '&')
				 {
					 tmplen++;
					 tmpidx--;
				 }

				 if (data[tmpidx] != '&')
				 {
					 // It is not one of the XML escape sequence. It is treated
					 // as Type 2.
					 idx++;
					 stop = true;
					 break;
				 }

				 // check &quot;
				 if (tmplen == 6 &&
					 data[tmpidx+1] == 'q' &&
					 data[tmpidx+2] == 'u' &&
					 data[tmpidx+3] == 'o' &&
					 data[tmpidx+4] == 't')
				 {
					 // It is '&quot;'. 
					 buff[wordlen++] = ';';
					 idx++;
					 stop = true;
					 break;
				 }

				 // check &apos; 
				 if (tmplen == 6 &&
					 data[tmpidx+1] == 'a' &&
					 data[tmpidx+2] == 'p' &&
					 data[tmpidx+3] == 'o' &&
					 data[tmpidx+4] == 's')
				 {
					 // It is '&apos;'. 
					 buff[wordlen++] = ';';
					 idx++;
					 stop = true;
					 break;
			 	}

			 	// check &lt;  
			 	if (tmplen == 4 &&
					 data[tmpidx+1] == 'l' &&
				 	data[tmpidx+2] == 't')
			 	{
					 // It is '&lt;'. 
					 buff[wordlen++] = ';';
					 idx++;
					 stop = true;
					 break;
				 }

				 // check &gt; 
				 if (tmplen == 4 &&
					 data[tmpidx+1] == 'g' &&
					 data[tmpidx+2] == 't')
				 {
					 // It is '&gt;'. 
					 buff[wordlen++] = ';';
					 idx++;
					 stop = true;
					 break;
				 }

				 // check &amp;
				 if (tmplen == 5 &&
					 data[tmpidx+1] == 'a' &&
					 data[tmpidx+2] == 'm' &&
					 data[tmpidx+3] == 'p')
				 {
					 // It is '&amp;'. 
					 buff[wordlen++] = ';';
					 idx++;
					 stop = true;
					 break;
				 }

				 // check &#dd;
				 if (data[tmpidx+1] != '#')
				 {
					 // It is not an XML escape sequence. Treat it as Type 2.
					 idx++;
					 stop = true;
					 break;
				 }

				 if (data[tmpidx+2] == 'x')
				 {
					 // It should be '&#xXX;'
					 char cc;
					 bool isValid = true;
					 for (int i=tmpidx+3; i<tmplen-1; i++)
					 {
						cc = data[i];
						if (!((cc >= '0' && cc <= '9') ||
							 (cc >= 'a' && cc <= 'f') ||
							 (cc >= 'A' && cc <= 'F')))
						{
							// Not a valid digit. 
							isValid = false;
							break;
						}
					 }

					 if (isValid)
					 {
						 buff[wordlen++] = ';';
					 }
					 idx++;
					 stop = true;
					 break;
				 }
				 
				 if (data[tmpidx+2] >= '0' && data[tmpidx+2] <= '9')
				 {
					 // It should be '&#dd;'
					 char cc;
					 bool isValid = true;
					 for (int i=tmpidx+3; i<tmplen-1; i++)
					 {
						cc = data[i];
						if (!(cc >= '0' && cc <= '9'))
						{
							// Not a valid digit. 
							isValid = false;
							break;
						}
					 }

					 if (isValid)
					 {
						 buff[wordlen++] = ';';
					 }
					 idx++;
					 stop = true;
					 break;
				 }

				 // Not a valid XML escape sequence
				 idx++;
				 stop = true;
				 break;

			default:
				 OmnAlarm << "Unrecognized separator type: " << sgWordDeli[c] << enderr;
				 break;
			}
		}

		if (stop) break;

		// It is a valid char for the word
		// aos_assert_r(wordlen < maxlen, false);
		if (wordlen >= maxlen)
		{
			// OmnAlarm << wordlen << "<" << maxlen << ":" << &data[start_idx]
			// 	<< ":" << idx << enderr;
			return false;
		}

		buff[wordlen++] = c;
		idx++;
	}

	len = idx - start_idx;
	buff[wordlen] = 0;
	return true;
}


bool AosNextXmlWordChar(
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


OmnString AosSeUtil_createNewObjid(
		const AosXmlTagPtr &objtemp, 
		const OmnString &dft, 
		bool &changed)
{
	// This function creates a new objid based on the template 
	// 'objtemp', which is in the following format: 
	//		<template>
	//			<entry type="1|2|...">xxx</entry>
	//			<entry type="1|2|...">xxx</entry>
	//			...
	//		</template>
	aos_assert_r(objtemp, dft);
	
	changed = false;
	OmnString objid;
	AosXmlTagPtr entry = objtemp->getFirstChild();
	while (entry)
	{
		OmnString type = entry->getAttrStr("type");
		if (type == AOSOBJTEMPTYPE_CONSTANT)
		{
			objid << entry->getNodeText();
		}
		else if (type == AOSOBJTEMPTYPE_SEQNO)
		{
			// It is a sequence number
			OmnString seqno = entry->getNodeText();
			objid << seqno;
			seqno.increment();
			entry->setText(seqno, false);
			changed = true;
		}
		else if (type == AOSOBJTEMPTYPE_YEAR)
		{
			objid << AosGetSystemYear();
		}
		else if (type == AOSOBJTEMPTYPE_MONTH)
		{
			objid << AosGetSystemMonth();
		}
		else if (type == AOSOBJTEMPTYPE_DAYOFMONTH)
		{
			objid << AosGetSystemDayOfMonth();
		}
		else
		{
			objid << "Unknown";
			OmnAlarm << "Unknown type: " << type << ". Data: " 
				<< objtemp->getData() << enderr;
			return "";
		}

		entry = objtemp->getNextChild();
	}

	return objid;
}


u32 AosGetEpochForm1(const AosXmlTagPtr &def)
{
	// It converts string time to Epoch time. It assumes 'def'
	// is in the form:
	// 	<time AOSTAG_YEAR="xxx"
	// 		  AOSTAG_MONTH="xxx"
	// 		  AOSTAG_DAY="xxx"
	// 		  AOSTAG_HOUR="xxx"
	// 		  AOSTAG_MINUTE="xxx"
	// 		  AOSTAG_SECOND="xxx"/>
	// If AOSTAG_YEAR is missing, current year is assumed.
	// If AOSTAG_MONTH is missing, current month is assumed.
	// If AOSTAG_DAY is missing, current day is assumed.
	// If AOSTAG_HOUR is missing, 0 is assumed and subsequent tags are ignored.
	// If AOSTAG_MINUTE is missing, 0 is assumed and subsequent tags are ignored.
	// If AOSTAG_SECOND is missing, 0 is assumed.
	// After retrieved all the values, call:
	// inline int AosGetEpoch(...)
	OmnNotImplementedYet;
	return 0;
}


u32 AosGetEpochForm2(const OmnString &def)
{
	// It converts string time to Epoch time. It assumes 'def'
	// is in the form:
	// 	year="xxx",month="xxx",day="xxx",hour="xxx",min="xxx",sec="xxx"
	// If year is missing, current year is assumed.
	// If month is missing, current month is assumed.
	// If day is missing, current day is assumed.
	// If hour is missing, 0 is assumed and subsequent tags are ignored.
	// If min is missing, 0 is assumed and subsequent tags are ignored.
	// If sec is missing, 0 is assumed.
	// After retrieved all the values, call:
	// inline int AosGetEpoch(...)
	OmnNotImplementedYet;
	return 0;
}


OmnString
AosParseArgs(const OmnString &args, const OmnString &name)
{
	// It expects only one parm whose name is 'name'
	OmnStrParser1 parser(args, ",");
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		AosStrSplit split;
		bool finished;
		OmnString pair[2];
		//split.splitStr(args.data(), "=", pair, 2, finished);
		//aos_assert_r(pair[0] == name, false);
		split.splitStr(word.data(), "=", pair, 2, finished);
		if(pair[0] != name)	continue;	// Ketty 2012/05/22
		pair[1].replace("#61", "=", true);
		return pair[1];
	}
	return "";
}


bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name, 
		OmnString &value)
{
	// It expects only one parm whose name is 'name'
	AosStrSplit split;
	bool finished;
	OmnString pair[2];
	split.splitStr(args.data(), "=", pair, 2, finished);
	aos_assert_r(pair[0] == name, false);
	value = pair[1];

	// Chen Ding, 2011/02/10
	value.replace("#61", "=", true);
	return true;
}

bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2)
{
	// Args are in the form:
	// 	name:value,name:value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	return true;
}

bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3)
{
	// Args are in the form:
	// 	name=value,name=value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
			if (pair[0] == name3) { value3 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	value3.replace("#61", "=", true);
	return true;
}	


bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4)
{
	// Args are in the form:
	// 	name=value,name=value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
			if (pair[0] == name3) { value3 = pair[1]; continue; }
			if (pair[0] == name4) { value4 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	value3.replace("#61", "=", true);
	value4.replace("#61", "=", true);
	return true;
}	


bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5)
{
	// Args are in the form:
	//  name=value,name=value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
			if (pair[0] == name3) { value3 = pair[1]; continue; }
			if (pair[0] == name4) { value4 = pair[1]; continue; }
			if (pair[0] == name5) { value5 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	value3.replace("#61", "=", true);
	value4.replace("#61", "=", true);
	value5.replace("#61", "=", true);
	return true;
}


bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5,
		const OmnString &name6, OmnString &value6,
		const OmnString &name7, OmnString &value7)
{
	// Args are in the form:
	//  name=value,name=value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
			if (pair[0] == name3) { value3 = pair[1]; continue; }
			if (pair[0] == name4) { value4 = pair[1]; continue; }
			if (pair[0] == name5) { value5 = pair[1]; continue; }
			if (pair[0] == name6) { value6 = pair[1]; continue; }
			if (pair[0] == name7) { value7 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	value3.replace("#61", "=", true);
	value4.replace("#61", "=", true);
	value5.replace("#61", "=", true);
	value6.replace("#61", "=", true);
	value7.replace("#61", "=", true);
	return true;
}


bool
AosParseArgs(
		const OmnString &args,
		const OmnString &name1, OmnString &value1,
		const OmnString &name2, OmnString &value2,
		const OmnString &name3, OmnString &value3,
		const OmnString &name4, OmnString &value4,
		const OmnString &name5, OmnString &value5,
		const OmnString &name6, OmnString &value6)
{
	// Args are in the form:
	//  name=value,name=value,...
	OmnString parts[eAosMaxArgs];
	AosStrSplit split;
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, eAosMaxArgs, finished);
	OmnString pair[2];
	for (int i=0; i<nn; i++)
	{
		int mm = split.splitStr(parts[i].data(), "=", pair, 2, finished);
		if (mm != 2)
		{
			OmnAlarm << "Args are incorrect: " << args << enderr;
		}
		else
		{
			if (pair[0] == name1) { value1 = pair[1]; continue; }
			if (pair[0] == name2) { value2 = pair[1]; continue; }
			if (pair[0] == name3) { value3 = pair[1]; continue; }
			if (pair[0] == name4) { value4 = pair[1]; continue; }
			if (pair[0] == name5) { value5 = pair[1]; continue; }
			if (pair[0] == name6) { value6 = pair[1]; continue; }
		}
	}

	// Chen Ding, 2011/02/10
	value1.replace("#61", "=", true);
	value2.replace("#61", "=", true);
	value3.replace("#61", "=", true);
	value4.replace("#61", "=", true);
	value5.replace("#61", "=", true);
	value6.replace("#61", "=", true);
	return true;
}


OmnString
AosGetVersionUID()
{
	//Zky3233, Linda, 2011/04/18
	//Verification Code format: second+ millisecond
	u64 s = OmnGetSecond();
	u64 ms = OmnGetUsec();
	OmnString str;
	str << s <<"_"<< ms;
	return str;
}

OmnString
AosMD5Encrypt(const OmnString &str)
{
	//Zky3233, Linda, 2011/04/18
	//MD5 Encrypt
	if (str == "") return "";
	OmnString id;
	const char *data = str.data();
	unsigned char md[16];
	char tmp[3]={'\0'};
	//felicia,2013/09/13
	//MD5((unsigned char*)data, strlen(data),md);
	MD5((unsigned char*)data, str.length(),md);
	for (int i=0; i<16; i++)
	{
		sprintf(tmp,"%2.2x",md[i]);
		id << tmp;
	}
	return id;
}


OmnString AosCalSHA1(const OmnString &value)
{
	// This function signs the value 'value' with the system secret key. 
	// In the current implementations, the system secret key is hard coded.
	// In the future, it should be generated dynamically.
	static const int lsSh1Length = 20;
	static const int lsBase64Sh1Length = 30;

	if (value == "") return "";
	const char *data = value.data();
	int size = value.length();
	unsigned char hash[lsSh1Length + 10];
	SHA_CTX s;

	SHA1_Init(&s);
	SHA1_Update(&s, data, size);
	SHA1_Final(hash, &s);

	char sh1_base64[lsBase64Sh1Length + 10];
	int len = EncodeBase64((unsigned  char*)hash, (unsigned char*)sh1_base64, 
			lsSh1Length, lsBase64Sh1Length);
	sh1_base64[len] = 0;
	return sh1_base64;
};


//ken 2011/12/07
bool
AosInlineSearchSplit(
		const OmnString &value,
		const OmnString &splittype,
		set<OmnString> &str)
{
	str.clear();
	int len = value.length();
	if(splittype == "prefix_match")
	{
		for(int i=1; i<=len; i++)
		{
			OmnString s(&value.data()[0], i);
			str.insert(s);
		}
	}
	else
	{
		for(int i=1; i<=len; i++)
		{
			for(int j=0; j<len; j++)
			{
				if(i+j <= len)
				{
					OmnString s(&value.data()[j], i);
					str.insert(s);
				}
			}
		}
	}
	return true;
}
