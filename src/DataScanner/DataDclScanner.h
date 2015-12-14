////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_DclScanner_h
#define AOS_DataScanner_DclScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/DataCacherObj.h"

class AosDclScanner : public AosDataScanner
{
	OmnDefineRCObject;

private:
	AosDataCacherObjPtr mCacher;

public:
	AosDclScanner(const bool flag);
	AosDclScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDclScanner();

	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);

	virtual char * nextRecord(int &len, const AosRundataPtr &rdata);
	virtual bool   getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool   getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata){return false;}
	virtual AosDataRecordObjPtr getDataRecord() const;
	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;
	virtual int64_t getTotalSize() const;
};
#endif

