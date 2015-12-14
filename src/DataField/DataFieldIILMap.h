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
#ifndef Aos_DataField_DataFieldIILMap_h
#define Aos_DataField_DataFieldIILMap_h

#include "DataField/DataField.h"
#include "IILEntryMap/IILEntryMapMgr.h"


class AosDataFieldIILMap : virtual public AosDataField
{
	OmnDefineRCObject;

	bool				mHaveDefault;
	OmnString			mIILName;
	AosIILEntryMapPtr	mIILMap;
	bool				mEmptyKey;
	OmnString			mEmptyKeyValue;
	AosDataType::E      mDataType;

public:
	AosDataFieldIILMap(const bool reg);
	AosDataFieldIILMap(const AosDataFieldIILMap &rhs);
	~AosDataFieldIILMap();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{	
		return mDataType;
	}	

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

	virtual bool	needValueFromField();

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

};

#endif

