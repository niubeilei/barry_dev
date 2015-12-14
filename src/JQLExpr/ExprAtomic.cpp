////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprAtomic.h"

#include "API/AosApi.h"
#include "Util/String.h"
#include "API/AosApi.h"


AosExprAtomic::AosExprAtomic(const char *data)
{
	mValue = data;
}

