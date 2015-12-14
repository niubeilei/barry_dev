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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSetRundataDoc_h
#define Aos_SdocAction_ActSetRundataDoc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <vector>


class AosActSetRundataDoc : virtual public AosSdocAction
{
public:
	AosActSetRundataDoc(const bool flag);
	~AosActSetRundataDoc();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif
