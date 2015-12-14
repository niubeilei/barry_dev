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
#ifndef Aos_Dataset_DatasetDummy_h
#define Aos_Dataset_DatasetDummy_h

#include "Dataset/Dataset.h"


class AosDatasetDummy : virtual public AosDataset
{
	OmnDefineRCObject;

public:
	AosDatasetDummy(const u32 version);
	~AosDatasetDummy();

/*
	virtual bool reset(const AosRundataPtr &rdata);
	virtual int64_t getNumEntries();
	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset);
	
	virtual int64_t getTotalSize() const{return -1;}
	
	virtual void setTask(const AosTaskObjPtr &t){mTask = t;}


	virtual bool addDataConnector(const AosRundataPtr &rdata, 
	 				const AosDataConnectorObjPtr &data_cube);
	
	virtual bool addUnrecogContents(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data,
					const char *data,
					const int64_t &start_pos,
					const int64_t &length);

	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
*/

};
#endif



