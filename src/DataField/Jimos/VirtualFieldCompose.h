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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_Jimos_VirtualFieldCompose_h
#define Aos_DataField_Jimos_VirtualFieldCompose_h

#include "SEInterfaces/Ptrs.h"
#include "DataField/DataField.h"



class AosVirtualFieldCompose : public AosDataField
{
	OmnDefineRCObject;

private:
	struct Field
	{
		int				mFieldIdx;
		AosValueRslt	mNullDefault;
		AosValueRslt	mEmptyDefault;
		bool			mNeedTrim;
		OmnString		mSep;
	};

	vector<Field>		mFields;
	AosExprObjPtr		mPreFilter;
	AosExprObj*			mPreFilterRaw;
	AosExprObjPtr		mPostFilter;
	AosExprObj*			mPostFilterRaw;

public:
	AosVirtualFieldCompose(const int version);
	~AosVirtualFieldCompose();

	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(const AosXmlTagPtr &def,
							AosDataRecordObj *record,
							AosRundata *rdata);

	virtual bool getFieldValue(
							AosRundata *rdata, 
							AosDataRecordObj *record, 
							AosValueRslt &value);

	virtual bool getValueFromRecord(
							AosDataRecordObj* record,
							const char * data,
							const int len,
							int &idx,
							AosValueRslt &value, 
							const bool copy_flag,
							AosRundata* rdata);
};

#endif
