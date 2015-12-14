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
// 08/08/2011	Created by tom
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActCreateLog_h
#define Aos_SdocAction_ActCreateLog_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>


class AosActCreateLog : virtual public AosSdocAction
{
public:
	AosActCreateLog(const bool flag);
	~AosActCreateLog();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

