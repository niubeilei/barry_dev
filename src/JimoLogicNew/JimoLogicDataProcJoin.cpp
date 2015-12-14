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
#include "JimoLogicNew/JimoLogicDataProcJoin.h"

#include "API/AosApi.h"

#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include <string>

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcJoin_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcJoin(version);
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


AosJimoLogicDataProcJoin::AosJimoLogicDataProcJoin(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
	mReverse = "false";
}


AosJimoLogicDataProcJoin::~AosJimoLogicDataProcJoin()
{
}


bool 
AosJimoLogicDataProcJoin::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataset inner_join datasetname
	// 	(
	// 		inputs: [<dataset_name>, <datasetname>],
	// 		join_keys: [(<fieldname>, <fieldname>), (<fieldname>, <fieldname>)...],
	// 		left_alias_fields: [(<fieldname>, <fieldname>), (<fieldname>, <fieldname>)...],
	// 		join_keys: [(<fieldname>, <fieldname>), (<fieldname>, <fieldname>)...],
	// 		cache: "true" | "false",
	// 		conditions: <expr>
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	if(mKeywords[1] != "leftjoin")
	{
		aos_assert_rr(mKeywords[1] == "join", rdata, false);
	}
	mErrmsg = "dataproc join ";

	// parse dataset name
	OmnString dataset_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataset_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataset_name;
	
	if(dft)
		mIsService = dft;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	mDataProcName = dataset_name;
	mNameValueList = name_value_list;

	// Configure the common attributes
	if (!configCommonAttrs(rdata,jimo_parser, name_value_list,mErrmsg))
	{
	     AosLogError(rdata, true, "missing_common_attributes");
         return false;
	}

	//parse save doc
//	if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
//	{
//		AosLogError(rdata, true, "missing_savedoc_attributes");
//		return false;
//	}

	// parse datasets
	//vector<OmnString> inputs;
	//OmnString attrname  = "inputs";
	//rslt = jimo_parser->getParmArrayStr(rdata, attrname, name_value_list, inputs);
	//if (!rslt || inputs.size() < 2)
	//{
	//	AosLogError(rdata, true, "missing_key_fields") << enderr;
	//	return false;
	//}
	//stmt->mInputs = inputs;

	//parse reverse
	OmnString reverse = jimo_parser->getParmStr(rdata, "reverse", name_value_list);
	if(reverse != "")
	{
		mReverse = reverse;
	}
	

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
	
	//parse alias
	vector<OmnString> alias;	
	rslt = jimo_parser->getParmArrayStr(rdata,"alias",name_value_list,alias);
	mAlias = alias;
	/*vector<AosExprObjPtr> exprs;
	vector<OmnString> aliasFields 
	for (u32 i=0;i<alias.size();i++)
	{
		exprs = alias[i]->getExprList();

		for (u32 j = 0; j < exprs.size();j++)
		{
			const char *sp = " ";
			OmnString aliasField = exprs[j]->dumpbyNoQuote();
			int num1 = AosSplitStr(aliasField, sp, aliasFields, 3);
			if(num1 == 2)
			{
				const char * sp1 = ".";
				int num2 = AosSplitStr(aliasFields[0], sp1, aliasFields, 3);

			}
			else if (num1 == 3)
			{
			
			}
			else
			{
				setErrMsg(rdata, eInvalidParm, "alias",mErrmsg);
				return false;
			}
		}
						
	}
	OmnString left_inputalias = exprs[0]->dumpByNoQuote();
	OmnString right_inputalias = exprs[1]->dumpByNoQuote();
	stmt->mLeftInputAlias = left_inputalias;
	stmt->mRightInputAlias = right_inputalias;*/

	// parse join key
	vector<AosExprObjPtr> join_keys;
	rslt = jimo_parser->getParmArray(rdata, "join_keys", name_value_list, join_keys);
	if (!rslt || join_keys.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "join_keys",mErrmsg);
		return false;
	}                                                                                

	for(u32 i=0; i<join_keys.size(); i++)
	{
		vector<AosExprObjPtr> exprs = join_keys[i]->getExprList();
		if(exprs.size() == 2)
		{
			const char *sep = ".";
			vector<OmnString> left_keys, right_keys;

			OmnString left_key = exprs[0]->dumpByNoQuote();
			int num1 = AosSplitStr(left_key, sep, left_keys, 2);
			OmnString right_key = exprs[1]->dumpByNoQuote();
			int num2 = AosSplitStr(right_key, sep, right_keys, 2);
			//2015/8/13 xuqi JIMODB-327
			if (num1 != 2 || num2 != 2)
			{
				setErrMsg(rdata, eInvalidParm, "join_keys",mErrmsg);
				return false;
			}
			if ( (left_keys[0] != "left_input" && left_keys[0] != "right_input")
				 || (right_keys[0] != "left_input" && right_keys[0] != "right_input")
				 || (left_keys[0] == right_keys[0]) )
			{
				setErrMsg(rdata, eInvalidParm, "join_keys",mErrmsg);
				return false;
			}

			if (left_keys[0] == "left_input")
			{
				mLeftJoinKeys.push_back(left_keys[1]);
				mRightJoinKeys.push_back(right_keys[1]);
			}
			else
			{
				mLeftJoinKeys.push_back(right_keys[1]);
				mRightJoinKeys.push_back(left_keys[1]);
			}
		}
		else
		{
			setErrMsg(rdata, eSyntaxError, "join_keys",mErrmsg);
			return false;
		}
	}

