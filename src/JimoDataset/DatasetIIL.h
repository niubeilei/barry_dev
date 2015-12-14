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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataset_DatasetIIL_h
#define Aos_JimoDataset_DatasetIIL_h

#include "AosConf/DataRecord.h"
#include "Util/Ptrs.h"
#include "JimoDataset/JimoDataset.h"
#include "DataRecord/Ptrs.h"
#include "SEUtil/SeTypes.h"


class AosDatasetIIL : public AosJimoDataset
{
	OmnDefineRCObject		

public:
		AosDatasetIIL(const int ver);
		~AosDatasetIIL();

		AosJimoPtr cloneJimo() const;

public:
		bool	config(
					const OmnString &jsonstr, 
					const OmnString &dsname,
					const AosRundataPtr &rdata,
					const AosJimoProgObjPtr &prog);

		bool 	createByJql(
					AosRundata *rdata,
					const OmnString &obj_name,
					const OmnString &jsonstr,
					const AosJimoProgObjPtr &prog);
};

#endif
