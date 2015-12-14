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
#if 0
#ifndef Aos_SEInterfaces_DLLStatProcObj_h
#define Aos_SEInterfaces_DLLStatProcObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLDataProcObj.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosDLLStatProcObj : public AosDLLDataProcObj
{
public:
	AosDLLStatProcObj(const OmnString &name);
	virtual ~AosDLLStatProcObj();
};
#endif
#endif
