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
// 2015/02/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/QueryFieldMeasure.h"
/*
AosQueryFieldMeasure::AosQueryFieldMeasure(
					const int field_idx,
					OmnString fieldName, 
					OmnString aggrFunc)
*/
AosQueryFieldMeasure::AosQueryFieldMeasure(
					const int field_idx,
					const bool is_time_field)
:
AosQueryFieldNum(0, false)
//mAggrFunc(aggrFunc)
{
}

AosQueryFieldMeasure::~AosQueryFieldMeasure()
{
}
