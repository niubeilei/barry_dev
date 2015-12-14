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
// 07/15/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlUtil/HtmlUtil.h"

#include "HtmlUtil/HtmlRc.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"


static u32 sgId = 1000;
static OmnMutex sgLock;

OmnString AosGetHtmlElemId()
{
	sgLock.lock();
	u32 id = sgId++;
	sgLock.unlock();
	OmnString ss;
	ss << id;
	return ss;
}


int AosGetXmlValue(
			const AosXmlTagPtr &boundobj, 
			const OmnString &bind, 
			const OmnString &dft_value, 
			OmnString &value, 
			AosXmlTagPtr &xml)
{
	xml = 0;
	if (!boundobj || bind == "") 
	{
		value = dft_value;
		return eAosRc_Ok;
	}

	bool exist;
	value = boundobj->xpathQuery(bind, exist, dft_value);
	return eAosRc_Ok;
}


