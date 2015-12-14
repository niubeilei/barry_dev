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
// 2013/12/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Config_ConfigHandler_h
#define Aos_Config_ConfigHandler_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosConfigHandler : virtual public OmnRCObject
{
private:
	OmnString 	mConfigHandlerName;

public:
	AosConfigHandler(const OmnString &name)
	:
	mConfigHandlerName(name)
	{
	}

	OmnString getConfigHandlerName() const {return mConfigHandlerName;}
	virtual bool configModule(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &config) = 0;
};
#endif
