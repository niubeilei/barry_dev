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
// Statement Syntax:
// 		DATASET mydataset
// 		(
// 		    name: value,
// 		    name: value,
// 		    ...
// 		    name: value,
//		);
//
// Modification History:
// 2015/05/18 Created by Chen Ding
// 2015/05/19 Worked on by Xia Fan
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcSelect.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSelect_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSelect(version);
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


AosJimoLogicDataProcSelect::AosJimoLogicDataProcSelect(const int version)
:
AosJimoLogicDataProc(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDataProcSelect::~AosJimoLogicDataProcSelect()
{
}


bool
AosJimoLogicDataProcSelect::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is in the form:
	// 	dataproc select datasetname
	// 	(
	// 		name: value,
	// 		name: value,
	// 		...
	// 		name: value
	// 		saveOpr: true or false
	// 		into: tableName
	// 	);
	mIdx = 0;
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataproc", rdata, false);
	aos_assert_rr(keywords[1] == "select", rdata, false);
	mErrmsg = "dataproc select ";

	// set service dft
	if(dft)
		mIsService = dft;

	// parse dataproc name
	OmnString dpname = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, dpname, mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mDPName = dpname;
	mErrmsg << dpname;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = dpname;
	mNameValueList = name_value_list;

	// Configure the common attributes
	if (!configCommonAttrs(rdata,jimo_parser, name_value_list,mErrmsg))
	{
		AosLogError(rdata, true, "missing_common_attributes");
		return false;
	}

	//parse saveOpr
	OmnString saveOpr = jimo_parser->getParmStr(rdata, "saveOpr", name_value_list);
	if(saveOpr == "") saveOpr = "false";
	mSaveOpr = saveOpr;

	// parse input
	OmnString input = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	//if (input == "")
	//{
	//	AosLogError(rdata, true, "missing_inputs") << enderr;
	//	return false;
	//}
	mInput = input;
	//AosRundataPtr rdataPtr(rdata);
	//AosXmlTagPtr doc = AosJqlStatement::getDoc(rdataPtr,JQLTypes::eTableDoc,stmt->mInput);
	// parse output
	mOutput = jimo_parser->getParmStr(rdata, "output", name_value_list);
	// parse data_opr
	OmnString data_opr = jimo_parser->getParmStr(rdata, "data_opr", name_value_list);
	if (mInput == "")
	{
		mOpr = "add";
	}
	mOpr = data_opr;

	//get parser for a moment
	OmnString tableName = jimo_parser->getParmStr(rdata, "into", name_value_list);
	if (tableName == "" && mSaveOpr == "true")
	{
		setErrMsg(rdata,eMissingParm, "into",mErrmsg);
		return false;
	}
	mTableName = tableName;

	//get Parser
	if(mTableName != "" && mSaveOpr != "")
	{
		AosRundataPtr rdataPtr(rdata);
		AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTableName);
		if (!table_doc)
		{
			setErrMsg(rdata, eNotExist, mTableName, mErrmsg);
			return false;
		}
		if(mIsService)
		{
			mDB = table_doc->getAttrStr("zky_database", "");
		}
		OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
		aos_assert_rr(schema_objid != "", rdata, false);
		AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
		aos_assert_rr(schema_doc, rdata, false);
		mParser = schema_doc->getAttrStr("zky_name","");
		if(mIsService)
		{
			AosXmlTagPtr record_doc = schema_doc->getFirstChild();
			record_doc = record_doc->getFirstChild();
			mSchemaName = record_doc->getAttrStr("zky_name");
		}
	}
	// parse distinct
    // 2015/8/25
    OmnString distinct = jimo_parser->getParmStr(rdata, "distinct", name_value_list);
    if (distinct != "")
     mDistinct = distinct;

	//parse fields
	rslt = parseFields(rdata, name_value_list, jimo_parser,mErrmsg);
	if (!rslt) return false;

	//set iilname
	if(mSplitter.mType != "distribution_map")
	{
		if(mIsService)
		{
			mIILName << "_zt44_" << mDB << "_" << mTableName
			<< "_" << mIndexKey;
		}
		else
			mIILName << "_zt45_" << mDataProcName << "_cache";
	}
	else
	{
		if(mIsService)
		{
			mIILName << "_zt44_" << mDB << "_" << mTableName
			<< "_" << mIndexKey;
		}
		else
			mIILName << "_zt4i_" << mDataProcName << "_cache_$group";
	}

	//parse save doc
	//if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
	//{
	//	AosLogError(rdata, true, "missing_savedoc_attributes");
	//	return false;
	//}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcSelect::parseFields(
		AosRundata *rdata,
		vector<AosExprObjPtr> &name_value_list,
		AosJimoParserObj *jimo_parser,
		const OmnString& errmsg)
{
	vector<AosExprObjPtr> distincts;
	vector<AosExprObjPtr> field_names;
	vector<OmnString>	fields;
    /*
	bool rslt = jimo_parser->getParmArray(rdata, "distinct", name_value_list, distincts);
	if (rslt)
	{
		if (distincts.size() <= 0)
		{
			setErrMsg(rdata,eMissingParm,"distinct",mErrmsg);
			return false;
		}
		stmt->mDistinct = distincts;
		if (distincts.size() > 0 && distincts[0u]->getValue(rdata) != "all")
		{
			if (stmt->mCache == "true")
			{
				for (size_t i = 0; i < stmt->mDistinct.size(); i++)
				{
					OmnString name = (stmt->mDistinct)[i]->dumpByNoQuote();
					stmt->mSubFields.push_back(name);
				}
			}
		}
	}
    */
	// Fields must be in the following format:
	// 	fields: [(alias:ExprString("f1"), fname:"key_field1", type:"u64", max_length:8),
	//			 (alias:"f1", fname:"key_field1", type:"u64", max_length:8),
	//			 ...]
	//  'alias' is optional,
	//  'fname': identifies the field name. This is mandatory.
	//  'type': is the field's data type. This is mandatory.
	//  'max_length': is the field's max length. It is optional.
	//if (distincts.size() <= 0 || distincts[0u]->getValue(rdata) == "all")
	//{
	bool rslt = jimo_parser->getParmArray(rdata, "fields", name_value_list, field_names);
	if (rslt)
	{
		if(field_names.size() <= 0)
		{
			setErrMsg(rdata, eGenericError, "fields",mErrmsg);
			return false;
		}
	}
	JSONValue fieldJSON;
	for (u32 i=0; i<field_names.size(); i++)
	{
		// {"fname":"xxx", "alias":"xxx","alias":"xxx","max_len":"xxx},
		// {"fname":"xxx", "alias":"xxx","alias":"xxx","max_len":"xxx},

		if(!mIsService)
		{
			if (field_names[i]->getType() != AosExprType::eBrackets)
			{
				AosLogError(rdata, true, "incorrect_field_specs") << enderr;
				return false;
			}
		}

		vector<AosExprObjPtr> field_exprs = field_names[i]->getExprList();
		OmnString alias, fname, type, max_length;
		for (u32 k=0; k<field_exprs.size(); k++)
		{
			if (field_exprs[k]->getType() != AosExprType::eNameValue)
			{
				AosLogError(rdata, true, "expr type error") << enderr;
				return false;
			}

			OmnString nn = field_exprs[k]->getName().toLower();
			if (nn == "alias") { alias = field_exprs[k]->getValue(rdata);}
			else if (nn == "fname") 
			{
				fname = field_exprs[k]->getValue(rdata);
				if(i == 0 && k == 0)
				{
					if(mIsService)
					{
						mIndexKey = fname;
					}
				}
			}
			else if (nn == "type") { type = field_exprs[k]->getValue(rdata);}
			else if (nn == "max_length") { max_length = field_exprs[k]->getValue(rdata);}
		}

		JSONValue fieldJV;
		if (fname == "")
		{
			AosLogError(rdata, true, "missing_fname") << enderr;
			return false;
		}
		else
		{
			fieldJV["fname"] = fname.data();
		}
		if (type != "")
		{
			fieldJV["type"] = type.data();
		}

		if (alias == "")  alias = fname;
		fieldJV["alias"] = alias.data();
		fields.push_back(alias);
		if (max_length != "")
		{
			fieldJV["max_length"] = max_length.toInt();
		}
		mFieldsJV.append(fieldJV);
		mFieldsName.push_back(fname);
		mSubFields.push_back(alias);
	}
	mFields = field_names;
	if(!checkOrderFieldIsExist(rdata,errmsg,fields))
		return false;
	//}
	return true;
}


