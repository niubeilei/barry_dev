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
#ifndef Aos_DataFormat_DataElemStrRandom_h
#define Aos_DataFormat_DataElemStrRandom_h

#include "DataFormat/DataElem.h"
#include "DataFormat/DataProcStatus.h"
#include "IILClient/Ptrs.h"


class AosDataElemStrRandom : virtual public AosDataElem
{
	AosRandType::E	mStrType;
	int				mMinLen;
	char 			mPadding;
	bool			mLeftAlgin;

	bool			mIsUnique;

	int				mPhoneNumLen;
	bool			mWithCountryCode;
	
	int				mMinYear;
	int				mMaxYear;

	OmnString		mValue;
	OmnString		mCrtValue;
	OmnString		mPrevValue;
	int64_t			mCrtCount;
	int64_t			mPrevCount;

public:
	AosDataElemStrRandom(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosDataElemStrRandom();
	
	virtual bool 	setRecordByPrevValue(const AosDataRecord *record);
	virtual bool	setValueToRecord(
						const AosDataRecord *record,
						const AosValueRslt &value, 
						const AosRundataPtr &rdata);

	virtual bool	getValueFromRecord(
						const AosDataRecord *record,
						AosValueRslt &value, 
						const bool copy_flag,
						const AosRundataPtr &rdata);

	virtual bool	serializeFrom( 	
						OmnString &docstr,
						int &idx, 
						const char *data, 
						const int datalen, 
						const AosRundataPtr &rdata);

	virtual bool	serializeTo(
						const AosXmlTagPtr &doc, 
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	
	virtual AosDataProcStatus::E convertData(
						const char *record,
						const int data_len,
						int &idx,
						const bool need_convert,
						const AosBuffPtr &buff, 
						const bool need_value,
						AosValueRslt &value,
						const AosRundataPtr &rdata);

	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	virtual bool saveAndClear();
	// virtual bool setRecord(const AosDataRecordPtr &record);
	virtual bool updateData(const AosDataRecordPtr &record, bool &updated);
	// virtual u64 getU64(const char *record, const u64 &dft) const;
	// virtual const char * getCharStr(const char *record, int &idx, int &len);

	// virtual bool 	setRecord(char *record, const int record_len, const char value);
	// virtual bool 	setRecord(char *record, const int record_len, const u32 value);
	// virtual bool 	setRecord(char *record, const int record_len, const int value);
	// virtual bool 	setRecord(char *record, const int record_len, const u64 &value);
	// virtual bool 	setRecord(char *record, const int record_len, const int64_t &value);
	// virtual bool 	setRecord(char *record, const int record_len, const double &value);
	// virtual bool 	setRecord(
	//  					char *record, 
	//  					const int record_len, 
	//  					const char *data, 
	//  					const int data_len);
	// virtual char   	getValueChar(const char *rcd, const int rlen, const char v);
	// virtual int		getValueInt(const char *rcd, const int rlen, const int v);
	// virtual u32		getValueU32(const char *rcd, const int rlen, const u32 v);
	// virtual int64_t	getValueInt64(const char *rcd, const int rlen, const int64_t &v);
	// virtual u64		getValueU64(const char *rcd, const int rlen, const u64 &v);
	// virtual double 	getValueDouble(const char *rcd, const int rlen, const double &);
	// virtual OmnString getValueOmnStr(const char *rcd, const int rlen, const OmnString &);
	// virtual const char *getValueCharStr(
	// 					const char *rcd, 
	// 					const int rcd_len, 
	// 					int &len, 
	// 					const char *dft,
	// 					const int dft_len);
	virtual AosDataType::E getDataType() const {return AosDataType::eString;}

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	inline void setCrtValue(const char *record)
	{
		if (mIsConst)
		{
			mCrtValue = mValue;
		}
		else
		{
			mCrtValue.assign(&record[mOffset], mDataLen);
		}
	}

	inline bool setValue(char *record, const int len)
	{
		memset(&record[mOffset], ' ', mDataLen);
		if (mPrevValue.length() > 0)
		{
			strncpy(&record[mOffset], mPrevValue.data(), mPrevValue.length());
		}
		return true;
	}
};

#endif
#endif
