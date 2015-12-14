////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlItem.cpp
// Description:
//	An XML Item is expressed by this class as:
//		Tag
//		Tag Contents (a string)   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "XmlParser/XmlItem.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/ErrId.h"
#include "Debug/Except.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlParser/XmlParser.h"




OmnXmlItem::OmnXmlItem(const OmnString &data)
{
	//
	// 'data' defines one and only one XML Item. If not, it is an error.
	// It throws an exception. Otherwise, it retrieves the item tag
	// and its value.
	//
	OmnXmlItemPtr item = mParser->nextItem();
	if (item.isNull())
	{
		//
		// Did not get any item.
		//
		OmnString str;
		str << "Trying to create an Item but failed: " << data;
		OmnAlarm << str << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eItemNotFound, str);
		throw e;
	}

	//
	// Otherwise, it successfully created one item. This should be the only
	// item 'data' defines. 
	//
	OmnXmlItemPtr anotherItem = mParser->nextItem();
	if (!anotherItem.isNull())
	{
		//
		// 'data' contains more than one item. This is wrong.
		//
		OmnString str;
		str << "Trying to create an item but failed. Data contains "
			<< "more than one item: " << data;
		OmnAlarm << str << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eItemNotFound, str);
		throw e;
	}

	//
	// Successful.
	//
	mTag = item->getTag();

	mParser->reset();
	int valueStartIndex;
	OmnString tag;

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	//
	// mParser->nextOpenTag(tag, valueStartIndex);
	int fieldLength;
	mParser->nextOpenTag(tag, valueStartIndex, fieldLength);

	int valueEndIndex;

	// 
	// Chen Ding, 09/19/2003, 2003-0262
	//
	// mParser->nextCloseTag(tag, valueEndIndex);
	mParser->nextCloseTag(tag, valueEndIndex, fieldLength);

	//
	// Now, we have determine the item's starting and ending index of
	// its value. Using this, we may remove the tag.
	//
    char *buff = (char *)data.data();
    int length = valueEndIndex - valueStartIndex+1;
    OmnString tmp(&buff[valueStartIndex], length);

    mParser->setData(tmp);
}


OmnXmlItem::OmnXmlItem(const OmnString &tag,
					   const uint valueStartIndex,
					   const uint valueEndIndex,
					   OmnString &data)
					   :
mTag(tag)
{
	if (valueStartIndex < 0)
	{
		OmnString str;
		str << "StartIndex = " << OmnStrUtil::itoa(valueStartIndex)
			<< ": " << data;
		OmnAlarm << str << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, str);
		throw e;
	}

	if (valueEndIndex >= (uint)data.length())
	{
		OmnString str;
		str << "EndIndex: " << OmnStrUtil::itoa(valueEndIndex)
			<< ": " << data;
		OmnAlarm << str << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, str);
		throw e;
	}

	char *buff = (char *)data.data();
	int length = valueEndIndex - valueStartIndex+1;
	OmnString tmp(&buff[valueStartIndex], length); 

	mParser = OmnNew OmnXmlParser(tmp);
}


// 
// Chen Ding, 08/26/2005
// The item's tag has been removed. "data" contains the tag contents.
// 
OmnXmlItem::OmnXmlItem(const OmnString &tag, 
					   const char *data, 
					   const unsigned int len)
:
mTag(tag)
{
	OmnString tmp(data, len);
	mParser = OmnNew OmnXmlParser(tmp);
}


OmnXmlItem::~OmnXmlItem()
{
}


OmnString
OmnXmlItem::getStr()
{
	//
	// If the contents contain no other tags, it is an atomic 
	// item. Return the value as string.
	// Otherwise, return the 'defaultValue'.
	//
	if (mParser->isStringValue())
	{
		return mParser->getContents();
	}
	else
	{
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, 
			OmnString("eFailedToGetStrValueFromXmlItem: ") <<
			 toString());
		throw e;
	}
}


OmnString
OmnXmlItem::getStr(const OmnString &itemName)
{
	return getItem(itemName)->getStr();
}


OmnIpAddr
OmnXmlItem::getIpAddr(const OmnString &name)
{
	return getItem(name)->getIpAddr();
}


OmnIpAddr
OmnXmlItem::getIpAddr()
{
	//
	// IP addresses are all expressed in the dot notation. Currently we
	// only support IPv4.
	//
	OmnString data = getStr();
	if (data.length() == 0)
	{
		//
		// No value found
		//
		OmnAlarm 
			<< "Failed to get value from Xml Item" << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
		throw e;
	}

	return OmnIpAddr(data, OmnIpAddr::eIPv4DotNotation);
}


OmnIpAddr
OmnXmlItem::getIpAddr(const OmnString &name, const OmnIpAddr &dv)
{
	try
	{
		OmnXmlItemPtr item = getItem(name);
		if(item)
		{
			return item->getIpAddr();
	
		}
		else
		{
			return dv;
		}
	}

	catch (const OmnExcept &)
	{
//		OmnAlarm << "Exception caught: " << e.getErrMsg() << enderr;
		return dv;
	}
}



long
OmnXmlItem::getLong()
{
	//
	// If the contents define an integer, it is an atomic item.
	// Return the value as long. Otherwise, return the defaultValue.
	//
	if (mParser->isAllDigits())
	{
		return mParser->getLong();
	}
	else
	{
		OmnAlarm 
			<< "Failed to get value" << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
		throw e;
	}
}


