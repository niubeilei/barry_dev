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
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_Jimos_StatDoc_h
#define Aos_DataRecord_Jimos_StatDoc_h

#include "DataRecord/DataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/JimoType.h"


class AosStatDoc : public AosDataRecord
{
	OmnDefineRCObject;

public:
	AosStatDoc(const OmnString &version);
	~AosStatDoc();

	// AosJimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecord interface
	virtual bool		isFixed() const {return true;}
	virtual int			getRecordLen() const {return mRecordLen;}
	virtual int			getDataLen() const {return mDataLen;}
	virtual char *		getData() {return mData;}
	virtual bool 		setData(char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const bool need_copy);
	virtual void 		clear();
	AosDataRecordType::E getType() const {return mType;}

	virtual bool config(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc, 
						const AosXmlTagPtr &jimo_doc);

	virtual AosDataRecordObjPtr clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;

	virtual AosXmlTagPtr serializeToXmlDoc(
			               	const char *data,
							const int data_len,
							const AosRundataPtr &rdata);

	virtual AosBuffPtr	serializeToBuff(
			               	const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata);

	virtual bool		setFieldValue(
							const int idx, 
							const AosValueRslt &value, 
							const AosRundataPtr &rdata);

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual bool		getFieldValue(
							const char *data,
							const int data_len,
							const OmnString &field_name,
							AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual bool		getFieldValue(
							const char *data,
							const int data_len,
							const int field_idx,
							AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual bool		createRandomDoc(
							const AosBuffPtr &buff,
							const AosRundataPtr &rdata);

private:
};

#endif

