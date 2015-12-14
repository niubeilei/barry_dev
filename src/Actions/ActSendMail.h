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
#ifndef Aos_SdocAction_ActSendMail_h
#define Aos_SdocAction_ActSendMail_h

#include "Actions/SdocAction.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosActSendMail : public AosSdocAction 
{

public:
	AosActSendMail(const bool flag);
	~AosActSendMail();

	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};

#endif

