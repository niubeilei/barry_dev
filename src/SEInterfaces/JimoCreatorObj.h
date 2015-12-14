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
// 09/29/2012 Created by Chen Ding
// 2013/05/29 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_JimoCreatorObj_h
#define Aos_SEInterfaces_JimoCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosJimoCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerJimo(const OmnString &name, AosJimoObj *cacher) = 0;
	virtual AosJimoObjPtr createJimo(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	virtual AosJimoObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
};
#endif
#endif
