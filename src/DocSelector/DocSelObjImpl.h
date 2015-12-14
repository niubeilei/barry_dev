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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocSelector_DocSelObjImpl_h
#define Aos_DocSelector_DocSelObjImpl_h

#include "SEInterfaces/DocSelObj.h"


class AosDocSelObjImpl : virtual public AosDocSelObj
{
	OmnDefineRCObject;

public:
	AosDocSelObjImpl();
	~AosDocSelObjImpl();

	virtual AosXmlTagPtr selectDoc(
					const AosXmlTagPtr &sdoc, 
					const AosRundataPtr &rdata);
};
#endif
