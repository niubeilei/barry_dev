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
// 2015/07/06 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcDoc.h"
#include "JQLStatement/JqlStatement.h"
#include "SEUtil/JqlTypes.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcDoc_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcDoc(version);
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


AosJimoLogicDataProcDoc::AosJimoLogicDataProcDoc(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcDoc::~AosJimoLogicDataProcDoc()
{
}


bool
AosJimoLogicDataProcDoc::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc doc data_proc_index_name
	// 	(
	//		inputs: input_name,
	//		table: t1,
	//		schema: <schema>,
	//		fields:
	//		[
	//			<input_fieldname> [[as] <output_fieldname>],
	//			<input_fieldname> [[as] <output_fieldname>],
	//			...
	//		]
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "doc", rdata, false);
	mErrmsg = "dataproc doc ";
	// set service dft
	if(dft)
		mIsService = dft;

	// parse dataproc name
	OmnString dataproc_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, dataproc_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataproc_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = dataproc_name;
	mNameValueList = name_value_list;

	// parse table
	mTable = jimo_parser->getParmStr(rdata, "table", name_value_list);
	if (mTable == "")
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}
	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTable);
	if (!table_doc)
	{
		setErrMsg(rdata, eNotExist, mTable, mErrmsg);
		return false;
	}
	
		// parse inputs
	mInput = jimo_parser->getParmStr(rdata, "inputs", name_value_list);

	//parse format
	mFormat = jimo_parser->getParmStr(rdata, "format", name_value_list);

	/*
	if(!mIsService)
	{
	*/
		//parse fields
		rslt = jimo_parser->getParmArrayStr(rdata, "fields", name_value_list, mFields);
	//}
	//else
	//{
	//	(stmt->mFields).clear();
	//	mFields.clear();
	//}
	// parse prime
	if(mIsService)
	{
		OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
		aos_assert_rr(schema_objid != "", rdata, false);
		AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
		aos_assert_rr(schema_doc, rdata, false);
		AosXmlTagPtr record_doc = schema_doc->getFirstChild();
		record_doc = record_doc->getFirstChild();
		mSchemaName = record_doc->getAttrStr("zky_name");

		if((mFields).size()==0)
		{
			OmnString name = "";
			AosXmlTagPtr fieldsXml = record_doc->getFirstChild();
			AosXmlTagPtr fieldXml = fieldsXml->getFirstChild();
			while(fieldXml)
			{
				// skip virtual fields
				// 2015.12.07
				OmnString type = fieldXml->getAttrStr("type", "");
				if(type == "expr")
				{
					fieldXml = fieldsXml->getNextChild();
					continue;
				}
				name = fieldXml->getAttrStr("zky_name");
				mFields.push_back(name);
				fieldXml = fieldsXml->getNextChild();
			}
		}
	}

	mPrime = jimo_parser->getParmStr(rdata, "prime", name_value_list);
	if (mPrime == "")
	{
		mPrime = "true";
	}

	//columns
	//AosXmlTagPtr columns_doc = table_doc->getFirstChild("columns");
	//if (columns_doc)
	//{
	//	AosXmlTagPtr column_doc = columns_doc->getFirstChild("column");
	//	if (column_doc)
	//	{
	//		stmt->mHaveColumn = true;
	//	}
	//}

	//parse schema
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	aos_assert_rr(schema_objid != "", rdata, false);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
	aos_assert_rr(schema_doc, rdata, false);
	mParser = schema_doc->getAttrStr("zky_name","");
	
	AosXmlTagPtr tag = schema_doc->getFirstChild("datarecord", "");
	aos_assert_r(tag, false);
	mRecordName = tag->getAttrStr("zky_name", "");

//	AosXmlTagPtr tag = schema_doc->getFirstChild("datarecord", "");
//	aos_assert_r(tag, false);
//	tag = tag->getFirstChild("datafields", "");
//	aos_assert_r(tag, false);
//	rslt = getDataFields(stmt->mFields, tag, rdata);
//	aos_assert_r(rslt, false);
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcDoc::getDataFields(
		vector<OmnString> &fields,
		const AosXmlTagPtr &tag,
		AosRundata * rdata)

