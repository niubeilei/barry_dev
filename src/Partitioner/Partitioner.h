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
#ifndef Aos_Partitioner_Partitioner_h
#define Aos_Partitioner_Partitioner_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/PartitionerObj.h"
#include "SEInterfaces/PartitionerType.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosPartitioner : virtual public AosPartitionerObj
{
private:

public:
	AosPartitioner(
				const OmnString &name, 
				const AosPartitionerType::E type,
				const bool regflag);
	~AosPartitioner();

	static bool init();
};

#endif

