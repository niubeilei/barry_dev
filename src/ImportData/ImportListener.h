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
// 03/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImportData_ImportListener_h
#define Aos_ImportData_ImportListener_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "XmlUtil/Ptrs.h"

class AosImportListener : virtual public OmnRCObject
{
public:
    virtual void importFinished(
						const bool rslt, 
						const AosXmlTagPtr &status_doc,
						const AosRundataPtr &rdata) = 0;
};
#endif

