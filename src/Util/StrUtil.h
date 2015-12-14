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
// 01/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_StrUtil_h
#define Aos_Util_StrUtil_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosStrUtil
{
public:
	static int selectValue(
					const OmnString &value, 
					const int min_len,
					const vector<OmnString> &values, 
					const u32 num);
	static OmnString cdata(const OmnString &value);
};
#endif
