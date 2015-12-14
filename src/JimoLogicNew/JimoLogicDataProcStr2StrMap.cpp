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
#include "JimoLogicNew/JimoLogicDataProcStr2StrMap.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcStr2StrMap_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcStr2StrMap(version);
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


AosJimoLogicDataProcStr2StrMap::AosJimoLogicDataProcStr2StrMap(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcStr2StrMap::~AosJimoLogicDataProcStr2StrMap()
{
}


bool 
AosJimoLogicDataProcStr2StrMap::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc str2str <name>
	// 	(
	// 		table	   :name,
	// 		inputs     :[<name>,<name>..],
	// 		outputs    :[<name>,<name>..],					//option
	// 		condition  :a>b,
	// 		key_fields :[<name>,<name>...],
	//		value_field:<name>,
	//		max_length:35,
	//		data_type : u64,
	//		aggr_opr:sum
	// 	);	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "str2strmap", rdata, false);
	mErrmsg << "dataproc str2strmap ";
	// parse dataset name
	OmnString str2str_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,str2str_name,mErrmsg))
	{
		// This is not what it expects.
		return true;
	}
	mErrmsg << str2str_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = str2str_name;
	mNameValueList = name_value_list;

	// parse table
	mTable = jimo_parser->getParmExpr(rdata, "table", name_value_list);
	if(!mTable)
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}

	//parse inputs and outputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	//if (!rslt || stmt->mInputs.size() <= 0)
	//{
	//	AosLogError(rdata, true, "missing_inputs") << enderr;
	//	return false;
	//}

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
	if(mMaxKeyLen <= 0)
	{
		setErrMsg(rdata,eMissingParm,"max_length",mErrmsg);
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
AosJimoLogicDataProcStr2StrMap::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	bool rslt = createStr2StrMapDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = createIILBatchoprDataproc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addStr2StrMapTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	rslt = addIILTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosJimoLogicDataProcStr2StrMap::createStr2StrMapDataProc(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	OmnString str2strmap_str = "create dataprocstr2strmap ";
	str2strmap_str << mNewDataProcName << " { "
				<< "\"type\":\"" << mKeywords[1] << "\",";
	if (mCond)
		str2strmap_str << "\"condition\": \"" << mCond->dumpByNoQuote() << "\",";
	str2strmap_str << "\"value_field\":\"" << mValueField << "\","
					<< "\"max_length\": " << mMaxKeyLen << ",";
//	if(mDataType != "")
//		str2strmap_str << "\"data_type\":\"" << mDataType << "\",";
//	if(mAggrOpr != "")
//		str2strmap_str << "\"aggr_opr\":\"" << mAggrOpt << "\",";

	str2strmap_str<< "\"key_fields\":[";
	for(size_t i =0; i<mKeyFields.size();i++)
	{
		if(i>0)
			str2strmap_str << ",";
		str2strmap_str << "\""<< mKeyFields[i] << "\"";

	}

	str2strmap_str << "]};";
	OmnScreen << str2strmap_str << endl;
	statement_str << "\n" << str2strmap_str;

	bool rslt = parseRun( rdata, str2strmap_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcStr2StrMap::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	OmnString iilName;
	iilName << "_zt44_"<< mNewDataProcName << "_map" ;

	mDataProcIILName << "_dp_" << mDataProcName << "_reduce" ;
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
AosJimoLogicDataProcStr2StrMap::addStr2StrMapTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString add_str2str_str;
	OmnString taskname;
	vector<OmnString> outputNames;	
	if (!mInputs.empty())
	{
		bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
		aos_assert_rr(rslt ,rdata ,false);
	}

	
	taskname << "task_str2strmap_"<< mDataProcName;
	add_str2str_str << " {"
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
			add_str2str_str << ",";
		add_str2str_str << "\"" << outputNames[i] << "\"";
	}
	add_str2str_str << "],";
	add_str2str_str << "\"outputs\":[\"" << mNewDataProcName << "_output\"";
	add_str2str_str << "]}]}";

	if(prog)
		prog->appendStatement(rdata, "tasks",add_str2str_str);

	OmnScreen << add_str2str_str << endl;
	return true;
}


bool
AosJimoLogicDataProcStr2StrMap::addIILTask(
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
AosJimoLogicDataProcStr2StrMap::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcStr2StrMap*>(this);
	mNewDataProcName << "_dp_" << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	OmnString output_name;
	output_name << mNewDataProcName << "_output";
	mOutputNames.push_back(output_name);
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcStr2StrMap::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcStr2StrMap(*this);
}
