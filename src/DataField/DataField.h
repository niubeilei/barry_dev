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
#ifndef Aos_DataField_DataField_h
#define Aos_DataField_DataField_h

#include "Conds/Ptrs.h"
#include "SEUtil/StrValueInfo.h"
#include "DataField/FieldUtil.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEUtil/FieldOpr.h"
#include "Util/UtUtil.h"


class AosDataField : public AosDataFieldObj
{
protected:
	AosDataFieldType::E	mType;
	OmnString			mTypeName;
	OmnString			mName;
	bool				mIgnoreSerialize;
	bool				mIsFixed;
	bool				mIsConst;
	OmnString			mConstValueStr;
	int					mValueFromFieldIdx;

	AosValueRslt		mDftValue;

public:
	AosDataField(
			const AosDataFieldType::E type,
			const OmnString &name,
			const bool flag);
	AosDataField(const AosDataField &rhs);
	~AosDataField();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		OmnShouldNeverComeHere;
		return AosDataType::eInvalid;	
	}

	virtual AosDataFieldType::E getType() const {return mType;}
	virtual OmnString getTypeName() const {return mTypeName;}
	virtual OmnString getName() const {return mName;}
	virtual void 	setName(const OmnString &name) {mName = name;}
	virtual bool	isIgnoreSerialize() const {return mIgnoreSerialize;}

	virtual bool	clear() {return true;}
	virtual bool	getDefaultIgnoreSerialize() {return true;}
	virtual bool	needValueFromField() {return true;}

	virtual bool	setValueToRecord(
						char * const data,
						const int data_len,
						const AosValueRslt &value, 
						bool &outofmem,
						AosRundata* rdata);

	virtual bool	setValueToRecord(
						AosDataRecordObj* record,
						const AosValueRslt &value, 
						bool &outofmem,
						AosRundata* rdata);

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value, 
						const bool copy_flag,
						AosRundata* rdata);

	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	serializeToBuff(
						const AosXmlTagPtr &doc, 
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	serializeToXmlDoc(
						OmnString &docstr,
						int &idx, 
						const char *data, 
						const int data_len,
						AosRundata* rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const = 0;

	virtual AosDataFieldObjPtr createDataField(
						const AosXmlTagPtr &def, 
						AosDataRecordObj *record,
						AosRundata *rdata);

	virtual AosDataFieldObjPtr createStrField(AosRundata *rdata);

	// Chen Ding, 2013/12/27
	// virtual AosValuePtr getFieldValue(AosRundata *rdata);

	static bool init();

	virtual int		getMinFieldLength() const {OmnShouldNeverComeHere; return 0;}

	AosJimoPtr cloneJimo() const {return 0;}

protected:
	bool registerDataField(
				const AosDataFieldType::E type,
				const OmnString &name, 
				AosDataField * field);

	virtual bool config(
				const AosXmlTagPtr &def,
				AosDataRecordObj *record,
				AosRundata *rdata);
   
	virtual bool isVirtualField();


public:
	// Chen Ding, 2015/01/24
	virtual AosDataFieldObjPtr createDataField(
					AosRundata *rdata, 
					const AosDataType::E data_type);


	//yang,2015/08/13
	virtual bool setNullFlagToRecord(
						AosRundata *rdata,
						const int& idx,
						AosDataRecordObj *record,
						int bitmap_offset)
	{
		return true;
	}

	//yang,2015/08/13
	virtual u64 getBitmapLen()
	{
		return -1;
	}

};

#endif
