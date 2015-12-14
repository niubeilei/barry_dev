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
//
// Modification History:
// 04/01/2009 Created by Sally Sheng
////////////////////////////////////////////////////////////////////////////////////
#include "LogServer/LogEntry.h"


AosLogEntry::AosLogEntry()
:
mSeqno("1"),
mApplication("app")
{
}


AosLogEntry::AosLogEntry(TiXmlNode *logentry) 
{
    TiXmlElement *entry = logentry->ToElement();
	mSeqno = entry->Attribute("seqno");
	mTime = entry->Attribute("time");
	mApplication = entry->Attribute("app");
	mUser = entry->Attribute("user");
	mType = entry->Attribute("type");
	TiXmlNode *desc = entry->FirstChild();
	TiXmlElement *descbody = desc->ToElement();
	mDesc = descbody->GetText();
    TiXmlNode *content = entry->FirstChild();
    TiXmlElement *contentbody = content->ToElement();
    mContents = contentbody->GetText();

}


AosLogEntry::~AosLogEntry()
{
}


