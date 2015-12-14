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
// 2015/04/23 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicDoc2.h"

#include "JSON/JSON.h"
#include "JimoAPI/JimoParserAPI.h"
#include "Rundata/Rundata.h"
#include "API/AosApiG.h"
AosJLDoc2::AosJLDoc2(
		const AosRundataPtr &rdata,
		const string &input_name,
		const AosXmlTagPtr &table_doc,
		const string &tablename,
		const AosJimoProgObjPtr &jimo_prog,
		const bool prime_table)
:AosJimoLogic(1)
{
	aos_assert(table_doc);
	mTableName = tablename;
	mInput = input_name;
	mJimoProg = jimo_prog;
	mPrime = prime_table;

	procVirtualField(rdata, table_doc);
	
	OmnString record_name;
	OmnString scheam_doc_objid = table_doc->getAttrStr("zky_use_schema");
	AosXmlTagPtr schema_docs = AosGetDocByObjid(scheam_doc_objid, rdata);
	
	if (schema_docs)
	{
		AosXmlTagPtr records = schema_docs->getFirstChild("datarecords");
		if (records)
		{
			AosXmlTagPtr record = records->getFirstChild(true);
			while (record)
			{
				record_name = record->getAttrStr("key");
				mRecords.push_back(record_name);
				record = records->getNextChild();
			}
		}
		else
		{
			AosXmlTagPtr record = schema_docs->getFirstChild(true);
			aos_assert(record);
			record_name = record->getAttrStr("zky_datarecord_name");
			mRecords.push_back(record_name);
		}
	}

	bool rslt = createJsonStr(rdata);
	if (!rslt)
	{
		OmnAlarm << "error ." << enderr;
	}
}

bool 
AosJLDoc2::createJsonStr(const AosRundataPtr &rdata)
{
	OmnString str;
	for (u32 i = 0; i < mRecords.size(); i++)
	{
		str << "dataproc doc " << mTableName;
		if (mRecords[i] != "")
		{
			str << "_" << mRecords[i] << "(";
		}
		else
		{
			str << "(" ;
		}

		// Add fields by Young, 2015/05/14
		if (mVirtualFieldsExpr.size())
		{
			JSONValue fieldsJSON;
			for (size_t j=0; j<mVirtualFieldsExpr.size(); j++)
			{
				JSONValue field(mVirtualFieldsExpr[j]);
				fieldsJSON.append(field);
			}
			str << "fields : " << fieldsJSON.toStyledString() << ",";
		}

		if (mTableName != "") {
			str << " table : \"" << mTableName << "\",";
		}
		if (mRecords[i] != "") {
			str << " recordname : \"" << mRecords[i] << "\", ";
		}

		str << "inputs : \"" << mInput << "\"";
		// jimodb-632
		str << ", prime : \"" << mPrime << "\"";
		str << ");";

		AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
		aos_assert_rr(jimoParser, rdata.getPtr(), 0);
		vector<AosJqlStatementPtr> statements;
		AosJimoProgObj *prog = mJimoProg.getPtrNoLock();
		jimoParser->parse(rdata.getPtr(),prog, str, statements);

		OmnString ss;
		for (u32 i=0; i<statements.size(); i++)
		{
			statements[i]->setContentFormat("xml");
			statements[i]->run(rdata.getPtrNoLock(), prog, ss, false);
		}                                                                                      
	}
	return true;
}

AosJLDoc2::AosJLDoc2(int version)
:AosJimoLogic(version)
{
}


AosJLDoc2::AosJLDoc2(const AosJLDoc2 &rsh)
:
AosJimoLogic(rsh.mJimoVersion),
mInput(rsh.mInput),
mTableName(rsh.mTableName),
mJimoProg(rsh.mJimoProg)
{
	mRecords.clear();
	mRecords.insert(mRecords.end(), rsh.mRecords.begin(), rsh.mRecords.end());
}


AosJLDoc2::~AosJLDoc2()
{
}


AosJimoPtr 
AosJLDoc2::cloneJimo() const
{
	return OmnNew AosJLDoc2(*this);
}


bool
AosJLDoc2::procVirtualField(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &tabledoc)
{
	// 1. find virtual field 
	AosXmlTagPtr columnsNode = tabledoc->getFirstChild("columns");
	aos_assert_r(columnsNode, false);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		AosXmlTagPtr exprNode = columnNode->getFirstChild("expr");
		if (exprNode)
		{
			// 2.  get virtual field expr 
			OmnString expr = exprNode->getNodeText();
			mVirtualFieldsExpr.push_back(expr);
		}

		columnNode = columnsNode->getNextChild("column");
	}


	return true;	
}
