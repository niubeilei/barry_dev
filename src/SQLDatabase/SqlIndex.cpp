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
#include "SQLDatabase/SqlIndex.h"

AosSqlIndex::AosSqlIndex()
{
	mName = "";
	mComment = "";
	mColumns = 0;
}

AosSqlIndex::AosSqlIndex(OmnString name)
:mName(name)
{
}

AosSqlIndex::~AosSqlIndex()
{
	int count;

	if (mColumns) 
	{
		count = mColumns->size();
		for (int i = 0; i < count; i++) delete (*mColumns)[i];
		delete mColumns;
	}
}

/*********************************
 * Getters/Setters
 *********************************/
void
AosSqlIndex::setName(OmnString name)
{
	mName = name;
}

OmnString 
AosSqlIndex::getName() const
{
	return mName;
}

void
AosSqlIndex::setComment(OmnString comment) 
{
	mComment = comment;
}

void
AosSqlIndex::setType(OmnString type) 
{
	mType = type;
}


void 
AosSqlIndex::setColumns(vector<AosJqlIndexColumn*> *columns)
{
	int count;

	if (mColumns) 
	{
		count = mColumns->size();
		for (int i = 0; i < count; i++) delete (*mColumns)[i];
		delete mColumns;
	}

	mColumns = columns;
}

void 
AosSqlIndex::setBlockSize(u32 blockSize)
{
	mBlockSize = blockSize;
}


/*********************************
 * member functions
 *********************************/
void 
AosSqlIndex::addColumn(AosJqlIndexColumn *column)
{
	if (!mColumns) mColumns = new vector<AosJqlIndexColumn *>; 

	mColumns->push_back(column);
}
