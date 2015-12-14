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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActModifyRundataAttr_h
#define Aos_SdocAction_ActModifyRundataAttr_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>

class AosActModifyRundataAttr : virtual public AosSdocAction
{
public:
	AosActModifyRundataAttr(const bool flag);
	~AosActModifyRundataAttr();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

