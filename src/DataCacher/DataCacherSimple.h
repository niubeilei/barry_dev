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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataCacher_DataCacherSimple_h
#define Aos_DataCacher_DataCacherSimple_h

#include "DataBlob/Ptrs.h"
#include "DataCacher/DataCacher.h"
#include "Thread/Ptrs.h"


class AosDataCacherSimple : public AosDataCacher
{

private:
	OmnMutexPtr			mLock;
	AosDataBlobPtr 		mBlob;

public:
	AosDataCacherSimple(const bool regflag);
	AosDataCacherSimple(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	~AosDataCacherSimple();

	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);

	virtual bool 	appendEntry(
							const AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual bool 	appendRecord(
							const AosDataRecord &record,
							const AosRundataPtr &rdata);

	virtual AosDataCacherPtr clone(
							const AosXmlTagPtr &config, 
							const AosRundataPtr &rdata);

	virtual char* 	nextValue(int&);
	virtual bool 	clear();
	virtual AosDataCacherPtr clone();

	AosDataBlobPtr getBlob(){return mBlob;}
private:
};

#endif
#endif
