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
// 04/14/2011   Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocIsDeleted_h
#define Aos_SmartDoc_SdocIsDeleted_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include <vector>

class AosSdocIsDeleted : public AosSmartDoc
{

	enum
	{
		eDefaultNumIsDeleteds = 1,
		eDefaultNumHours = 0
	};

public:
	AosSdocIsDeleted(const bool flag);
	~AosSdocIsDeleted();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocIsDeleted(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};

#endif