bool
OmnXmlItem::getBool()
{
	if (mParser->isAllDigits())
	{
	 	return mParser->getBool();
	}

	//
	// Chen Ding, 03/12/2007
	// If the contents is "true", return true. If it is "false", 
	// return false. Otherwise, raise an exception. 
	//
	OmnString str = getStr();
	if (str == "true") return true;
	if (str == "false") return false;
	
	OmnAlarm << "The tag is supposed to be a boolean tag, which "
		<< "can be either 'true' or 'false'. Make sure the tag "
		<< "is defined correctly: " << toString() << enderr;
	OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
	throw e;
	return false;
}


int
OmnXmlItem::getInt()
{
	//
	// If the contents define an integer, it is an atomic item.
	// Return the value as long. Otherwise, return the defaultValue.
	//
	if (mParser->isAllDigits(false))
	{
		return mParser->getInt();
	}
	else
	{
		OmnAlarm << "Failed to get value: " << toString() << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
		throw e;
	}
}


int64_t
OmnXmlItem::getInt64()
{
	//
	// If the contents define an integer, it is an atomic item.
	// Return the value as long. Otherwise, return the defaultValue.
	//
	if (mParser->isAllDigits())
	{
		return mParser->getInt64();
	}
	else
	{
		OmnAlarm 
			<< "Failed to get value" << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
		throw e;
	}
}


u64
OmnXmlItem::getUint64()
{
	//
	// If the contents define an integer, it is an atomic item.
	// Return the value as long. Otherwise, return the defaultValue.
	//
	if (mParser->isAllDigits())
	{
		return mParser->getUint64();
	}
	else
	{
		OmnAlarm 
			<< "Failed to get value" << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eProgError, toString());
		throw e;
	}
}


bool
OmnXmlItem::getBool(const OmnString &name)
{
	return getItem(name)->getBool();
}


char
OmnXmlItem::getChar(const OmnString &name, const char dflt)
{
	OmnXmlItemPtr item = getItem(name);
	if (item)
	{
		OmnString str = item->getStr();
		if (str.length() != 1)
		{
			OmnAlarm << "Contents are not a character: " << str << enderr;
			return dflt;
		}
		char *buff = (char *)str.data();
		return buff[0];
	}

	return dflt;
}


int64_t
OmnXmlItem::getInt64(const OmnString &name)
{
	return getItem(name)->getInt64();
}


u64
OmnXmlItem::getUint64(const OmnString &name)
{
	return getItem(name)->getUint64();
}


OmnXmlItemPtr	
OmnXmlItem::getItem(const OmnString &tag)
{
	//
	// If it is an atomic item, it returns null. Otherwise, it searches
	// for the specified member. If found, it returns the item. otherwise,
	// it returns null.
	//
	try
	{
		return mParser->getItem(tag);
	}

	catch (OmnExcept &e)
	{
		return 0;
	}
}


OmnXmlItemPtr
OmnXmlItem::tryItem(const OmnString &tag)
{
	return mParser->tryItem(tag);
}


bool
OmnXmlItem::isAtomicItem() const
{
	return mParser->isStringValue();
}


void
OmnXmlItem::reset()
{
	mParser->reset();
}


OmnXmlItemPtr
OmnXmlItem::next()
{
	return mParser->nextItem();
}


int 
OmnXmlItem::entries() const
{
	return mParser->entries();
}


bool
OmnXmlItem::hasMore()
{
	//
	// If there is no more item (users should use ::nextItem()), it returns false.
	// Otherwise, it returns true.
	//
	return mParser->hasMore();
}


OmnString
OmnXmlItem::toString() const
{
	return OmnString("<") << mTag 
			<< ">" << mParser->toString() << "</" << mTag << ">";
}


OmnString
OmnXmlItem::getStr(const OmnString &name, const OmnString &dv)
{
	OmnXmlItemPtr item = mParser->tryItem(name);
	if (item.isNull())
	{
		return dv;
	}
	else
	{
		try
		{
			return item->getStr();
		}

		catch (...)
		{
			return dv;
		}
	}
}


int
OmnXmlItem::getInt(const OmnString &name, const int dv)
{
    OmnXmlItemPtr item = tryItem(name);
	if (item.isNull())
	{
		//
		// Did not find the item.
		//
		return dv;
	}

    try
	{
		return item->getInt();
	}

	catch (OmnExcept &e)
	{
		OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
		return dv;
	}
}


int64_t
OmnXmlItem::getInt64(const OmnString &name, const int64_t &dv)
{
    OmnXmlItemPtr item = tryItem(name);
	if (item.isNull())
	{
		//
		// Did not find the item.
		//
		return dv;
	}

    try
	{
		return item->getInt64();
	}

	catch (OmnExcept &e)
	{
		OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
		return dv;
	}
}


u64
OmnXmlItem::getUint64(const OmnString &name, const u64 &dv)
{
    OmnXmlItemPtr item = tryItem(name);
	if (item.isNull())
	{
		//
		// Did not find the item.
		//
		return dv;
	}

    try
	{
		return item->getUint64();
	}

	catch (OmnExcept &e)
	{
		OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
		return dv;
	}
}


bool
OmnXmlItem::getBool(const OmnString &name, const bool dv)
{
    OmnXmlItemPtr item = tryItem(name);
	if (item.isNull())
	{
		//
		// Did not find the item.
		//
		return dv;
	}

    try
	{
		return item->getBool();
	}

	catch (OmnExcept &e)
	{
		OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
		return dv;
	}
}


// 
// 02/19/2007, Chen Ding
// 
bool
OmnXmlItem::readFromFile(const OmnString &filename)
{
	OmnFile f(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the file: " << filename << enderr;
		return false;
	}

	OmnString data;
	f.readToString(data);

	mParser = OmnNew OmnXmlParser(data);
	mTag = "";
	return true;
}


