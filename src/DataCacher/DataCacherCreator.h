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
#ifndef Aos_DataxCacher_DataCacherCreator_h
#define Aos_DataxCacher_DataCacherCreator_h

#include "SEInterfaces/DataCacherCreatorObj.h"


class AosDataCacherCreator : public AosDataCacherCreatorObj
{
	OmnDefineRCObject;

public:
	virtual bool registerCacher(const OmnString &name, const AosDataCacherObjPtr &cacher);
	
	virtual AosDataCacherObjPtr createCacher(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	virtual AosDataCacherObjPtr createCacher(
	 						const AosDataScannerObjPtr &scanner,
	 						const AosDataBlobObjPtr &blob, 
	 						const AosRundataPtr &rdata);

	virtual AosDataCacherObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata);
private:
	bool init(const AosRundataPtr &rdata);
};
#endif

