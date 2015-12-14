////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlParser.cpp
// Description:
//	This is a very simplified XML document parser. An XML document this
//  class accepts are in the following format:
//		<Tag>...</Tag> ... <Tag>...</Tag>
//
//	where <Tag>...</Tag> is an XmlItem. An XmlItem can contain XmlItems.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "XmlParser/XmlParser.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Debug/Rslt.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"

// This is atest

OmnXmlParser::OmnXmlParser()
:
mCrtIndex(0),
mLength(0)
{
}


OmnXmlParser::OmnXmlParser(const OmnString &data)
:
mData(data),
mCrtIndex(0),
mLength(mData.length())
{
	removeComments();
}


OmnXmlParser::~OmnXmlParser()
{
}


void
OmnXmlParser::setData(const OmnString &data)
{
	mData = data;
	mLength = mData.length();
	removeComments();
	mCrtIndex = 0;
}


// 
// 02/19/2007, Chen Ding
// 
bool
OmnXmlParser::readFromFile(const OmnString &filename)
{
	// 
	// Chen Ding, 05/16/2007
	// Add the ability to read include files
	//
	OmnString files[100];
	int numFiles = 0;
	files[numFiles++] = filename;
	OmnString contents;
	while (numFiles != 0)
	{
		OmnString fn = files[0];
		for (int i=0; i<numFiles-1; i++)
		{
			files[i] = files[i+1];
		}
		numFiles--;

		OmnFile f(fn, OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!f.isGood())
		{
			OmnAlarm << "Failed to open the file: " << fn << enderr;
			return false;
		}

		OmnString data;
		f.readToString(data);
		
		// 
		// Process its include files
		//
		int index = 1;
		while (1)
		{
			OmnString line = data.getLine(false, index);
			if (line.isEmptyLine() || line.length() <= 0)
			{
				// 
				// It is an empty line. It is the end of the include section
				//
				break;
			}

			OmnStrParser1 parser(line, " ", false, false);
			OmnString include = parser.nextWord("");
			if (include != "#include")
			{
				index++;
				continue;
			}

			parser.skipWhiteSpace();
			if (!parser.expectNext('"', true))
			{
				OmnAlarm << "Invalid include line: " << line << enderr;
				return false;
			}

			OmnString f = parser.nextEngWord("");
			if (f == "")
			{
				OmnAlarm << "Invalid include line: " << line << enderr;
				return false;
			}

			files[numFiles++] = f;

			// 
			// Remove the line from the file
			//
			data.getLine(true, index);
		}

		contents << data;
	}
		
	setData(contents);
	return true;
}


