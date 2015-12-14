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
// Modification History:
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/NameValueParser.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/NameValueDoc.h"
#include "Util/String.h"
#include "Util/NameValueDoc.h"

// Names are made of letters, digits, dashes, and underscores
// if Name character is invalid,we set the relative pos 0
static char sgNameMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 0-9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 10-19
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 20-29
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 		// 30-39
	0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 		// 40-49, dash (45), digits [48-57]
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 		// 50-59, digits
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 		// 60-69, Capitals: [64-90]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 70-79
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 80-89
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 		// 90-99, '_' (95), lower case letters: [97-122]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 100-109
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 110-119
	1, 1, 1, 0, 0, 0, 0, 0  			// 120-127
};

static char sgWhiteSpaceMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 		// 0-9
	0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 		// 10-19
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 20-29
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 		// 30-39
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 40-49, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 50-59, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 60-69, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 80-89
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 90-99, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 110-119
	0, 0, 0, 0, 0, 0, 0, 0  			// 120-127
};

// This map defines the invalid first characters
// for unquoted values.
// 	', ", (, [, ), ], space, tab, new line, 
// 	carriage return, comma, ':', ';', '=', backslash
// 	'{', '}', 127
static char sgInvalidStartCharForValue[128] =
{                                                 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 0-9
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 10-19
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 20-29
	1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 		// 30-39
	1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 		// 40-49
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 		// 50-59
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 		// 60-69
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 80-89
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 		// 90-99
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 110-119
	0, 0, 0, 1, 0, 1, 0, 1  			// 120-127
};                                                

static char sgUnQuoteValueMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 0-9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 10-19
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 20-29
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 		// 30-39
	1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 		// 40-49, dash (45), digits [48-57]
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 		// 50-59, digits
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 60-69, Capitals: [64-90]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 70-79
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 80-89
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 		// 90-99, '_' (95), lower case letters: [97-122]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 100-109
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 		// 110-119
	1, 1, 1, 0, 0, 0, 0, 0  			// 120-127
};
bool
AosNameValueParser::parse(
		AosRundata *rdata, 
		const char *data,
		const int len,
		int &cursor,
		AosNameValueDoc &doc)
{
	// This function assumes the inputs are a Name-Value Doc:
	//  (
	// 		name: string,
	// 		name: 'string',
	// 		name: "string",
	// 		name: [doc_value, doc_value, ...],
	// 		name: (doc_value, doc_value, ...)
	// 		name: (name:doc_value, name:doc_value, ...)
	// 	)
	// The cursor stops at '('. White spaces before '(' are possible.

	doc.clear();
	doc.setType(AosNameValueDoc::eDoc);
	mErrMsg = "";
	int originCursor = cursor;
	bool rslt = expectChar(rdata, data, len, originCursor, '(', 0);
	if(!rslt) return false;   
	rslt = parseNext(rdata, data, len, cursor, doc);
	return rslt;
}
	
