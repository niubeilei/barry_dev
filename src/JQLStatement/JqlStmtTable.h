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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtTable_H
#define AOS_JQLStatement_JqlStmtTable_H

#include "JQLStatement/JqlStatement.h"
#include "JQLExpr/ExprMemberOpt.h"

#include "SEInterfaces/ExprObj.h"

//#include "SQLDatabase/SqlTable.h"
#include "Util/String.h"

class AosJqlTableVirtulFieldDef: public AosJqlDataStruct
{
public:
	OmnString 		mName;
	OmnString 		mType;
	AosExprList*	mValues;

public:
	AosJqlTableVirtulFieldDef(){mValues = 0;};
	~AosJqlTableVirtulFieldDef(){OmnDelete mValues; mValues  = 0;};

	OmnString toString()
	{
		OmnString conf;
		AosExprObjPtr expr;
		AosExprMemberOptPtr member;
		conf << "<column name=\"" << mName << "\" type=\"" << mType << "\">";
		aos_assert_r(mValues, "");
		for (u32 i = 0; i < mValues->size(); i++)
		{
			expr = (*mValues)[i];
			aos_assert_r(expr->getType() == AosExprType::eMemberOpt, "");
			member = dynamic_cast<AosExprMemberOpt*>(expr.getPtr());
			conf << "<value recordname=\"" << member->getMember2()
				 << "\" fieldname=\"" <<member->getMember3() << "\" ></value>";
		}
		conf << "</column>";
		return conf;
	}
};

class AosJqlStmtTable : public AosJqlStatement
{
public:
	AosXmlTagPtr 					mDoc;
	vector<u64>						mDocids;
	AosRundataPtr					mRundata;
	AosMySqlRecord*					mRecord;
	OmnString 						mCurDatabase;
	OmnString						mSchemaName;
	OmnString						mInputDataFormat;

	// for create default index
	OmnString						mFirstFieldName;

	// for hbase
	OmnString						mDataSource;
	AosExprList						*mRawKeys;
	OmnString						mDbName;
	OmnString						mDbAddr;
	OmnString						mDbPort;
	OmnString						mDbTableName;


	//data from JQLParser
	OmnString						mName;
	OmnString						mTableName;
	//OmnString						mNewName;
	OmnString						mLikeTable;
	vector<AosJqlColumnPtr>			*mColumns;
	u32								mChecksum;
	u32								mAutoIncrement;
	OmnString						mComment;
	u32								mRecordLen;

	map<OmnString, AosJqlColumnPtr>	mColumnsMap;

	u32 							mOffset;

	// show tables
	u32								mFull;
	OmnString						mFrom;
	OmnString						mLike;
	OmnString						mErrmsg;

	// drop tables 
	vector<OmnString>				mTableNames;
	bool								mIsVirtual;
	vector<AosJqlTableVirtulFieldDefPtr>	*mVirtualColumns;
	bool							mIsExists;

public:
	AosJqlStmtTable(const OmnString &errmsg);
	AosJqlStmtTable();
	~AosJqlStmtTable();

	//getter/setters
	void setName(OmnString name);
	void setLikeTable(OmnString likeTable);
	void setOption(AosJqlTableOption *opt);
	void setColumns(vector<AosJqlColumnPtr> *columns);

	void setFullOption(u32 opt);
	void setFromOption(OmnString opt);
	void setLikeOption(OmnString opt);
	void setSchemaName(OmnString name);
	void setExists(bool flag);

	void setTbNameOption(const AosExprList* tblname_list);
	void setVirtual(bool flag){mIsVirtual = flag;}
	bool runDropTable(
			const AosRundataPtr &rdata,
			const OmnString &tbl_name,
			const AosXmlTagPtr &defs);

	bool createTables(const AosRundataPtr &rdata);
	bool showTables(const AosRundataPtr &rdata);
	bool dropTable(const AosRundataPtr &rdata);
	bool describeTable(const AosRundataPtr &rdata);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual bool cloneTable(const AosRundataPtr &rdata); 
	virtual AosJqlStatement *clone();
	virtual void dump();
	static OmnString getTableName(const OmnString &table, const OmnString &database_name);
	static AosXmlTagPtr getTable(
			const OmnString &table, 
			const AosRundataPtr &rdata);

	static OmnString getDatabaseFromFullName(OmnString const name);
	static OmnString getTableFromFullName(OmnString const name);
	static bool removeDefaultIndex(const AosRundataPtr &rdata, const AosXmlTagPtr &doc);

	static bool checkTable(
			const AosRundataPtr &rdata,
			const OmnString &database_name,
			const OmnString &table_name);

private:
	bool setDatabase(const AosRundataPtr &rdata);
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	OmnString generateTableConfBySchema(const AosRundataPtr &rdata, OmnString &map_str);
	bool generateDftSchema(const AosRundataPtr &rdata);
	OmnString generateVirtualColumnConf();
	OmnString generateRawKeyConf(const AosRundataPtr &rdata);
	OmnString generateMultiRecordSchema(const AosRundataPtr &rdata, const AosXmlTagPtr &schema_doc);
	void createColumnsMap();
	bool createDefaultIndex(const AosRundataPtr &rdata);
	OmnString getDefaultSchemaName();
};

#endif