// 
// Chen Ding, 09/19/2003, 2003-0262
//
OmnRslt
OmnXmlParser::nextOpenTag(OmnString &tag, 
						  int &valueStartIndex,
						  int &fieldLength)
{
	//
	// From its current position, it moves until it sees '<xyyy>', where
	// 'x' != '/'. If yes, it sets 'xyyy' to 'tag', mCrtIndex and 
	// 'valueStartIndex' to the index right after the '>'
	// If not, it is an error.
	// If any error, it restores mCrtIndex value.
	//

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	// If the field is in form of <name length=ddd>, then fieldLength is set
	// to 'ddd' (converted into integer. Otherwise, fieldLength is set to -1, 
	// indicating that the field is determined by the closing tag.
	//
	fieldLength = -1;

	//
	// Remove leading spaces
	//
	int startIndex = mCrtIndex;

	//
	// Looking for the open '<'
	//
	const char *buf = mData.data();
	while (mCrtIndex < mLength && buf[mCrtIndex] != '<') mCrtIndex++;

	//
	// Check whether it is out of bound
	//
	if (mCrtIndex >= mLength)
	{
		//
		// Out of bound. There is no more open tag. This does not necessarily
		// mean an error. 
		//
		return OmnRslt(OmnErrId::eWarnSyntaxError);
	}

	//
	// Found a '<'. Check whether the next is '/'. If not, it is an openning
	// tag. 
	//
	mCrtIndex++;
	if (mCrtIndex >= mLength)
	{
		//
		// Out of bound. This is a malformated XML document.
		//
		mCrtIndex = startIndex;
		OmnWarn << "String ended with '<': " << mData << enderr;
	}

	if (buf[mCrtIndex] == '/')
	{
		//
		// It is '</'. This is not an open tag. It is considered an error
		//
		OmnWarn 
			<< "Expecting an open tag but found a closing tag at: " 
			<< mCrtIndex
			<< ": " 
			<< &buf[mCrtIndex] << enderr;
		mCrtIndex = startIndex;
		return false;
	}

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	// Skip the leading spaces between '<' and the first printable 
	// character.
	//
	while (mCrtIndex < mLength && (buf[mCrtIndex] < 33 || buf[mCrtIndex] > 126))
	{
		mCrtIndex++;
	}
	if (mCrtIndex >= mLength)
	{
		//
		// Out of bound.
		//
		mCrtIndex = startIndex;
		OmnWarn	<< "Expecting an open tag, saw <, but run out of bound: " 
			<< buf << enderr;
	}

	//
	// Retrieve the value
	//
	int tagStartIndex = mCrtIndex;
	mCrtIndex++;

	//
	// Chen Ding, 09/19/2003, 2003-0262
	// Looking for '>', ' ', '\t'.
	//
	// while (mCrtIndex < mLength && buf[mCrtIndex] != '>') mCrtIndex++;
	while (mCrtIndex < mLength && 
		   buf[mCrtIndex] != '>' &&
		   buf[mCrtIndex] != ' ' &&
		   buf[mCrtIndex] != '\t') mCrtIndex++;

	if (mCrtIndex >= mLength)
	{
		//
		// It is a mal-functioned XML document
		//
		mCrtIndex = startIndex;
		OmnWarn << "Expecting > but out of bound at: "
			<< mCrtIndex << ": " << mData << enderr;
		return false;
	}

	//
	// Chen Ding, 09/19/2003, 2003-0262
	// Now, it stops at one of '>', ' ', '\t'. If it is ' ' or '\t', need to
	// continue to the next printable character.
	//
	int tagEndIndex = mCrtIndex-1;
	if (buf[mCrtIndex] == ' ' || buf[mCrtIndex] == '\t')
	{
		//
		// Move to the next printable character.
		//
		mCrtIndex++;
		while (mCrtIndex < mLength && (buf[mCrtIndex] == ' ' || buf[mCrtIndex] == '\t'))
		{
			mCrtIndex++;
		}

		if (mCrtIndex >= mLength)
		{
			OmnWarn	<< "Expecting an open tag but run out of bound: " 
				<< buf << enderr;
			return false;
		}

		//
		// So far, we have seen: "<name" + space/tab + printable-charater. 
		// What follows should be either "length=ddd", or ">"
		//
		if (buf[mCrtIndex] == '>')
		{
			// 
			// This is the closing of the tag. Do nothing.
			//
		}
		else if (mCrtIndex + 7 < mLength &&
			(buf[mCrtIndex]   == 'l' || buf[mCrtIndex]   == 'L') &&
			(buf[mCrtIndex+1] == 'e' || buf[mCrtIndex+1] == 'E') &&
			(buf[mCrtIndex+2] == 'n' || buf[mCrtIndex+2] == 'N') &&
			(buf[mCrtIndex+3] == 'g' || buf[mCrtIndex+3] == 'G') &&
			(buf[mCrtIndex+4] == 't' || buf[mCrtIndex+4] == 'T') &&
			(buf[mCrtIndex+5] == 'h' || buf[mCrtIndex+5] == 'H') &&
			buf[mCrtIndex+6] == '=') 
		{
			// 
			// It is 'length='. Check whether the next word is all
			// digits.
			//
			mCrtIndex += 7;

			// 
			// The next should be all digits. After that should be spaces (optional)
			// and the closing ">"
			//
			int lengthStart = mCrtIndex;

			// 
			// Move until it sees a non-digit character.
			//
			while (mCrtIndex < mLength && buf[mCrtIndex] >= '0' && buf[mCrtIndex] <= '9')
			{
				mCrtIndex++;
			}

			if (mCrtIndex == lengthStart)
			{
				// 
				// The one immediately follows '=' is not a digit.
				//
				mCrtIndex = startIndex;
				OmnWarn	<< "The one immediately follows length= is not a digit: " 
					<< &buf[mCrtIndex] << ". " << buf << enderr;
				return false;
			}

			//
			// Found an integer [lengthStart, mCrtIndex-1].
			//
			fieldLength = atoi(&buf[lengthStart]);
			if (fieldLength >= eMaxFieldLength || fieldLength <= 0)
			{
				mCrtIndex = startIndex;
				OmnWarn << "Field length too big: " 
					<< &buf[lengthStart-10] << enderr;
				return false;
			}

			// 
			// Move to the closing ">"
			//
			if (buf[mCrtIndex] != '>')
			{
				mCrtIndex = startIndex;
				OmnWarn << "Expecting > after length=ddd: "
					<< &buf[lengthStart-10] << enderr;
				return false;
			}
		}
		else
		{
			// 
			// It is an error.
			// 
			mCrtIndex = startIndex;
			OmnWarn << "Found an invalid tag parameter at: " 
				<< &buf[mCrtIndex]
				<< ".\n" << buf << enderr;	
			return false;
		}
	}
	
	//
	// Chen Ding, 09/19/2003, 2003-0262
	// Now mCrtIndex stops at ">", 
	//	   tagStartIndex points to the beginning of the tag name
	//	   tagEndIndex points to the end of the tag name
	//
	// if (mCrtIndex == tagStartIndex)
	if (tagEndIndex - tagStartIndex < 1)
	{
		//
		// It is an empty tag item: '<>'
		//
		mCrtIndex = startIndex;
		OmnWarn << "Found an empty tag at "
				<< mCrtIndex-1
				<< ": " << mData << enderr;
		return false;
	}

	tag = OmnString(&buf[tagStartIndex], tagEndIndex - tagStartIndex + 1);
	mCrtIndex++;
	valueStartIndex = mCrtIndex;
	return true;
}


