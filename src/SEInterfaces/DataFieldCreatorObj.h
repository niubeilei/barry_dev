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
// 07/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataFieldCreatorObj_h
#define Aos_SEInterfaces_DataFieldCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataFieldCreatorObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual AosDataFieldObjPtr createStrField(AosRundata *rdata) = 0;

	virtual AosDataFieldObjPtr createDataField(
							const AosXmlTagPtr &def, 
							const AosDataRecordObjPtr &record,
							AosRundata *rdata) = 0;
	
	virtual bool registerDataField(
							const OmnString &name, 
							const AosDataFieldObjPtr &cacher) = 0;
};

#endif

