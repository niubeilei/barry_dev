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
// 2014/08/20 Created By Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskUtil_ReduceCreateTaskFile_h
#define AOS_TaskUtil_ReduceCreateTaskFile_h

#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/ReduceTaskCreator.h"

class AosReduceCreateTaskFile : public AosReduceTaskCreator 
{

private:
	int							mPhysicalId;
	OmnString					mFileDefName;
	list<AosXmlTagPtr> 			mDocList;

public:
	AosReduceCreateTaskFile(
			const OmnString &type,
			const int physical_id,
			const AosXmlTagPtr &datacol_tag,
			list<AosXmlTagPtr> &doc_list);

	~AosReduceCreateTaskFile();

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