bool
AosJimoLogicDataProcSelect::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	//savedoc
	//if (mSaveDoc == "true")
	//{
		AosJqlStatementPtr stmt = dynamic_cast<AosJqlStatement*>(this);
		OmnString jql = stmt->getOrigStatement();
		bool rslt = createJimoLogicDoc(rdata, mDataProcName, jql);
		aos_assert_rr(rslt, rdata, false);
	//}

	if(mIsService)
	{	
		//jimodb-753
		//vector<pair<OmnString, pair<OmnString,OmnString> > >fieldconfs;
		//bool rslt = getFieldsConf(rdata, prog, fieldconfs);

		vector<OmnString> field_strs;                       
		bool rslt = getFieldsConf(rdata, prog, field_strs); 
		aos_assert_r(rslt, false);
		
		AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSelect*>(this);
		prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
		mIdx++;
	}
	//proc inputs
	aos_assert_rr(prog, rdata, false);
	if (mInput != "")
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInput, prog, mInputName, statements_str);
		if (!rslt)
		{
			OmnString msg = "dataproc select ";
			msg << mDataProcName << " missing input : " << mInput << ".";
			rdata->setJqlMsg(msg);
			return false;
		}

		if (mInputName == "")
		{
			AosLogError(rdata, true, "failed_retrieving_output_name")
				<< AosFN("Input Name") << mInput << enderr;
			return false;
		}
	}

	rslt = createSelectDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	if(!mIsService)
	{
		//task for select
		rslt = addSelectTask(rdata, prog);
		aos_assert_rr(rslt, rdata, prog);
	}

	if(mSaveOpr == "true")
	{
		mInput = mDataProcName;

		rslt = createDataProcIndex(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);
		
		if(mIsService)
		{
			rslt = createIILBatchoprDataproc(rdata, prog, statements_str);
			aos_assert_rr(rslt, rdata, false);
		}

        // 2015/08/24
        rslt = createDataProcDoc(rdata,prog,statements_str);
        aos_assert_rr(rslt,rdata,false);
		/*
		if(!mIsService)
		{
			rslt = addDocTask(rdata, prog);
			aos_assert_rr(rslt, rdata, false);
		}
		*/
	}
	else
	{
		if (mCache == "true")
		{
			rslt = createIndexDataproc(rdata, prog, statements_str);
			aos_assert_rr(rslt, rdata, false);

			rslt = createIILBatchoprDataproc(rdata, prog, statements_str);
			aos_assert_rr(rslt, rdata, false);

			if(!mIsService)
			{
				//task for indnx
				rslt = addIndexTask(rdata, prog);
				aos_assert_rr(rslt, rdata, false);

				//task for iil
				rslt = addIILTask(rdata, prog);
				aos_assert_rr(rslt, rdata, false);
			}
		}
	}
	return true;
}


bool
AosJimoLogicDataProcSelect::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata,jimo_prog,  stmt, statements);
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
		if(mSaveOpr == "true")
		{
			rslt = statements[i]->compileJQL(rdata, jimo_prog);
			aos_assert_r(rslt, false);
		}
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool
AosJimoLogicDataProcSelect::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'.
	// 2.
	//
	// mInput format:dataset or dataproc.output
	vector<pair<OmnString, pair<OmnString,OmnString> > >fieldconfs;
	 vector<OmnString> field_strs;
	bool rslt = getFieldsConf(rdata, prog, field_strs);
	aos_assert_r(rslt, false);

	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSelect*>(this);
	rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	mJobName = prog->getJobname();
	mDataprocSelectName << "_dsdp_" << mJobName << "_" << mDataProcName;
	if (mOutput == "")
	{
		mOutputName << mDataprocSelectName << "_output";
	}
	else
	{
		bool rslt = processOutput(rdata, prog, mOutput);
		aos_assert_r(rslt, false);
	}
	mOutputNames.push_back(mOutputName);

	mDsOutputNames[mDataProcName] = mOutputName;
	return true;
}


