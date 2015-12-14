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
// 2015/9/11 Created by xuqi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetMatrix_h
#define Aos_Dataset_Jimos_DatasetMatrix_h

#include "SEInterfaces/DatasetMatrixObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "Util/DataTypes.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/Ptrs.h"

class AosDatasetMatrix : public AosDatasetMatrixObj
{
	OmnDefineRCObject;

private:
	u32 				   mRowsNum;
    u32         		   mColumnsNum;
	u64					   mTaskDocid;
    AosBuffPtr             mBuff;
	AosSchemaObjPtr        mSchema;

protected:
	AosRecordsetObjPtr			mRecordset;
public:
	AosDatasetMatrix(
			const OmnString &type,
			const int version);
	
	AosDatasetMatrix(const int version);

	
	~AosDatasetMatrix();

	virtual AosJimoPtr cloneJimo()  const;

    bool initBuff();

	bool config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc);

	bool setCellValue(
			AosRundata *rdata,
			const u32 &curColumnsNum,
			const u32 &curRowsNum,
			const AosValueRslt &value);
	
	bool nextRecordset(
			const AosRundataPtr &rdata,
			AosRecordsetObjPtr &recordset);

	virtual AosRecordsetObjPtr  cloneRecordset(AosRundata *rdata){return mRecordset->clone(rdata);}

	virtual void setTaskDocid(const u64 task_docid){mTaskDocid = task_docid;};

	virtual bool	getRecords(vector<AosDataRecordObjPtr> &records);
	
};
#endif

