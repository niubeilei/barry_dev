////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 3/19/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProgramAid_Field_h
#define Aos_ProgramAid_Field_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosField : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	OmnString	mFieldName;
	OmnString	mDataTypeName;
	OmnString	mDataName;
	OmnString	mDefaultValue;
	bool		mIsPrimaryKey;

public:
	AosField();
	AosField(const OmnString &fieldname, 
			const OmnString &tname, 
			const OmnString &dataname, 
			const OmnString &keyFlag, 
			const OmnString &dflt);
	~AosField();

	OmnString	getFieldName() const {return mFieldName;}
	OmnString	getMemDataTypeName() const {return mDataTypeName;}
	OmnString	getMemDataName(const bool capital) const;
	OmnString	getMemDataName() const;
	OmnString	getDefaultValue() const;
	OmnString	getRecordTypeName() const;
	bool		isKey() const {return mIsPrimaryKey;}
	OmnString	getHeader() const;
	OmnString	getSingleQuote() const;
	bool		isIntegralType() const;
	bool		isStrType() const;
	bool		isFloatType() const;
private:
};
#endif

