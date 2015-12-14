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
// 12/20/2013 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldDocid_h
#define Aos_DataField_DataFieldDocid_h

#include "DataField/DataField.h"


class AosDataFieldDocid : virtual public AosDataField
{
	OmnDefineRCObject;

	u64					mDocids;
	int					mNumDocids;
	OmnString			mRcdTypeKey;
	int					mDocSize;
	AosXmlTagPtr		mRecordDoc;

	u32					mNumCubes;
	queue<u64>			mGroupDocids;

public:
	AosDataFieldDocid(const bool reg);
	AosDataFieldDocid(const AosDataFieldDocid &rhs);
	~AosDataFieldDocid();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return AosDataType::eU64;
	}

	virtual bool	isConst() const {return true;}
	virtual bool	needValueFromField() {return false;}
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

	u64 			getNextDocid(
						AosRundata *rdata,
						AosDataRecordObj *record);

	bool 			getDocid(AosRundata* rdata);

};

#endif

