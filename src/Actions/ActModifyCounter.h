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
// 06/13/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActModifyCounter_h
#define Aos_SdocAction_ActModifyCounter_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>


class AosActModifyCounter : virtual public AosSdocAction
{
public:
	AosActModifyCounter(const bool flag);
	~AosActModifyCounter();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

