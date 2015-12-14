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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Partitioner_PartitionDocid_h
#define Aos_Partitioner_PartitionDocid_h

#include "Partitioner/Partitioner.h"


class AosPartitionDocid : virtual public AosPartitioner
{
	OmnDefineRCObject;

private:

public:
	AosPartitionDocid(const bool regflag);
	AosPartitionDocid(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosPartitionDocid();

	// AosPartitioner Interface
	virtual bool partition(
				const AosDataTablePtr &input, 
				vector<AosDataTablePtr> &outputs, 
				const AosRundataPtr &rdata);

	virtual AosPartitionerObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

