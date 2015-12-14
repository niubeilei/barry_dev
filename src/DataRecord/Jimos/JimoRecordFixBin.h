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
// 07/23/2013 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_Jimos_JimoRecordFixBin_h
#define Aos_DataRecord_Jimos_JimoRecordFixBin_h

#include "DataRecord/Jimos/JimoDataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Jimo/JimoType.h"
#include "SEInterfaces/DataRecordObj.h"


class AosJimoRecordFixBin : public AosJimoDataRecord
{
	OmnString	mInternalData;
	char *		mData;
	int		 	mDataLen;
	int		 	mRecordLen;
	AosDataRecordType::E mType;

public:
	AosJimoRecordFixBin(const bool flag AosMemoryCheckDecl);
	AosJimoRecordFixBin(
			const AosJimoRecordFixBin &rhs,
			const AosRundataPtr &rdata AosMemoryCheckDecl);
	AosJimoRecordFixBin(
        const AosRundataPtr &rdata, 
        const AosXmlTagPtr &jimo_doc, 
        const OmnString &version);
	~AosJimoRecordFixBin();

	// AosJimo Interface
	virtual bool run(   const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)  const;
	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);
	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);

	// AosDataRecord interface
	virtual bool		isFixed() const {return true;}
	virtual int			getRecordLen() const {return mRecordLen;}
	virtual int			getDataLen() const {return mDataLen;}
	virtual char *		getData() {return mData;}
	virtual bool 		setData(char *data, const int len, const AosBuffDataPtr &metaData, const bool need_copy);
	virtual void 		clear();
	AosDataRecordType::E getType() const {return mType;}

	virtual bool config(const AosXmlTagPtr &worker, const AosRundataPtr &rdata)
	{
		return config(rdata, worker);
	}

	virtual AosDataRecordObjPtr clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const AosTaskObjPtr &task,
							const AosRundataPtr &rdata AosMemoryCheckDecl) const;

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
	bool 				init(         
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &jimo_doc);

	bool 				registerMethods();


};

#endif

