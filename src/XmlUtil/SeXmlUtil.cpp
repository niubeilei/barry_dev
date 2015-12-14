////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/SeXmlUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/ValueDefs.h"
#include "SEBase/SecUtil.h"

AosDocMetaAttr AosXmlUtil::smMetaAttrs[eMaxMetaAttrs];
int AosXmlUtil::smNumMetaAttrs = 0;

// Chen Ding, 11/24/2012
// Moved from SEUtil/SeUtil.cpp
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



bool
AosXmlUtil::signAttr(
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const OmnString &sign_name, 
		const AosRundataPtr &rdata)
{
	// This function signs the attribute 'aname' with a system secret key. 
	// In the current implementations, the secret key is hard coded in 
	// the program. In the future, it should be reliably generated randomly.
	//
	// The signature is SH1(aname, secret_key). The results are then converted
	// through base64 and saved in 'sign_name'. 
	aos_assert_rr(doc, rdata, false);
	aos_assert_r(aname != "", false);
	OmnString value = doc->getAttrStr(aname);
	if (value == "") return false;
	OmnString signature = AosSecUtil::signValue(value);
	aos_assert_r(signature != "", false);
	doc->setAttr(sign_name, signature);
	return true;
}


bool 
AosXmlUtil::verifySignedAttr(
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const OmnString &sign_name, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the value of 'aname'. It is empty, it is an error.
	// Otherwise, it retrieves the value's signature from 'sign_name'. 
	// It then uses the system secret key to hash the value and then compare
	// the signature. If they are the same, the value is returned. Otherwise, 
	// it is an error.
	aos_assert_rr(doc, rdata, false);

	// Note that if the doc is not from clients, which means that it was not retrieved
	// from outside and returned back, it will not do the verification (since 
	// it may not have the signature at all).
	if (!doc->isFromClient()) return true;

	aos_assert_rr(aname != "", rdata, false);
	aos_assert_rr(sign_name != "", rdata, false);

	OmnString value = doc->getAttrStr(aname);
	if (value == "") return false;

	OmnString signature = doc->getAttrStr(sign_name);
	if (signature == "") return false;

	OmnString results = AosSecUtil::signValue(value);
	return (signature == results);
}


bool
AosXmlUtil::verifySignedAttr(
		const AosXmlTagPtr &doc,
		OmnString &fields,
		const OmnString &aname,
		const OmnString &sign_name, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(doc, rdata, false);
	if (!doc->isFromClient()) return true;

	aos_assert_rr(aname != "", rdata, false);
	fields = doc->getAttrStr(aname);
	if (fields == "") return true;
	OmnString signature = doc->getAttrStr(sign_name);
	aos_assert_rr(signature != "", rdata, false);
	OmnString aa = AosSecUtil::signValue(fields);
	return (aa == signature);
}


bool 
AosXmlUtil::verifySysAcct(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);

	OmnString signature = doc->getAttrStr(AOSTAG_SYSACCT_SIGNATURE);
	if (signature == "") return false;

	OmnString results = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
	return (signature == results);
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


// Chen Ding, 11/24/2012
// Moved from SEUtil/SeXmlUtil.cpp
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


bool
AosXmlUtil::removeMetaAttrs(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	for (int i=0; i<smNumMetaAttrs; i++)
	{
		doc->removeAttr(smMetaAttrs[i].attrname);
	}
	return true;
}
void
AosXmlUtil::setMetaAttrs()
{
	smNumMetaAttrs = 0;
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_CREATOR, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_OBJID, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_OTYPE, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_PARENTC, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_MODUSER, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_CTIME, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_CT_EPOCH, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_MTIME, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_MT_EPOCH, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_SUBTYPE, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERCM, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERDW, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERLK, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERRC, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERWR, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERUP, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_COUNTERRD, false, true);
	smMetaAttrs[smNumMetaAttrs++].set(AOSTAG_DOCID, false, true);
	if (smNumMetaAttrs > eMaxMetaAttrs)
	{
		OmnAlarm << "Too many meta attributes. Please fix the problem!"
			<< enderr;
		exit(-1);
	}
}
const u64 sgDocLocationMask = 0x00ffffffffffffffULL;
u64
AosXmlUtil::getOwnDocid(const u64 &docid)
{
 	u64 did = docid & sgDocLocationMask;
 	return did;
}                                       
