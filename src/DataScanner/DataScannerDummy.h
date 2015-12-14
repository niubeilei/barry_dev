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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_DataScannerDummy_h
#define AOS_DataScanner_DataScannerDummy_h

#include "DataScanner/DataScanner.h"
#include "Thread/Ptrs.h"


class AosDataScannerDummy : virtual public AosDataScanner
{
	OmnDefineRCObject;
	
public:
	AosDataScannerDummy(const u32 version);

	~AosDataScannerDummy();

	// Data Scanner Interface
	virtual int 	getPhysicalId() const;
	virtual bool 	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata);
	virtual int64_t	getTotalSize() const;
	virtual AosJimoPtr cloneJimo()  const;
};
#endif
