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
#ifndef Aos_SQLDatabase_SqlTable_h
#define Aos_SQLDatabase_SqlTable_h

#include "SQLDatabase/Ptrs.h"
#include "SEInterfaces/SqlTableObj.h"
#include "SEInterfaces/DataFieldObj.h"
#include "Rundata/Ptrs.h"
#include "SQLDatabase/SqlIndex.h"
#include "SQLDatabase/SqlColumn.h"

class AosSqlTable : public AosSqlTableObj
{
	OmnDefineRCObject;

protected:
	AosXmlTagPtr	mDoc;
	bool mIsSet;
	OmnString mName;
	vector<AosSqlColumn*> *mColumns;
	vector<AosSqlIndex*> *mIndexes;
	u32 mAutoIncrement;
	OmnString mComment;
	bool mChecksum;
	OmnString mLikeTable;

public:
	AosSqlTable();
	AosSqlTable(OmnString name);
	virtual ~AosSqlTable();

	//getters/setters
	virtual OmnString getName() const;
 	virtual void setName(OmnString name);
 	virtual void setDoc(AosXmlTagPtr doc);
 	virtual void setAutoIncrement(u32 autoInc);
 	virtual void setComment(OmnString comment);
 	virtual void setChecksum(bool check);
	virtual void setColumns(vector<AosSqlColumn*> *columns);
	virtual void setIndexes(vector<AosSqlIndex*> *indexes);
	virtual void setLikeTable(OmnString likeTable);

	//member functions
	virtual bool addColumn(AosSqlColumn* column);

	virtual bool addIndex(AosSqlIndex* index);

	virtual AosDataFieldObjPtr getField(
						AosRundata *rdata, 
						const char *field_name);

	virtual bool getSchema(
						AosRundata *rdata, 
						const char *schema_name);

	virtual bool fieldExist(AosRundata *rdata, const char *field_name);

	AosJimoPtr cloneJimo() const; 

	//table operations
	//virtual bool createTable(
	//					AosRundata *rdata, 
	//					const AosXmlTag* &new_def);

	virtual bool createTable(
						const AosRundataPtr &rdata, 
						AosXmlTag* new_def);

	virtual bool dropTable(AosRundata *rdata); 

	virtual bool alterTable(
						AosRundata *rdata, 
						const AosXmlTag* &new_def);

	virtual bool cloneFrom(const char *likeTable);
};
#endif

