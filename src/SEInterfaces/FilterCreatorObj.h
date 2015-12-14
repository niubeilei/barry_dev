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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataFilterCreatorObj_h
#define Aos_SEInterfaces_DataFilterCreatorObj_h

#include "XmlUtil/Ptrs.h"

class AosFilterCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerFilter(const OmnString &name, const AosFilterObjPtr &cacher) = 0;
	virtual AosFilterObjPtr createFilter(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
};
#endif

