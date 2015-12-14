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
// 2015/07/21 Created by Bryant Zhou 
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcDistinct.h"

#include "API/AosApi.h"

#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcDistinct_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcDistinct(version);
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


AosJimoLogicDataProcDistinct::AosJimoLogicDataProcDistinct(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcDistinct::~AosJimoLogicDataProcDistinct()
{
}


bool 
AosJimoLogicDataProcDistinct::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//<dataproc><![CDATA[
	//    {
	//         "left_fields":["key_field1", "key_field2"...],
	//         "right_fields":["key_field1", "key_field2"...],
	//         inputs:
	//         (
	//         		"left_input":"input1",
	//         		"right_input":"input2"
	//         )
	//         "leftadd":"true",
	//         "rightadd":"true"
	//     }
	//]]></dataproc>


	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "distinct", rdata, false);
	mErrmsg = "dataproc distinct ";

	// parse dataproc name
	OmnString dataproc_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataproc_name,mErrmsg))
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
		//setErrMsg(rdata, 0, "",mErrmsg);
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	mDataProcName = dataproc_name;
	mNameValueList = name_value_list;

	// Configure the common attributes
	if (!configCommonAttrs(rdata,jimo_parser, name_value_list,mErrmsg))
	{
	     AosLogError(rdata, true, "missing_common_attributes");
         return false;
	}

	//parse save doc
	//if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
	//{
	//	AosLogError(rdata, true, "missing_savedoc_attributes");
	//	return false;
	//}

	
	//parse inputs
	vector<AosExprObjPtr> inputs;
	rslt = jimo_parser->getParmArray(rdata, "inputs", name_value_list, inputs);
	if (!rslt)
	{
		setErrMsg(rdata, eMissingParm, "inputs",mErrmsg);
		return false;
	}

	OmnString input = jimo_parser->getParmStr(rdata, "left_input", inputs);
	if (input == "")
	{
		setErrMsg(rdata, eMissingParm, "left_input",mErrmsg);
		return false;
	}
	mInputs.push_back(input);
	input = jimo_parser->getParmStr(rdata, "right_input", inputs);
	if (input == "")
	{
		setErrMsg(rdata, eMissingParm, "right_input",mErrmsg);
		return false;
	}
	mInputs.push_back(input);

	//parse output
	mOutput = jimo_parser->getParmStr(rdata, "outputs", inputs);

	// parse left_fields 
	vector<AosExprObjPtr> left_fields;
	rslt = jimo_parser->getParmArray(rdata, "left_fields", name_value_list, left_fields);
	if (!rslt || left_fields.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "left_fields",mErrmsg);
		return false;
	}                                                                                

	for(u32 i=0; i<left_fields.size(); i++)
	{
		OmnString left_key = left_fields[i]->dumpByNoQuote();
		mLeftDistinctKeys.push_back(left_key);
	}

	// parse right_fields 
	vector<AosExprObjPtr> right_fields;
	rslt = jimo_parser->getParmArray(rdata, "right_fields", name_value_list, right_fields);
	if (!rslt || right_fields.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "right_fields",mErrmsg);
		return false;
	}                                                                                

	for(u32 i=0; i< right_fields.size(); i++)
	{
		OmnString right_key = right_fields[i] -> dumpByNoQuote();
		mRightDistinctKeys.push_back(right_key);
	}

	//parse leftadd
	mLeftadd = jimo_parser->getParmStr(rdata, "leftadd", name_value_list);

	//parse rightadd
	mRightadd = jimo_parser->getParmStr(rdata, "rightadd", name_value_list);

	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcDistinct::compileJQL(
			AosRundata *rdata, 
			AosJimoProgObj *prog)
{
	mDataprocDistinctName ="";
	mOutputName ="";
	aos_assert_rr(prog, rdata, false);

	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcDistinct*>(this);
	//felicia 2015/08/07 
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic); 
	aos_assert_rr(rslt, rdata, false);

	OmnString jobName = prog->getJobname();
	mDataprocDistinctName << "_dp_" << jobName << "_" << mDataProcName;

	if (mOutput == "")
	{
		mOutputName << mDataprocDistinctName << "_output";
	}
	else
	{
		mOutputName = mOutput;
	}
	mOutputNames.push_back(mOutputName);
	
	return true;
}


