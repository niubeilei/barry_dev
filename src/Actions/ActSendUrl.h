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
// 04/09/2013 Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSendUrl_h
#define Aos_SdocAction_ActSendUrl_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <vector>


class AosActSendUrl : virtual public AosSdocAction
{
public:
	AosActSendUrl(const bool flag);
	~AosActSendUrl();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

