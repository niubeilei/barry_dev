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
// 12/17/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocDelCompDocMembers_h
#define Aos_SmartDoc_SdocDelCompDocMembers_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include <iostream>


class AosSdocDelCompDocMembers : public AosSmartDoc
{
private:

public:
	AosSdocDelCompDocMembers(const bool flag);
	~AosSdocDelCompDocMembers();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocDelCompDocMembers(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};

#endif

