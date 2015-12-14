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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetByExternalDoc_h
#define Aos_Dataset_Jimos_DatasetByExternalDoc_h

#include "Dataset/Jimos/DatasetByDataScanner.h"


class AosDatasetByExternalDoc : public AosDatasetByDataScanner
{
	OmnString			mExternalField;
	AosDatasetObjPtr	mExternalDataset;

public:
	AosDatasetByExternalDoc(
		const OmnString &type, 
		const int version);
	AosDatasetByExternalDoc(const int version);
	~AosDatasetByExternalDoc();
	
	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool	setValueBuff(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);

};
#endif