// 
// Chen Ding, 09/19/2003, 2003-0262
//
OmnRslt
OmnXmlParser::nextCloseTag(const OmnString &tag, 
						   int &valueEndIndex,
						   const int fieldLength)
{
	//
	// From the current position, it searches for the first '</xxx>', where
	// 'xxx' == 'tag'.
	// If not found, it is an error.
	// If found, it sets 'valueEndIndex' to 
	// be the index right before '</'. If any error occurs, it restores
	// mCrtIndex to its original value.
	//

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	// If fieldLength > 0, the caller already knew how long the field is. 
	// Move the cursor from the current position to mCrtIndex + fieldLength.
	// </name> should immediately follow. Otherwise, it should go over 
	// searching for </name>.
	//
	int startIndex = mCrtIndex;

	if (fieldLength > 0)
	{
		mCrtIndex += fieldLength;
		valueEndIndex = mCrtIndex-1;

		// 
		// The next should be </name>
		//
		const char *buf = mData.data();
		if (buf[mCrtIndex] == '<' && buf[mCrtIndex+1] == '/')
		{
			// 
			// Check whether the tag matches.
			//
			mCrtIndex += 2;
			int tagLength = tag.length();
			int tagIndex = 0;
			while (tagIndex < tagLength && mCrtIndex < mLength 
				&& tag.data()[tagIndex] == buf[mCrtIndex])
			{
				tagIndex++;
				mCrtIndex++;
			}

			if (tagIndex >= tagLength)
			{
				//
				// It is a matching closing tag
				// Check whether the next is '>'
				//
				if (buf[mCrtIndex] == '>')
				{
					//
					// Found the matching tag. Move mCrtIndex to the next
					//
					mCrtIndex++;
					return true;
				}
				
				// 
				// It is an error
				//
				mCrtIndex = startIndex;
				OmnWarn	<< "Failed to get >: " 
					<< &buf[startIndex] << ". " << buf << enderr;
				return false;
			}
			else
			{
				// 
				// The tag name didn't match
				//
				mCrtIndex = startIndex;
				OmnWarn << "Mismatching tag name: " 
					<< &buf[startIndex] << ". " << buf << enderr;
				return false;
			}
		}
		else
		{
			// 
			// It is not "</". 
			// 
			mCrtIndex = startIndex;
			OmnWarn << "Expecting </ around: " 
				<< &buf[startIndex] << ". " << buf << enderr;
			return false;
		}
	}
	else
	{
		// 
		// To find the closing tag: </name>
		//
		const char *buf = mData.data();
		while (1)
		{
			while (mCrtIndex < mLength && buf[mCrtIndex] != '<') mCrtIndex++;

			if (mCrtIndex >= mLength)
			{
				//
				// It is a mal-functioned XML document
				//
				mCrtIndex = startIndex;
				OmnWarn << "Missing closing tag at "
			   			<< startIndex
						<<  " for tag: " << tag
						<< ": " << mData << enderr;
				return false;
			}

			//
			// Check whether the next is '/'
			//
			mCrtIndex++;
			if (mCrtIndex >= mLength)
			{
				//
				// It is a mal-functioned XML document
				//
				mCrtIndex = startIndex;
				OmnWarn << "Missing closing tag at the end: " << mData << enderr;
				return false;
			}

			if (buf[mCrtIndex] != '/')
			{
				//
				// Continue the search
				//
				continue;
			}

			//
			// Found '</' so far
			//
			valueEndIndex = mCrtIndex-2;
	
			mCrtIndex++;
			int tagLength = tag.length();
			int tagIndex = 0;
			while (tagIndex < tagLength && mCrtIndex < mLength 
				&& tag.data()[tagIndex] == buf[mCrtIndex])
			{
				tagIndex++;
				mCrtIndex++;
			}

			if (tagIndex >= tagLength)
			{
				//
				// It is a matching closing tag
				// Check whether the next is '>'
				//
				if (mCrtIndex >= mLength)
				{
					//
					// Out of bound
					//
					mCrtIndex = startIndex;
					OmnWarn << "Expecting a matching closing tag: <" << tag 
						<< "> but out of bound at: "
						<< mCrtIndex << ": " << mData << enderr;
					return false;
				}

				if (buf[mCrtIndex] == '>')
				{
					//
					// Found the matching tag. Move mCrtIndex to the next
					//
					mCrtIndex++;
					return true;
				}
			
				//
				// It is not a true maching tag. Continue the search.
				//
				continue;
			}
			else
			{
				//
				// It is not a matching tag. Continue the search
				//
				continue;
			}
		}
	}
}


