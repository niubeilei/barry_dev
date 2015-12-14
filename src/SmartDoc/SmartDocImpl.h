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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SmartDoc_SmartDocImpl_h
#define Aos_SmartDoc_SmartDocImpl_h

#include "SEInterfaces/SmartDocObj.h"

class AosSmartDocImpl : virtual public AosSmartDocObj
{
	OmnDefineRCObject;

public:
	AosSmartDocImpl();
	~AosSmartDocImpl();

	virtual bool runSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata);
	virtual bool runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosXmlTagPtr createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif
#endif
