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
// 09/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/SeXmlParser.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Debug/Rslt.h"
#include "SEUtil/DocTags.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XpUtil.h"
#include "XmlUtil/XmlTag.h"

// 0. NUL null  	8. BS backspace  	16. DLE data link escape  	24. CAN cancel
// 1. SOH  			9. HT hori tab      17. DC1 device control 1 	25.	EM 	end of medium
// 2. STX 			10. LF line feed 	18. DC2 device control 2 	26. SUB substitute
// 3. ETX  			11. VT vertical tab	19. DC3 device control 3 	27. ESC escape
// 4. EOT end of tr	12. FF 	form feed 	20. DC4 device control 4 	28.	FS 	file separator
// 5. ENQ enquiry 	13. CR 	carri ret 	21. NAK negative ack		29. GS 	group separator
// 6. ACK ack 		14. SO 	shift out 	22. SYN synch idle 			30. RS 	record separator
// 7. BEL bell 		15.	SI 	shift in 	23. ETB end of trans block 	31. US 	unit separator
//
// 127. 	DEL 	delete
//
// 32.  	48.  	0  	64.  	@  	80.  	P  	96.  	`  	112.  	p  	   	   	   	 
// 33. 	! 	49. 	1 	65. 	A 	81. 	Q 	97. 	a 	113. 	q 	  	  	  	 
// 34. 	" 	50. 	2 	66. 	B 	82. 	R 	98. 	b 	114. 	r 	  	  	  	 
// 35. 	# 	51. 	3 	67. 	C 	83. 	S 	99. 	c 	115. 	s 	  	  	  	 
// 36. 	$ 	52. 	4 	68. 	D 	84. 	T 	100. 	d 	116. 	t 	  	  	  	 
// 37. 	% 	53. 	5 	69. 	E 	85. 	U 	101. 	e 	117. 	u 	  	  	  	 
// 38. 	& 	54. 	6 	70. 	F 	86. 	V 	102. 	f 	118. 	v 	  	  	  	 
// 39. 	' 	55. 	7 	71. 	G 	87. 	W 	103. 	g 	119. 	w 	  	  	  	 
// 40. 	( 	56. 	8 	72. 	H 	88. 	X 	104. 	h 	120. 	x 	  	  	  	 
// 41. 	) 	57. 	9 	73. 	I 	89. 	Y 	105. 	i 	121. 	y 	  	  	  	 
// 42. 	* 	58. 	: 	74. 	J 	90. 	Z 	106. 	j 	122. 	z 	  	  	  	 
// 43. 	+ 	59. 	; 	75. 	K 	91. 	[ 	107. 	k 	123. 	{ 	  	  	  	 
// 44. 	, 	60. 	< 	76. 	L 	92. 	\ 	108. 	l 	124. 	| 	  	  	  	 
// 45. 	- 	61. 	= 	77. 	M 	93. 	] 	109. 	m 	125. 	} 	  	  	  	 
// 46. 	. 	62. 	> 	78. 	N 	94. 	^ 	110. 	n 	126. 	~ 	  	  	  	 
// 47. 	/ 	63. 	? 	79. 	O 	95. 	_ 	111. 	o 	127. 	   	

static u8 	sgAosFirstWordChars[256] = {1};
static bool	sgParserInited = false;



AosXmlParser::AosXmlParser()
:
mPartialParsing(false)
{
	init();
}


AosXmlParser::~AosXmlParser()
{
	mBuff = 0;
	mRoot = 0;
	mCrtTag = 0;
}


void
AosXmlParser::init()
{
	if (sgParserInited) return;
	sgParserInited = true;

	memset(sgAosFirstWordChars, 0, 31);				// Control chars
	memset(&sgAosFirstWordChars[128], 1, 128);		// Non ASCII chars
	memset(&sgAosFirstWordChars[33], 1, 127-33);	// Non ASCII chars

	sgAosFirstWordChars[32] = 0;	// Space
	sgAosFirstWordChars[33] = 0;	// '!' 
	sgAosFirstWordChars[34] = 0;	// '"' 
	sgAosFirstWordChars[39] = 0;	// ''' 
	//sgAosFirstWordChars[40] = 0;	// '(' 
	//sgAosFirstWordChars[41] = 0;	// ')' 
	sgAosFirstWordChars[44] = 0;	// comma 
	sgAosFirstWordChars[46] = 0;	// '.' 
	sgAosFirstWordChars[47] = 0;	// '/' 
	sgAosFirstWordChars[60] = 0;	// '<' 
	sgAosFirstWordChars[62] = 0;	// '>' 
	sgAosFirstWordChars[92] = 0;	// '\' 
	sgAosFirstWordChars[96] = 0;	// '`' 
	sgAosFirstWordChars[127] = 0;
}


