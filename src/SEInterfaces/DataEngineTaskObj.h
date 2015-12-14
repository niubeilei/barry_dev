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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataEngineTaskObj_h
#define Aos_SEInterfaces_DataEngineTaskObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "XmlUtil/Ptrs.h"

class AosDataEngineTaskObj : virtual public OmnRCObject
{
private:
	static AosDataEngineTaskObjPtr smObject;

public:
	static void setDataEngineTask(const AosDataEngineTaskObjPtr &obj) {smObject = obj;}
	static AosDataEngineTaskObjPtr getDataEngineTask() {return smObject;}
};
#endif

