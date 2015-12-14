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
// 2013/03/17 Created by Chen Ding
// 2014/01/03 Copied from DataRecord
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_DataletImp_h
#define Aos_Datalet_DataletImp_h

#include "SEInterfaces/Datalet.h"


class AosDataletImp : public AosDatalet
{
	OmnDefineRCObject;

protected:
	enum FieldStatus
	{
		eInvalid, 

		eFieldParsed,
		eFieldModified,
		eFieldDeleted,
		eCleanEmbeddedDoc,
		eDirtyEmbeddedDoc,
		eInValueVector,

		eMaxStatusEntry
	};

	struct FieldInfo
	{
		AosDataType::E	type;
		int				start_pos;
		int				len;
		int				index;
		FieldStatus		status;

		FieldInfo()
		:
		type(AosDataType::eInvalid),
		start_pos(-1),
		len(-1),
		index(-1),
		status(eInvalid)
		{
		}

		FieldInfo(const AosDataType::E t, 
				const int spos, 
				const int ln, 
				const int n, 
				const FieldStatus s)
		:
		type(t),
		start_pos(spos),
		len(ln),
		index(n),
		status(s)
		{
		}
	};

	typedef hash_map<const char *, FieldInfo, char_str_hash, compare_charstr> map_t;
	typedef hash_map<const char *, FieldInfo, char_str_hash, compare_charstr>::iterator mapitr_t;

	OmnString				mInternalData;
	char *					mData;
	int						mDataLen;
	int						mDataletLen;
	int						mRecordLen;
	bool					mIsDataParsed;
	FieldInfo *				mFieldInfo;
	int						mTotalFields;
	AosBuffPtr				mBuff;
	AosBuff *				mBuffRaw;
	map_t					mNameMap;
	char					mEndian;
	bool					mIsDirty;
	bool					mIgnoreDuplicatedFields;
	AosBuffDataPtr			mMetadata;
	vector<OmnString>		mFieldNames;
	vector<AosValueRslt>	mFieldValues;
	vector<AosDataletPtr>	mEmbeddedDocs;
	AosBuffDataPtr			mMetaData;
	i64						mOffset;

public:
	AosDataletImp(const u32 version);
	~AosDataletImp();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDatalet interface
	virtual bool		isFixed() const {return false;}
	virtual int			getDataletLen() const {return mDataletLen;}
	virtual void 		clear();

	virtual bool 		setData(
							char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const int64_t offset,
							const bool need_copy);

	virtual AosDataletPtr clone() const;

	virtual bool setFieldValue( AosRundata *rdata,
								const OmnString &field_name, 
								const AosValueRslt &value);

	virtual bool setFieldValue( AosRundata *rdata,
								const int field_idx, 
								const AosValueRslt &value); 

	virtual bool setFieldValue( AosRundata *rdata, 
								const char * const field_name, 
								const AosValueRslt &value); 

	virtual bool setFieldValueWPath( 
								AosRundata *rdata,
								const char * const name, 
								const AosValueRslt &value);

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *field_name, 
								AosValueRslt &value);

	virtual bool getFieldValue( AosRundata *rdata, 
								const int field_idx, 
								AosValueRslt &value);

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *data,
								const int data_len,
								const char *field_name,
								AosValueRslt &value);

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *data,
								const int data_len,
								const int field_idx,
								AosValueRslt &value);

	virtual bool getFieldValueWPath(
								AosRundata *rdata, 
								const char * const name, 
								AosValueRslt &value);

	virtual bool serializeToBuff(AosRundata *rdata, AosBuff *buff);

	virtual AosDataletType::E getType() const;

	/*
	virtual AosXmlTagPtr		getRecordDoc() const;
	virtual	u64 				getDocid() const;
	virtual u64					getDocidByIdx(const int idx) const;
	virtual	void 				setDocid(const u64 &docid);
	virtual bool 				setDocidByIdx(const int idx, const u64 &docid);
	virtual AosBuffDataPtr		getBuffedData();
	virtual int	getNumFields() const;
	virtual OmnString			getRecordName() const;
	virtual OmnString			getRecordObjid() const;

	virtual AosXmlTagPtr serializeToXmlDoc(
			               		const char *data,
								const int data_len,
								const AosRundataPtr &rdata);

	virtual AosBuffPtr serializeToBuff(
			               		const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata);
	
	virtual int	getFieldIdx( 	const OmnString &name,
								const AosRundataPtr &rdata);

	virtual AosXmlTagPtr getDataFieldConfig(
								const OmnString &name,
								const AosRundataPtr &rdata);
	
	virtual bool getRecordENV( 	const OmnString &name,
								OmnString &record_env,
								const AosRundataPtr &rdata);

	virtual int64_t getOffset() const;
	virtual void setTask(const AosTaskObjPtr &t);

	virtual bool isValid(		AosRundata *rdata);

	inline bool	setData(		const OmnString &record, 
								const int64_t offset,
								const bool need_copy)
	{
		return setData((char *)record.data(), record.length(), 0, offset, need_copy);
	}
	*/

private:
	bool	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def);

	bool	parseData(AosRundata *rdata);
	
	FieldInfo * getField(AosRundata *rdata, const char *name, int &idx);

	bool encodeFieldName(AosRundata *rdata, AosBuff *buff, const char *name)
	{
		// Name is encoded as:
		//  byte * + 0x00
		buff->setBuff(name, strlen(name)+1);
		return true;
	}
};

#endif

