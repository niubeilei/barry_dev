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
// 08/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WorkMgr_Work_h
#define AOS_WorkMgr_Work_h

#include "WorkMgr/Work.h"



class AosWork : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosWork(const AosWorkdId::E type);
	virtual ~AosWork();

	virtual bool start(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif
