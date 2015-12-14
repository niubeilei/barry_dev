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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataCacher_DataCacher_h
#define Aos_DataCacher_DataCacher_h

#include "DataCacher/DataCacherType.h"
#include "DataCacher/Ptrs.h"
#include "DataBlob/Ptrs.h"
#include "DataBlob/DataBlob.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataCacher : virtual public OmnRCObject
{

public:
	AosDataCacher(
			const OmnString &name, 
			const AosDataCacherType::E type, 
			const bool regflag);
	~AosDataCacher();

protected:
	
};

#endif
#endif
