////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/19/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HtmlModules_HtmlRuntime_h
#define AOS_HtmlModules_HtmlRuntime_h

#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"


class AosHtmlRuntime : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosXmlTagPtr 	mVpd;

public:
	AosHtmlRuntime(const AosXmlTagPtr &vpd);
	~AosHtmlRuntime();

	AosXmlTagPtr getElemById(const OmnString &id);

	AosXmlTagPtr 
	getGicById(const OmnString &id)
	{
		// This is the same as getElemById(id)
		return getElemById(id);
	}

	AosXmlTagPtr 
	getVarById(const OmnString &id)
	{
		// This is the same as getElemById(id)
		return getElemById(id);
	}

	OmnString getNewElemId();
	OmnString getElemId(const AosXmlTagPtr &vpd);

private:
};

#endif
