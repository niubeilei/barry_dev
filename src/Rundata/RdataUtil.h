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
// 08/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Rundata_RdataUtil_h
#define Aos_Rundata_RdataUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


class AosRdataUtil
{
public:
	static AosXmlTagPtr getCommand(const AosRundataPtr &rdata)
	{
		AosXmlTagPtr root = rdata->getRequestRoot();
		if (!root) return 0;
		AosXmlTagPtr child = root->getFirstChild();
		aos_assert_r(child, 0);
		child = child->getFirstChild("command");
		if (!child) return 0;
		return child->getFirstChild();
	}

	static AosXmlTagPtr getObjdef(const AosRundataPtr &rdata)
	{
		AosXmlTagPtr received_doc = rdata->getReceivedDoc();
		if (!received_doc) return 0;
		return received_doc->getFirstChild("objid");
	}

	// Chen Ding, 2015/02/02
	static void normalizeTagname(OmnString &tagname)
	{
		// This function should be called by Rundata only. 
		// Tag names must be made of digits, letters, 
		// '_', '-', '.', or ':'. All other characters are
		// ignored.
		char *data = (char *)tagname.data();
		int len = tagname.length();
		int mod_idx = 0;
		for (int i=0; i<len; i++)
		{
			char c = data[i];
			if ((c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				c == '_' ||
				c == '-' ||
				c == '.' ||
				c == ':')
			{
				data[mod_idx++] = data[i];
			}
		}
		tagname.setLength(mod_idx);
	}
};
#endif
