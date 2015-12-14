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
#include "JQLStatement/JqlGroupBy.h"
#include "JQLStatement/JqlSelectField.h"
#include "JQLStatement/JqlSelect.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"
	
AosJqlGroupBy::AosJqlGroupBy()
:
mFieldListPtr(0),
mRollupLists(0),
mCubeListPtr(0),
mIsRollUp(false),
mIsCube(false),
mIsAsc(true)
{
}

AosJqlGroupBy::~AosJqlGroupBy()
{
	//OmnDelete mFieldListPtr;
	if (mFieldListPtr) delete mFieldListPtr;
	mFieldListPtr = 0;

	if (mRollupLists) delete mRollupLists;
	mRollupLists = 0;

	if (mCubeListPtr) delete mCubeListPtr;
	mCubeListPtr = 0;
}


void
AosJqlGroupBy::setGroupFieldList(AosJqlSelectFieldList *field_list)
{
	mFieldListPtr = field_list;
}

void
AosJqlGroupBy::setRollupLists(vector<AosJqlSelectFieldList*> *rollup_lists)
{
	mRollupLists = rollup_lists;
	setIsRollUp(true);
}

void
AosJqlGroupBy::setCubeLists(AosJqlSelectFieldList *cube_lists)
{
	mCubeListPtr = cube_lists;
	setIsCube(true);
}

void 
AosJqlGroupBy::setIsRollUp(bool flag)
{
	mIsRollUp = flag;
}

void 
AosJqlGroupBy::setIsCube(bool flag)
{
	mIsCube = flag;
}

void 
AosJqlGroupBy::setIsAsc(bool flag)
{
	mIsAsc = flag;
}

AosJqlSelectFieldList*
AosJqlGroupBy::getGroupFieldList()
{
	return mFieldListPtr;
}

vector<AosJqlSelectFieldList*> *
AosJqlGroupBy::getRollupLists()
{
	return mRollupLists;
}

AosJqlSelectFieldList*
AosJqlGroupBy::getCubeLists()
{
	return mCubeListPtr;
}

bool 
AosJqlGroupBy::getIsRollUp()
{
	return mIsRollUp;
}

bool 
AosJqlGroupBy::getIsCube()
{
	return mIsCube;
}

bool 
AosJqlGroupBy::getIsAsc()
{
	return mIsAsc;
}

