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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActRunDLL_h
#define Aos_SdocAction_ActRunDLL_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActRunDLL : virtual public AosSdocAction
{
public:
	AosActRunDLL(const bool flag);
	~AosActRunDLL();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

