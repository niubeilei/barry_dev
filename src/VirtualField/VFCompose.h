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
#ifndef Aos_VirtualField_VFCompose_h
#define Aos_VirtualField_VFCompose_h

#include "SEInterfaces/Ptrs.h"
#include "VirtualField/VirtualField.h"



class AosVFCompose : public AosVirtualField
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
	AosExprObjPtr		mFilter;
	AosExprObj*			mFilterRaw;
	AosExprObjPtr		mRsltFilter;
	AosExprObj*			mRsltFilterRaw;

public:
	AosVFCompose(const int version);
	~AosVFCompose();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool getFieldValue(
							AosRundata *rdata, 
							AosDataRecordObj *record, 
							AosValueRslt &value);
};

#endif
