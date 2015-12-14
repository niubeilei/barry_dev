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
//
// Modification History:
// 2015/01/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SyncJobObj_h
#define Aos_SEInterfaces_SyncJobObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosSyncJobObj : public AosJimo
{
private:

public:
	AosSyncJobObj(const int version);

	virtual bool proc() = 0;

	static AosSyncJobObjPtr createSyncJob(
					AosRundata *rdata, 
					const OmnString &syncher_name);
};
#endif

