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
// 02/19/2011   Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocSendMail_h
#define Aos_SmartDoc_SdocSendMail_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosSdocSendMail : public AosSmartDoc
{

public:
	AosSdocSendMail(const bool flag);
	~AosSdocSendMail();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocSendMail(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};

#endif

