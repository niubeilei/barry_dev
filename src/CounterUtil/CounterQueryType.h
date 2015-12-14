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
// 06/28/2010: Created by  Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_COUNTERUTIL_CounterQueryTypes_h
#define Aos_COUNTERUTIL_CounterQueryTypes_h

#include "Util/String.h"

#define AOSCOUNTER_QUERY_SINGLE  "single_counter_query"
#define AOSCOUNTER_QUERY_MULTI   "multi_counter_query"
#define AOSCOUNTER_QUERY_SUB   	 "sub_counter_query"

enum AosCounterQueryType
{
	eAosCounter_Query_Invalid = 0,

	eAosCounter_Query_Single,
	eAosCounter_Query_Multi,
	eAosCounter_Query_Sub,

	eAosCounter_Query_Max
};

extern AosCounterQueryType AosCounterQueryType_strToCode(const OmnString &typeName);
extern OmnString AosCounterQueryType_enumToStr(const AosCounterQueryType typeName);
#endif
#endif
