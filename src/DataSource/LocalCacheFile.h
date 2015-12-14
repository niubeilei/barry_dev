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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSource_DsLocalCacheFile_h
#define Aos_DataSource_DsLocalCacheFile_h

#include "DataSource/DataSource.h"

class AosDsLocalCacheFile : virtual public AosDataSource
{
	OmnDefineRCObject;

private:
	AosDataCacherObjPtr		mDataCacher;

public:
	AosDsLocalCacheFile(const bool regflag);
	AosDsLocalCacheFile(const AosDsLocalCacheFile &rhs);
	AosDsLocalCacheFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosDsLocalCacheFile();

	virtual bool 	create(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool 	destroy(const AosRundataPtr &rdata);
	virtual bool 	open(const AosRundataPtr &rdata);
	virtual bool 	close(const AosRundataPtr &rdata);
	virtual bool 	appendEntry(const AosValueRslt &value, const AosRundataPtr &);
	virtual bool 	appendRecord(const AosDataRecord &record, const AosRundataPtr &);
	virtual bool 	appendData(const AosBuffPtr &buff, const AosRundataPtr &);
	virtual char *	nextValue(int &len);
	virtual bool	nextValue(AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool	nextRecord(const AosDataRecordPtr &rec, const AosRundataPtr &rdata);
	virtual AosDsLocalCacheFilePtr clone();
	virtual AosDsLocalCacheFilePtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool copyMemberData(const AosDsLocalCacheFilePtr &obj);
};
#endif

