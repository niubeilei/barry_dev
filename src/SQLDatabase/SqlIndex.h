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
// 01/04/2014 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SQLDatabase_SqlIndex_h
#define Aos_SQLDatabase_SqlIndex_h

#include "SQLDatabase/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataFieldObj.h"
#include "JQLStatement/JQLCommon.h"


class AosSqlIndex
{
	OmnDefineRCObject;

protected:
	OmnString mName;
	vector<AosJqlIndexColumn*> *mColumns;
	//the following belongs to options
	OmnString mComment;
	OmnString mType;
	u32 mBlockSize;

public:
	//constructors/destructors
	AosSqlIndex();
	AosSqlIndex(OmnString name);
	virtual ~AosSqlIndex();

	//getters/setters
	OmnString getName() const;
 	virtual void setName(OmnString name);
 	virtual void setComment(OmnString comment);
 	virtual void setType(OmnString type);
 	virtual void setColumns(vector<AosJqlIndexColumn*> *columns);
 	virtual void setBlockSize(u32 blockSize);

	//member functions
 	virtual void addColumn(AosJqlIndexColumn *column);
};
#endif

