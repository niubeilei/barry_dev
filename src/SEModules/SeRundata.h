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
// 08/11/2011	created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEModules_SeRundata_h
#define AOS_SEModules_SeRundata_h

#include "Rundata/Ptrs.h"
#include "Security/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "Util/String.h"


class AosSeRundata : virtual public AosAppRundata
{
	OmnDefineRCObject;

private:
	AosSmartDocObjPtr	mSmartdoc;
	AosXmlTagPtr		mSdoc;

public:
	static AosXmlTagPtr getSdoc(const AosRundataPtr &rdata);
};
#endif
#endif