bool
AosJimoLogicDataProcDistinct::processInput(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str)
{
	OmnString inputName = "";
	for (u32 i = 0; i < mInputs.size(); i++)                                 
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInputs[i], prog, inputName, statements_str);
		aos_assert_r(rslt, false);

		if (inputName == "")
		{
			AosLogError(rdata, true, "failed_retrieving_output_name")
				<< AosFN("Input Name") << mInputs[i] << enderr;
			return false;
		}
		mInputNames.push_back(inputName);
	}

	if (mInputNames.size() != 2)
	{
		AosLogError(rdata, true, "joins_allows_two_table_joins_only") << enderr;
		return false;
	}
	return true;
}


bool 
AosJimoLogicDataProcDistinct::run(
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

	//proc inputs
	rslt = processInput(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);
	
	// distinct dataproc
	rslt = createDistinctDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	// task for distinct
	rslt = addDistinctTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcDistinct::parseRun(
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


bool
AosJimoLogicDataProcDistinct::getOutputName(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &outputName)
{
	outputName = mOutputName;
	return true;
}


bool
AosJimoLogicDataProcDistinct::createDistinctDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocDistinct_str = "create dataprocdistinct ";
	dataprocDistinct_str << mDataprocDistinctName << " { "
		<< "\"type\":" << "\"distinct\"" << ","
		<< "\"left_fields\":" << "[";
	for(u32 i=0; i<mLeftDistinctKeys.size(); i++)
	{
		if(i>0) dataprocDistinct_str << ",";
		dataprocDistinct_str << "\"" << mInputNames[0] << "." << mLeftDistinctKeys[i] << "\"";
	}
	dataprocDistinct_str << "]," << "\"right_fields\":" << "[";

	for(u32 i=0; i<mLeftDistinctKeys.size(); i++)
	{
	    if(i>0) dataprocDistinct_str << ",";
		dataprocDistinct_str <<"\"" << mInputNames[1] << "." << mRightDistinctKeys[i] << "\"";
	}
	dataprocDistinct_str << "],";
	dataprocDistinct_str << "\"leftadd\":" << mLeftadd << ","; 
	dataprocDistinct_str << "\"rightadd\":" << mRightadd << "};";

	OmnScreen << dataprocDistinct_str << endl;
	statements_str << "\n" << dataprocDistinct_str;

    bool rslt = parseRun(rdata, dataprocDistinct_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcDistinct::addDistinctTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addDistinct_str;
	//OmnString outputname;
 	//getOutputName(rdata, jimo_prog, outputname);
	OmnString output;
	OmnString taskName;
	taskName << "task_index_iil_" << mDataProcName;
	//input = mDataprocOutputs.find(mInput[i])->second;
    output << mDataprocDistinctName << "_output";
	addDistinct_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" 
		<< ",\"model_type\":" << "\"simple\"" 
		<< ",\"dataengine_type\":" << "\"dataengine_join2\"" 
		<< ",\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocDistinctName << "\""
		<< ",\"inputs\":" << "[" ;

	for(u32 i=0; i<mInputNames.size();i++)
	{
		if(i>0) addDistinct_str << ",";
		addDistinct_str << "\"" << mInputNames[i] << "\"";
	}
	
	addDistinct_str << "]";
    addDistinct_str << ",\"outputs\":" << "[\"" << output << "\"]"
        << "}]}";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks",addDistinct_str);
	OmnScreen << addDistinct_str << endl;
	return true;
}


bool
AosJimoLogicDataProcDistinct::getInputV(vector<OmnString> &inputs)
{
	inputs = mInputs;
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcDistinct::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcDistinct(*this);
}

