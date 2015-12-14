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
// 11/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocUtil_h
#define Aos_SmartDoc_SdocUtil_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include <vector>


class AosSdocUtil
{
public:
	static bool retrieveAttrValues(
			vector<OmnString> &m_values, 
			const AosXmlTagPtr &working_doc, 
			const AosXmlTagPtr &sdoc, 
			const OmnString &tagname,
			const AosRundataPtr &rdata);

	static void normalizeSha1(OmnString &objid);
};
#endif

