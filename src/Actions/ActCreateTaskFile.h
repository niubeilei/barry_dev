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
// 2014/08/20	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActCreateTaskFile_h
#define Aos_SdocAction_ActCreateTaskFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"

class AosActCreateTaskFile : virtual public AosSdocAction
{

public:
	AosActCreateTaskFile(const bool flag);
	~AosActCreateTaskFile();

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
};
#endif

