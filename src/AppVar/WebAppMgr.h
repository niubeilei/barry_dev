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
// Modification History:
// 07/15/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AppVar_WebAppMgr_h
#define Aos_AppVar_WebAppMgr_h

#include "AppVar/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosWebAppMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eNums = 100
	};

public:
	static int smNum;
	static bool createApp(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	static bool deleteApp(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	static bool resetApp(const OmnString &url, const AosRundataPtr &rdata);
	static bool modifyApp(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
};
#endif
