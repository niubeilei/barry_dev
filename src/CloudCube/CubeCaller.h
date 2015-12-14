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
// Modification History:
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CloudCube_CubeCaller_h
#define Aos_CloudCube_CubeCaller_h

#include "Porting/TimeOfDay.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosCubeCaller : virtual public OmnRCObject
{
private:

public:
	virtual bool procResp(const AosRundataPtr &rdata, 
					const AosTrans1Ptr &trans, 
					const AosXmlTagPtr &xml_doc) = 0;
};
#endif

