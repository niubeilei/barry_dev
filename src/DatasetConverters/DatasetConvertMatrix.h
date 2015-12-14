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
// 2015-08-10 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DatasetConverters_DatasetConvertMatrix_h
#define Aos_DatasetConverters_DatasetConvertMatrix_h

#include "SEInterfaces/DatasetConverterObj.h"
#include "Util/ValueRslt.h"

class AosDatasetConvertMatrix : public AosDatasetConverterObj
{
private:
	AosDatasetObjPtr 			mMatrixDataset;
	OmnString					mXName;
	OmnString					mYName;
	OmnString					mCellName;
	AosDateTime					mXStartTime;
	AosDateTime					mYStartTime;
	//map<OmnString, int>			mXRange;
	//map<OmnString, int>			mYRange;

public:
	AosDatasetConvertMatrix(const int ver);
	AosDatasetConvertMatrix(const AosDatasetConvertMatrix &jimo);
	~AosDatasetConvertMatrix();

	virtual bool procData(
			AosRundata* rdata_raw,
			AosDatasetObj* dataset);

	virtual AosDatasetObjPtr getOutput() const;
	virtual AosJimoPtr 		cloneJimo() const;
	virtual bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
