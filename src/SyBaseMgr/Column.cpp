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
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "SyBaseMgr/Column.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"

AosColumn::AosColumn()
{
}


AosColumn::~AosColumn()
{
}

void 
AosColumn::setColName(const OmnString &colname)
{
	mColName = colname;
}

void 
AosColumn::setColBuffer(const OmnString &colbuffer)
{
	mColBuffer = colbuffer;
}

void 
AosColumn::setColType(const int coltype)
{
	mColType = coltype;
}

void
AosColumn::setColSize(const int colsize)
{
	mColSize = colsize;
}

void
AosColumn::setColStatus(const int colstatus)
{
	mColStatus = colstatus;
}

OmnString
AosColumn::getColName()
{
	return mColName;
}

OmnString
AosColumn::getColBuffer()
{
	return mColBuffer;
}

OmnString&
AosColumn::getColBufferRef()
{
	return mColBuffer;
}

int
AosColumn::getColType()
{
	return mColType;
}

int
AosColumn::getColSize()
{
	return mColSize;
}

int
AosColumn::getColStatus()
{
	return mColStatus;
}

int&
AosColumn::getColStatusRef()
{
	return mColStatus;
}

