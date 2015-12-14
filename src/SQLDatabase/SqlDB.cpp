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
#include "SQLDatabase/SqlDB.h"
#include "SQLDatabase/SqlTable.h"
#include "API/AosApiC.h"
#include "API/AosApiG.h"

AosSqlDB::AosSqlDB()
{
	mIsSet = false;
	mName = "";
	mComment = "";
	mTables = 0;
}

AosSqlDB::AosSqlDB(OmnString name)
:mName(name)
{
}

AosSqlDB::~AosSqlDB()
{
	int count;

	if (mTables) 
	{
		count = mTables->size();
		for (int i = 0; i < count; i++) delete (*mTables)[i];
		delete mTables;
	}
}

/*********************************
 * Getters/Setters
 *********************************/
void
AosSqlDB::setName(OmnString name)
{
	mName = name;
}

OmnString 
AosSqlDB::getName() const
{
	return mName;
}

void
AosSqlDB::setComment(OmnString comment) 
{
	mComment = comment;
}

void
AosSqlDB::setTables(vector<AosSqlTable*> *tables)
{
	int count;

	if (mTables) 
	{
		count = mTables->size();
		for (int i = 0; i < count; i++) delete (*mTables)[i];
		delete mTables;
	}

	mTables = tables;
}

/*********************************
 * member functions 
 *********************************/
bool
AosSqlDB::addTable(AosSqlTable* table)
{
	if (!table)
	{
		OmnAlarm << "Add table to SqlDB error: empty table" << enderr;	
		return false;
	}

	if (!mTables) mTables = new vector<AosSqlTable *>;

	mTables->push_back(table);
	return true;
}

AosSqlTablePtr
AosSqlDB::getTable(OmnString tname, AosRundata* rdata)
{
	AosXmlTagPtr doc;

	doc = AosGetDocByObjid(tname, rdata);
	if (doc)
	{
		AosSqlTablePtr table = OmnNew AosSqlTable();
		table->setName(tname);
		table->setDoc(doc);
		return table;
	}

	return 0;
}

//Get the table name containng the user specified field  
//from the given table names
//
//Return true if find one and only one
bool 
AosSqlDB::getTablename(
	AosRundata *rdata, 
	const vector<OmnString> &tablenames, 
	const OmnString &fname, 
	OmnString &tablename)
{
	tablename = "";
	for (u32 i=0; i<tablenames.size(); i++)
	{
		OmnString tname = tablenames[i];
		AosSqlTablePtr table = getTable(tname, rdata);
		if (!table) continue;

		if (table->fieldExist(rdata, fname.data()))
		{
			if (tablename != "") 
			{
				cout << "Field exists in more than one tables." << endl;
				return false;
			}
			tablename = tname;
		}
	}

	if (tablename == "")
	{
		cout << "No table contains the field." << endl;
		return false;
	}

	return true;
}