/*
	// parse cache
	attrname = "cache";
	AosExprObjPtr cache = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (!cache)
	{
		AosLogError(rdata, true, "missing_inputs") << enderr;
		return false;
	}
	stmt->mCache = cache->dumpByNoQuote();

	// parse condition
    attrname = "conditions";
    AosExprObjPtr cond= jimo_parser->getParmExpr(rdata, attrname, name_value_list);
    if (cond)
    {
        stmt->mCond = cond->dumpByNoQuote();
    }

	// parse shuffle_type
    attrname = "shuffle_type";
	AosExprObjPtr shuffle_type = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (shuffle_type)
	{
		//AosLogError(rdata, true, "missing_input") << enderr;
        stmt->mShuffleType= shuffle_type->dumpByNoQuote();
	}
*/
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcJoin::compileJQL(
			AosRundata *rdata, 
			AosJimoProgObj *prog)
{
	aos_assert_rr(prog, rdata, false);

	// jimodb-753
	OmnString field_str;
	bool rslt = getInputSchema(rdata, prog, mFieldStr);
	aos_assert_rr(rslt, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcJoin*>(this);
	rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic); 
	aos_assert_rr(rslt, rdata, false);

	mDataprocJoinName = "_dp_";
	mDataprocJoinName << mDataProcName;
	if (mOutput == "")
	{
		mOutputName ="";
		mOutputName << mDataprocJoinName << "_output";
	}
	else
	{
		mOutputName = mOutput;
	}
	mOutputNames.push_back(mOutputName);
	
	OmnString str = "";   
	if(mCond != "")
		confCond(rdata, prog, jimologic, str);

	return true;
}


bool
AosJimoLogicDataProcJoin::processInput(
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
AosJimoLogicDataProcJoin::run(
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
		AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcJoin*>(this);
		bool rslt_t = prog->addJimoLogicNew(rdata, mDataProcName, jimologic); 
		aos_assert_rr(rslt_t, rdata, false);
		
		aos_assert_rr(mInputs.size()==2, rdata, false);
		vector<OmnString> inputs;
		inputs.clear();
		char sep = '.';
		OmnString input = "";
		for(u32 i=0; i<mInputs.size(); i++)
		{
			int found = mInputs[i].find(sep,true);
			if (found == -1)  continue;
			else
			{
				input = mInputs[i].substr(0,found-1);

				mInputMap[input] = i;
				isSameInput(rdata, prog, input);
				inputs.push_back(input);
			}
        }
		mJoinMap[mDataProcName] = inputs;
		bool rslt = parseInputFields(rdata, prog);
		aos_assert_rr(rslt, rdata, false);
		mLeftInput = inputs[0];
		mRightInput = inputs[1];
	}
	else
	{
		//proc inputs
		rslt = processInput(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);
	}
	
	if(mIsService)
	{
		//for left
		mRightIILJoinKeys = mRightJoinKeys;
		mLeftIILJoinKeys = mLeftJoinKeys;
		createDataprocIILJoin(rdata, prog, statements_str);
		//for right
		createIILJoinDataproc(rdata, prog, statements_str);
		mIILJoinMap[mDataProcName] = mIILJoinList;
		
		aos_assert_rr(mJoinMap.size()>0, rdata, false);
		prog->setJoinMap(mJoinMap);
	}
	else
	{
		// join dataproc
		rslt = createJoinDataproc(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);

		// task for join
		rslt = addJoinTask(rdata, prog);
		aos_assert_rr(rslt, rdata, false);
	}
	return true;
}


