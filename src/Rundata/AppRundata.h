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
// 08/11/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Rundata_AppRundata_dd_h
#define Aos_Rundata_AppRundata_dd_h

#include "Rundata/Ptrs.h"
//#include "SmartDoc/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "XmlUtil/Ptrs.h"


class AosAppRundata : virtual public OmnRCObject
{
public:
	virtual bool  	  reset() = 0;
	virtual bool 	  copyFrom(const void *adata) = 0;
	virtual AosAppRundataPtr clone() = 0;
	virtual AosLocale::E getLocale(const AosRundataPtr &rdata) = 0;
};
#endif