OmnString
AosJimoLogicDataProcSelect::getNewDataProcName()const
{
	return mDataprocSelectName;
}


bool
AosJimoLogicDataProcSelect::processOutput(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		const OmnString &output)
{
	vector<OmnString> outputs;
	const char *sep = ".";
	int num = AosSplitStr(output, sep, outputs, 3);
	aos_assert_rg(num <= 2, rdata, false,
			AosErrMsg("invalid_output").field("Output", output));
	aos_assert_rg(num >= 1, rdata, false, AosErrMsg("internal_error"));
	if (num == 1)
	{
		mOutputName = output;
	}
	bool rslt = AosJimoLogicNew::procOutput(rdata, outputs, prog,  mOutputName, mIILName);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosJimoLogicDataProcSelect::createDataProcIndex(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	//wo create a dataproc index to save our data
	OmnString dataprocdocbatch_str = "";
	OmnString dataprocIndex_str = "";
	if(mIsService)
	{
		OmnString dataprocName = "";
		OmnString mapname_str;

		dataprocName = "dpIndex_";
		dataprocName << mSelName;
		mSelectList.push_back(dataprocName);
		mIndexName = dataprocName;
		OmnString inName = mSelectName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mIndexName);

		dataprocIndex_str << "\"" <<dataprocName << "\""
			<< " : { "
			<< "\"type\":" << "\"dataprocindex\"" << ","
			<< "\"format\":" << "\"" << mSchemaName << "\","
			<< "\"keys\":[";
		/*
		if (mDistinct.size() > 0)
		{
			for(u32 i=0; i< mDistinct.size(); i++)
			{
				OmnString name = mDistinct[i]->dumpByNoQuote();
				if(i>0)
					dataprocIndex_str << ",";
				dataprocIndex_str <<  "\"" << name << "\"" ;
			}
		}
		else
		{*/
			for(u32 i=0; i< mFields.size(); i++)
			{
				if(i>0) dataprocIndex_str << ",";
				//dataprocIndex_str <<  "\"" << mFieldsName[i]<< "\"" ;
				dataprocIndex_str <<  "\"" << mSubFields[i]<< "\"" ;
			}
		//}
		
		dataprocIndex_str << "],";
		if (mCond != "")
		{
			dataprocIndex_str << "\"condition\":" << "\"" << mCond << "\""; 
		}
		dataprocIndex_str << "\"record_type\":" << "\"buff\"" << ","
			<< "\"docid\":" << "\"docid\""
			<< "} ";
	}
	else
	{
		dataprocdocbatch_str = "dataproc index ";
		//use dp_dataProcName to incadite the docBatchopr name;
		mDocBatchOprName = "";
		mDocBatchOprName <<  "dp_" << mDataProcName << "_idx";

		//	mDataprocIILName << "_dsdp_iil" << mDataProcName;
		dataprocdocbatch_str << mDocBatchOprName << " (";

		dataprocdocbatch_str << "inputs :" << mInput;
		dataprocdocbatch_str << ",table :" << mTableName;

		dataprocdocbatch_str << ");";
	}

	OmnScreen << dataprocdocbatch_str << endl;
	statements_str << "\n" << dataprocdocbatch_str;

	if(mIsService)
		jimo_prog->setConf(dataprocIndex_str);
	else
	{
    	bool rslt = parseRun(rdata,dataprocdocbatch_str,jimo_prog);
	    aos_assert_rr(rslt,rdata,false);
	}
	return true;
}

bool
AosJimoLogicDataProcSelect::createDataProcDoc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	//wo create a dataproc index to save our data
	OmnString dataprocdocbatch_str = "dataproc doc ";
    OmnString dataprocname;
	//use dp_dataProcName to incadite the docBatchopr name;
	dataprocname <<  "dp_" << mDataProcName << "_doc";

	OmnString inName = mSelectName;
	inName << ".output";
	jimo_prog->insertToDataFlowMap(inName, dataprocname);

	//	mDataprocIILName << "_dsdp_iil" << mDataProcName;
	dataprocdocbatch_str << dataprocname << " (";

	dataprocdocbatch_str << "inputs :" << mInput;
	dataprocdocbatch_str << ",table :" << mTableName;
    dataprocdocbatch_str << ",format :" << mParser;
    dataprocdocbatch_str << ",fields :[" ;

	for (size_t i = 0; i < mSubFields.size(); i++)
	{
		if (i>0)
			dataprocdocbatch_str << ", ";
		dataprocdocbatch_str << "\"" << mSubFields[i] << "\"";
	}
	dataprocdocbatch_str << "] ";

	dataprocdocbatch_str << ");";

	OmnScreen << dataprocdocbatch_str << endl;
	statements_str << "\n" << dataprocdocbatch_str;

    bool rslt = parseRun(rdata,dataprocdocbatch_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}

