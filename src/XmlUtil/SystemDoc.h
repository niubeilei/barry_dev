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
// 09/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_SystemDoc_h
#define Aos_XmlUtil_SystemDoc_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"


class AosSystemDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosXmlTagPtr	mDoc;

public:
	AosSystemDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	~AosSystemDoc() {}

	static bool isSystemDoc(const AosXmlTagPtr &doc)
	{
		aos_assert_r(doc, false);
		return doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_SYSTEM_DOC;
	}

	u64 getOwnerDocid() const
	{
		aos_assert_r(mDoc, 0);
		return mDoc->getAttrU64(AOSTAG_OWNER_DOCID, 0);
	}

	OmnString getAttrStr(const OmnString &name, const OmnString &dft)
	{
		if (!mDoc) return dft;
		return mDoc->getAttrStr(name, dft);
	}

	u64 getAttrU64(const OmnString &name, const u64 &dft)
	{
		if (!mDoc) return dft;
		return mDoc->getAttrU64(name, dft);
	}

	AosXmlTagPtr getDoc() const {return mDoc;}
};
#endif

