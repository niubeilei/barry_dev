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
// 11/09/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_Sdoc00002_h
#define Aos_SmartDoc_Sdoc00002_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"

class AosSdoc00002 : public AosSmartDoc
{

public:
	AosSdoc00002(const bool flag);
	~AosSdoc00002();

	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdoc00002(false);}
private:

};
#endif