// The function parses the XML document. The 
// document is stored in 'data'. If 'expected' is not empty,
// the parsing will stop when the expected is found.
//
// If it encounters errors, it will report the errors and 
// abort the processing. If successful, it returns true.
AosXmlTagPtr 
AosXmlParser::parse(
		const OmnConnBuffPtr &buff,
		const OmnString &expected AosMemoryCheckDecl)
{
	mBuff = buff;
	aos_assert_r(mBuff, 0);
	AosXmlTagPtr xml = parsePrivate(expected AosMemoryCheckerFileLine);
	aos_assert_r(xml, 0);
	return xml;
}


AosXmlTagPtr
AosXmlParser::parse(
		const char *data,
		const int len,
		const OmnString &expected AosMemoryCheckDecl)
{
	try
	{
		mBuff = OmnNew OmnConnBuff(data, len, false);
	}
	catch(...)
	{
		OmnAlarm << "Failed creating the buff (possibly too long: " << len << enderr;
		return 0;
	}

	AosXmlTagPtr xml = parsePrivate(expected AosMemoryCheckerFileLine);
	aos_assert_r(xml, 0);
	return xml;
}


AosXmlTagPtr
AosXmlParser::parse(
		const OmnString &data,
		const OmnString &expected AosMemoryCheckDecl)
{
	try
	{
		mBuff = OmnNew OmnConnBuff(data.data(), data.length(), false);
	}
	catch(...)
	{
		OmnAlarm << "Failed creating data: " << data.length() << enderr;
		return 0;
	}
	AosXmlTagPtr xml = parsePrivate(expected AosMemoryCheckerFileLine);
	aos_assert_r(xml, 0);
	return xml;
}


