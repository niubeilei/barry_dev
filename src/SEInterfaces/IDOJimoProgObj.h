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
// 2015/03/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IDOJimoProgObj_h
#define Aos_SEInterfaces_IDOJimoProgObj_h

#include "IDO/IDO.h"
#include "SEInterfaces/JimoCallPackage.h"
#include "SEInterfaces/GenericObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include <ext/hash_map>

class AosIDOJimoProgObj : public AosGenericObj,
						  public AosIDO
{
public:
	enum
	{
		eMethodGetStr = 1,
		eMethodSetStr
	};

public:
	virtual bool setStr(AosRundata *rdata, 
						const OmnString &name,
						const OmnString &value) = 0;
	virtual OmnString getStr(AosRundata *rdata, 
						const OmnString &name, 
						const OmnString &dft) const = 0;
};

#endif
