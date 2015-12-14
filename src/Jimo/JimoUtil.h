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
// 2013/07/28 Renamed to Jimo by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoUtil_h
#define Aos_Jimo_JimoUtil_h

#include "Util/HashUtil.h"
#include <hash_map>
using namespace std;

class AosJimoUtil
{
public:
 	typedef hash_map<const OmnString, AosMethodDef, Omn_Str_hash, compare_str> funcmap_t;
 	typedef hash_map<const OmnString, AosMethodDef, Omn_Str_hash, compare_str>::iterator funcmapitr_t;
};
#endif