{
	aos_assert_r(tag, false);
	AosXmlTagPtr field = tag->getNextChild("datafield");
	OmnString fieldname;
	while(field)
	{
		OmnString type = field->getAttrStr("type", "");
		if (type == "expr")
		{
			AosXmlTagPtr expr = field->getNextChild("expr");
			aos_assert_r(expr, false);
			fieldname = expr->getNodeText();
		}
		else
		{
			fieldname = field->getAttrStr("zky_name", "");
			aos_assert_r(fieldname != "", false);
		}
		fields.push_back(fieldname);
		field = tag->getNextChild();
	}
	return true;
}


bool
AosJimoLogicDataProcDoc::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser)
{
	aos_assert_rr(prog, rdata, false);
	bool rslt = procInput(rdata, mInput, prog, mInputName, statement_str);
	aos_assert_r(rslt, false);

	if (mInputName == "")
	{
		mInputName = mInput;
	}

	OmnString jobname = prog->getJobname();
	mDocName = jobname;
	mDocName << "_" << mDataProcName;

    if(mIsService)
	{
		rslt = createSelectDataproc(rdata, prog, statement_str);
		aos_assert_rr(rslt, rdata, false);
		
		rslt = createIndexDataproc(rdata, prog, statement_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = createIILBatchoprDataproc(rdata, prog,statement_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = createDocBatachoprDataproc(rdata, prog, statement_str);
		aos_assert_rr(rslt, rdata, false);
	}
	
	// jimodb-623
	if (mPrime == "true" && !mIsService)
	{
		rslt = createDocBatachoprDataproc(rdata, prog, statement_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = addDocTask(rdata, prog);
		aos_assert_rr(rslt, rdata, prog);
	}

	if (!mIsService)
	{
		rslt = createIndexDataproc(rdata, prog, statement_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = createIILBatchoprDataproc(rdata, prog,statement_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = addIndexTask(rdata, prog);
		aos_assert_rr(rslt, rdata, prog);

		rslt = addIILTask(rdata,prog);
		aos_assert_rr(rslt, rdata, prog);

	}
//	else
//	{
//		prog->insertToDocMap(mInput, mDocList);
//	}
	return true;
}


bool
AosJimoLogicDataProcDoc::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'.
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcDoc*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcDoc::createDocBatachoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	if (mIsService)
	{
		OmnString inName = mSelectName;
		inName << ".output";
		prog->insertToDataFlowMap(inName, mDocName);

		OmnString doc_str;
		doc_str << "\"" << mDocName << "\" : {"
			<< "\"type\":\"dataprocdocbatchopr\""
			<< ",\"opr\":\"batch_insert\""
			<< ",\"table_objid\":\"" << AosJqlStatement::getObjid(rdata, JQLTypes::eTableDoc, mTable) << "\""
			<< ",\"format\":\"" << mRecordName << "\""
			<< ",\"fields\":[";
		for (size_t i = 0; i < mFields.size(); i++)
		{
			if (i>0)
				doc_str << ", ";
			doc_str << "\"" << mFields[i] << "\"";
		}
		doc_str << "],\"docid\":\"docid\"}";
		OmnScreen << doc_str << endl;
		statement_str << "\n" << doc_str;
		mDocList.push_back(mDocName);
//		prog->setDataProcName(mDocName);
//		prog->setDataProcDocBatchOpr(mDocName);
		prog->setConf(doc_str);
		return true;
	}
	
	
	OmnString doc_str = "create dataprocdocbatchopr ";
	doc_str << mDocName << " {"
		<< "\"type\":\"docbatchopr\""
		<< ", \"opr\":\"batch_insert\"";
	//if (!mHaveColumn)
	//{
	//	doc_str << ", \"format\":\"" << mParser << "\""
	//		    << ", \"fields\":[";
	//	for (size_t i = 0; i < mFields.size(); i++)
	//	{
	//		if (i>0)
	//			doc_str << ", ";
	//		doc_str << "\"" << mFields[i] << "\"";
	//	}
	//	doc_str << "] ";
	//}
	doc_str << ", \"format\":\"" << mFormat << "\""
		<< ", \"table_objid\":\"" << AosJqlStatement::getObjid(rdata, JQLTypes::eTableDoc, mTable) << "\""
		<< ", \"fields\":[";
	for (size_t i = 0; i < mFields.size(); i++)
	{
		if (i>0)
			doc_str << ", ";
		doc_str << "\"" << mFields[i] << "\"";
	}
	doc_str << "] ";

	doc_str << ", \"docid\":" << "\"getDocid('" << mParser << "')\"};";

	OmnScreen << doc_str << endl;
	statement_str << "\n" << doc_str;

	bool rslt = parseRun(rdata,doc_str,prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcDoc::createIndexDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
	//	int max_length)
{
	if (mIsService)
	{
		OmnString dataprocIndex_str;
		mDataProcIndexName << "dpIndex" << "_" << mDocName;
		dataprocIndex_str << "\"" << mDataProcIndexName << "\" : {"
						  << "\"type\":\"dataprocindex\""
						  << ",\"record_type\":\"buff\""
						  << ",\"docid\":" << "\"getDocid(\'" << mParser << "\')\","
						  << "\"format\":" << "\"" << mSchemaName << "\","
						  << "\"keys\":[" << "\"getDocid(\'" << mParser << "\')\"]}";

		OmnString inName = mSelectName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataProcIndexName);

//		mDocList.push_back(mDataProcIndexName);
		jimo_prog->setConf(dataprocIndex_str);
		return true;
	}

	//OmnString dataprocIndex_str = getHead(rdata,keys_type.first);
	OmnString dataprocIndex_str = "create dataprocindex ";
    mDataProcIndexName << "def_idx_" << mTable;
	dataprocIndex_str << mDataProcIndexName << " { ";

	dataprocIndex_str << "\"type\":" <<"\"index\",";

	dataprocIndex_str << "\"record_type\":" << "\"buff\",";

	dataprocIndex_str<< "\"docid\":" << "\"getDocid(\'" << mParser << "\')\",";

	dataprocIndex_str << "\"keys\":[" << "\"getDocid(\'" << mParser << "\')\"]};";

	OmnScreen << dataprocIndex_str << endl;
	statement_str << "\n" << dataprocIndex_str;

	bool rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcDoc::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	OmnString dataprociil_str = "";
	mDataProcIILName << mDataProcIndexName << "_reduce";
	if (!mIsService)
	{
		dataprociil_str = "create dataprociilbatchopr ";
		dataprociil_str << mDataProcIILName << " { ";
	}
	else
	{
		dataprociil_str << "\"" << mDataProcIILName << "\" : {"
			<< "\"type\":\"dataprociilbatchopr\",";
//		mDocList.push_back(mDataProcIILName);
		OmnString inName = mDataProcIndexName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataProcIILName);
	}

	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr,JQLTypes::eTableDoc,mTable);
	if (!table_doc)
	{
		setErrMsg(rdata,eNotExist,mTable,mErrmsg);
		return false;
	}

	OmnString iilname;
	OmnString dbname;
	dbname = table_doc->getAttrStr("zky_database");
	iilname << "_zt44_" << dbname << "_" << mTable;

	dataprociil_str << "\"iilname\":" << "\""
		<< iilname << "\"" << ","
		<< "\"record_type\":" << "\"buff\","
		<< "\"opr\":" << "\"add\","
		//<< "\"record_type\":" << "\"fixbin\","
		//<< "\"opr\":" << "\""<< type <<"add\"" << ","
		<< "\"iil_type\":" << "\"BigU64\"" << ","
		<< "\"build_bitmap\":" << "\"true\"}";

	if(!mIsService)
	{
		dataprociil_str << ";";
		OmnScreen << dataprociil_str << endl;
		statement_str << "\n" << dataprociil_str;
		bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	else
		jimo_prog->setConf(dataprociil_str);

	return true;
}


	bool
AosJimoLogicDataProcDoc::addIndexTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addindex_str;
	OmnString taskname;
	taskname << "task_index_iil_" << mDataProcIndexName;
	addindex_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcIndexName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"], "
		<< "\"outputs\":" << "[\"" << mDataProcIndexName << "_output"<< "\"";
	addindex_str <<"]}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks",addindex_str);
		jimo_prog->addTask(rdata, addindex_str, "");
	}
	OmnScreen << addindex_str << endl;
	return true;
}


bool
AosJimoLogicDataProcDoc::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addiil_str;
 	OmnString input;
	OmnString taskname;
	taskname << "task_batchopr_iil_" << mDataProcIILName;
	input << mDataProcName << "_output";
	//addgroupby_str << jobname;
	addiil_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcIILName << "\","
		<< "\"inputs\":"<< "["
		<< "\"" << mDataProcIndexName << "_output"<< "\"";
	addiil_str << "]}]}";

	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks",addiil_str);
		jimo_prog->addTask(rdata, addiil_str, "");
	}

	OmnScreen << addiil_str << endl;
	return true;
}
bool
AosJimoLogicDataProcDoc::addDocTask(
		AosRundata *rdata,
		AosJimoProgObj *prog)
{
	OmnString add_doc_str;
	OmnString taskname;
	taskname << "task_index_" << mDocName;
	add_doc_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDocName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"]";
	add_doc_str <<"}]}";

	if(prog)
	{
		prog->appendStatement(rdata, "tasks",add_doc_str);
		prog->addTask(rdata, add_doc_str, "");
	}

	OmnScreen << add_doc_str << endl;
	return true;
}


