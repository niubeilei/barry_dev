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
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#ifndef Aos_DataFormat_DataElem_h
#define Aos_DataFormat_DataElem_h

#include "alarm_c/alarm.h"
#include "Actions/Ptrs.h"
#include "Conds/Condition.h"
#include "Conds/Ptrs.h"
#include "DataFormat/DataFormatType.h"
#include "DataFormat/DataElemType.h"
#include "DataFormat/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEUtil/FieldOpr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DataTypes.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/SeXmlUtil.h"
#include "XmlUtil/Ptrs.h"

class AosValueRslt;

class AosDataElem : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	enum RawFormat
	{
		eRawFormat_Invalid,
		eRawFormat_U64,
		eRawFormat_Str
	};

	RawFormat			mRawFormat;
	AosDataElemType::E	mType;
	OmnString			mName;
	int					mDataLen;
	int					mOffset;
	bool				mIsConst;
	bool				mIsFixedPosition;
	AosConditionPtr		mFilter;
	bool				mHasValidValue;
	AosFieldOpr::E		mFieldOpr;
	int					mFromDocidIdx;
	bool				mSetRecordDocid;

public:
	AosDataElem(const AosDataElemType::E type);
	~AosDataElem();

	virtual bool	serializeFrom( 	
						OmnString &docstr,
						int &idx, 
						const char *data, 
						const int datalen, 
						const AosRundataPtr &rdata) = 0;

	virtual bool	serializeTo(
						const AosXmlTagPtr &doc, 
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata) = 0;

	virtual AosDataProcStatus::E convertData(
						const char *record,
						const int data_len,
						int &idx,
						const bool need_convert,
						const AosBuffPtr &buff, 
						const bool need_value,
						AosValueRslt &value,
						const AosRundataPtr &rdata) = 0;
			
	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;

	// These are the functions to let an element modify a record.
	// Each element is capable of determining how to modify, such as
	// whether to align, how, whether to pad, etc. 
	// virtual bool setRecord(const AosDataRecordPtr &record) = 0;
	// virtual bool setRecord(
	// 					char *record, 
	//					const int record_len, 
	//					const char *data, 
	//					const int data_len) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const char value) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const u32 value) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const int value) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const u64 &value) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const int64_t &value) = 0;
	// virtual bool 	setRecord(char *record, const int record_len, const double &value) = 0;
	virtual bool setRecordByPrevValue(const AosDataRecord *record) = 0;
	virtual bool setValueToRecord(
						const AosDataRecord *record, 
						const AosValueRslt &value, 
						const AosRundataPtr &rdata) = 0;
	virtual bool getValueFromRecord(
						const AosDataRecord *record, 
						AosValueRslt &value, 
						const bool copy_flag,
						const AosRundataPtr &rdata) = 0;

	// These are the functions to retrieve values from record. 
	// All elements are capable of retrieving values from record based
	// on their configurations.
	// virtual char   	getValueChar(const char *rcd, const int rlen, const char v) = 0;
	// virtual int		getValueInt(const char *rcd, const int rlen, const int v) = 0;
	// virtual u32		getValueU32(const char *rcd, const int rlen, const u32 v) = 0;
	// virtual int64_t	getValueInt64(const char *rcd, const int rlen, const int64_t &v) = 0;
	// virtual u64		getValueU64(const char *rcd, const int rlen, const u64 &v) = 0;
	// virtual double 	getValueDouble(const char *rcd, const int rlen, const double &) = 0;
	// virtual OmnString getValueOmnStr(const char *rcd, const int rlen, const OmnString &) = 0;
	// virtual const char *getValueCharStr(
	// 					const char *rcd, 
	// 					const int rlen, 
	// 					int &len, 
	// 					const char *dft,
	// 					const int dft_len) = 0;

	virtual bool saveAndClear() = 0;
	virtual bool updateData(const AosDataRecordPtr &record, bool &updated) = 0;
	virtual AosDataType::E getDataType() const = 0;

	int		getDataLen() const {return mDataLen;} 
	bool	isFixed() const {return mIsFixedPosition;}

	static AosDataElemPtr createElem(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);
	
protected:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif
#endif