bool
AosJimoLogicDataProcSelect::createDataProcDocBatchOpr(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	OmnString dataprocdocbatch_str = "create dataprocdocbatchopr ";
	dataprocdocbatch_str << mDataProcName << "_DocBatchOpr"<< " {"
		<< "\"type\":\"docbatchopr\""
		<< ", \"data_opr\":\"batch_insert\"";
	//if (!mHaveColumn)
	//{
	//	dataprocdocbatch_str << ", \"format\":\"" << mParser << "\""
	//		    << ", \"fields\":[";
	//	for (size_t i = 0; i < mFields.size(); i++)
	//	{
	//		if (i>0)
	//			dataprocdocbatch_str << ", ";
	//		dataprocdocbatch_str << "\"" << mFields[i] << "\"";
	//	}
	//	dataprocdocbatch_str << "] ";
	//}
	if(!mIsService)
		dataprocdocbatch_str << ", \"format\":\"" << mParser << "\"";
	else
		dataprocdocbatch_str << ", \"format\":\"" << mSchemaName << "\"";
	
		dataprocdocbatch_str << ", \"fields\":[";
	for (size_t i = 0; i < mSubFields.size(); i++)
	{
		if (i>0)
			dataprocdocbatch_str << ", ";
		dataprocdocbatch_str << "\"" << mSubFields[i] << "\"";
	}
	dataprocdocbatch_str << "] ";

	if(!mIsService)
		dataprocdocbatch_str << ", \"docid\":" << "\"getDocid('" << mParser << "')\"};";
	else
		dataprocdocbatch_str << ", \"docid\":" << "\"docid\"} ";

	OmnScreen << dataprocdocbatch_str << endl;
	statements_str << "\n" << dataprocdocbatch_str;

	if(mIsService)
		jimo_prog->setConf(dataprocdocbatch_str);
	else
	{
		bool rslt = parseRun(rdata,dataprocdocbatch_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;

}

bool
AosJimoLogicDataProcSelect::createStreamSelectDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	OmnString dataprocSelect_str = "";
	OmnString dataprocName = "";
	
	dataprocName = mDPName;
	dataprocName << "_" << mIdx;
	mSelName = dataprocName;

	dataprocName = mDPName;
	mSelectName = dataprocName;
	if(mInput != "")
	{
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
	}

	//set select null
	mSelectList.clear();
	mSelectList.push_back(mInput);
	mSelectList.push_back(dataprocName);
		
	dataprocSelect_str << "\"" << dataprocName << "\"" << " : { "
					   << "\"type\":" << "\"" << "dataprocselect" << "\""
					   << ",\"assign_docid\": \"true\""
					   << ",\"format\": \"" << mSchemaName << "\""
					   << ",\"docid\":" << "\"getDocid('" << mParser << "')\""	
					   << ", \"record_type\":" << "\"buff\"";


	if( mCond != "")
	{
    	dataprocSelect_str << ",\"condition\":" << "\"" << mCond << "\"" ;
	}

	if (mGroupByFields.size()>0)
	{
		dataprocSelect_str << ", \"groupby_fields\":[";
		for (u32 i=0; i<mGroupByFields.size(); i++)
		{
			if (i > 0) dataprocSelect_str << ", ";
			dataprocSelect_str << "\"" << mGroupByFields[i] << "\"";
		}
		dataprocSelect_str << "]";

	}
	if (mOrderFields.size() > 0)
	{
		dataprocSelect_str << ", \"orderby_fields\":[";
		for (u32 i=0; i<mOrderFields.size(); i++)
		{
			if (i > 0) dataprocSelect_str << ", ";
			dataprocSelect_str << "\"" << mOrderFields[i] << "\"";
		}
		dataprocSelect_str << "]";
	}

	if (mOrderType.size() > 0)
	{
		dataprocSelect_str << ", \"order_by_type\":[";
		for (u32 i=0; i<mOrderType.size(); i++)
		{
			if (i > 0) dataprocSelect_str << ", ";
			dataprocSelect_str << "\"" << mOrderType[i] << "\"";
		}
		dataprocSelect_str << "]";
	}

	if(mSplitter.mShuffleType != "")
	{
        dataprocSelect_str << ",\"shuffle_type\":" << "\"" << mSplitter.mShuffleType<< "\"" ;
	}
	if(mSplitter.mSplitField != "")
	{
       dataprocSelect_str << ",\"shuffle_field\":" << "\"getcubeidfromdistmap(" << mSplitter.mDistributionMapName << "," << mSplitter.mSplitField << ")" << "\"";
	}

	if (mDistinct != "")
	{
		dataprocSelect_str << ",\"distinct\":" << mDistinct;
	}
    
	if (mFieldsJV.empty())
    {
        return false;
    }

	dataprocSelect_str << ",\"fields\":" << mFieldsJV.toStyledString();
/*
	if (mDistinct.size() > 0)
	{
		dataprocSelect_str << ",\"distinct\":" << "[";
		if (!mFieldsJV.empty())
		{
			dataprocSelect_str << "\"all\"" << "]" << ",\"fields\":" << mFieldsJV.toStyledString();
		}

		else
		{
    		for(u32 i=0; i< mDistinct.size(); i++)
    		{
				OmnString name = mDistinct[i]->dumpByNoQuote();
        		if(i>0) dataprocSelect_str << ",";
        		dataprocSelect_str <<  "\"" << name << "\"";
    		}
			dataprocSelect_str << "]";
		}
	}
	else
	{
		dataprocSelect_str << ",\"fields\":" << mFieldsJV.toStyledString();
		// dataprocSelect_str << ",\"fields\":" << "[";
		// for(u32 i=0; i< mFields.size(); i++)
		// {
		// 	    if(i>0) dataprocSelect_str << ",";
		// 		    dataprocSelect_str <<  "\"" << mFields[i]->dumpByNoQuote() << "\"" ;
		// }
	}
	*/
	dataprocSelect_str << "} ";


OmnScreen <<"qqqqqqqqqqqqqqqqqqq" <<  dataprocSelect_str << endl;
	statements_str << "\n" << dataprocSelect_str;
	jimo_prog->setConf(dataprocSelect_str);
	
	return true;
}

bool
AosJimoLogicDataProcSelect::createSelectDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	if (mIsService)
	{
		return createStreamSelectDataproc(rdata, jimo_prog, statements_str);
	}
	OmnString dataprocSelect_str = "";
	dataprocSelect_str << "create dataprocselect " << mDataprocSelectName;

	JSONValue selectJV;
	selectJV["type"] = "select";
	if (mCond != "") selectJV["condition"] = mCond.data();
	selectJV["output_name"] = mOutputName.data();
	selectJV["record_type"] = "buff";

	if (!mGroupByFields.empty())
	{
		JSONValue groupbyJV;
		for (u32 i=0; i<mGroupByFields.size(); i++)
		{
			groupbyJV.append(mGroupByFields[i].data());
		}
		selectJV["groupby_fields"] = groupbyJV;
	}

	if (mOrderFields.size() > 0)
	{
		JSONValue orderbyJV;
		for (u32 i=0; i<mOrderFields.size(); i++)
		{
			orderbyJV.append(mOrderFields[i].data());
		}
		selectJV["orderby_fields"] = orderbyJV;
	}

	if (mOrderType.size() > 0)
	{
		JSONValue orderbyTypeJV;
		for (u32 i=0; i<mOrderType.size(); i++)
		{
			orderbyTypeJV.append(mOrderType[i].data());
		}
		selectJV["order_by_type"] = orderbyTypeJV;
	}

	if(mSplitter.mShuffleType != "") selectJV["shuffle_type"] = mSplitter.mShuffleType.data();
	if(mSplitter.mSplitField != "")
	{
		OmnString shuffleField = "";
		shuffleField << "\"getcubeidfromdistmap(" << mSplitter.mDistributionMapName << "," << mSplitter.mSplitField << ")" << "\"";
		selectJV["shuffle_field"] = shuffleField.data();
	}

    if (mDistinct != "")
    {
        selectJV["distinct"] = mDistinct.data();
    }

    if (mFieldsJV.empty())
    {
        return false;
    }

    selectJV["fields"] = mFieldsJV;
    /*
	if (mDistinct.size() > 0)
	{
		JSONValue distJV;
		if (!mFieldsJV.empty())
		{
			//distJV.append("all");
            if(mDistinct != "")
			selectJV["distinct"] = mDistinct.data();
			selectJV["fields"] = mFieldsJV;
		}
		else
		{
    		for(u32 i=0; i< mDistinct.size(); i++)
    		{
				OmnString name = mDistinct[i]->dumpByNoQuote();
				distJV.append(name.data());
    		}
			selectJV["distinct"] = distJV;
		}
	}
	else
	{
		selectJV["fields"] = mFieldsJV;
	}
    */
	dataprocSelect_str << " " << selectJV.toStyledString() << ";";
OmnScreen <<  dataprocSelect_str << endl;
	statements_str << "\n" << dataprocSelect_str;
    bool rslt = parseRun(rdata,dataprocSelect_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicDataProcSelect::createStreamIndexDataproc(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	OmnString dataprocIndex_str = "";
	OmnString dataprocName;
	OmnString mapname_str;
	dataprocName = "dpIndex_";
	dataprocName << mSelName;
	mSelectList.push_back(dataprocName);

	mIndexName = dataprocName;
	OmnString inName = mSelectName;
	inName << ".output";
	jimo_prog->insertToDataFlowMap(inName, mIndexName);

	dataprocIndex_str << "\"" << dataprocName << "\" : { "
					  << "\"type\":" << "\"dataprocindex\","
        			  << "\"docid\":" << "\"docid\","
					  << "\"record_type\":" << "\"buff\","
					  << "\"format\":" << "\"" << mSchemaName << "\","
					  << "\"keys\":[";
/*
	if (mDistinct.size() > 0)
	{
    	for(u32 i=0; i< mDistinct.size(); i++)
    	{
			OmnString name = mDistinct[i]->dumpByNoQuote();
        	if(i>0)
            	dataprocIndex_str << ",";
        	dataprocIndex_str <<  "\"" << name << "\"" ;
		}
    }

	else
	{*/
		for(u32 i=0; i< mFields.size(); i++)
		{
			if(i>0) dataprocIndex_str << ",";
			dataprocIndex_str <<  "\"" << mSubFields[i]<< "\"" ;
		}
	//}
    dataprocIndex_str << "]";
	if (mCond != "")
	{
		dataprocIndex_str << ",\"condition\":" << "\"" << mCond << "\""; 
	}
	dataprocIndex_str << "}";

	OmnScreen << dataprocIndex_str << endl;
	statements_str << "\n" << dataprocIndex_str;
	
	jimo_prog->setConf(dataprocIndex_str);
	return true;
}

bool
AosJimoLogicDataProcSelect::createIndexDataproc(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	if (mIsService)
	{
		return createStreamIndexDataproc(rdata, jimo_prog, statements_str);
	}
	//get Fields maxLength from JimoProg
	u64 totalKeyLen = 0;
	bool rslt;
	//if(mDistinct.size() > 0)
	//{
		vector<OmnString> schema_names;
		rslt = getSchemaName(rdata, jimo_prog ,schema_names);
		aos_assert_rr(rslt, rdata, false);

		vector<AosXmlTagPtr> datafields_docs;
		rslt = getDataFieldsDocs(rdata, schema_names, datafields_docs);
		aos_assert_rr(rslt, rdata, false);

		rslt = getFieldsInfo(rdata, datafields_docs, totalKeyLen);
		aos_assert_rr(rslt, rdata, false);
	//}

	OmnString dataprocName;
    OmnString mapname_str;
	// 6/11
	mDataprocIndexName << "_dsdp_" << mJobName << "_" << mDataProcName << "_cache_map";

	OmnString dataprocIndex_str = "";
	dataprocIndex_str << "create dataprocindex " << mDataprocIndexName ;

	JSONValue indexJV;
	indexJV["type"] = "index";
	JSONValue keysJV;
    /*
	if (!mDistinct.empty())
	{
    	for(u32 i=0; i< mDistinct.size(); i++)
    	{
			OmnString name = mDistinct[i]->dumpByNoQuote();
			keysJV.append(name.data());
		}
	}
    */
	//else
	//{
	// jimodb-745 
	// 2015.09.11
	for(u32 i=0; i< mFieldsJV.size(); i++) keysJV.append(mSubFields[i].data());
	//}

	indexJV["keys"] = keysJV;
	//if (mCond != "") indexJV["condition"] = mCond.data();
	indexJV["record_type"] = "buff";
	indexJV["max_keylen"] = JSONValue((UInt)totalKeyLen);
	indexJV["docid"] = "1";

	dataprocIndex_str << " " << indexJV.toStyledString() << ";";
	OmnScreen << dataprocIndex_str << endl;
	statements_str << "\n" << dataprocIndex_str;

    rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcSelect::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	OmnString dataprociil_str = "";
	if(!mIsService)
	{
		dataprociil_str = "create dataprociilbatchopr ";
		mDataprocIILName << "_dsdp_" << mJobName << "_" << mDataProcName << "_cache_reducer";
		dataprociil_str << mDataprocIILName << " { "
						<< "\"iilname\":" << "\"" << mIILName << "\"";
	}
	else
	{
		OmnString dataprocName = "";
		dataprocName = "dpIILBatchOpr_";
		dataprocName << mSelName;
		mDataprocIILName = dataprocName; 
		mSelectList.push_back(dataprocName);
		OmnString inName = mIndexName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataprocIILName);

		dataprociil_str << "\"" <<  mDataprocIILName << "\"" << " : { "
						<< "\"iilname\":" << "\"" << mIILName << "\""
						<< ",\"iil_type\" :" << "\"BigStr\""
						<< ",\"type\" :" << "\"dataprociilbatchopr\"";
	}
	//jimodb-750
	//2015-09-11 Arvin
	if (mOpr == "add")
	{
//		if(mIsService)
			dataprociil_str << ",\"opr\":" << "\"add\"";
//		else
//			dataprociil_str << ",\"data_opr\":" << "\"add\"";
	}
	if (mOpr == "del")
	{
//		if(mIsService)
			dataprociil_str << ",\"opr\":" << "\"del\"";
//		else
//			dataprociil_str << ",\"data_opr\":" << "\"del\"";
	}
	else
	{
//		if(mIsService)
			dataprociil_str << ",\"opr\":" << "\"add\"";
//		else
//			dataprociil_str << ",\"data_opr\":" << "\"add\"";
	}

	if(mSplitter.mShuffleType != "")
	{
       dataprociil_str << ",\"shuffle_type\":" << "\"" << mSplitter.mShuffleType << "\"";
	}
	if(!mIsService)
		dataprociil_str << "};";
	else
		dataprociil_str << "} ";

	OmnScreen << dataprociil_str << endl;
	statements_str << "\n" << dataprociil_str;

	if(mIsService)
		jimo_prog->setConf(dataprociil_str);
	else
	{
		bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}


bool
AosJimoLogicDataProcSelect::addSelectTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addselect_str;
	OmnString output;
	OmnString taskName;

	JSONValue idxtaskJV;
	JSONValue dataprocsJV;
	JSONValue dataprocJV;
	JSONValue inputJV;
	JSONValue outputJV;

	taskName << "task_select_" << mDataProcName;

    dataprocJV["dataproc"] = mDataprocSelectName.data();
    inputJV.append(mInputName.data());
    dataprocJV["inputs"] = inputJV;

    outputJV.append(mOutputName.data());
    dataprocJV["outputs"] = outputJV;

    dataprocsJV.append(dataprocJV);

    idxtaskJV["name"] = taskName.data();
    idxtaskJV["dataengine_type"] = "dataengine_scan2";

    idxtaskJV["dataprocs"] = dataprocsJV;
    addselect_str << idxtaskJV.toStyledString() << ";";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata,"tasks", addselect_str);
	OmnScreen << addselect_str << endl;
	return true;

}


