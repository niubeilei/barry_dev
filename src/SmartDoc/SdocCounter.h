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
#ifndef Aos_SmartDoc_SdocCounter_h
#define Aos_SmartDoc_SdocCounter_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosSdocCounter : public AosSmartDoc
{

public:
	AosSdocCounter(const bool flag);
	~AosSdocCounter();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCounter(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	u64 getUid(
		const OmnString &siteid,
		const AosSessionPtr &session,
		const AosRundataPtr &rdata);
};

#endif

