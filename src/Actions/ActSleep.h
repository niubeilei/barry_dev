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
// 2012/05/25	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSleep_h
#define Aos_SdocAction_ActSleep_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActSleep : virtual public AosSdocAction
{
public:
	AosActSleep(const bool flag);
	~AosActSleep();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	virtual bool run(const AosTaskObjPtr &task, 
					 const AosXmlTagPtr &sdoc,
					 const AosRundataPtr &rundata);		// Chen Ding, 05/02/2012
};
#endif