bool
AosJimoLogicDataProcSelect::addIndexTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addindex_str;
	OmnString output;
	OmnString taskName;

	JSONValue idxtaskJV;
	JSONValue dataprocsJV;
	JSONValue dataprocJV;
	JSONValue inputJV;
	JSONValue outputJV;

	taskName << "task_select_index_" << mDataProcName;
	mIndexOutput << mDataprocIndexName << "_output";

    dataprocJV["dataproc"] = mDataprocIndexName.data();
    inputJV.append(mOutputName.data());
    dataprocJV["inputs"] = inputJV;

    outputJV.append(mIndexOutput.data());
    dataprocJV["outputs"] = outputJV;

    dataprocsJV.append(dataprocJV);

    idxtaskJV["name"] = taskName.data();
    idxtaskJV["dataengine_type"] = "dataengine_scan2";

    idxtaskJV["dataprocs"] = dataprocsJV;
    addindex_str << idxtaskJV.toStyledString() << ";";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata,"tasks", addindex_str);
	OmnScreen << addindex_str << endl;
	return true;
}


	bool
	AosJimoLogicDataProcSelect::addDocTask(
				AosRundata *rdata,
				AosJimoProgObj *prog)
	{
		OmnString add_doc_str;
		OmnString taskname;
		taskname << "task_index_" << mDataProcName << "_DocBatchOpr";
		//input << mDataprocIndexName << "_output";
		//addgroupby_str << jobname;
		JSONValue doctaskJV;
		JSONValue dataprocsJV;
		JSONValue dataprocJV;
		JSONValue inputJV;

		OmnString dataproc_name;
		dataproc_name << mDataProcName << "_DocBatchOpr";
		dataprocJV["dataproc"] = dataproc_name.data();
		inputJV.append(mOutputName.data());
		dataprocJV["inputs"] = inputJV;

		dataprocsJV.append(dataprocJV);

		doctaskJV["name"] = taskname.data();
		doctaskJV["dataengine_type"] = "dataengine_scan2";

		doctaskJV["dataprocs"] = dataprocsJV;
		add_doc_str << doctaskJV.toStyledString() << ";";

		if(prog)
		{
			prog->appendStatement(rdata, "tasks",add_doc_str);
			prog->addTask(rdata, add_doc_str, "");
		}

		OmnScreen << add_doc_str << endl;
		return true;
	}


	bool
	AosJimoLogicDataProcSelect::addIILTask(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog)
	{
		OmnString addiil_str;
		OmnString taskName;
		taskName << "task_select_iil_" << mDataProcName;
		//input << mDataprocIndexName << "_output";
		//addgroupby_str << jobname;
		JSONValue iiltaskJV;
		JSONValue dataprocsJV;
		JSONValue dataprocJV;
		JSONValue inputJV;


		dataprocJV["dataproc"] = mDataprocIILName.data();
		inputJV.append(mIndexOutput.data());
		dataprocJV["inputs"] = inputJV;

		dataprocsJV.append(dataprocJV);

		iiltaskJV["name"] = taskName.data();
		iiltaskJV["dataengine_type"] = "dataengine_scan2";

		iiltaskJV["dataprocs"] = dataprocsJV;
		addiil_str << iiltaskJV.toStyledString() << ";";
		if(jimo_prog)
			jimo_prog->appendStatement(rdata,"tasks" ,addiil_str);
		OmnScreen << addiil_str << endl;
		return true;
	}


