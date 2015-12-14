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
#ifndef Aos_Dataset_DatasetByQueryRslts_h
#define Aos_Dataset_DatasetByQueryRslts_h

#include "Dataset/Dataset.h"


class AosDatasetByQueryRslts : virtual public AosDataset
{
	OmnDefineRCObject;

public:
	AosDatasetByQueryRslts(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &def);
	~AosDatasetByQueryRslts();

private:
	bool config(const AosRundataPtr &rdata,
				const AosXmlTagPtr &def);
};
#endif