AosXmlTagPtr
AosXmlParser::parsePrivate(const OmnString &expected AosMemoryCheckDecl)
{
	mExpectValue = false;
	if (expected.length() > 0) procExpected(expected);

	int crtIdx = 0;
	Status status = eLookForOpenTag;

	mRoot = OmnNew AosXmlTag(0, 0, mBuff, 0, 0, 0 AosMemoryCheckerFileLine);
	aos_assert_r(mRoot, 0);
	mRoot->setRootFlag();
	mCrtTag = mRoot;

	aos_assert_r(mBuff, NULL);
	aos_assert_r(prepareData(), NULL);
	int datalen = mBuff->getDataLength();
	u8 *buffData = (u8 *)mBuff->getData();

	bool rslt;
	while (crtIdx < datalen && buffData[crtIdx])
	{
		int pre_crtIdx = crtIdx;
		crtIdx = AosSkipWhitespaces(buffData, datalen, crtIdx);
		if (crtIdx >= datalen) break;

		switch (status)
		{
		case eLookForOpenTag:
			 // Looking for an open tag. The state may transit to
			 // one of the following:
			 // 1. eProcTagAttrs
			 //    The tag was found but not its '>'. This is the place
			 //    where tag attributes should be processed.
			 // 2. eProcTagBody
			 //    The whole tag is found. The tag body and its closing
			 //    tag is expected. 
			 // 3. eLookForOpenTag
			 //    This is the case '<tagname/>'. 
			 crtIdx = AosSkipWhitespaces(buffData, datalen, crtIdx);

			 // If the next character is not '<', it should be tag
			 // body. If it is '<', it can be either an open tag or
			 // the close tag of the current tag. 
			 if (buffData[crtIdx] != '<')
			 {
				 rslt = procTagBody(crtIdx, status);
				 aos_assert_r(rslt, 0);
				 break;
			 }

			 // Check whether it is '<![CDATA['
			 if (crtIdx + 9 < datalen && 
				 strncmp((char *)&buffData[crtIdx], "<![CDATA[", 9) == 0)
			 {
				 rslt = procTagBody(crtIdx, status);
				 aos_assert_r(rslt, 0);
				 break;
			 }
			 // Linda
			 // Check whether it is '<![BDATA['
			 if (crtIdx + 9 < datalen &&
				 strncmp((char *)&buffData[crtIdx], "<![BDATA[", 9) == 0)
			 {
				 rslt = procTagBody(crtIdx, status);
				 aos_assert_r(rslt, 0);
				 break;
			 }

			 aos_assert_r(crtIdx <= datalen-4, 0);
			 rslt = procOpenTag(crtIdx, status);
			 aos_assert_r(rslt, 0);
			 break;

		case eProcTagAttrs:
			 // An open tag found, but '>' has not found yet. Attributes
			 // are allowed here. The state may transit to eProcTagBody
			 // or eLookForOpenTag.
			 rslt = procTagAttrs(crtIdx, status); 
			 aos_assert_r(rslt, 0);
			 break;

		case eProcTagBody:
			 // An open tag has been found and the tag should have 
			 // a body. A tag body should be in the form:
			 // <tag ...>
			 //		text ....
			 //		<subtag .../>
			 //		<subtag .../>
			 // </tag>
			 // Note that 'text' may appear anywhere (we relax the syntax).
			 // It is possible to have multiple segments of texts, 
			 // interplaced with subtags. 
			 crtIdx = pre_crtIdx;
			 rslt = procTagBody(crtIdx, status);
			 aos_assert_r(rslt, 0);
			 break;

		default:
			 OmnAlarm << "Unrecognized status: " << status << enderr;
			 return 0;
		}
		
		datalen = mBuff->getDataLength();

		if (mPartialParsing && mCrtTag == mRoot) break;
	}

	if (mCrtTag != mRoot)
	{
		OmnAlarm << "Failed to parse: " << buffData << enderr;
		return 0;
	}

	if (mRoot->getRootNumSubtags() > 1)
	{
		OmnAlarm << "More than one root node is found: " << enderr;
		OmnScreen << (const char *)buffData << endl;
		return 0;
	}

	if (crtIdx < datalen)
	{
		OmnAlarm << "Found null in the data. Data length: " << datalen
			<< ". Crt idx: " << crtIdx << enderr;
		OmnScreen << buffData << endl;
	}

	AosXmlTagPtr child = mRoot->getFirstChild();
	if (!child || child->getTagEnd() < 0)
	{
		OmnAlarm << "Ill formulated XML doc: "
			<< (char *)buffData << enderr;
		return 0;
	}

	mRoot->setTagEnd(child->getTagEnd());
	// rslt = mRoot->unescapeCdataContents();
	rslt = mRoot->normalize();
	aos_assert_r(rslt, 0);	
	return mRoot;
}


