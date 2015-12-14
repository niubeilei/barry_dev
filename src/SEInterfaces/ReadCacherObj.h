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
#ifndef Aos_SEInterfaces_ReadCacherObj_h
#define Aos_SEInterfaces_ReadCacherObj_h

#include "SEInterfaces/DataCacherObj.h"


class AosReadCacherObj : virtual public AosDataCacherObj
{
protected:
	static AosReadCacherObjPtr		smReadCacher;

public:
	// Read cacher interface
	virtual bool 	nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata) = 0;
	virtual bool	nextValue(AosValueRslt &value, const AosRundataPtr &rdata) = 0;
	virtual bool	nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata) = 0;
	virtual bool	firstValueInRange(
							const int64_t &idx, 
							AosValueRslt &value, 
							const AosRundataPtr &rdata) = 0;
	virtual bool	nextValueInRange(AosValueRslt &value, const AosRundataPtr &rdata) = 0;
	virtual bool	firstRecordInRange(
							const int64_t &idx, 
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata) = 0;
	virtual bool	nextRecordInRange(
							const AosDataRecordObjPtr &record, 
							const AosRundataPtr &rdata) = 0;

	virtual AosReadCacherObjPtr convertToReadCacher() {return this;}

	static void setReadCacher(const AosReadCacherObjPtr &obj);
	static AosReadCacherObjPtr getReadCacher(const AosReadCacherObjPtr &obj);
};

#endif

