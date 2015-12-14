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
#ifndef AOS_TaskUtil_ReduceCreateDatasetDoc_h
#define AOS_TaskUtil_ReduceCreateDatasetDoc_h

#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/ReduceTaskCreator.h"

class AosCreateDatasetDoc : public AosReduceTaskCreator 
{

private:
	int							mPhysicalId;
	list<AosXmlTagPtr> 			mDocList;
	AosXmlTagPtr				mDatasetConf;

public:
	AosCreateDatasetDoc(
			const OmnString &type,
			const AosXmlTagPtr &dataset_conf,
			const int physical_id,
			list<AosXmlTagPtr> &doc_list);

	~AosCreateDatasetDoc();

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