bool
AosJimoLogicDataProcSelect::setOutputName(
		    AosRundata *rdata,
			AosJimoProgObj *prog,
			const OmnString &name)
{
	mOutputName = name;
	return true;
}


bool
AosJimoLogicDataProcSelect::setUnionDatasetName(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			const OmnString &name)
{
	mUnionDatasetName = name;
	return true;
}


bool
AosJimoLogicDataProcSelect::setFields(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			const OmnString &field_str)
{
	JSONReader reader;
	bool rslt = reader.parse(field_str, mFieldsJV);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosJimoLogicDataProcSelect::getFields(vector<AosExprObjPtr> &fields)
{
	fields = mFields;
	return true;
}

bool
AosJimoLogicDataProcSelect::getDistinct(vector<AosExprObjPtr> &distinct)
{
	//distinct = mDistinct;
	return true;
}

OmnString
AosJimoLogicDataProcSelect::getLogicName() const
{
	const OmnString name = "select";
	return name;
}


AosJimoPtr 
AosJimoLogicDataProcSelect::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSelect(*this);
}


bool
AosJimoLogicDataProcSelect::getInputV(vector<OmnString> &inputs)
{
	inputs.push_back(mInput);
	return true;
}

// jimodb-753
/*
bool
AosJimoLogicDataProcSelect::getDataFieldsDocs(
		AosRundata *rdata,
		const vector<OmnString> &schema_names,
		vector<AosXmlTagPtr> &datafields_docs)
{
	aos_assert_rr( schema_names.size()>0, rdata, false);
	datafields_docs.clear();

	for(size_t i = 0; i < schema_names.size(); i++ )
	{
		AosXmlTagPtr schema_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, schema_names[i]);
		aos_assert_rr(schema_doc, rdata, false);
		AosXmlTagPtr data_fields_doc = schema_doc->getFirstChild("datafields");
		aos_assert_rr(data_fields_doc, rdata, false);
		datafields_docs.push_back(data_fields_doc);
	}
	return true;
}
*/

