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
#ifndef Aos_DataFormat_DataElemU64_h
#define Aos_DataFormat_DataElemU64_h

#include "API/AosApi.h"
#include "DataFormat/DataElem.h"
#include "DataRecord/DataRecord.h"
#include "DataFormat/DataProcStatus.h"
#include "IILClient/Ptrs.h"


class AosDataElemU64 : virtual public AosDataElem
{
private:
	u64			mConstValue;
	u64			mCrtValue;
	u64			mPrevValue;
	u64			mCrtCount;
	u64			mPrevCount;
	int			mStrValueLen;
	int			mFromFieldIdx;
	int			mToFieldIdx;
	OmnString	mSeparator;
	OmnString	mConstValueStr;

public:
	AosDataElemU64(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata);
	~AosDataElemU64();
	
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
	virtual u64 getU64(const char *record, const u64 &dft) const;
	virtual const char * getCharStr(const char *record, int &idx, int &len);

	// virtual bool 	setRecord(char *record, const int record_len, const char value);
	// virtual bool 	setRecord(char *record, const int record_len, const u32 value);
	// virtual bool 	setRecord(char *record, const int record_len, const int value);
	// virtual bool 	setRecord(char *record, const int record_len, const u64 &value);
	// virtual bool 	setRecord(char *record, const int record_len, const int64_t &value);
	// virtual bool 	setRecord(char *record, const int record_len, const double &value);
	// virtual bool 	setRecord(
	// 					char *record, 
	// 					const int record_len, 
	// 					const char *data, 
	// 					const int data_len);
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
	virtual AosDataType::E getDataType() const {return AosDataType::eU64;}

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	inline u64 getU64Priv(const AosDataRecordPtr &record, const u64 &dft) const
	{
		if (mIsConst) return mConstValue;

		if (mFromDocidIdx >= 0)
		{
			return record->getDocid(mFromDocidIdx);
		}

		if (mRawFormat == eRawFormat_Str)
		{
			if (mFromFieldIdx < 0)
			{
				char *ptr;
		 		return strtoull(&record->getData()[mOffset], &ptr, 10);
			}

			// Chen Ding, 05/30/2012
			const char *vv = 0;
			int vv_len = 0;
			bool rslt = AosGetField(vv, vv_len, record->getData(), 
					record->getRecordLen(), mFromFieldIdx, mSeparator, 0);

			// Chen Ding, 06/15/2012
			// if (!rslt) return dft;
			if (!rslt || vv_len <= 0) return dft;
			if (!vv) return dft;
			char c = vv[vv_len];
			((char*)vv)[vv_len] = 0;
			u64 uu = AosStrToU64(vv);
			((char*)vv)[vv_len] = c;
			return uu;
		}
	 	return *(u64*)&record->getData()[mOffset];
	}

	inline u64 getU64Priv(const char *record, const u64 &dft) const
	{
		if (mIsConst) return mConstValue;

		if (mRawFormat == eRawFormat_Str)
		{
			char *ptr;
		 	return strtoull(&record[mOffset], &ptr, 10);
		}
	 	return *(u64*)&record[mOffset];
	}

	bool setValue(char *record, const u64 &value)
	{
		if (mRawFormat == eRawFormat_Str)
		{
			/*
			char buff[100];
			sprintf(buff, "%06llu", value);
			int len = strlen(buff);
			if (len > mStrValueLen)
			*/
			OmnNotImplementedYet;
			return false;
		}

		*(u64*)&record[mOffset] = value;
		return true;
	}


	inline void setCrtValue(const AosDataRecordPtr &record)
	{
		if (mIsConst)
		{
			mCrtValue = mConstValue;
			return;
		}

		mCrtValue = getU64Priv(record, 0);
	}
};

#endif
#endif
