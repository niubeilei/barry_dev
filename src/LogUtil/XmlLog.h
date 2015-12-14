////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Logering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Logering, Inc. or its derivatives
//
// Modification History:
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogUtil_XmlLog_h
#define AOS_LogUtil_XmlLog_h

#include "LogUtil/XmlLogEntry.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosXmlLog : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mContents;
	AosXmlLogEntry	mEntry;

public:
	AosXmlLog();
	~AosXmlLog();

	AosXmlLog & operator << (const OmnString &ss)
	{
		mEntry << ss;
		return *this;
	}
	
	AosXmlLog & operator << (ostream & (*f)(ostream &outs))
	{
		mEntry.finish();
		mContents.append(mEntry.getCharStr(), mEntry.getStrLength());
		mEntry.reset();
		return *this;
	}
};
#endif

