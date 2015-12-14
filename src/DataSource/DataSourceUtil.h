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
#ifndef Aos_DataSource_DataSource_h
#define Aos_DataSource_DataSource_h

#include "DataSource/DataSourceType.h"
#include "DataSource/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/ValueRslt.h"

class AosDataSource : virtual public OmnRCObject
{
protected:
	AosDataSourceType::E		mType;

public:
	AosDataSource(
			const OmnString &name, 
			const AosDataSourceType::E type, 
			const bool regflag);
	~AosDataSource();

	virtual bool 	create(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;
	virtual bool 	destroy(const AosRundataPtr &rdata) = 0;
	virtual bool 	open(const AosRundataPtr &rdata) = 0;
	virtual bool 	close(const AosRundataPtr &rdata) = 0;
	virtual bool 	appendEntry(const AosValueRslt &value, const AosRundataPtr &) = 0;
	virtual bool 	appendRecord(const AosDataRecord &record, const AosRundataPtr &) = 0;
	virtual char *	nextValue(int &len) = 0;
	virtual bool	nextValue(AosValueRslt &value, const AosRundataPtr &rdata) = 0;
	virtual bool	nextRecord(const AosDataRecordPtr &rec, const AosRundataPtr &rdata) = 0;
	virtual AosDataSourcePtr clone() = 0;
	virtual AosDataSourcePtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;

	static AosDataSourcePtr createDataSource(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

protected:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool copyMemberData(const AosDataSourcePtr &obj);
	
private:
	static bool init(const AosRundataPtr &rdata);
	bool registerStore(const OmnString &name, AosDataSource *proc);
};

#endif

