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
#ifndef Aos_SEInterfaces_WriteCacherObj_h
#define Aos_SEInterfaces_WriteCacherObj_h

#include "SEInterfaces/DataCacherObj.h"


class AosWriteCacherObj : public AosDataCacherObj
{
protected:

public:
	AosWriteCacherObj();
	AosWriteCacherObj(
			const OmnString &nmae, 
			const AosDataCacherType::E type, 
			const bool flag);
	~AosWriteCacherObj();

	virtual bool 	appendData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata) = 0;
	virtual bool 	appendRecord( const AosDataRecordObjPtr &record, const AosRundataPtr &) = 0;
	virtual bool 	appendEntry( const AosValueRslt &value, const AosRundataPtr &rdata) = 0;
	virtual bool 	finish(const AosRundataPtr &rdata) = 0;
};

#endif

