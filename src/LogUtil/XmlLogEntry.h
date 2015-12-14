////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet LogEntryering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet LogEntryering, Inc. or its derivatives
//
// Modification History:
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogUtil_XmlLogEntry_h
#define AOS_LogUtil_XmlLogEntry_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosXmlLogEntry
{
private:
	OmnString	mContents;

public:
	AosXmlLogEntry();
	~AosXmlLogEntry();

	AosXmlLogEntry & operator << (const OmnString &ss)
	{
		mContents.append(ss.data(), ss.length());
		return *this;
	}
	
	AosXmlLogEntry & operator << (ostream & (*f)(ostream &outs))
	{
		mContents << "</entry>";
		return *this;
	}

	void finish()
	{
		mContents << "</entry>";
	}
	void reset();
	const char *getCharStr() const {return mContents.data();}
	int getStrLength() const {return mContents.length();}
};
#endif