OmnXmlItemPtr
OmnXmlParser::nextItem()
{
	//
	// It retrieves the next item. If any, it returns the item. Otherwise,
	// it throws an exception.
	//

	//
	// Retrieve the opening tag
	//
	int valueStartIndex, valueEndIndex;

	//
	// Retrieve the open tag
	//
	OmnString tag;

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	//
	// OmnRslt rslt = nextOpenTag(tag, valueStartIndex);
	int fieldLength;
	OmnRslt rslt = nextOpenTag(tag, valueStartIndex, fieldLength);
	if (!rslt)
	{
		//
		// No more. This does not necessarily mean an error. 
		//
		return 0;
	}

	//
	// Retrieve the close tag
	//
	// Chen Ding, 09/19/2003, 2003-0262
	//
	// rslt = nextCloseTag(tag, valueEndIndex);
	rslt = nextCloseTag(tag, valueEndIndex, fieldLength);
	if (!rslt)
	{
		//
		// Failed to find the close tag
		//
		OmnString err = "Failed to retrieve the close tag: ";
		err << tag << ". ";
		err << mData;
		OmnAlarm << err << enderr;
		OmnExcept e(OmnFileLine, rslt.getErrId(), err);

		throw e;
	}

	//
	// The value is in between
	//
	return OmnNew OmnXmlItem(tag, valueStartIndex, valueEndIndex, mData);
}


bool
OmnXmlParser::isStringValue() const
{
	//
	// If mData contains a string value for an atomic item, it returns true.
	// mData is a string value if it contains no '<' (we do not consider
	// escape yet). Otherwise, it returns false. If mData is an empty
	// string, it also returns true.
	//

	// 
	// Chen Ding, 05/30/2005
	// We should consider allowing "<" and ">" in a string value.
	// Simply comment these out.
	//
	//int index = 0;
	//const char *buf = mData.data();
	//while (index < mLength && buf[index] != '<') index++;

	//return (index >= mLength);
	return true;
}


bool
OmnXmlParser::isAllDigits(const bool flag) const 
{
	//
	// If mData contains only digits, return true. Otherwise, return false.
	// If the string is empty, it returns false. 
	// If 'flag' is false, the leading character can be '-', '+', and '.'. 
	//
	if (mLength == 0)
	{
		return false;
	}

	int index = 0;
	const char *buf = mData.data();
	if (!flag)
	{
		if (buf[0] != '-' && buf[0] != '+' && buf[0] != '.' &&
			(buf[0] < '0' || buf[0] > '9'))
		{
			return false;
		}

		index = 1;
	}
		
	while (index < mLength && buf[index] >= '0' && buf[index] <= '9') index++;

	return (index >= mLength);
}


long
OmnXmlParser::getLong() const 
{
	return atol(mData.data());
}


bool
OmnXmlParser::getBool() const 
{
	return (atoi(mData.data()) != 0);
}


int
OmnXmlParser::getInt() const 
{
	return atoi(mData.data());
}


int64_t
OmnXmlParser::getInt64() const 
{
	// 
	// Chen Ding, 09/08/2005
	//
	return aos_atoll(mData.data());
}


