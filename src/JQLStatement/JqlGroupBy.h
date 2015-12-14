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
#ifndef AOS_JQLStatement_JqlGroupBy_H
#define AOS_JQLStatement_JqlGroupBy_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlDataStruct.h"
#include "Util/String.h"
#include <vector>

class AosJqlSelectField;
class AosJqlSelect;

typedef vector<AosJqlSelectFieldPtr> AosJqlSelectFieldList;
class AosJqlGroupBy : public AosJqlDataStruct
{
public:
	AosJqlSelectFieldList*  mFieldListPtr;
	vector<AosJqlSelectFieldList *>	*mRollupLists;
	AosJqlSelectFieldList*	mCubeListPtr;
	bool				mIsRollUp;
	bool				mIsCube;
	bool				mIsAsc;

public:
	AosJqlGroupBy();
	~AosJqlGroupBy();

	void setGroupFieldList(AosJqlSelectFieldList *field_list);
	void setRollupLists(vector<AosJqlSelectFieldList*> *rollup_lists);
	void setCubeLists(AosJqlSelectFieldList *cube_lists);
	void setIsRollUp(bool flag);
	void setIsCube(bool flag);
	void setIsAsc(bool flag);
	AosJqlSelectFieldList* getGroupFieldList();
	vector<AosJqlSelectFieldList*> *getRollupLists();
	AosJqlSelectFieldList* getCubeLists();
	bool getIsRollUp();
	bool getIsCube();
	bool getIsAsc();
};

#endif