AosJimoPtr
AosJimoLogicDataProcDoc::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcDoc(*this);
}


OmnString 
AosJimoLogicDataProcDoc::getInput() const
{
	return mInput;
}

OmnString 
AosJimoLogicDataProcDoc::getTableName() const
{
	return  mTable;
}

bool 
AosJimoLogicDataProcDoc::isExist(const OmnString &name,const OmnString &parm)
{
	if(name == "input")
	{
		return parm==mInput;
	}
	else if(name == "table")
	{
		return parm==mTable;
	}
	else if(name != "")
	{
		OmnShouldNeverComeHere;
	}
	return false;
}

bool
AosJimoLogicDataProcDoc::createSelectDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocSelect_str = "";
	OmnString dataprocName = "";

	//dataprocName << "dpSelect" << "_" << mDocName;
	dataprocName << "dpSelect" << "_" << mInput;
	//dataprocName << "_" << mIdx;
	//mDocList.clear();
	//mDocList.push_back(mInput);
	//mDocList.push_back(dataprocName);

	mSelectName = dataprocName;
	if(jimo_prog->hasSelect(mSelectName))
	{
		return true;
	}

	AosJimoLogicObjNew * jimo_logic = jimo_prog->getJimoLogic(rdata, mInput);
	if (jimo_logic)
	{
		AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
		if (type == AosJimoLogicType::eDataset)
		{
			jimo_prog->insertToDataFlowMap(mInput, mSelectName);
			jimo_prog->setDatasetMap(mInput);
		}
		else
		{
			OmnString inName = mInput;
			inName << ".output";
			jimo_prog->insertToDataFlowMap(inName, mSelectName);
		}
	}
	else
	{
		OmnAlarm << "ddddddddddd" << enderr;
	}

	dataprocSelect_str << "\"" << dataprocName << "\"" << " : { "
		<< "\"type\":" << "\"" << "dataprocselect" << "\"";
	dataprocSelect_str << ",\"assign_docid\": \"true\""
		<< ", \"docid\":" << "\"getDocid('" << mParser << "')\""
		<< ",\"format\": \"" << mSchemaName << "\"";
	dataprocSelect_str << ", \"record_type\":" << "\"buff\"";

	dataprocSelect_str << "} ";
	jimo_prog->setConf(dataprocSelect_str);
	jimo_prog->setSelectMap(mSelectName);
	
	return true;
}
