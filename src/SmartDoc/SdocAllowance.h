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
#ifndef Aos_SmartDoc_SdocAllowance_h
#define Aos_SmartDoc_SdocAllowance_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosSdocAllowance : public AosSmartDoc
{

public:
	AosSdocAllowance(const bool flag);
	~AosSdocAllowance();

	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocAllowance(false);}
private:

};
#endif

