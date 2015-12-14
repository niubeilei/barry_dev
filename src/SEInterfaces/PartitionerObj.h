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
#ifndef Aos_SEInterfaces_PartitionerObj_h
#define Aos_SEInterfaces_PartitionerObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/PartitionerType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosPartitionerObj : virtual public OmnRCObject
{
protected:
	OmnString			mName;
	AosPartitionerType::E	mType;
	bool				mIsTemplate;

public:
	AosPartitionerObj()
	:
	mType(AosPartitionerType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosPartitionerObj(
				const OmnString &name, 
				const AosPartitionerType::E type, 
				const bool reg);
	~AosPartitionerObj();

	// AosPartitioner Interface
	virtual bool partition(
				const AosDataTablePtr &input, 
				vector<AosDataTablePtr> &outputs, 
				const AosRundataPtr &rdata) = 0;

	virtual AosPartitionerObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosPartitionerObjPtr getPartitionerStatic(
				const AosPartitionerType::E type, 
				const AosRundataPtr &rdata);

	static AosPartitionerObjPtr getPartitionerStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerPartitioner(const AosPartitionerObjPtr &partitioner);
};
#endif

