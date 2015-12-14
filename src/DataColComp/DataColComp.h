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
// 09/03/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataColComp_DataColComp_h
#define Aos_DataColComp_DataColComp_h

#include "DataColComp/Ptrs.h"
#include "DataColComp/DataColCompType.h"
#include "Rundata/Ptrs.h"
#include "Util/ValueRslt.h"

class AosDataColComp : virtual public OmnRCObject
{
	OmnDefineRCObject;        

private:
	AosDataColCompType::E		mType;

public:
	AosDataColComp(
			const OmnString &name, 
			const AosDataColCompType::E type,
			const bool flag);
	AosDataColComp(const AosDataColComp &rhs);
	~AosDataColComp();

	virtual bool convertToInteger(
			const char *data, 
			const int len, 
			AosValueRslt &valueRslt,
			AosDataType::E &data_type,
			const AosRundataPtr &rdata) = 0;

	virtual bool convertToStr(
			const char *data, 
			const int len, 
			AosValueRslt &valueRslt,
			const AosRundataPtr &rdata) = 0;

	virtual AosDataColCompPtr clone() const = 0;
	virtual AosDataColCompPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata) = 0;

	static AosDataColCompPtr createDataColComp(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

protected:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool registerColComp(const OmnString &name, const AosDataColCompPtr  &blob);
	static bool init(const AosRundataPtr &rdata);
};

#endif

