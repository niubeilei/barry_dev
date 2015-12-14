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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlLimit.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"

AosJqlLimit::AosJqlLimit()
:
mOffset(0),
mRowCount(0)
{
}

AosJqlLimit::~AosJqlLimit()
{
}

void 
AosJqlLimit::setOffset(u64 offset)
{
	mOffset = offset;
}

void 
AosJqlLimit::setRowCount(u64 row_count)
{	
	mRowCount = row_count;
}

u64
AosJqlLimit::getOffset()
{
	return mOffset;
}

u64 
AosJqlLimit::getRowCount()
{
	return mRowCount;
}

