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
// A Doc Task is defined as:
// 	Create doc docname
// 	(
//     	table="<tablename>",
//      Inputs="<input-data>",
//      KeyFields=[<expression-list>],
//  );
//
// It will create the following:


// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicCreateDoc.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicCreateDoc_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicCreateDoc(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosJimoLogicCreateDoc::AosJimoLogicCreateDoc(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
	mInput = NULL;
}


AosJimoLogicCreateDoc::~AosJimoLogicCreateDoc()
{
}


AosJimoPtr
AosJimoLogicCreateDoc::cloneJimo() const
{
	return OmnNew AosJimoLogicCreateDoc(*this);
}


bool
AosJimoLogicCreateDoc::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	Create Doc <doc-name>
	// 	(
	// 		name-value-list
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "importdoc", rdata, false);
	mErrmsg = "create importdoc ";

	mDocName = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, mDocName, mErrmsg)) return false;
	mErrmsg << mDocName;

	bool rslt = jimo_parser->getNameValueList(rdata, mNameValueList);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata, eGenericError, "", mErrmsg);
		return true;
	}

	// Parse record
    AosExprObjPtr record = jimo_parser->getParmExpr(rdata, "recordname", mNameValueList);
	if(record)
	{
		mRecordName = record->dumpByNoQuote();
	}
	// Parse table
	AosExprObjPtr table = jimo_parser->getParmExpr(rdata, "table", mNameValueList);
	if(!table)
	{
		setErrMsg(rdata, eMissingParm, "table", mErrmsg);
		return false;
	}
	mTableName = table->dumpByNoQuote();

	// Parse fields
    rslt = jimo_parser->getParmArrayStr(rdata, "fields", mNameValueList, mKeyFields);

	//Parse Input
	AosExprObjPtr mInput = jimo_parser->getParmExpr(rdata, "inputs", mNameValueList);
	if(!mInput)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	if(!checkNameValueList(rdata, mErrmsg, mNameValueList))
		return false;

	parsed = true;
	return true;
}


bool 
AosJimoLogicCreateDoc::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	// This function creates an AosJimoLogicCreateDoc.
    mJobName = prog->getJobname();
	bool rslt = createDocDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addDocTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicCreateDoc::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}



//create dataprocdoc
bool
AosJimoLogicCreateDoc::createDocDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocDoc_str = "create dataprocdocbatchopr ";
	mDataprocDoc << mJobName << "_dp_doc_" << mTableName << "_" << mDocName;
	dataprocDoc_str << mDataprocDoc << " { "
		<< "\"type\":" << "\"" << "docbatchopr" << "\"" << ","
	    << "\"opr\":" << "\"batch_insert\","; 

	if (mKeyFields.size())
	{
		JSONValue fieldsJSON;
		for (size_t i=0; i<mKeyFields.size(); i++)
		{
			string field_str = mKeyFields[i];
			JSONValue field(field_str);
			fieldsJSON.append(field);
		}
		dataprocDoc_str << "\"fields\": " << fieldsJSON.toStyledString() << ",";
	}

	dataprocDoc_str << "\"format\":\"" <<  jimo_prog->getEnv("schema_name") << "\","
		<< "\"docid\":" << "\"" << "getDocid('" << jimo_prog->getEnv("schema_name") << "')" << "\""
		<< "};";

	OmnScreen << dataprocDoc_str << endl;
	statements_str << "\n" << dataprocDoc_str;

    bool rslt = parseRun(rdata,dataprocDoc_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}



bool
AosJimoLogicCreateDoc::addDocTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{

	OmnString addDoc_str;
 	OmnString input = mInput->dumpByNoQuote();
	OmnString TaskName;
	TaskName << "task_doc_" << mTableName << "_" << mDocName;
	addDoc_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocDoc << "\","
		<< "\"inputs\":" << "[\"" << input << "\"]" << "}]}";
	jimo_prog->addTask(rdata, addDoc_str, "");
	OmnScreen << addDoc_str << endl;
    //bool rslt = parseRun(rdata,adddoc_str,jimo_prog);
    //aos_assert_rr(rslt,rdata,false);
	return true;
}


