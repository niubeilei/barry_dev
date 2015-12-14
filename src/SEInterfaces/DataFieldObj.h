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
// 07/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataFieldObj_h
#define Aos_SEInterfaces_DataFieldObj_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Jimo/Jimo.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/StrValueInfo.h"
#include "SEInterfaces/DataFieldType.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util/DataTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Value/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosDataFieldObj : public AosJimo
{
public:
	AosStrValueInfo     mFieldInfo;

protected:
	static AosDataFieldObjPtr	smCreator;
	bool		mIsNull;

public:
	AosDataFieldObj();
	AosDataFieldObj(const int version);

	// DataField Interface
	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) const = 0;
	virtual AosDataFieldType::E getType() const = 0;
	virtual OmnString getTypeName() const = 0;
	virtual OmnString getName() const = 0;
	virtual bool isIgnoreSerialize() const = 0;

	// felicia, 2014/04/24 for record container
	virtual bool setValueToRecord(
					char * const data,
					const int data_len,
					const AosValueRslt &value, 
					bool  &outofmem,
					AosRundata* rdata) = 0;

	virtual bool setValueToRecord(
					AosDataRecordObj* record,
					const AosValueRslt &value, 
					bool  &outofmem,
					AosRundata* rdata) = 0;

	virtual bool getValueFromRecord(
					AosDataRecordObj* record,
					const char * data,
					const int len,
					int &idx,
					AosValueRslt &value, 
					const bool copy_flag,
					AosRundata* rdata) = 0;

	// Queries
	virtual bool needValueFromField() = 0;
	//virtual bool isConst() const = 0;
	bool isNull() const {return mIsNull;}

	// Others
	virtual bool clear() = 0;
	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const = 0;
	virtual AosDataFieldObjPtr createStrField(AosRundata *rdata) = 0;
	virtual bool config(
					const AosXmlTagPtr &worker,
					AosDataRecordObj *record, 
					AosRundata *rdata) = 0;

	virtual bool createRandomValue(
					const AosBuffPtr &buff,
					AosRundata *rdata) = 0;
	
	virtual bool serializeToBuff(
					const AosXmlTagPtr &doc, 
					const AosBuffPtr &buff,
					AosRundata *rdata) = 0;

	virtual bool serializeToXmlDoc(
					OmnString &docstr,
					int &idx, 
					const char *data, 
					const int data_len,
					AosRundata* rdata) = 0;

	virtual AosDataFieldObjPtr createDataField(
					const AosXmlTagPtr &def, 
					AosDataRecordObj *record,
					AosRundata *rdata) = 0;

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const = 0;

	void setNull() {mIsNull = true;}		// Chen Ding, 2014/08/16
	void setNotNull() {mIsNull = false;}		// Chen Ding, 2014/08/16

	// virtual AosValuePtr getFieldValue(AosRundata *rdata) = 0;

	virtual int		getMinFieldLength() const = 0; 
	// Static member functions
	static AosDataFieldObjPtr createStrFieldStatic(AosRundata *rdata);
	static AosDataFieldObjPtr createDataFieldStatic(
					const AosXmlTagPtr &def, 
					AosDataRecordObj *record,
					AosRundata *rdata);
	
	virtual bool isVirtualField() {return false;}
	static bool	isVirtualField(const AosXmlTagPtr &def);

	static void setCreator(const AosDataFieldObjPtr &creator) {smCreator = creator;}
	static AosDataFieldObjPtr getCreator() {return smCreator;}

	static vector<AosXmlTagPtr> getVirtualField(
							const AosXmlTagPtr &table_doc,
							const AosRundataPtr &rdata);

	static vector<AosXmlTagPtr> getVirtualFieldConf(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &tableDoc,
							const AosXmlTagPtr &def);

	static vector<AosXmlTagPtr> getMapDatafieldConf(
							const AosRundataPtr &rdata, 
							const AosXmlTagPtr &tableDoc,
							const AosXmlTagPtr &mapNode);

	// Chen Ding, 2015/01/24
	virtual AosDataFieldObjPtr createDataField(
					AosRundata *rdata, 
					const AosDataType::E data_type) = 0;
	virtual void 	setName(const OmnString &name) = 0;


	//yang,2015/08/13
	virtual bool setNullFlagToRecord(
						AosRundata *rdata,
						const int& idx,
						AosDataRecordObj *record,
						int bitmap_offset) = 0;

	//yang,2015/08/13
	virtual u64 getBitmapLen() = 0;


};

#endif

