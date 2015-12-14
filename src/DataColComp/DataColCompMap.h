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
// 09/02/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataColComp_DataColCompMap_h
#define AOS_DataColComp_DataColCompMap_h

#include "DataColComp/DataColComp.h"
#include "Util/DataTypes.h"

class AosDataColCompMap : public AosDataColComp
{
	OmnDefineRCObject;

private:
	AosStr2Str_t                mMap;  
	OmnMutexPtr                 mLock; 
	AosDataType::E				mDataType;

public:
	AosDataColCompMap(const bool flag);
	AosDataColCompMap(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	AosDataColCompMap(const AosDataColCompMap &rhs);
	~AosDataColCompMap();

	virtual AosDataColCompPtr clone() const;
	virtual AosDataColCompPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool convertToStr(
			const char *data, 
			const int len, 
			AosValueRslt &valueRslt, 
			const AosRundataPtr &rdata);

	virtual bool convertToInteger(
			const  char *data, 
			const int len, 
			AosValueRslt &valueRslt, 
			AosDataType::E &data_type,
			const AosRundataPtr &rdata);
private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	int getValue(const char *data);
};
#endif

