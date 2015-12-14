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
#include "SQLDatabase/SqlTable.h"
#include "API/AosApiC.h"
#include "API/AosApiG.h"
#include "alarm_c/alarm.h"

AosSqlTable::AosSqlTable()
:
AosSqlTableObj(0)
{
	mIsSet = false;
	mName = "";
	mAutoIncrement = 0;
	mComment = "";
	mChecksum = false;
	mColumns = 0;
	mIndexes = 0;
	mDoc = 0;
}

AosSqlTable::AosSqlTable(OmnString name)
:
AosSqlTableObj(0),
mName(name)
{
}

AosSqlTable::~AosSqlTable()
{
	int count;

	if (mColumns) 
	{
		count = mColumns->size();
		for (int i = 0; i < count; i++) delete (*mColumns)[i];
		delete mColumns;
	}

	if (mIndexes) 
	{
		count = mIndexes->size();
		for (int i = 0; i < count; i++) delete (*mIndexes)[i];
		delete mIndexes;
	}
}

/*********************************
 * Getters/Setters
 *********************************/
void
AosSqlTable::setName(OmnString name)
{
	mName = name;
}

OmnString 
AosSqlTable::getName() const
{
	return mName;
}

void
AosSqlTable::setDoc(AosXmlTagPtr doc)
{
	mDoc = doc;
}

void
AosSqlTable::setChecksum(bool checksum) 
{
	mChecksum = checksum;
}

void
AosSqlTable::setComment(OmnString comment) 
{
	mComment = comment;
}

void
AosSqlTable::setAutoIncrement(u32 autoInc)
{
	mAutoIncrement = autoInc;
}

void
AosSqlTable::setColumns(vector<AosSqlColumn*> *columns)
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
AosSqlTable::setIndexes(vector<AosSqlIndex*> *Indexes)
{
	int count;

	if (mIndexes) 
	{
		count = mIndexes->size();
		for (int i = 0; i < count; i++) delete (*mIndexes)[i];
		delete mIndexes;
	}

	mIndexes = Indexes;
}

void
AosSqlTable::setLikeTable(OmnString likeTable)
{
	mLikeTable = likeTable;
}

/*********************************
 * member functions 
 *********************************/
AosDataFieldObjPtr 
AosSqlTable::getField(
				AosRundata *rdata, 
				const char *field_name)
{
	/*
	int count = mColumns.size();

	for (int i = 0; i < count; i++) 
	{
		if (mColumns[i]->getName() == field_name)
			return mColumns[i];
	}
	*/

	return 0;
}

bool 
AosSqlTable::getSchema(
				AosRundata *rdata, 
				const char *schema_name)
{
	return true;
}

bool 
AosSqlTable::fieldExist(AosRundata *rdata, const char *field_name)
{
	AosXmlTagPtr doc;
	AosXmlTagPtr colDoc;

	if (!field_name)
	{
		OmnAlarm << "Find field in talbe error: empty field name" << enderr;	
		return false;
	}

	if (!mDoc)
	{
		mDoc = AosGetDocByObjid(mName, rdata);
		if (!mDoc) return false;
	}

	doc = mDoc->getFirstChild("columns");
	aos_assert_r(doc, false);

	colDoc = doc->getFirstChild("column");
	aos_assert_r(colDoc, false);
	while (colDoc)
	{
		if (colDoc->getAttrStr("name") == field_name) return true;

		colDoc = doc->getNextChild();
	}

	return false;
}

bool
AosSqlTable::addColumn(AosSqlColumn* column)
{
	if (!column)
	{
		OmnAlarm << "Add column to SqlTable error: empty column" << enderr;	
		return false;
	}

	if (!mColumns) mColumns = new vector<AosSqlColumn *>;

	mColumns->push_back(column);
	return true;
}

bool
AosSqlTable::addIndex(AosSqlIndex* Index)
{
	if (!Index)
	{
		OmnAlarm << "Add Index to SqlTable error: empty Index" << enderr;	
		return false;
	}

	if (!mIndexes) mIndexes = new vector<AosSqlIndex *>;
	mIndexes->push_back(Index);
	return true;
}

AosJimoPtr
AosSqlTable::cloneJimo() const                            
{
	try
	{
		AosSqlTable *table = OmnNew AosSqlTable();
		return table; 
	}   

	catch (...)
	{
		OmnAlarm << "failed create expr" << enderr;
		return 0;
	}   

	OmnShouldNeverComeHere;
	return 0;
}

/*********************************
 * table operations 
 *********************************/
bool 
AosSqlTable::createTable(
				const AosRundataPtr &rdata, 
				AosXmlTag* new_def)
{
	//generate the xml data firstly
	OmnString query;
	AosSqlColumn *column;

	// 	<sqltable zky_objid="db.tname" 
	// 		zky_otype="zky_table"
	// 		zky_stype="zky_table"
	// 		zky_ptrs="tables"
	// 		...>
	//		<columns>
	//     		<column name="col1" datatype="int"/>
	//    		<column name="col2" datatype="string"/>
	//    	</columns>
	//  </sqltable>
	query << "<sqltable zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_objid=\""<< mName << "\" zky_otype=\"zky_table\" zky_stype=\"zky_table\" zky_ptrs=\"tables\">";
	query << "<columns>";
	
	for (u32 i = 0; i < mColumns->size(); i++ )
	{
		column = (*mColumns)[i];
		query << "<column name=\"" << column->getName() << "\" datatype=\"" << column->getTypeStr() << "\"/>";
	}

	query << "</columns>";
	query << "</sqltable>";

	//Aos_assert_r(new_def->getAttrStr(AOSTAG_OTYPE) == "zky_table", false);
	//AosCreateTable(rdata, new_def);

	return true;
}

bool 
AosSqlTable::dropTable(AosRundata *rdata)
{
	return true;
}

bool 
AosSqlTable::alterTable(
				AosRundata *rdata, 
				const AosXmlTag* &new_def)
{
	return true;
}

bool 
AosSqlTable::cloneFrom(const char *likeSqlTableName)
{
	return true;
}
