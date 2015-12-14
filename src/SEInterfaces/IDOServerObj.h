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
#ifndef Aos_SEInterfaces_IDOServer_h
#define Aos_SEInterfaces_IDOServer_h

#include "SEInterfaces/JimoCallPackage.h"
#include "SEInterfaces/GenericObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include <ext/hash_map>

class AosIDOServerObj : public AosJimoCallPackage, 
						public AosGenericObj
{
public:
	enum
	{
		eMethodRunIDOByName = 1,
		eMethodCreateIDOByName,
		eMethodGetIDOByDocid
	};

public:
	virtual ~AosIDOServerObj();

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call) = 0;
	virtual bool isValid() const = 0;
};

#endif
