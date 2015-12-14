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
#ifndef Aos_SQLDatabase_SqlDB_h
#define Aos_SQLDatabase_SqlDB_h

#include "SQLDatabase/Ptrs.h"
#include "SEInterfaces/DataFieldObj.h"
#include "Rundata/Ptrs.h"
#include "SQLDatabase/SqlIndex.h"
#include "SQLDatabase/SqlTable.h"

class AosSqlDB
{
	OmnDefineRCObject;

protected:
	bool mIsSet;
	OmnString mName;
	vector<AosSqlTable*> *mTables;
	//vector<AosSqlUsers*> *mUsers;
	OmnString mComment;

public:
	//constructors/destructors
	AosSqlDB();
	AosSqlDB(OmnString name);
	virtual ~AosSqlDB();

	//getters/setters
	virtual OmnString getName() const;
 	virtual void setName(OmnString name);
 	virtual void setComment(OmnString comment);
	virtual void setTables(vector<AosSqlTable*> *tables);

	//member functions
	virtual bool addTable(AosSqlTable* table);
	AosSqlTablePtr getTable(OmnString tname, AosRundata* rdata);
	virtual bool getTablename(
					AosRundata *rdata, 
					const vector<OmnString> &tablenames, 
					const OmnString &fname, 
					OmnString &tablename);

};
#endif

