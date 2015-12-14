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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCacherCreatorObj_h
#define Aos_SEInterfaces_DataCacherCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataCacherCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerCacher(const OmnString &name, const AosDataCacherObjPtr &cacher) = 0;
	virtual AosDataCacherObjPtr createCacher(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	virtual AosDataCacherObjPtr createCacher(
	 						const AosDataScannerObjPtr &scanner,
	 						const AosDataBlobObjPtr &blob, 
	 						const AosRundataPtr &rdata) = 0;

	virtual AosDataCacherObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
};
#endif