u64
OmnXmlParser::getUint64() const 
{
	// 
	// Chen Ding, 09/08/2005
	//
	// return OmnULL((char *)mData.data(), true);
	return aos_atoull(mData.data());
}


bool
OmnXmlParser::hasMore()
{
	//
	// If there is no more item (users should use ::nextItem()), it returns false.
	// Otherwise, it returns true.
	//
	int index = mCrtIndex;
	try
	{
		//
		// Try to get the next item. If success, it means there is more. 
		// (Remember to restore the index!)
		OmnXmlItemPtr item = nextItem();
		if (item.isNull())
		{
			//
			// There is no more item. 
			// 
			return false;
		}

		mCrtIndex = index;
		return true;
	}

	catch (const OmnExcept e)
	{
		//
		// This means there is no more
		//
OmnTrace << "********** " << endl;
		return false;
	}
}


OmnXmlItemPtr
OmnXmlParser::getItem(const OmnString &tag)
{
	//
	// It uses nextItem() to search for an item whose tag matches 'tag'. 
	// If not found, it throws an exception. 
	// If 'recursive' is true, it will search recursively.
	//
	reset();

	while (hasMore())
	{
		OmnXmlItemPtr item = nextItem();
		if (item.isNull())
		{
			//
			// No more item. 
			// 
			break;
		}

		//
		// Found the item
		//
		if (item->getTag() == tag)
		{
			//
			// Found it. 
			//
			return item;
		}
	}

	//
	// This means it does not contain the item.
	//
	//OmnTrace << "********** Failed to find the item: " 
	//	<< tag << endl;
	OmnString errMsg;
	errMsg << "XmlItemNotFound: " << tag;
	OmnExcept e(OmnFileLine, OmnErrId::eItemNotFound, errMsg);
	throw e;
}


OmnXmlItemPtr
OmnXmlParser::tryItem(const OmnString &tag)
{
    //
    // It uses nextItem() to search for an item whose tag matches 'tag'.
    // If not found, it returns 0.
    // If 'recursive' is true, it will search recursively.
    //
    reset();

    while (hasMore())
    {
        OmnXmlItemPtr item = nextItem();
		if (item.isNull())
		{
			// 
			// Didn't find the item
			//
			return 0;
		}

        if (item->getTag() == tag)
        {
            //
            // Found it.
            //
            return item;
        }
    }

    //
    // This means it does not contain the item.
    //
	return 0;
}


void
OmnXmlParser::removeComments()
{
	//
	// Comments must begin with "#" and end with new line
	//
	
	// 
	// Chen Ding, 05/10/2007
	// We should also remove the following line:
	//
	// <?xml version="1.0" encoding="ISO-8859-1" standalone="yes" ?>
	//
	if (mData.findSubString("<?xml version=", 0) == 0)
	{
		// 
		// It is a header line. Remove it.
		//
		int endPos = mData.findSubString("?>", 10);
		if (endPos < 10)
		{
			OmnAlarm << "Incorrect header line: " << mData << enderr;
		}
		else
		{
			mData.removeLine(0);
		}
	}
	
	OmnString str = mData;

	char *data1	= (char *)str.data();
	char *data2 = (char *)mData.data();

	int fromIndex = 0;
	int toIndex = 0;
	while (fromIndex < mData.length())
	{
		if (data2[fromIndex] == '#')
		{
			//
			// Make sure it is at the beginning of the string
			//
			if (fromIndex == 0)
			{
				//
				// it is at the beginning of the string. It is a 
				// comment. 
				//
				while (fromIndex < mData.length() && data2[fromIndex] != '\n') 
				{
					fromIndex++;
				}
				continue;
			}

			//
			// Make sure it is a leading #
			//
			int i = fromIndex-1;
			while (i >= 0 && (data2[i] == ' ' || data2[i] == '\t')) i--;
			if (i < 0 || data2[i] == '\n')
			{
				//
				// The previous character is new line. This means it is at the 
				// beginning of a new line. It is a comment.
				//
				while (fromIndex < mData.length() && data2[fromIndex] != '\n') 
				{
					fromIndex++;
				}
                continue;
            }

			//
			// Otherwise, it is not a comment.
			//
		}

		data1[toIndex++] = data2[fromIndex++];
	}

	data1[toIndex] = 0;

	mData.assign(data1, toIndex);

	mLength = mData.length();
}
			

int
OmnXmlParser::entries()
{
	int crtIndex = mCrtIndex;
	int numItems = 0;
	while (hasMore())
	{
		numItems++;
		nextItem();
	}

	mCrtIndex = crtIndex;
	return numItems;
}
