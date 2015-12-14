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
// 2012/12/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Groupby_GroupField_h
#define Aos_Groupby_GroupField_h

#include "Groupby/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/FieldOpr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosGroupField : public OmnRCObject
{
	OmnDefineRCObject;

protected:
	enum FieldType
	{
		eInvalid,
		eU64,
		eStr
	};
	
	enum RawFormat
	{
		eRawFormat_Invalid,
		eRawFormat_U64,
		eRawFormat_Str
	};
	
	FieldType			mType;
	RawFormat			mRawFormat;
	OmnString			mName;
	
	AosFieldOpr::E		mFieldOpr;
	int					mFromDocidIdx;
	bool				mSetRecordDocid;
	int					mFromFieldIdx;
	int					mToFieldIdx;
	OmnString			mSep;
	
	int64_t				mCount;
	int64_t				mPrevCount;

public:
	AosGroupField(const FieldType type);
	~AosGroupField();
	
	virtual FieldType getType() const {return mType;}
	virtual OmnString getName() const {return mName;}
	
	virtual OmnString getDefaultRawFormat() {return "str";}
	
	virtual bool	updateData(
						const AosDataRecordObjPtr &record,
						bool &updated, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	setRecordByPrevValue(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata) = 0;
	virtual bool	saveAndClear() = 0;
	
	static AosGroupFieldPtr createFieldStatic(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

protected:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
