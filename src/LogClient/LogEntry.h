//////////////////////////////////////////////////////////////////////////
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
// 04/13/2009: Created by Sharon Shen
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_Client_LogEntry_h
#define AOS_Client_LogEntry_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Client/Ptrs.h"
#include "TinyXml/TinyXml.h"
#include "Util/OmnNew.h"
#include "Util/String.h"

#include <stdio.h>


class AosLogEntry : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxNode = 50
	};

protected:
	TiXmlElement  * mEntry;
	OmnString		mOper;
	OmnString		mValue;

public:
	AosLogEntry();
	~AosLogEntry();
	void			setOper(const OmnString oper);
	OmnString		getOper();
	TiXmlElement *  getRoot();
	bool		    addAttribute(const OmnString &pathName, const OmnString &attrValue);
	bool		    addTag(const OmnString &pathName, const OmnString &tagValue);
	TiXmlElement *  getEntryXml();

	AosLogEntry&	operator << (const OmnString &value);
	AosLogEntry&	operator << (const int &value);

};
#endif