bool
AosNameValueParser::parseNext(
		AosRundata *rdata, 
		const char *data,
		const int len,
		int &cursor,
		AosNameValueDoc &doc)
{
	int pos = 0;   //this pos idicate which name-value is invalid
	OmnString name;
	doc.setType(AosNameValueDoc::eDoc);
	mErrStartIdx = cursor;
	
	bool rslt = expectChar(rdata, data, len, cursor, '(', pos);
	if (!rslt)
	{
	//	AosLogError(rdata, true, "not_valid_name_value_list")
	//		<< AosFN("Data") << data << enderr;
		mErrMsg << "missing \"(\" around " << "\"" << OmnString(data + mErrStartIdx, cursor - mErrStartIdx) << "\""; 
		rdata->setJqlMsg(mErrMsg);
		return false;
	}

	skipWhiteSpace(rdata, data, len, cursor);
	if (cursor >= len)
	{
		//AosLogError(rdata, true, "not_valid_name_value_list")
		//	<< AosFN("Data") << data << enderr;
		return false;
	}

	if (data[cursor] == ')')
	{
		// It is an empty name-value list
		cursor++;
		return true;
	}
	
	while (cursor < len)
	{
		mErrStartIdx = cursor;
		rslt = nextName(rdata, data, len, cursor, name, pos);
		mPriName = name;
		//aos_assert_rr(rslt, rdata, false);
		if(!rslt)
		{
			if(mErrMsg == "")
			{
				mErrMsg << "invalid name around " << "\"" << OmnString(data + mErrStartIdx, cursor - mErrStartIdx) << "\"";
				rdata->setJqlMsg(mErrMsg);
			}
			return false;
		}
		
		mErrStartIdx = cursor;
		rslt = expectChar(rdata, data, len, cursor, ':', pos);
		if(!rslt)
		{
			if(mErrMsg == "")
			{
				mErrMsg << "missing \":\" around " << "\"" << OmnString(data + mErrStartIdx, cursor - mErrStartIdx) << "\"";
				rdata->setJqlMsg(mErrMsg);
			}
			return false;
		}
		//aos_assert_rr(rslt, rdata, false);

		AosNameValueDoc value_doc;
		mErrStartIdx = cursor;
		rslt = nextDocValue(rdata, value_doc, data, len, cursor, pos);
		if(!rslt)
		{
			if(mErrMsg == "")
			{
				mErrMsg << "invalid docValue around " << "\"" << OmnString(data + mErrStartIdx, cursor - mErrStartIdx) << "\"";
				rdata->setJqlMsg(mErrMsg);
			}
			return false;
		}
		//aos_assert_rr(rslt, rdata, false);

		doc.appendField(name, value_doc);

		mErrStartIdx = cursor;
		skipWhiteSpace(rdata, data, len, cursor);
		if (cursor >= len) break;

		char c = data[cursor];
		if (c == ')')
		{
			cursor++;
			return true;
		}

		if (c != ',')
		{
			//AosLogError(rdata, true, "expecting_comma")
			//	<< AosFN("Position") << pos
			//	<< AosFN("Data") << data << enderr;
			mErrMsg << "missing \",\" near name :" << "\"" << name << "\"";
			rdata->setJqlMsg(mErrMsg);
			return false;
		}
		cursor++;	
		skipWhiteSpace(rdata, data, len, cursor);
		pos++;
	}

	//AosLogError(rdata, true, "invalid_name_value_list")
	//	<< AosFN("Data") << data << enderr;
	mErrMsg << "missing terminating symbol ";
	rdata->setJqlMsg(mErrMsg);
	return false;
}


void
AosNameValueParser::skipWhiteSpace(
		AosRundata *rdata, 
		const char *data, 
		const int len,
		int &cursor)
{
	// It skips the white spaces. White spaces are defined by the 
	// charset map sgWhiteSpaceMap. It will stop at the first 
	// non-white space character or at the end of the input.
	u8 c;
	while (cursor < len) 
	{
		c = data[cursor];
		if (c >= 128) return;

		if (!sgWhiteSpaceMap[c]) return;
		cursor++;
	}
	return;
}


bool
AosNameValueParser::nextName(
		AosRundata *rdata, 
		const char *data, 
		const int len, 
		int &cursor, 
		OmnString &name, 
		const int pos)
{
	// This function retrieves name. Names are made of characters defined in
	// 'sgNameMap' with up to eMaxNameLen bytes long. Non-ASCII characters
	// are allowed.
	int name_start = cursor;
	int name_len = 0;
	name = "";
	while (cursor < len && name_len < eMaxNameLen)
	{
		u8 c = data[cursor++];
		if (c <= 127 && !sgNameMap[c]) 
		{
			if((char)c != ':' && (char)c != ' ')
			{
				OmnString errMsg = OmnString(data + name_start, name_len);
				if(errMsg == "")
				{
					mErrMsg << "unexpected " << "\"" << (char)c << "\" around " << mPriName;
				}
				else
				{
					mErrMsg << "unexpected " << "\"" << (char)c << "\" around " << errMsg;
				}
				rdata->setJqlMsg(mErrMsg);
				return false;
			}
			cursor--;
			break;
		}
		name_len++;
	}

	if (name_len <= 0)
	{
		AosLogError(rdata, true, "missing_name")
			<< AosFN("Positoin") << pos
			<< AosFN("Data") << data << enderr;
		return false;
	}

	if (cursor >= len)
	{
		AosLogError(rdata, true, "expecting name_but_run_out_data") 
			<< AosFN("Positoin") << pos
			<< AosFN("Data") << data << enderr;
		return false;
	}

	if (name_len >= eMaxNameLen)
	{
		AosLogError(rdata, true, "name_too_long")
			<< AosFN("Position") << pos 
			<< AosFN("Max Length") << eMaxNameLen << enderr;
		return false;
	}

	name.assign(&data[name_start], name_len);
	return true;
}


