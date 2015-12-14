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
// 01/03/2014 Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "SQLDatabase/SqlColumn.h"
#include "Util/DataTypes.h"

AosSqlColumn::AosSqlColumn()
{
	mName = 0;
	mComment = 0;
	mDefault = 0;
}

AosSqlColumn::AosSqlColumn(OmnString name)
{
	AosSqlColumn();
	mName = name;
}

AosSqlColumn::~AosSqlColumn()
{
}

/*********************************
 * Getters/Setters
 *********************************/
void
AosSqlColumn::setName(OmnString name)
{
	mName = name;
}

OmnString 
AosSqlColumn::getName() const
{
	return mName;
}

void
AosSqlColumn::setComment(OmnString comment) 
{
	mComment = comment;
}

void
AosSqlColumn::setDefault(OmnString defaultVal) 
{
	mDefault = defaultVal;
}

void 
AosSqlColumn::setType(AosDataType::E type)
{
	mType = type;
}

OmnString 
AosSqlColumn::getTypeStr() const
{
	return AosDataType::getTypeStr(mType);
}

AosDataType::E 
AosSqlColumn::getType() const
{
	return mType;
}

/*********************************
 * member functions
 *********************************/
