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
#ifndef AOS_JQLStatement_JqlLimit_H
#define AOS_JQLStatement_JqlLimit_H

#include "JQLStatement/JqlDataStruct.h"
#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"

class AosJqlLimit : public AosJqlDataStruct
{
public:
	u64 	mOffset;
	u64		mRowCount;

public:
	AosJqlLimit();
	~AosJqlLimit();

	void setOffset(u64 offset);
	void setRowCount(u64 row_count);
	u64 getOffset();
	u64 getRowCount();
};

#endif
