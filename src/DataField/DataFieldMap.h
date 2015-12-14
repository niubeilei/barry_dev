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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldMap_h
#define Aos_DataField_DataFieldMap_h

#include "DataField/DataField.h"
#include <map>


struct AosMapInfo
{
	int64_t start;
	int64_t end;
};


class AosDataFieldMap : virtual public AosDataField
{
	OmnDefineRCObject;

	OmnString				mDefault;
	map<int, AosMapInfo>	mMapInfo;
	map<int, OmnString>		mMap;

public:
	AosDataFieldMap(const bool reg);
	AosDataFieldMap(const AosDataFieldMap &rhs);
	~AosDataFieldMap();
	
	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value, 
						const bool copy_flag,
						AosRundata* rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const;

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

	bool			mapData(
						const int64_t &source,
						OmnString &dst);
};

#endif

