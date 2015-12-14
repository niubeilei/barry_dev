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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_InmemCounterCreatorObj_h
#define Aos_SEInterfaces_InmemCounterCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosInmemCounterCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerCounter(const OmnString &name, AosInmemCounterObj *cacher) = 0;
	virtual AosInmemCounterObjPtr createCounter(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
};
#endif

