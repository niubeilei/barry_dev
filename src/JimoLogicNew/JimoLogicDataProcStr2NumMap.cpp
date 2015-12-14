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
// 2015/05/25 Created by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcStr2NumMap.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "JQLStatement/JqlStatement.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcStr2NumMap_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcStr2NumMap(version);
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


AosJimoLogicDataProcStr2NumMap::AosJimoLogicDataProcStr2NumMap(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcStr2NumMap::~AosJimoLogicDataProcStr2NumMap()
{
}


bool 
AosJimoLogicDataProcStr2NumMap::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc str2nummap <name>
	// 	(
	// 		table	   : name,
	// 		inputs     :[<name>,<name>..],
	// 		condition  :a>b,
	// 		key_fields :[<name>,<name>...],
	//		value_field:<name>,
	//		max_length: ddd,
	//		data_type  : u64,
	//		aggr_opr   : sum
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "str2nummap", rdata, false);
	mErrmsg = "dataproc str2nummap ";

	// parse dataproc name
	OmnString str2num_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,str2num_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << str2num_name;
	
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = str2num_name;
	mNameValueList = name_value_list;

	// parse table
	mTable = jimo_parser->getParmExpr(rdata, "table", name_value_list);
	if (!mTable)
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}
	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTable->dumpByNoQuote());
	if (!table_doc)
	{
		setErrMsg(rdata, eNotExist, mTable->dumpByNoQuote(), mErrmsg);
		return false;
	}


	//parse inputs and outputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);

	// parse condition
	mCond = jimo_parser->getParmExpr(rdata, "condition", name_value_list);

	// parse key_fields
	rslt = jimo_parser->getParmArrayStr(rdata, "key_fields", name_value_list, mKeyFields);
	if(!rslt || mKeyFields.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"key_fields",mErrmsg);
		return false;
	}

	//parse value_field
	mValueField = jimo_parser->getParmStr(rdata, "value_field", name_value_list);
	if(mValueField == "")
	{
		setErrMsg(rdata,eMissingParm,"value_field",mErrmsg);
		return false;
	}
	
	//parse max_length
	mMaxKeyLen = jimo_parser->getParmInt(rdata, "max_length", name_value_list);
	if(mMaxKeyLen<=0)
	{
		setErrMsg(rdata,eMissingParm,"max_length",mErrmsg);
		return false;
	}

	//parse aggr_opr
	mAggrOpr = jimo_parser->getParmStr(rdata, "aggr_opr", name_value_list);
	if(mAggrOpr == "")
	{
		setErrMsg(rdata,eMissingParm,"aggr_opr",mErrmsg);
		return false;
	}

	//parse data_type
	mDataType = jimo_parser->getParmStr( rdata, "data_type", name_value_list);
	if(mDataType =="")
	{
		setErrMsg(rdata,eMissingParm,"data_type",mErrmsg);
		return false;
	}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcStr2NumMap::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	
	bool rslt = createStr2NumMapDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = createIILBatchoprDataproc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addStr2NumMapTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	rslt = addIILTask(rdata,prog);
	aos_assert_rr(rslt, rdata, prog);

	return true;
}


bool
AosJimoLogicDataProcStr2NumMap::createStr2NumMapDataProc(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString &statement_str)
{
	OmnString str2nummap_str = "create dataprocstr2nummap ";
	str2nummap_str << mNewDataProcName << " { "
			<< "\"type\":\"" << mKeywords[1] << "\",";
	if (mCond)
		str2nummap_str << "\"condition\":\"" << mCond->dumpByNoQuote() << "\",";
	str2nummap_str << "\"data_type\":\"" << mDataType << "\","
			<< "\"aggr_opr\":\"" << mAggrOpr << "\","
			<< "\"value_field\":\"" << mValueField << "\","
			<< "\"max_length\":" << mMaxKeyLen << ",";
	str2nummap_str << "\"key_fields\":[";
	for(size_t i =0; i<mKeyFields.size();i++)
	{
		if(i>0)
			str2nummap_str << ",";
		str2nummap_str << "\""<< mKeyFields[i] << "\"";
	}

	str2nummap_str << "]};";
	OmnScreen << str2nummap_str << endl;
	statement_str << "\n" << str2nummap_str;
	
	bool rslt = parseRun( rdata, str2nummap_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcStr2NumMap::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	OmnString iilName;
	iilName << "_zt44_" << mNewDataProcName << "_map";

	mDataProcIILName << mNewDataProcName << "_reduce" ;
	OmnString dataprociil_str = "create dataprociilbatchopr ";
	dataprociil_str << mDataProcIILName << " { "
		<< "\"iilname\":" << "\"" << iilName << "\"" << ","
		<< "\"opr\":" << "\"stradd\"" << ","
        << "\"build_bitmap\":" << "\"true\""
		<< "};";

	OmnScreen << dataprociil_str << endl;
	statement_str << "\n" << dataprociil_str;

	bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicDataProcStr2NumMap::addStr2NumMapTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString add_str2num_str;
	OmnString taskname;
	vector<OmnString> outputNames;	
	if (!mInputs.empty())
	{
		bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
		aos_assert_rr(rslt ,rdata ,false);
	}

	taskname << "task_str2nummap_"<< mDataProcName;
	add_str2num_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_scan2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mNewDataProcName << "\","
			<< "\"inputs\":" << "[";

	if (outputNames.empty())
	{
		outputNames = mInputs;
	}
	for(size_t i = 0;i < outputNames.size();i++)
	{
		if(i > 0)
			add_str2num_str << ",";
		add_str2num_str << "\"" << outputNames[i] << "\"";
	}
	add_str2num_str << "],";
	add_str2num_str << "\"outputs\":[\"" << mNewDataProcName << "_output\"";
	add_str2num_str << "]}]}";

	if(prog)
		prog->appendStatement(rdata, "tasks",add_str2num_str);

	OmnScreen << add_str2num_str << endl;

	return true;
}


bool
AosJimoLogicDataProcStr2NumMap::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	OmnString TaskName;
	TaskName << "task_iilbatchopr_" << mDataProcName;

	OmnString addiil_str;
	addiil_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcIILName << "\","
		<< "\"inputs\":"<< "["
		<< "\"" << mNewDataProcName << "_output"<< "\"";
	addiil_str << "]}]}";

	if(jimo_prog)
		jimo_prog->appendStatement(rdata,"tasks" ,addiil_str);
	OmnScreen << addiil_str << endl;

	return true;
}


bool
AosJimoLogicDataProcStr2NumMap::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcStr2NumMap*>(this);
	mNewDataProcName << "_dp_" << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	OmnString output_name;
	output_name << mNewDataProcName << "_output";
	mOutputNames.push_back(output_name);
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcStr2NumMap::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcStr2NumMap(*this);
}