// jimodb-753
bool
AosJimoLogicDataProcSelect::configFields(
		AosRundata *rdata,
		vector<OmnString> &field_strs)
{	
	JSONValue fieldsJV;
	JSONReader reader; 
	OmnString fname;
	for (size_t k = 0; k < field_strs.size(); k++)
	{
		// jimodb-1054, 2015.10.27
		reader.parse(field_strs[k], fieldsJV); 

		if (mFieldsJV.size() <= 0)
		{
			//mFieldsJV = fieldsJV;
			for( size_t i = 0; i < fieldsJV.size(); i++)
			{
				fname =  fieldsJV[i]["alias"].asString();
				if ( fname == "")
				{
					fname = fieldsJV[i]["fname"].asString();
				}
				mFieldsJV.append(fieldsJV[i]);
				mSubFields.push_back(fname);
			}

			if (field_strs.size()-1 == k)
			{
				return true;
			}

		}
		
		else
		{
			for (size_t i = 0; i < mFieldsJV.size(); i++)
			{
				fname = mFieldsJV[i]["fname"].asString();
				int flag = false;
				for(size_t j = 0; j < fieldsJV.size(); j++)
				{
					OmnString name = fieldsJV[j]["fname"].asString();
					OmnString type; 
					if (mFieldsJV[i]["type"].asString() == "")
					{
						if (fname != name)
						{
							name = fieldsJV[j]["alias"].asString();
							if (fname != name)
							{
								continue;
							}
						}
						flag = true;
						OmnString ftype = fieldsJV[j]["type"].asString();
						if (ftype == "string")
						{
							ftype = "str";
						}
						mFieldsJV[i]["type"] = ftype.data();
					}
					else if(mFieldsJV[i]["type"].asString() == "string")
					{
						type = "str";
						mFieldsJV[i]["type"] = type.data();
					}
					break;
				}

				if (!flag && mFieldsJV[i]["type"].asString() == "" && 0 >= field_strs.size())
				{
					mErrmsg << "could not find key  \"" << mFieldsJV[i]["fname"].asString() << "\" in input's fields";
					setErrMsg(rdata,eGenericError,"",mErrmsg);
					return false;
				}
			}
		}
	}
	return true;
}

