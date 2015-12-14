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
// 2015/06/10 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcSelectMin.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSelectMin_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSelectMin(version);
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


AosJimoLogicDataProcSelectMin::AosJimoLogicDataProcSelectMin(const int version)
:
AosJimoLogicDataProc(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDataProcSelectMin::~AosJimoLogicDataProcSelectMin()
{
}


bool 
AosJimoLogicDataProcSelectMin::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc selectmin datasetname
	// 	(
	// 		inputs:[name, name],
	// 		group_by:[name, name],
	// 		fields:[name, name]
	// 	);
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataproc", rdata, false);
	aos_assert_rr(keywords[1] == "selectmin", rdata, false);
	mErrmsg = "dataproc selectmin ";

	// parse dataproc name
	OmnString dp_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dp_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dp_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = dp_name;
	mNameValueList = name_value_list;

	// parse input
	mInput = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	if (mInput == "")
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}
	//parse groupby
	rslt = jimo_parser->getParmArrayStr(rdata, "group_by", name_value_list, mGroupBys);
	if (!rslt && mGroupBys.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"group_by",mErrmsg);
		return false;
	}

	//parse fields
	rslt = jimo_parser->getParmArrayStr(rdata, "fields", name_value_list, mFields);
	if (!rslt && mFields.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "fields", mErrmsg);
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
AosJimoLogicDataProcSelectMin::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{

	bool rslt = createSelectDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcSelectMin::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
   //AosJimoLogicDataProcSelectMinPtr jimologic = dynamic_cast<AosJqlStatementPtr>(this);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSelectMin*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

//	OmnString outputName = mDataProcName;
//	outputName << "_output";
//	mOutputNames.push_back(outputName);
	return true;
}


bool
AosJimoLogicDataProcSelectMin::createSelectDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	//dataproc select xxxx
	//	(
	//	 inputs:xxxx,                           
	//	 fields:[                                  
	//	 (                                  
	//	  alias:min_field1,                      
	//	  fname:min(field1)
	//	 ),
	//	 (
	//	  alias:min_field2,                      
	//	  fname:min(field2)
	//	 )                               
	//	 ], 
	//	 groupby_fields:[f1, f2,.....]
	//	)

	OmnString dataproc_select_str = "dataproc select ";
	dataproc_select_str << mDataProcName;
	dataproc_select_str << " ( "
						<< "inputs:\"" << mInput << "\"";
	dataproc_select_str << ", fields:[";
	for (size_t i = 0; i < mFields.size(); i++)
	{
		if (i>0)
			dataproc_select_str << ", ";
		OmnString alias_name = "min_";
		alias_name << mFields[i];

		OmnString fname = "\"min(";
		fname << mFields[i] << ")\" ";
		dataproc_select_str << " ( alias:" << alias_name << ", ";
		dataproc_select_str << "fname:" << fname << " )";
	}
	dataproc_select_str << "]";
	dataproc_select_str << ", group_by:[";
	for (size_t i = 0; i < mGroupBys.size(); i++)
	{
		if (i>0)
			dataproc_select_str << ", ";
		dataproc_select_str << mGroupBys[i];
	}
	dataproc_select_str << "] )";

	OmnScreen << dataproc_select_str << endl;
	statements_str << "\n" << dataproc_select_str;

    bool rslt = parseRun(rdata, dataproc_select_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcSelectMin::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata,jimo_prog, stmt, statements);
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
		statements[i]->compileJQL(rdata, jimo_prog);
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcSelectMin::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSelectMin(*this);
}