bool
AosXmlParser::procOpenTag(
		int &crtIdx, 
		Status &status)
{
	// The current position should point right at the beginning 
	// of a tag (i.e., '<'). 
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
	unsigned char c = buffData[crtIdx];
	aos_assert_r(c == '<', false);
	int tagStartIdx = crtIdx++;

	crtIdx = AosSkipWhitespaces(buffData, datalen, crtIdx);
	c = buffData[crtIdx];

	if (c == '/')
	{
		// It encountered a closing tag. Check whether
		// it is allowed.
		aos_assert_r(mCrtTag != mRoot, false);
		crtIdx++;
		aos_assert_r(procClosingTag(crtIdx, true), false);
		status = eLookForOpenTag;
		return true;
	}

	// Look for the first tag character. 
	if (c <= 127 && !sgAosFirstWordChars[c])
	{
		if (crtIdx > 10)
		{
			OmnAlarm << "Invalid first char for tag: "
				<< &buffData[crtIdx-1] << enderr;
		}
		else
		{
			OmnAlarm << "Invalid first char for tag: "
				<< &buffData[crtIdx-1] << enderr;
		}
		return false;
		// Chen Ding, 09/30/2010
		// the first valid first character.
		// crtIdx++;
		// while (crtIdx < datalen && !sgAosFirstWordChars[buffData[crtIdx]]) crtIdx++;
	}

	// It should be an open tag
	int tagnamestart, tagnamelen;
	crtIdx = AosNextEnWord(buffData, sgAosFirstWordChars, 
		0, 0, crtIdx, datalen, tagnamestart, tagnamelen);

	// Check whether the next character is '/>' or '>'. 
	AosSkipWhitespaces(buffData, datalen, crtIdx);
	aos_assert_r(crtIdx < datalen, false);
	c = buffData[crtIdx];
	switch (c)
	{
	case '>':
	 	 addTag(tagStartIdx, 
			tagnamestart, tagnamestart+tagnamelen-1);
		 status = eProcTagBody;
		 crtIdx++;
		 return true;

	case '/':
		 // The next char should be '>'
		 if (crtIdx+1 < datalen && buffData[crtIdx+1] == '>')
		 {
	 	 	 addTag(tagStartIdx, tagnamestart, tagnamestart+tagnamelen-1);
			 aos_assert_r(popTag(crtIdx+1, 0, 0, false), false);
			 status = eLookForOpenTag;
			 crtIdx += 2;
			 return true;
		 }
		 else   
		 {
			 // This is an error. 
			 OmnAlarm << "Failed parsing: " << crtIdx
				 << ". " << buffData << enderr;
			 return false;
		 }
		 break;

	default:
		 // Found an opening tag that has attributes.
	 	 addTag(tagStartIdx, tagnamestart, tagnamestart+tagnamelen-1);
		 aos_assert_r(procTagAttrs(crtIdx, status), false); 
		 return true;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosXmlParser::procClosingTag(
		int &idx, 
		const bool flag) 
{
	// The current position should point right after '/'. 
	// If 'flag' is true, it means the current one is 
	// in the form: </xxx>. Otherwise, it is in the form
	// <xxx/>. 
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
	idx = AosSkipWhitespaces(buffData, datalen, idx);

	// Looking for '>'
	int tagstart = idx;
	while (idx < datalen && buffData[idx] != '>') idx++;
	aos_assert_r(idx <= datalen, false);
	if (!popTag(idx, tagstart, idx-1, flag))
	{
		OmnAlarm << "Failed to parse: " << buffData << enderr;
		return false;
	}

	// Found a closing tag
	idx++;
	return true;
}


bool
AosXmlParser::procTagBody(
		int &idx,
		Status &state)
{
	// An open tag has been found and the tag should have 
	// a body. A tag body should be in the form:
	// <tag ...>
	//		<![CDATA[		// Optional
	//		text ....
	//		<subtag .../>
	//		<subtag .../>
	//		]]>
	// </tag>
	// Note that 'text' may appear anywhere (we relax the syntax).
	// It is possible to have multiple segments of texts, 
	// interplaced with subtags. 
	// Unless 'text' is escaped by '<!CDATA[', text should not 
	// contain '<'. Otherwise, it is interpreted as a tag. 

	unsigned char c;
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
//	idx = AosSkipWhitespaces(buffData, datalen, idx);
	while (idx < datalen)
	{
		c = buffData[idx++];
		if (c == '<')
		{
			// Check whether it is '<![CDATA[' (case sensitive)
			if (idx + 7 < datalen && buffData[idx] == '!' &&
				buffData[idx+1] == '[' && buffData[idx+2] == 'C' && 
				buffData[idx+3] == 'D' && buffData[idx+4] == 'A' && 
				buffData[idx+5] == 'T' && buffData[idx+6] == 'A' && 
				buffData[idx+7] == '[')
			{
				idx += 8;
				aos_assert_r(procCdata(idx), false);
				continue;
			}

			// Linda
			// Check whether it is '<![BDATA[' (case sensitive)
			if (idx + 7 < datalen && buffData[idx] == '!' &&
				buffData[idx+1] == '[' && buffData[idx+2] == 'B' &&
				buffData[idx+3] == 'D' && buffData[idx+4] == 'A' &&
				buffData[idx+5] == 'T' && buffData[idx+6] == 'A' &&
				buffData[idx+7] == '[')
			{
				idx += 8;
				aos_assert_r(procBdata(idx), false);
				continue;
			}

			// Check whether it is an openining or close tag
			idx = AosSkipWhitespaces(buffData, datalen, idx);
			if (buffData[idx] == '/')
			{
				// It is a closing tag. It should close the current tag.
				idx++;
				aos_assert_r(procClosingTag(idx, true), false);
				return true;
			}

			// It must be an open tag. Regardless of whether 'procOpenTag(...)'
			// successful or not, 'idx' will move at least one char. 
			idx--;
			state = eLookForOpenTag;
			return true;	
		}	
		else
		{
			// It is 'text'. 
			idx--;
			bool rslt = procTagText(idx);
			aos_assert_r(rslt, false);
		}
	}

	OmnScreen << "Data: " << mBuff->getData() << endl;
	OmnShouldNeverComeHere;
	return false;
}


bool
AosXmlParser::procTagText(int &idx) 
{
	// Any string is treated as strings unless it sees a '<'. 
	int startidx = idx;
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
	while (idx < datalen && buffData[idx] != '<') idx++;
	if (idx >= datalen)
	{
		OmnAlarm << "Failed the parsing: " << datalen << enderr;
		OmnScreen << buffData << endl;
		return false;
	}
	int thelen = idx - startidx;
	mCrtTag->addText(startidx, thelen);
	return true;	
}


bool
AosXmlParser::procCdata(int &idx) 
{
	// 'idx' points right after "<![CDATA[".
	// Any string is treated as strings unless it sees ']]>'. 
	// idx = AosSkipWhitespaces(buffData, len, idx);
	int startidx = idx;
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
	while (idx < datalen)
	{
		if (buffData[idx] == ']' && idx < datalen - 2 &&
			buffData[idx+1] == ']' && buffData[idx+2] == '>')
		{
			// It found the end of the CDATA. Note that the text
			// should start at <![CDATA[ and the actual length
			// should be the text plus 12.
			int thelen = idx - startidx + 12;
			mCrtTag->addCdata(startidx-9, thelen);
			idx += 3;
			return true;
		}	
		idx++;
	}

	OmnAlarm << "Trying to parse tag text but end of "
		<< "data found: " << (char *)&buffData[startidx] << enderr;
	return false;
}


//Linda
bool
AosXmlParser::procBdata(int &idx) 
{
	// 'idx' points right after "<![BDATA[".
	// Any string is treated as strings unless it sees ']]>'. 
	// idx = AosSkipWhitespaces(buffData, len, idx);
	int startidx = idx;
	int maxLength = 20;
	int pos = startidx;
	char *buffData = mBuff->getData();
	while(maxLength--)
	{
	    if (buffData[pos++] == ':')
	        break;
	}

	OmnString strlen(&buffData[startidx], pos-startidx -1);
	aos_assert_r(strlen != "", 0);
	int bufflen = atoi(strlen.data());
	aos_assert_r(bufflen >= 0 , 0);

	// The text's foramt must be:
	// <![BDATA[lenght:buffdata]]>
	int thelen = bufflen +strlen.length() + 1 + 12;
	mCrtTag->addBdata(startidx -9, thelen);

	//move lenght:buffdata]]>
	idx = idx + strlen.length()  + 1 + bufflen + 3;
	return true;
}



bool
AosXmlParser::procTagAttrs(
		int &idx, 
		Status &status)
{
	// 'idx' points to the start of a tag attribute. 
	int avstart, avlen, anstart, anlen;
	u8 c;
	u8 *buffData = (u8 *)mBuff->getData();
	int datalen = mBuff->getDataLength();
	idx = AosSkipWhitespaces(buffData, datalen, idx);
	while (idx < datalen)
	{
		c = buffData[idx];
		switch (c)
		{
		case '/':
			 // It should be the end of the current tag. Check 
			 // whether the next char is '>'. 
			 idx++;
			 procClosingTag(idx, false);
			 status = eLookForOpenTag;
			 return true;

		case '>':
			 // The open tag finished. Allowed are texts or 
			 // open tag.
			 idx++;
			 status = eProcTagBody;
			 return true;

		default:
			 // It may be an attribute. 
if(c=='(')
{
	OmnMark;
}
			 aos_assert_r(sgAosFirstWordChars[c], false);
			 aos_assert_r(AosNextEnWord(buffData, sgAosFirstWordChars, 
				(u8 *)"=", 1, idx, datalen, anstart, anlen), false);
			 aos_assert_r(anlen > 0, false);	

			 idx = anstart + anlen;
			 idx = AosSkipWhitespaces(buffData, datalen, idx);
			 aos_assert_r(idx < datalen, false);
			 c = buffData[idx++];
if (c != '=')
{
	OmnScreen << idx << ":" << datalen << ":" << buffData << endl;
}
			 aos_assert_r(c == '=', false);
	
			 // Found 'attrname' + '='. What follows should be the
			 // attribute value. 
			 idx = AosSkipWhitespaces(buffData, datalen, idx);
			 aos_assert_r(idx < datalen, false);
			 c = buffData[idx++];
			 aos_assert_r(c == AOSTAG_SINGLEQUOTE || 
				c == AOSTAG_DOUBLEQUOTE, false);
	 	
			 // Retrieve the attribute value. Attribute values
			 // may contain escaped quotes. If the attribute
			 // is quoted by single quote, double quotes may be 
			 // used in the value, and vice vesa. 
			 aos_assert_r(AosNextQuotedStr(buffData, datalen, c, idx, avstart, avlen, 
					(const unsigned char *)"/>", 2), false);

			 // Found an attribute and its value. Check whether it is an empty
			 // attribute. If yes, remove it.
			 if (avlen == 0)
			 {
				 int aend = avstart + 1;
				 if (buffData[aend] != ' ') aend--;
				 int alen = aend - anstart + 1;
				 int movelen = datalen - aend - 1;
				 if (movelen > 0)
				 	memmove(&buffData[anstart], &buffData[aend+1], movelen);
				 datalen -= alen;
				 buffData[datalen] = 0;
				 mBuff->setDataLength(datalen);
				 idx = anstart;
			 }
			 else
			 {
				int nn = 0;
			 	mCrtTag->addAttr(buffData, anstart, anstart+anlen-1, avstart, avlen, nn);
				buffData = (u8 *)mBuff->getData();
				datalen = mBuff->getDataLength();
				idx += nn;
			 }
			 idx = AosSkipWhitespaces(buffData, datalen, idx);
			 break;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosXmlParser::addTag(
	const int tagStartIdx,
	const int namestartIdx, 
	const int nameendIdx)
{
	// It adds a new tag. The tag name starts at 'startIdx'
	AosXmlTagPtr tag = getTagObj(mCrtTag,
		tagStartIdx, namestartIdx, nameendIdx);
	aos_assert_r(tag, false);
	mCrtTag = tag;
	return true;	
}


bool
AosXmlParser::popTag(
	const int tagEndIdx,
	const int tagnamestart, 
	const int tagnameend, 
	const bool flag)
{
	// The current tag finished. Need to pop the tag. 
	// If 'flag' is true, the 'tagnamestart' and 'tagnameend'
	// identifies the closing tag. Otherwise, it should
	// not be used.
	mCrtTag->setTagEnd(tagEndIdx);
	if (flag)
	{
		// aos_assert_r(mCrtTag->verifyTagName(tagnamestart, 
		// 	tagnameend), false);
		if (!mCrtTag->verifyTagName(tagnamestart, tagnameend))
		{
			return false;
		}
	}
	mCrtTag = mCrtTag->getParentTag();
	aos_assert_r(mCrtTag, false);
	return true;
}


bool
AosXmlParser::procExpected(const OmnString &expected)
{
	// 'expected' is a string of the following format:
	//	<tag>/<tag>/.../<tag>[/_aos_attr:<attrname>]
	// It identies either a tag or an attribute.
	// If 'expected' is not empty, the parser is not to 
	// parse the xml object but to retrieve either a tag
	// or an attribute.
	/*
	mPath.splitStr(expected.data(), "/", eMaxTagsInPath);
	mPathLen = mPath.entries();
	aos_assert_r(mPathLen > 0, false);
	mCrtPathIdx = 0;

	mExpectValue = true;
	OmnString str = mPath.getEntry(mPath.entries()-1);
	const char *data = str.data();
	if (str.length() > 9 && strncmp(data, AOSTAG_ATTR, strlen(AOSTAG_ATTR)) == 0)
	{
		// The looked for is an attribute
		mAttrname = &data[10];
		mIsAttr = true;
	}
	else
	{
		mIsAttr = false;
	}
	*/
	
	return true;
}


AosXmlTagPtr	
AosXmlParser::getTagObj(const AosXmlTagPtr &parent,
				const int tagStartIdx,
				const int nameStart,
				const int nameEnd)
{
	aos_assert_r(mRoot, 0);
	// Ken Lee 2013/05/13
	//return OmnNew AosXmlTag(mRoot, parent, mBuff, 
	//	tagStartIdx, nameStart, nameEnd AosMemoryCheckerGetMembers(this));
	return OmnNew AosXmlTag(mRoot, parent, mBuff, 
		tagStartIdx, nameStart, nameEnd AosMemoryCheckerArgs);
}


bool
AosXmlParser::prepareData()
{
	// This function removes the following:
	// 	&#10;			the newline
	// 	&#13;			carriage return
	// 	&#x0A;			newline
	// 	&#x0a;			newline
	// 	&#x0D;			carriage return
	// 	&#x0d;			carriage return
	// 	&lt;![CDATA[	<![CDATA[
	// 	]]&gt;			]]>
	//
	// When it encounters one of the following:
	// 	"   &quot;
	// 	'   &apos;
	// 	<   &lt;
	// 	>   &gt;
	// 	&   &amp;
	//
	// it checks whether it is in a node text. If yes, it checks whether
	// it is CDATA. If not, it changes it to CDATA. It then replace 
	// these escaped characters.
	//
	// For &quot and &apos, if it is in an attribute, check whether the
	// attribute is quoted using the same quote. If not, replace it. 
	// For instance:
	// 	<xxx name="this is &apos;good&apos;" .../>
	// should be converted to:
	// 	<xxx name="this is 'good'" .../>
	//  
	// Or:
	// 	<xxx name='this is &quot;good&quot;' .../>
	// should be converted to:
	// 	<xxx name='this is "good"' .../>
	//
	// Note that '&lt;', '&gt;', and '&amp;' cannot be unescaped when 
	// they appear in an attribute.
	//
	aos_assert_r(mBuff, false);
	int datalen = mBuff->getDataLength();
	char *data = mBuff->getData();

//	char *ptr;
	int idx = 0;
	int changedlen = datalen;
	OmnString crt_tagname;
	while (idx < changedlen)
	{
/*
		ptr = strchr(&data[idx], '&');
		if (!ptr)
		{
			if (datalen != changedlen)
			{
				mBuff->setDataLength(changedlen);
			}
			return true;
		}

		idx = (ptr - data);
*/
		int remainlen = changedlen - idx;
		if (remainlen < 4)
		{
			if (datalen != changedlen)
			{
				mBuff->setDataLength(changedlen);
			}
			return true;
		}
		
		// ice, 2013/09/13
		//if (!strncmp(&data[idx], "<![BDATA[", changedlen-idx))
		//<!BDATA[XXX]]>	
		if (changedlen-idx > 14 && !strncmp(&data[idx], "<![BDATA[", 9))
		{
			idx = idx+9;
			procBdata(idx);
			continue;
		}

		if (data[idx] != '&')
		{
			idx++;
			continue;
		}

		switch (data[idx+1])
		{
		case '#':
			 if (remainlen >= 5)
			 {
				// Check '&#10;' and '&#13;'
			 	if ((data[idx+2] == '1' && data[idx+3] == '0' && data[idx+4] == ';') ||
			 		(data[idx+2] == '1' && data[idx+3] == '3' && data[idx+4] == ';'))
			 	{
					memmove(&data[idx], &data[idx+5], (changedlen-idx-4));
					changedlen -=5;
					break;
				}
			 }

			 if (remainlen >= 6)
			 {
			 	if ((data[idx+2] == 'x' && data[idx+3] == '0' && 
					 data[idx+4] == 'A' && data[idx+5] == ';') ||
			 	    (data[idx+2] == 'x' && data[idx+3] == '0' && 
					 data[idx+4] == 'a' && data[idx+5] == ';') ||
			 	    (data[idx+2] == 'x' && data[idx+3] == '0' && 
					 data[idx+4] == 'D' && data[idx+5] == ';') ||
			 	    (data[idx+2] == 'x' && data[idx+3] == '0' && 
					 data[idx+4] == 'd' && data[idx+5] == ';'))
				{
					memmove(&data[idx], &data[idx+6], (changedlen-idx-5));
					changedlen -=6;
					break;
				}
			 }
			 idx++;
			 break;
				
		case 'l':
			 if (remainlen >= 12 && strncmp(&data[idx], "&lt;![CDATA[", 12) == 0)
			 {
			 	 // Chen Ding, 2011/02/10
				 // It is a '&lt;'. Checks whether it is the beginning of a CDATA. 
				 // If yes, it needs to convert "&lt;" to '<'. 
			     if (getOpeningTag(data, idx, crt_tagname))
			     {
			  	 	 // Change it to '<![CDATA['
			  	     data[idx] = '<';
			  	     memmove(&data[idx+1], &data[idx+4], (changedlen-idx-3));
			  	     changedlen -= 3;
					 idx += 9;
			      }
				  else
				  {
					  // It is not the beginning of a CDATA. Increment num_cdata.
					  idx += 12;
				  }
			 }
			 else
			 {
			     idx++;
			 }
			 break;
			 
		case 'g':
			 if (remainlen >= 4 && idx > 3 && strncmp(&data[idx-2], "]]&gt;", 6) == 0)
			 {
				 // Chen Ding, 2011/02/11
				 // Check whether it is in the form:
				 // <tagname><![CDATA[xxxx]]&gt;</tagname>. If yes, it is a good 
				 // indication that it should be unescaped.
				 if (getClosingTag(data, idx, changedlen, crt_tagname))
				 {
			  	     // Change it to ']]>'
			  	     data[idx] = '>';
			  	     memmove(&data[idx+1], &data[idx+4], (changedlen-idx-3));
			  	     changedlen -= 3;
					 idx += 3;
				 }
				 else
				 {
					 // Do not unescape it
					 idx += 6;
				 }
			  }
			  else
			  {
			     idx++;
			  }
			  break;

		default:
			 idx++;
			 break;
		}
	}

	if (datalen != changedlen)
	{
		mBuff->setDataLength(changedlen);
	}

	return true;
}


u8 *
AosXmlParser::checkAttrValue(
		u8 *buffData, 
		const int avstart, 
		int &avlen)
{
	// The following characters are not allowed in attribute values:
	//  Control characters (<32) (except '\n'(10), '\r'(12), and 
	//  'vertical tab'(11). 
	//
	// The following are reserved characters that should not appear
	// in attributes:
	//  ", %, &, ', <, >,
	//
	// These characters are escaped as follows:
	//  '&':	&amp;
	//  '<': 	&lt;
	//  '>':	&gt;
	//  '"':	&quot;
	//  ''':	&apos;
	
	// Check whether the
	// To Be Done
	return buffData;
}


bool
AosXmlParser::getOpeningTag(
		const char *data, 
		const int idx, 
		OmnString &tagname)
{
	// It checks whether it is in the form:
	// 		<tagname>&lt;[CDATA[xxx
	// where 'idx' points to '&'. If yes, it sets the tagname to 'tagname' 
	// and returns true. Otherwise, it returns false.
	
	if (idx < 0 || data[idx-1] != '>') return false;
	
	// Find '<'
	int startidx = idx-1;
	while (startidx > 0)
	{
		char c = data[startidx];
		if (c == ' ' || c == '/') return false;
		if (c == '<')
		{
			// Found it.
			tagname.assign(&data[startidx+1], idx - startidx - 2);
			return true;
		}
		startidx--;
	}
	return false;
}

			
bool
AosXmlParser::getClosingTag(
		const char *data, 
		const int idx, 
		const int len,
		const OmnString &tagname)
{
	// It checks whether it is in the form:
	// 		xxx&gt;</tagname>	
	// where 'idx' points to '&' and tagname matches 'tagname.
	// If yes, it returns true. Otherwise, it returns false.
	
	if (idx+tagname.length() + 6 >= len) return false;
	if (data[idx+4] != '<') return false;
	if (data[idx+5] != '/') return false;
	
	// Find '>'
	int startidx = idx+2;
	while (startidx < len)
	{
		char c = data[startidx];
		if (c == ' ') return false;
		if (c == '>' && strncmp(tagname.data(), &data[idx+6], tagname.length()) == 0)
		{
			return true;
		}
		startidx++;
	}
	return false;
}


AosXmlTagPtr
AosXmlParser::parse(const OmnString &str AosMemoryCheckDecl)
{
	// Chen Ding, 2013/08/25
	if (str == "") return 0;

	AosXmlParser parser;
	return parser.parse(str, "" AosMemoryCheckerFileLine);
}


AosXmlTagPtr
AosXmlParser::parse(const char *str, const int len AosMemoryCheckDecl)
{
	if (!str || len == 0) return 0;

	AosXmlParser parser;
	return parser.parse(str, len, "" AosMemoryCheckerFileLine);
}


AosXmlTagPtr
AosXmlParser::parseStatic(const OmnConnBuffPtr &buff AosMemoryCheckDecl)
{
	AosXmlParser parser;
	return parser.parse(buff, "" AosMemoryCheckerFileLine);
}