// jimodb-753
#if 0
bool
AosJimoLogicDataProcSelect::configFields(
		AosRundata *rdata,
		const vector<AosXmlTagPtr> &datafields_docs)
{
	JSONValue fieldJV;
    bool rslt;
    if (mFieldsJV.size()<=0)
    {
        rslt =  getFieldsFromSchema(rdata, datafields_docs);
        return rslt;
    }
	for(size_t i = 0; i < mFieldsJV.size(); i++)
	{
		for(size_t j = 0; j < datafields_docs.size(); j++)
		{
			bool flag = false;
	 		AosXmlTagPtr data_field_doc = datafields_docs[j]->getFirstChild();
			while(data_field_doc)
			{
				OmnString fname = data_field_doc->getAttrStr("zky_name","");
				OmnString name =  mFieldsJV[i]["fname"].asString();
				if(fname == name)
				{
					OmnString ftype,flength;
					ftype = data_field_doc->getAttrStr("type","");
					if(ftype == "")
					{
						AosLogError(rdata, true, "miss_field_type") << enderr;
						aos_assert_r(false, false);
						return false;
					}
					if(ftype == "expr")
					{
						ftype = data_field_doc->getAttrStr("data_type","");
						if(ftype == "varchar") ftype = "str";
						else if(ftype == "datetime") ftype = "datetime";
						else if(ftype == "str") ftype = "str";
						else if(ftype == "string") ftype = "str";
						else if(ftype == "u64") ftype = "u64";
						else if(ftype == "double") ftype = "double";
						else 
						{
							aos_assert_r(false, false);
							return false;
						}
					}
					flength = data_field_doc->getAttrStr("zky_length","");
					if(flength == "")
					{
						aos_assert_r(false, false);
						AosLogError(rdata, true, "miss_field_length") << enderr;
						return false;
					}

					if (mFieldsJV[i]["type"].asString() == "")
						mFieldsJV[i]["type"] = ftype.data();
					//if (mFieldsJV[i]["max_length"].asString() == "")
					//	mFieldsJV[i]["max_length"] = flength.toInt();
					//JSONValue fieldInfoJV;
					//fieldInfoJV["fname"] = fname.data();
					//fieldInfoJV["type"] = ftype.data();
					//arvin 2015.08.07
					//JIMODB-309
					//fieldInfoJV["max_length"] = flength.toInt();
					//fieldJV.append(fieldInfoJV);
					flag = true;
					break;
				}
				data_field_doc = datafields_docs[j]->getNextChild();
			}
			if(flag)
				break;
		}
	}
	//mFieldsJV = fieldJV;
	return true;
}
#endif

bool
AosJimoLogicDataProcSelect::getFieldsInfo(
		AosRundata *rdata,
		const vector<AosXmlTagPtr> &datafields_docs,
		u64   &totalLength)
{
	for(size_t i = 0; i < mSubFields.size(); i++)
	{
		for(size_t j = 0; j < datafields_docs.size(); j++)
		{
	 		AosXmlTagPtr data_field_doc = datafields_docs[j]->getFirstChild();
			while(data_field_doc)
			{
				OmnString fname = data_field_doc->getAttrStr("zky_name","");
				if(fname == mSubFields[i])
				{
					OmnString ftype,flength;
					flength = data_field_doc->getAttrStr("zky_length","");
					totalLength	+= flength.toU64();
					break;
				}
				data_field_doc = datafields_docs[j]->getNextChild();
			}
		}
	}
	return true;
}

// jimodb-753
bool
AosJimoLogicDataProcSelect::getFieldStr(vector<OmnString> &field_strs)
{
	//field_str = mFieldsJV.toStyledString();
	field_strs.push_back(mFieldsJV.toStyledString());
	return true;
}

//jimodb-753
bool
AosJimoLogicDataProcSelect::getFieldsConf(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		vector<OmnString> &field_strs)
		//vector<pair<OmnString, pair<OmnString,OmnString> > >&fieldconfs)
		//vector<pair<OmnString, OmnString> > &fieldconfs)
{
	bool rslt = getInputSchema(rdata, prog, field_strs);
	aos_assert_rr(rslt, rdata, false);                  

	rslt = configFields(rdata, field_strs);   
	aos_assert_rr(rslt, rdata, false);        

	/*
	vector<OmnString> schema_names;
	bool rslt = getSchemaName(rdata, prog ,schema_names);
	aos_assert_rr(rslt, rdata, false);

	vector<AosXmlTagPtr> datafields_docs;
	rslt = getDataFieldsDocs(rdata, schema_names, datafields_docs);
	aos_assert_rr(rslt, rdata, false);

	rslt = configFields(rdata, datafields_docs);
	aos_assert_rr(rslt, rdata, false);
	*/

	return true;
}


// 2015/8/25
// if no fields selected, get all the fields from schema_doc
bool
AosJimoLogicDataProcSelect::getFieldsFromSchema(
        AosRundata *rdata,
        const vector<AosXmlTagPtr> &datafields_docs)
{

    OmnString fname,ftype,flength;
    JSONValue fieldJV;
	for(size_t j = 0; j < datafields_docs.size(); j++)
	{
		AosXmlTagPtr data_field_doc = datafields_docs[j]->getFirstChild();
		while(data_field_doc)
		{
			OmnString fname = data_field_doc->getAttrStr("zky_name","");
			fieldJV["fname"] = fname.data();
			ftype = data_field_doc->getAttrStr("type","");
			if(ftype == "")
			{
				AosLogError(rdata, true, "miss_field_type") << enderr;
				aos_assert_r(false, false);
				return false;
			}
			flength = data_field_doc->getAttrStr("zky_length","");
			if(flength == "")
			{
				AosLogError(rdata, true, "miss_field_length") << enderr;
				aos_assert_r(false, false);
				return false;
			}

		    fieldJV["type"] = ftype.data();
			fieldJV["max_length"] = flength.toInt();

            mFieldsJV.append(fieldJV);

			// jimodb-745
			// 2015.09.11, xiafan
			mSubFields.push_back(fname);

		    data_field_doc = datafields_docs[j]->getNextChild();
		}
	}
    return true;
}

////////////////////////////////////////////////////////////////////
//arvin 2015.11.25
//Jimodb-1251 check order_by_fields must appear in the select_fields
///////////////////////////////////////////////////////////////////
bool
AosJimoLogicDataProcSelect::checkOrderFieldIsExist(
		AosRundata *rdata,
		const OmnString& errmsg,
		const vector<OmnString>& fields)
{
	if(!mOrderFields.size()) return true;

	vector<OmnString>::const_iterator itr;
	for(size_t i = 0; i < mOrderFields.size(); i++)
	{
		OmnString fieldName = mOrderFields[i];
		itr = find(fields.begin(),fields.end(),fieldName);
		if(itr == fields.end())
		{
			OmnString msg = "[ERR] : ";
			msg << errmsg << " orderBy fields \"" << fieldName << "\" doesn't exist!";
			rdata->setJqlMsg(msg);
			return false;
		}
	}
	return true;
}
