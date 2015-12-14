////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/01/08 Created By Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskUtil_ReduceIILBatchOpr_h
#define AOS_TaskUtil_ReduceIILBatchOpr_h

#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/ReduceTaskCreator.h"

class AosReduceIILBatchOpr : public AosReduceTaskCreator 
{

private:
	int							mPhysicalId;
	AosXmlTagPtr				mDataColTag;
	list<AosXmlTagPtr> 			mInfoList;

public:
	AosReduceIILBatchOpr(
		const OmnString &type,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list);

	~AosReduceIILBatchOpr();

	virtual bool		serializeTo(
							AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		serializeFrom(
							const AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif

#endif