bool
AosNameValueParser::nextDocValue(
		AosRundata *rdata, 
		AosNameValueDoc &doc,
		const char *data, 
		const int len,
		int &cursor, 
		const int pos)
{
	// This function parses the next value. It assumes 'cursor'
	// stops right after ':'. 
	//
	// A DocValue can be one of the following:
	// 	1. Quoted:   value,
	// 	2. Unquoted: value, 
	// 	3. Tuple: 	 (doc_value, doc_value, ...)
	// 	4. Array: 	 [doc_value, doc_value, ...]
	// 	5. ArrayDoc: [name:doc_value, name:doc_value, ...]
	//	6. Doc:      (name:doc_value, name:doc_value, ...)
	
	skipWhiteSpace(rdata, data, len, cursor);
	//cursor--;      //now the cursor has pointed to the next char after whiteSpace
	if (cursor >= len)
	{
		AosLogError(rdata, true, "missing_value")
			<< AosFN("Position") << pos 
			<< AosFN("Data") << data << enderr;
		return false;
	}

	char c = data[cursor++];
	switch (c)
	{
	case '(':
		 return nextTuple(rdata, doc, data, len, cursor, pos);

	case '[':
		 return nextArray(rdata, doc, data, len, cursor, pos);

	case '\'':
	case '\"':
		 return nextQuotedValue(rdata, doc, data, len, cursor, c, pos);

	default:
		 if (c < 128 && sgInvalidStartCharForValue[(u8)c])
		 {
			 AosLogError(rdata, true, "invalid_value")
				 << AosFN("Position") << pos
				 << AosFN("Data") << data << enderr;
			 return false;
		 }
		 return nextUnquotedValue(rdata, doc, data, len, cursor, pos);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosNameValueParser::nextTuple(
		AosRundata *rdata, 
		AosNameValueDoc &doc,
		const char *data, 
		const int len, 
		int &cursor, 
		const int pos)
{
	// A tuple is in the following format:
	// 	(doc_value, doc_value, ...) or 		(type is eTupleDoc)
	// 	(name:doc_value, name:doc_value)	(type is eDoc)
	//
	// Empty tuple is allowed. This function assumes 'cursor' stops
	// right after '('. 
	//
	// It parses the values into 'value'.
	//doc.clear();
	doc.setType(AosNameValueDoc::eTupleDoc);
	int num_values = 0;
	while (cursor < len)
	{
		int startCursor = cursor -1;
		aos_assert_rr(startCursor >= 0, rdata, false);

		if(num_values > 0) startCursor = cursor;
		
		AosNameValueDoc doc_value;
		bool rslt = nextDocValue(rdata, doc_value, data, len, cursor, pos);
		aos_assert_rr(rslt, rdata, false);

		skipWhiteSpace(rdata, data, len, cursor);
		if (cursor >= len)
		{
			AosLogError(rdata, true, "invalid_tuple")
				<< AosFN("Position") << pos
				<< AosFN("Data") << data << enderr;
			return false;
		}
		
		//Value format is name : value
		if (data[cursor] == ':')
		{
			// It is supposed to be (name:value, ...)
			doc.setType(AosNameValueDoc::eDoc);
			if (doc_value.getType() != AosNameValueDoc::eStrValueDoc)
			{
				AosLogError(rdata, true, "expecting_name_but_invalid_name")
					<< AosFN("Position") << pos
					<< AosFN("Data") << data << enderr;
				return false;
			}

			if (num_values > 0) // this mean value format is(value, name:value,value) now we can not support it
			{
				AosLogError(rdata, true, "invalid_value") 
					<< AosFN("Position") << pos
					<< AosFN("Data") << data << enderr;
				return false;
			}

			// The value is in the form:	
			// 		(name:value, name:value,...)
			cursor = startCursor;
			bool rslt = parseNext(rdata, data, len, cursor, doc);
			return rslt;
			aos_assert_r(rslt, false);
			aos_assert_r(doc.getType() == AosNameValueDoc::eDoc, false);
			//doc.appendValue(doc_value);
			return true;
		}
		
		num_values++;

		if (data[cursor] == ',') 
		{
			doc.appendValue(doc_value);
			cursor++;
			//startCursor = cursor + 1;
			continue;
		}

		if (data[cursor] == ')') 
		{
			doc.appendValue(doc_value);
			cursor++;
			return true;
		}

		break;
	}

	AosLogError(rdata, true, "invalid_tuple")
		<< AosFN("Position") << pos
		<< AosFN("Data") << data << enderr;
	return false;
}

bool
AosNameValueParser::nextArray(
		AosRundata *rdata, 
		AosNameValueDoc &doc,
		const char *data, 
		const int len, 
		int &cursor, 
		const int pos)
{
	// An array is in the following format:
	// 	[value, value, ...]
	// where 'value' can be any ValueType. Empty tuple is allowed. 
	// This function assumes 'cursor' stops right after '['. 
	//
	// It parses the values into 'value'.
	doc.setType(AosNameValueDoc::eArrayDoc);
	int num_values = 0;
	while (cursor < len)
	{
		int startCursor = cursor -1;
		if(num_values > 0) startCursor = cursor;

		AosNameValueDoc doc_value;
		bool rslt = nextDocValue(rdata, doc_value, data, len, cursor, pos);
		aos_assert_rr(rslt, rdata, false);

		skipWhiteSpace(rdata, data, len, cursor);
		if (cursor >= len)
		{
			AosLogError(rdata, true, "invalid_tuple")
				<< AosFN("Position") << pos
				<< AosFN("Data") << data << enderr;
			return false;
		}

		if (data[cursor] == ':')
		{
			doc.setType(AosNameValueDoc::eDoc);
			if (doc_value.getType() != AosNameValueDoc::eStrValueDoc)
			{
				AosLogError(rdata, true, "expecting_name_but_invalid_name")
					<< AosFN("Position") << pos
					<< AosFN("Data") << data << enderr;
				return false;
			}
			
			if (num_values > 0) // this mean value format is(value, name:value,value) now we can not support it
			{
				AosLogError(rdata, true, "invalid_value") 
				<< AosFN("Position") << pos
				<< AosFN("Data") << data << enderr;
				return false;
			}

			// The value is in the form:	
			// 		(name:value, name:value,...)
			cursor = startCursor;
			bool rslt = parseNext(rdata, data, len, cursor, doc);
			aos_assert_r(rslt, false);
			return true;
		}
		
		num_values++;
		
		if (data[cursor] == ',') 
		{
			doc.appendValue(doc_value);
			cursor++;
			continue;
		}

		if (data[cursor] == ']') 
		{
			doc.appendValue(doc_value);
			cursor++;
			return true;
		}

		break;
	}

	AosLogError(rdata, true, "invalid_tuple")
		<< AosFN("Position") << pos
		<< AosFN("Data") << data << enderr;
	return false;
}


bool
AosNameValueParser::nextQuotedValue(
		AosRundata *rdata, 
		AosNameValueDoc &doc,
		const char *data, 
		const int len, 
		int &cursor, 
		const char quote, 
		const int pos)
{
	// This function parses a quoted value. The value should be
	// ended with the same quote. If the quote appears inside
	// the value, it should be escaped by doubling or backslaching
	// the quote. 
	//
	// This function assumes 'cursor' points right after the beginning
	// quote.

	OmnString str_value;
	doc.setType(AosNameValueDoc::eStrValueDoc);
	while (cursor < len)
	{
		// Scan the string all the way to its closing quote.
		// If quote appears inside the quote, it should be 
		// escaped.
		//

		char c = data[cursor++];
		if (c == '\\')
		{
			// It needs to consider escape sequences:
			// 		\n		new line 
			// 		\t		tab
			if (cursor >= len)
			{
				AosLogError(rdata, true, "invalidValue")
					<< AosFN("Data") << enderr;
				return false;
			}

			c = data[cursor++];
			switch (c)
			{
			case 'a':
				 str_value << '\a';
				 break;

			case 'b':
				 str_value << '\b';
				 break;

			case 'f':
				 str_value << '\f';
				 break;
			
			case 'n':
				 str_value << '\n';
				 break;

			case 'r':
				 str_value << '\r';
				 break;

			case 't':
				 str_value << '\t';
				 break;

			case 'v':
				 str_value << '\v';
				 break;

			case '\'':
				 str_value << '\'';
				 break;

			case '"':
				 str_value << '\"';
				 break;

			case '?':
				 str_value << '\?';
				 break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				 // It is the octal number \ooo
				 {
					 if (cursor + 2 >= len)
					 {
						 AosLogError(rdata, true, "invalid value")
							 << AosFN("Data") << enderr;
				 		 return false;
					 }
			
					 int octalToDecValue = ((data[cursor - 1] - '0') << 6) + ((data[cursor] - '0') << 3)
										+ (data[cursor + 1] - '0');
					 str_value << octalToDecValue;
				 }
				 break;
			
			default:
					AosLogError(rdata, true, "invalid value")
						<< AosFN("Data") << enderr;
				 	return false;
			}
			continue;
		}

		if (c != quote)
		{
			str_value << c;
			continue;
		}
		
		if(c == quote)
		{
			doc.setStrValue(str_value);
			return true;
		}; 
		
		if (cursor >= len)
		{
			AosLogError(rdata, true, "value_ended_inmaturely")
				<< AosFN("Data") << enderr;
			return false;
		}

		// Check whether the next character is the same quote.
		if (data[cursor+1] == quote)
		{
			str_value << quote;
			//cursor++;
			continue;
		}

		// It finished the value. 
	}

	AosLogError(rdata, true, "invalid_value")
		<< AosFN("Data") << data << enderr;
	return false;
}


bool
AosNameValueParser::nextUnquotedValue(
		AosRundata *rdata, 
		AosNameValueDoc &doc,
		const char *data, 
		const int len, 
		int &cursor, 
		const int pos)
{
	// This function parses an unquoted value. 
	int value_start = --cursor;
	int value_len = 0;
	OmnString value;
	doc.setType(AosNameValueDoc::eStrValueDoc);
	while (cursor < len)
	{
		u8 c = data[cursor++];
		if (c <= 127 && !sgUnQuoteValueMap[c]) 
		{
			cursor--;
			break;
		}
		value_len++;
	}

	if (value_len <= 0)
	{
		AosLogError(rdata, true, "missing_name")
			<< AosFN("Positoin") << pos
			<< AosFN("Data") << data << enderr;
		return false;
	}

	if (cursor >= len)
	{
		AosLogError(rdata, true, "expecting name_but_run_out_data") 
			<< AosFN("Positoin") << pos
			<< AosFN("Data") << data << enderr;
		return false;
	}

	value.assign(&data[value_start], value_len);
	doc.setStrValue(value);
	return true;
}


bool
AosNameValueParser::expectChar(
		AosRundata *rdata, 
		const char *data, 
		const int len, 
		int &cursor,
		const char expected_char, 
		const int pos)
{
	// This function expects the next non-white-space character
	// is 'expected_char'. If yes, it moves the cursor right after
	// the expected character. Otherwise, it returns false.

	skipWhiteSpace(rdata, data, len, cursor);
	//cursor--;
	if (cursor >= len) 
	{
		AosLogError(rdata, true, "syntax_error")
			<< AosFN("Expecting") << expected_char
			<< AosFN("Position") << pos 
			<< AosFN("Data") << data << enderr;
		return false;
	}
	
	if (data[cursor] == expected_char) 
	{
		cursor++;
		return true;
	}

	/*AosLogError(rdata, true, "syntax_error")
		<< AosFN("Expecting") << expected_char
		<< AosFN("Actual") << data[cursor] 
		<< AosFN("Position") << pos 
		<< AosFN("Data") << data << enderr;*/
	return false;
}

