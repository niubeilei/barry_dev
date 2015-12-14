////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BatchQuery_BatchQueryCtlr_h
#define Aos_BatchQuery_BatchQueryCtlr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosBatchQueryCtlr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum Status
	{
		eInvalid,

		eActive,
		eNoResults,
		eError,

		eMax
	};

	OmnString		mErrmsg;
	Status			mStatus;

public:
	AosBatchQueryCtlr();
	~AosBatchQueryCtlr();

	bool noResults();
	void setError(const char *file, const int line, const OmnString &errmsg);
};
#endif

