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
// Created: 2011/02/11 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_Site_h
#define AOS_SEModules_Site_h

#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "XmlUtil/Ptrs.h"


class AosSite : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	u32 			mSiteid;
	OmnString		mLanguageCode;
	AosLocale::E	mDftLocale;

public:
	AosSite(const u32 siteid, const AosRundataPtr &rdata);
	AosSite(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	~AosSite();

	OmnString getUnknownCloudid(const AosRundataPtr &rdata) const;
	OmnString getGuestCloudid(const AosRundataPtr &rdata) const;
	OmnString getLanguageCode(const AosRundataPtr &rdata) const {return mLanguageCode;}
	AosLocale::E getDftLocale() const {return mDftLocale;}

	bool 	init(const AosRundataPtr &rdata);
};
#endif

