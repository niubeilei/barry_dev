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
// 11/29/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocWarehouse_h
#define Aos_SmartDoc_SdocWarehouse_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>


class AosSdocWarehouse : public AosSmartDoc
{
private:

public:
	AosSdocWarehouse(const bool flag);
	~AosSdocWarehouse();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocWarehouse(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};

#endif

