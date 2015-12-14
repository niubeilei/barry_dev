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
// 2015/09/11 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DatasetConverterObj_h
#define Aos_SEInterfaces_DatasetConverterObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"



class AosDatasetConverterObj : public AosJimo
{
	OmnDefineRCObject;

public:
	AosDatasetConverterObj(const u32 version)
	:
	AosJimo(AosJimoType::eDatasetConverter, version)
	{}
	AosDatasetConverterObj(const AosDatasetConverterObj &jimo)
	{
	}
	~AosDatasetConverterObj(){};

	virtual bool config(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) = 0;
	
	virtual bool procData(
					AosRundata *rdata_raw,
			        AosDatasetObj *dataset) = 0;

	virtual AosDatasetObjPtr getOutput() const = 0;
};
#endif