bool
AosJimoLogicDataProcJoin::parseRun(
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
AosJimoLogicDataProcJoin::getOutputName(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &outputName)
{
	outputName = mOutputName;
	return true;
}

bool
AosJimoLogicDataProcJoin::createIILJoinDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocName = "";
	if(mRightIILJoin != "")
		dataprocName = mRightIILJoin;
	else
	{
		dataprocName = "dpIILJoinRight";
		dataprocName << mRightInput;
	}
	AosJimoLogicObjNew * right_jimo_logic = jimo_prog->getJimoLogic(rdata, mRightInput);
	if (right_jimo_logic)
	{
		AosJimoLogicType::E type = right_jimo_logic->getJimoLogicType();
		if (type == AosJimoLogicType::eDataProcUnion)
		{
			jimo_prog->insertToOtherNameMap(mRightInput, dataprocName);
		}
		else
		{
			OmnString inName = mRightInput;
			inName << ".output";
			jimo_prog->insertToDataFlowMap(inName, dataprocName);
		}
	}
	else
	{
		OmnAlarm << "ddddddddddd" << enderr;
	}

	OmnString inName = mDataProcName;
	inName << ".output";
	
	OmnString otName = dataprocName;
	otName << ".output";
	jimo_prog->insertToNameMap(inName, otName);

	mIILJoinList.push_back(dataprocName);

	AosJimoLogicObjNew * logic = jimo_prog->getJimoLogic(rdata, mLeftInput);
	OmnString logicName = "";
	if (logic)
	{
		logicName = logic->getLogicName();                                    
	}

	OmnString dataprocIILJoin_str = "";
	dataprocIILJoin_str  << "\"" << dataprocName << "\"" << " : { "
		<< "\"type\":" << "\"dataprociiljoin\"" << ","
		<< "\"record_type\":" << "\"buff\"" << ","
		<< "\"iilname\":" << "\"_zt44_" 
		<< mDb << "_" << mLeftTable << "_" ;
	if (logicName == "union")
	{
		dataprocIILJoin_str << mLeftInput << "\",";
	}
	else
	{
		dataprocIILJoin_str << mLeftIILJoinKeys[0] << "\",";
	}
	dataprocIILJoin_str	<< "\"opr\":" << "\"stradd\"" << ","
		<< "\"schema\":" << "\""<<mSchemaName<<"\"" << ",";
       
	dataprocIILJoin_str << "\"left_keys\":" << "[";
	//for left field
	for(u32 i=0; i<mRightIILJoinKeys.size(); i++)
	{
		if(i == mRightIILJoinKeys.size()-1)
			dataprocIILJoin_str << "\"" << mRightIILJoinKeys[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mRightIILJoinKeys[i] << "\",";
	}
	
	dataprocIILJoin_str << "]," << "\"left_values\":" << "[";
	for(u32 i=0; i<mRightFieldsList.size(); i++)
	{
		if(mRightFieldsList[i] == mRightIILJoinKeys[0])
			continue;

		if(i == mRightFieldsList.size()-1)
			dataprocIILJoin_str << "\"" << mRightFieldsList[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mRightFieldsList[i] << "\",";
	}
	
	dataprocIILJoin_str << "], \"right_keys\":" << "[";

	//for right field
	for(u32 i=0; i<mLeftIILJoinKeys.size(); i++)
	{
		if(i == mLeftIILJoinKeys.size()-1)
			dataprocIILJoin_str << "\"" << mLeftIILJoinKeys[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mLeftIILJoinKeys[i] << "\",";
	}
    
	dataprocIILJoin_str << "]," << "\"right_values\":" << "[";
	
	for(u32 i=0; i<mLeftFieldsList.size(); i++)
	{
		if(mLeftFieldsList[i] == mLeftIILJoinKeys[0])
			continue;

		if(i == mLeftFieldsList.size()-1)
			dataprocIILJoin_str << "\"" << mLeftFieldsList[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mLeftFieldsList[i] << "\",";
	}

	dataprocIILJoin_str << "]}";

	jimo_prog->setConf(dataprocIILJoin_str);
	OmnScreen << dataprocIILJoin_str << endl;
	statements_str << "\n" << dataprocIILJoin_str;
    return true;
}


bool
AosJimoLogicDataProcJoin::createDataprocIILJoin(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocName = "";
	if(mLeftIILJoin != "")
	{
		dataprocName = mLeftIILJoin;
	}
	else
	{
		dataprocName = "dpIILJoinLeft";
		dataprocName << mLeftInput;
	}
	mIILJoinList.push_back(dataprocName);
	AosJimoLogicObjNew * left_jimo_logic = jimo_prog->getJimoLogic(rdata, mLeftInput);
	if (left_jimo_logic)
	{
		AosJimoLogicType::E type = left_jimo_logic->getJimoLogicType();
		if (type == AosJimoLogicType::eDataProcUnion)
		{
			jimo_prog->insertToOtherNameMap(mLeftInput, dataprocName);
		}
		else
		{
			OmnString inName = mLeftInput;
			inName << ".output";
			jimo_prog->insertToDataFlowMap(inName, dataprocName);
		}
	}
	else
	{
		OmnAlarm << "ddddddddddd" << enderr;
	}
	
	OmnString inName = mDataProcName;
	inName << ".output";

	OmnString otName = dataprocName;
	otName << ".output";
	jimo_prog->insertToNameMap(inName, otName);

	AosJimoLogicObjNew * logic = jimo_prog->getJimoLogic(rdata, mRightInput);
	OmnString logicName = "";
	if (logic)
	{
		logicName = logic->getLogicName();                                    
	}
	
	OmnString dataprocIILJoin_str = "";
	dataprocIILJoin_str << "\"" << dataprocName << "\"" <<": { "
		<< "\"type\":" << "\"dataprociiljoin\"" << ","
		<< "\"record_type\":" << "\"buff\"" << ","
		<< "\"iilname\":" << "\"_zt44_"
		<< mDb << "_" << mRightTable << "_";
	if (logicName == "union")
	{
		dataprocIILJoin_str << mRightInput << "\",";
	}
	else
	{
		dataprocIILJoin_str << mRightIILJoinKeys[0] << "\",";
	}
		dataprocIILJoin_str << "\"opr\":" << "\"stradd\"" << ","
		<< "\"schema\":" << "\""<<mSchemaName<<"\"" << ",";
    dataprocIILJoin_str << "\"left_keys\":" << "[";

   
	//for left field
	for(u32 i=0; i<mLeftIILJoinKeys.size(); i++)
	{
		if(i == mLeftIILJoinKeys.size()-1)
			dataprocIILJoin_str << "\"" << mLeftIILJoinKeys[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mLeftIILJoinKeys[i] << "\",";
	}
	
	dataprocIILJoin_str << "]," << "\"left_values\":" << "[";
	for(u32 i=0; i<mLeftFieldsList.size(); i++)
	{
		if(mLeftFieldsList[i] == mLeftIILJoinKeys[0])
			continue;

		if(i == mLeftFieldsList.size()-1)
			dataprocIILJoin_str << "\"" <<mLeftFieldsList[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mLeftFieldsList[i] << "\",";
	}
	
	//for right field
	dataprocIILJoin_str << "]," << "\"right_keys\":" << "[";
	for(u32 i=0; i<mRightIILJoinKeys.size(); i++)
	{
		if(i == mRightIILJoinKeys.size()-1)
			dataprocIILJoin_str << "\"" << mRightIILJoinKeys[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mRightIILJoinKeys[i] << "\",";
	}
	
	dataprocIILJoin_str << "]," << "\"right_values\":" << "[";
	for(u32 i=0; i<mRightFieldsList.size(); i++)
	{
		if(mRightFieldsList[i] == mRightIILJoinKeys[0])
			continue;

		if(i == mRightFieldsList.size()-1)
			dataprocIILJoin_str << "\"" << mRightFieldsList[i] << "\"";
		else
			dataprocIILJoin_str << "\"" << mRightFieldsList[i] << "\",";
	}
	dataprocIILJoin_str << "]}";
 
	jimo_prog->setConf(dataprocIILJoin_str);
	OmnScreen << dataprocIILJoin_str << endl;
	statements_str << "\n" << dataprocIILJoin_str;

    return true;
}



bool
AosJimoLogicDataProcJoin::checkJoinInput(         
		AosRundata *rdata,
		OmnString &imputName,
		OmnString &statements_str,
		JSONValue &root,     
		int &orderSize,      
		int &groupSize,      
		int &distinctSize,   
		int &orderTypeSize)
{
     OmnString tmpStr;   
	 OmnString tmpStr1;
	 int startPos = 0;
	 int lastPos = 0;
     orderSize = 0;
	 groupSize = 0;
	 distinctSize = 0;
	 orderTypeSize = 0;

	 tmpStr = imputName;
	 tmpStr.remove(0,tmpStr.length()-13);
	 if(tmpStr == "union_outputs")
	 {
		 OmnString errmsg;                                                               
		 errmsg<<"[ERR]: '"<<mDataProcName<<"' : left and right field must be 'dataproc select'";
		 rdata->setJqlMsg(errmsg);                                                       
		 return false;                                                                   
	 }
	 tmpStr = imputName;                                           
	 tmpStr.remove(tmpStr.length()-7,7);                                
	 startPos = statements_str.findSubString(tmpStr,startPos);
	 startPos = statements_str.findSubString(OmnString("{"),startPos);
	 lastPos = statements_str.findSubString(OmnString(";"),startPos);
	 tmpStr1 = statements_str.substr(startPos,lastPos);

	 JSONReader reader;
	 if(reader.parse(tmpStr1, root))
     {
		orderTypeSize = root["order_by_type"].size();
		if(orderTypeSize > 0)
		{
			for(int j = 0 ; j < orderTypeSize ; ++j)                            
			{                                                               
				if(OmnString("desc") == root["order_by_type"][j].asString())
			    {   
					OmnString errmsg;                  
					errmsg<<"[ERR]: '"<<subDpSelectName(imputName)<<"' : 'order_by_type' must be 'asc'";
					rdata->setJqlMsg(errmsg);          
					return false;                     
				}    	                                                                
			}                                                               
		}
		
		orderSize = root["orderby_fields"].size();
		groupSize = root["groupby_fields"].size();
		distinctSize = root["distinct"].size();
    }
	else
	{
		OmnString errmsg;                                                
		errmsg<<"[ERR]: '"<<mDataProcName<<"' : input is incorrect";
		rdata->setJqlMsg(errmsg);                                        
		return false;                                                    
	}
	return true;
}



bool
AosJimoLogicDataProcJoin::checkJoinSort(         
		AosRundata *rdata,
		OmnString &joinKey,
		JSONValue &root,    
		int &orderSize,     
		int &groupSize,     
		int &distinctSize,  
		int &orderTypeSize)
{
	bool exist = false;
	
	if(orderSize > 0)
	{
		for(int j = 0 ; j < orderSize ; ++j)
		{
			if(joinKey == root["orderby_fields"][j].asString())
			{
				exist = true;
				break;
			}
				
		}
	}
			
	if(groupSize > 0 && !exist)
	{
		for(int j = 0 ; j < groupSize ; ++j)                            
		{                                                               
	    	if(joinKey == root["groupby_fields"][j].asString())
			{                                                           
				exist = true;                                           
				break;                                                  
			}                                                           
						                                                                
		}                                                               
	
	}
			
	if(distinctSize > 0 && !exist) 
	{ 
		for(int j = 0 ; j < distinctSize ; ++j)                            
		{                                                               
			if(joinKey == root["distinct"][j].asString())
			{                                                           
				exist = true;                                           
				break;                                                 
			}                                                           
						                                                                
		}
	}
			
	//if(distinctSize == 1 && !exist)
	//{
		//int t = 0;
		// 2015.09.02
		//if(OmnString("all") == root["distinct"][t].asString())
		if(OmnString("true") == root["distinct"].asString())
		{                                                    
		    exist = true;                                         
		}                                                    
	//}	
    return exist;
}
	

OmnString 
AosJimoLogicDataProcJoin::subDpSelectName(OmnString &inputName)
{
	OmnString tmpStr;
	int startPos = 0;
	
	tmpStr = inputName;
    tmpStr.remove(tmpStr.length()-7,7);

	startPos = tmpStr.findSubString(OmnString("_"),startPos) + 1;
	startPos = tmpStr.findSubString(OmnString("_"),startPos) + 1;
	startPos = tmpStr.findSubString(OmnString("_"),startPos) + 1;
	tmpStr.remove(0,startPos);                                   

	return tmpStr;
}

bool
AosJimoLogicDataProcJoin::createJoinDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	//2015/8/6 xuqi JIMODB-280 JIMODB-316 JIMODB-288

	OmnString dataprocJoin_str;
	OmnString joinType;
	if(mKeywords[1] == "join")
	{
		dataprocJoin_str = "create dataprocjoin ";
		joinType = "join";
	}
	else if (mKeywords[1] == "leftjoin")
	{
		dataprocJoin_str = "create dataprocleftjoin ";
		joinType = "leftjoin";
	}
	dataprocJoin_str << mDataprocJoinName << " { "
		<< "\"type\":" << "\"" << joinType << "\"" << ","
		<< "\"record_type\":" << "\"buff\"" << ","
		<< "\"reverse\":" << "\"" << mReverse << "\"" << "," 
		<< "\"alias\":" << "[" ;
	for (u32 i = 0; i <mAlias.size(); i++)
	{
		if(i>0) dataprocJoin_str << ",";
		dataprocJoin_str << "\"" << mAlias[i] << "\"";
	}
	dataprocJoin_str << "],";
    dataprocJoin_str << "\"condition\":" << "\"" << mCond << "\"" << ","
		<< "\"left_condition_fields\":" << "[";
    
	JSONValue root; 
	int orderSize = 0;
	int groupSize = 0;
    int distinctSize = 0;
	int orderTypeSize = 0;
	bool exist = false;
    bool rslt;
	 
	rslt = checkJoinInput(
			rdata, 
			mInputNames[0],
			statements_str, 
			root, orderSize, 
			groupSize , 
			distinctSize, 
			orderTypeSize);
    aos_assert_r(rslt, false);

    for(u32 i=0; i<mLeftJoinKeys.size(); i++)
	{
			
		
        exist = checkJoinSort(
				rdata, 
				mLeftJoinKeys[i],
				root, 
				orderSize, 
				groupSize, 
				distinctSize, 
				orderTypeSize);
		
		if(exist)
	    {
	    	if(i>0) dataprocJoin_str << ",";
			dataprocJoin_str << "\"" << mInputNames[0] << "." << mLeftJoinKeys[i] << "\"";
		}
		else
		{
			OmnString errmsg;                                                  
			errmsg<<"[ERR]: '"<<subDpSelectName(mInputNames[0])
				<<"' field : '"<<mLeftJoinKeys[i]<<"' not sorted";
			rdata->setJqlMsg(errmsg);                                          
			return false;                                                      

		}
	}
	
	dataprocJoin_str << "]," << "\"right_condition_fields\":" << "[";
	
	rslt = checkJoinInput(
			rdata, 
			mInputNames[1], 
			statements_str,
			root, 
			orderSize, 
			groupSize, 
			distinctSize, 
			orderTypeSize);
	aos_assert_r(rslt, false);

	for(u32 i=0; i<mLeftJoinKeys.size(); i++)
	{
        exist = checkJoinSort(
				rdata, 
				mRightJoinKeys[i],
				root, 
				orderSize, 
				groupSize , 
				distinctSize, 
				orderTypeSize);

    	if(exist)
		{
    		if(i>0) dataprocJoin_str << ",";
			dataprocJoin_str <<"\"" << mInputNames[1] << "." << mRightJoinKeys[i] << "\"";
		}
		else
		{
			OmnString errmsg;                                              
			errmsg<<"[ERR]: '"<<subDpSelectName(mInputNames[1])
				<<"' field : '"<<mRightJoinKeys[i]<<"' not sorted";
			rdata->setJqlMsg(errmsg);                                      
			return false;
		}

	}

	dataprocJoin_str << "]" << "};";

	OmnScreen << dataprocJoin_str << endl;
	statements_str << "\n" << dataprocJoin_str;

    rslt = parseRun(rdata, dataprocJoin_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcJoin::addJoinTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addjoin_str;
	//OmnString outputname;
 	//getOutputName(rdata, jimo_prog, outputname);
	OmnString output;
	OmnString taskName;
	taskName << "task_index_iil_" << mDataProcName;
	//input = mDataprocOutputs.find(mInput[i])->second;
    output << mDataprocJoinName << "_output";
	addjoin_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" 
		<< ",\"dataset_splitter\":" << "{" << "\"type\":" << "\"file_join\"}" 
		<< ",\"model_type\":" << "\"simple\"" 
		<< ",\"dataengine_type\":" << "\"dataengine_join2\"" 
		<< ",\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocJoinName << "\""
		<< ",\"inputs\":" << "[" ;

	for(u32 i=0; i<mInputNames.size();i++)
	{
		if(i>0) addjoin_str << ",";
		addjoin_str << "\"" << mInputNames[i] << "\"";
	}
	
	addjoin_str << "]";
    addjoin_str << ",\"outputs\":" << "[\"" << output << "\"]"
        << "}]}";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks",addjoin_str);
	OmnScreen << addjoin_str << endl;
	return true;
}

bool
AosJimoLogicDataProcJoin::confCond(
        AosRundata *rdata,
        AosJimoProgObj *prog,
        AosJimoLogicObjNewPtr jimologic,
        OmnString &statements_str)
{
    vector<OmnString> inputs;
    vector<OmnString> condInputs;
    OmnString inputName;
    bool rslt = getInputs(rdata, mCond, inputs);
    if (!rslt)
    {
        AosLogError(rdata, true, "failed_retrieving_condition_input_name") << enderr;
        return false;
    }

    for(u32 i=0; i<inputs.size();i++)
    {
        OmnString input;
        if (inputs[i] == "left_input")
            input = mInputs[0];
        
        else
        {
            if (inputs[i] == "right_input")
                input = mInputs[1];
            else
            {
		continue;
               	//AosLogError(rdata, true, "incorrect_condition") << enderr;
                //return false;
            }
        }

        bool rslt = AosJimoLogicNew::procInput(rdata, input, prog, inputName, statements_str);
        aos_assert_r(rslt, false);

        if (inputName == "")
        {
            AosLogError(rdata, true, "failed_retrieving_output_name")
                << AosFN("Input Name") << inputs[i] << enderr;
            return false;
        }
        mCond.replace(inputs[i],inputName,true);
        //condInputs.push_back(inputName);
    }
    return true;
}

bool
AosJimoLogicDataProcJoin::getInputs(
        AosRundata *rdata,
        OmnString& str,
        vector<OmnString> &inputs)
{
    vector<OmnString> sep_inputs;
    const char *sep = " ";
    int num = AosSplitStr(str, sep, sep_inputs, 30);
    char sep1 = '.';
    for (int i=0; i<num; i++)
    {
        OmnString input;
        int found = sep_inputs[i].find(sep1,true);
        if (found == -1)  continue;
        else
        {
            input = sep_inputs[i].substr(0,found-1);
    //      if (input == "left_input")
    //          input = mInputs[0];
    //      if (input == "right_input")
    //          input = mInputs[1];
            inputs.push_back(input);
        }
    }
    if(inputs.size()<=0)
    {
        AosLogError(rdata, true, "incorrect_condition_input_name") << enderr;
        return false;
    }
    return true;
}


AosJimoPtr 
AosJimoLogicDataProcJoin::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcJoin(*this);
}

bool 
AosJimoLogicDataProcJoin::getInputV(vector<OmnString> &inputs)
{
	inputs = mInputs;
	return true;
}

bool
AosJimoLogicDataProcJoin::parseInputFields(AosRundata *rdata, AosJimoProgObj *jimo_prog)
{
	vector<OmnString> inputList = this->mInputs;
	vector<OmnString> nameList;
	vector<AosExprObjPtr> mLeftFields;
	vector<AosExprObjPtr> mRightFields;
	OmnString leftInput = "";
	OmnString rightInput = "";

	for(u32 i=0; i<inputList.size(); i++)
	{
		const char *sep = ".";
		int num = AosSplitStr(inputList[i], sep, nameList, 30);
		aos_assert_rr(num >0, rdata, false);
		if(i == 0)
			leftInput = nameList[0];
		else
			rightInput = nameList[0];
	}
	
	if(!leftInput.isNull() && !rightInput.isNull())
	{
		AosJimoLogicObjNew * leftSelect = jimo_prog->getJimoLogic(rdata, leftInput);
		bool rslt = leftSelect->getFields(mLeftList);
		aos_assert_rr(mLeftList.size()>0, rdata, false);
		mLeftTable = leftSelect->getTableName();
		if(mLeftTable != "")
		{
			AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata,JQLTypes::eTableDoc,mLeftTable); 
			aos_assert_rr(doc, rdata, false);
			AosXmlTagPtr jql_doc = doc->getFirstChild();
			mDb = jql_doc->getAttrStr("zky_database");
		}
		for(u32 i=0; i<mLeftList.size(); i++)
		{
			vector<AosExprObjPtr> field_exprs = mLeftList[i]->getExprList();
			OmnString mn = field_exprs[0]->getName().toLower();
			if(mn == "fname")
				mn = field_exprs[0]->getValue(rdata);
			if(mn != "")
				mLeftFieldsList.push_back(mn);
		}
		AosJimoLogicObjNew *rightSelect;
		AosJimoLogicObjNew * logic = jimo_prog->getJimoLogic(rdata, rightInput);
		rightSelect = logic;
		OmnString logicName = logic->getLogicName();

		//for union or select
		if(logicName == "union")
		{
			vector<OmnString> inputs;
			rslt = logic->getInputList(inputs);
			rightSelect = jimo_prog->getJimoLogic(rdata, inputs[0]);
		}
		
		rslt = rightSelect->getFields(mRightList);
		aos_assert_rr(mRightList.size()>0, rdata, false);	
		mRightTable = rightSelect->getTableName();
		aos_assert_rr(rslt, rdata, false);

		for(u32 i=0; i<mRightList.size(); i++)
		{
			vector<AosExprObjPtr> field_exprs = mRightList[i]->getExprList();
			OmnString mn = field_exprs[0]->getName().toLower();
			if(mn == "fname")
				mn = field_exprs[0]->getValue(rdata);
			if(mn != "")
				mRightFieldsList.push_back(mn);
		}
	}

	//for get schema Name
	OmnString statName = "";
	OmnString db_table = "";
	OmnString tab_name = "";

	bool rslt = jimo_prog->getStat(statName);
	aos_assert_rr(rslt,rdata, false);
	aos_assert_rr(statName!="", rdata, false);
	AosJimoLogicObjNew *stat = jimo_prog->getJimoLogic(rdata, statName);
	db_table = stat->getTableName();
	vector<OmnString> fields;
	int num = AosSplitStr(db_table, "_", fields, 4);
	aos_assert_rr(num > 0, rdata, false);
	tab_name = fields[1];
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdata,JQLTypes::eTableDoc, tab_name);
	if (!table_doc)
	{
		//setErrMsg(rdata,3,stmt->mTableName, mErrmsg);
		return false;
	}
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	aos_assert_rr(schema_objid != "", rdata, false);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdata);;
	aos_assert_rr(schema_doc, rdata, false);
	AosXmlTagPtr record_doc = schema_doc->getFirstChild();
	record_doc = record_doc->getFirstChild();
	mSchemaName = record_doc->getAttrStr("zky_name");

	return true;
}

bool
AosJimoLogicDataProcJoin::isSameInput(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString input)
{
	int idx = 1;
	map<OmnString, vector<OmnString> > join_map = jimo_prog->getJoinMap();
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> inputList;
	vector<OmnString> innerSelectList;
	u32 index;
	itr = join_map.begin();
	while(itr != join_map.end())
	{
		inputList = itr->second;
		for(u32 i=0; i< inputList.size(); i++)
		{
			if(inputList[i] == input)
			{
				//getSameSelect(rdata, jimo_prog, input, innerSelectList, i);
				index = mInputMap[input];
				if(index == 0)
				{
					mLeftIILJoin = "dpIILJoinLeft";
					mLeftIILJoin << mLeftInput << idx;
				}
				else
				{
					mRightIILJoin = "dpIILJoinRight";
					mRightIILJoin << mRightInput << idx;
				}
			}
		}
		itr++;
		idx++;
	}
	return true;
}

/*	
bool
AosJimoLogicDataProcJoin::getSameSelect(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString input, vector<OmnString> select_list, u32 idx)
{
	map<OmnString, vector<OmnString> > select_map = jimo_prog->getSelectMap();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = select_map.find(input);
	vector<OmnString> selectList;
	OmnString name = "";
	
	while(itr != select_map.end())
	{
		selectList = itr->second;
		for(u32 i=0; i<selectList.size(); i++)
		{
			if(i==0)	
				name << selectList[i];
			else
			{
				name << selectList[i] << idx;
			}
			select_list.push_back(name);
		}
		itr++;
	}
	OmnString input_name = "";
	input_name << input << idx;
	select_list = mSelectMap[input_name];
	

	return true;
}
*/
bool
AosJimoLogicDataProcJoin::getFieldStr(vector<OmnString> &field_strs)
{
	field_strs = mFieldStr;
	return true;
}
