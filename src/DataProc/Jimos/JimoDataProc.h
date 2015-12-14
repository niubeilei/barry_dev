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
// 2013/07/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_Jimos_JimoDataProc_h
#define Aos_DataProc_Jimos_JimoDataProc_h

#include "DataProc/DataProc.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <hash_map>
using namespace std;


class AosJimoDataProc : public AosJimo, public AosDataProc
{
protected:

public:
	AosJimoDataProc(
		const OmnString &type,
		const int version);
	~AosJimoDataProc();
};
#endif
