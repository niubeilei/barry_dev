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
// 05/02/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_PigLatin_LatinGroup_h
#define Aos_PigLatin_LatinGroup_h

#include "Actions/SdocAction.h"

class AosLatinGroup : public AosSdocAction
{
private:

public:
	AosLatinGroup();
	AosLatinGroup(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosLatinGroup();

	// Action Interface
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata);
	virtual bool run(const AosTaskPtr &task, 
					 const AosTaskDataPtr &def, 
					 const AosRundataPtr &rundata);

private:
	bool configGroup(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

