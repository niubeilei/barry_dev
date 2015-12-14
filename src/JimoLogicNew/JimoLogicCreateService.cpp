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
// A Service is defined as:
// 	Create JimoService Service_name
// 	(
// 		parm_name,
// 		parm_name,
// 		....
//  );
//
// It will create the following:
//
// Modification History:
// 2015/03/22 Created by Chen Ding
// 2015/05/20 Worked on by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicCreateService.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicCreateService_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicCreateService(version);
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


AosJimoLogicCreateService::AosJimoLogicCreateService(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version)
{
}


AosJimoLogicCreateService::~AosJimoLogicCreateService()
{
}


bool 
AosJimoLogicCreateService::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	Create Service Service_name(parm, parm, ...)
	// 	BEGIN
	// 		statement;
	// 		statement;
	// 		...
	// 		statement;
	// 	END;
	
	parsed = false;

	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "service", rdata, false);

	OmnString Service_name = jimo_parser->nextObjName(rdata);
	if (Service_name == "")
	{
		// This is not what it expects. 
		return true;
	}

	// Parser the parameters. 
	if (!jimo_parser->peekNextChar('('))
	{
		// The next character is not '('. 
		return true;
	}

	// From now on, we assume this is the Service statement. 
	jimo_parser->parseProcedureParms(rdata, mParms);
	
	// The next character should not be ';'.
	if (jimo_parser->peekNextChar(';'))
	{
		AosLogError(rdata, true, "expecting_statement_but_found_semi_column") << enderr;
		return false;
	}
	// The next keyword must be "begin"
	OmnString begin = jimo_parser->nextKeyword(rdata);
	if(begin.toLower() != "begin")
	{
		mErrmsg << " : miss keyword \"begin\"";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	
	mServiceName = Service_name;
	mOriginService = jimo_parser->getOrigStmt();
	//replace
	if(mParms.size() > 0)
	{
		OmnString stmt_str = jimo_parser->getOrigStmt();
		vector<OmnString> replace_parms;
		findSubString(rdata, stmt_str, replace_parms);
		if(replace_parms.size() > 0)
		{
			aos_assert_rr(replace_parms.size()==mParms.size(),rdata,false);

			for(size_t i =0; i<mParms.size();i++)
			{
				stmt_str.replace(replace_parms[i],mParms[i],false);	
			}
			jimo_parser->setOrigStmt(stmt_str);
		}
	}

	// Parse the statements and adds them to mStatements until
	// it hits "END Service".
	int guard = 1000000;
	OmnString expr;
	while (guard--)
	{
		AosJqlStatementPtr ss;
		bool rslt = jimo_parser->nextStatement(rdata, prog, expr, ss, true);
		if(!rslt)
		{
		//	AosLogError(rdata, true, "failed_parsing_statement") << enderr;
			return false;
		}
		if (!ss && rslt)
		{
			parsed = true;
			return true;			
		}
		// Found a statement for the Service
		appendStatement(rdata, ss);
	}
	//bool rslt = getDataFlow();

	OmnShouldNeverComeHere;
	return false;
}

bool
AosJimoLogicCreateService::parseRun(
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
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool 
AosJimoLogicCreateService::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
	AosJimoProgObjPtr jimo_Service = Jimo::jimoCreateJimoProg(rdata, "AosJimoProgService", 1);
	AosJimoProgObj *jimo_Service_raw = jimo_Service.getPtrNoLock();
	aos_assert_rr(jimo_Service_raw, rdata, false);
	jimo_Service_raw->setJobName(mServiceName);	
	//jimo_Service_raw->setOriginServiceCode(mOriginService);

	//for set stat to jimo_prog
	bool rslt;
	int i = mStatements.size()-1;
	if (mStatements.size()>0)
	{
		// AosJimoLogicObjNew *jimo_logic = dynamic_cast<AosJimoLogicObjNew *>(mStatements[i].getPtr());
		// aos_assert_rr(jimo_logic, rdata, false);
		rslt = mStatements[i]->compileJQL(rdata, jimo_Service_raw);
		aos_assert_r(rslt, false);
	}

	OmnString ser_conf = "";
	ser_conf << "create service " << mServiceName << " { ";
//		<< "\"dataflow\" : {";
	for (u32 i=0; i<mStatements.size(); i++)
	{
		rslt = mStatements[i]->run(rdata, jimo_Service_raw, statements_str, true);
		aos_assert_r(rslt, false);
	}

	map<OmnString, vector<OmnString> > dataflow_map = jimo_Service_raw->getDataFlowMap();
	map<OmnString, vector<OmnString> > othername_map = jimo_Service_raw->getOtherNameMap();
	map<OmnString, vector<OmnString> > name_map = jimo_Service_raw->getNameMap();

	map<OmnString, vector<OmnString> >::iterator itr = dataflow_map.begin();
	map<OmnString, vector<OmnString> >::iterator name_itr;
	vector<OmnString> outputs;
	JSONValue dataflowJV;
	OmnString input_name, output_name;
	int idx = 0;
	while(itr != dataflow_map.end())
	{
		idx++;
		JSONValue streamJV;
		if (dataflowJV.isMember(itr->first))
			streamJV = dataflowJV[itr->first];

		outputs = itr->second;
		for(size_t i=0; i<outputs.size(); i++)
		{
			output_name = outputs[i];
			output_name << ".input";
			streamJV.append(output_name.data());
		}
		name_itr = othername_map.find(itr->first);
		if(name_itr != othername_map.end())
		{
			outputs = name_itr->second;
			othername_map.erase(name_itr);
			name_itr = name_map.find(itr->first);
			aos_assert_r(name_itr == name_map.end(), false);
			
			for(size_t i=0; i<outputs.size(); i++)
			{
				output_name = outputs[i];
				output_name << ".input";
				streamJV.append(output_name.data());
			}
		}
		
		dataflowJV[itr->first] = streamJV;	
		++itr;
	}

	vector<OmnString> inputs;
	itr = othername_map.begin();
	while(itr != othername_map.end())
	{
		outputs = itr->second;

		name_itr = name_map.find(itr->first);
		if(name_itr == name_map.end())
		{
			//for groupby,only have groupby
			JSONValue streamJV;
			if (dataflowJV.isMember(itr->first))
				streamJV = dataflowJV[itr->first];
			
			for(size_t j=0; j<outputs.size(); j++)
			{
				output_name = outputs[j];
				output_name << ".input";
				streamJV.append(output_name.data());
			}
			dataflowJV[itr->first] = streamJV;
		}
		else
		{
			//for iiljoin and groupby
			inputs = name_itr->second;
			for(size_t i=0; i<inputs.size(); i++)
			{
				JSONValue streamJV;
				if (dataflowJV.isMember(inputs[i]))
					streamJV = dataflowJV[inputs[i]];

				for(size_t j=0; j<outputs.size(); j++)
				{
					output_name = outputs[j];
					output_name << ".input";
					streamJV.append(output_name.data());
				}
				dataflowJV[inputs[i]] = streamJV;
			}
		}
		++itr;
	}

	ser_conf << "\"dataflow\":" << string(dataflowJV.toStyledString());
/*
	mIsInSelectList.clear();
	//for config the dataflow
	mNameList = jimo_Service_raw->getDataProcNameList();
	mDatasetNameList= jimo_Service_raw->getDatasetList();
	mIndexList= jimo_Service_raw->getDataProcIndexList();
	mIILBatchOprList= jimo_Service_raw->getDataProcIILBatchOprList();
	mDocBatchOprList= jimo_Service_raw->getDataProcDocBatchOprList();
	mIILJoinList= jimo_Service_raw->getDataProcIILJoinList();
	mGroupByList= jimo_Service_raw->getDataProcGroupByList();
	mStatJoinList = jimo_Service_raw->getDataProcStatJoinList();
	mStatDocList = jimo_Service_raw->getDataProcStatDocList();
	mStatKeyList = jimo_Service_raw->getDataProcStatKeyList();
	mIILKeyList = jimo_Service_raw->getDataProcIILKeyList();
	mUnionSelectNameList = jimo_Service_raw->getUnionSelectName();

	//map
	mSelectMap = jimo_Service_raw->getSelectMap();
	mUnionMap = jimo_Service_raw->getUnionMap();
	mJoinMap = jimo_Service_raw->getJoinMap();
	mIILJoinMap = jimo_Service_raw->getIILJoinMap();
	mStatMap = jimo_Service_raw->getStatMap();
	mSelectInputMap = jimo_Service_raw->getSelectInputMap();
	mDataProcMap = jimo_Service_raw->getDataProcMap();
	mDocMap = jimo_Service_raw->getDocMap();
	mIndexMap = jimo_Service_raw->getIndexMap();
	mSelectConfMap = jimo_Service_raw->getSelectConfMap();
	mDatasetMap.clear();

	vector<OmnString> unionList;
	vector<OmnString> selectList;
	u32 idx = 1;
	if((mJoinMap.size()>0 && mUnionMap.size()>0) || (mIILJoinMap.size()>0 && mUnionMap.size()>0))
	{
		map<OmnString, vector<OmnString> >::iterator itr;
		itr = mJoinMap.begin();
		while(itr != mJoinMap.end())
		{
			OmnString join_name = itr->first;
			vector<OmnString> joinList = mJoinMap[join_name];
			vector<OmnString> iiljoinList = mIILJoinMap[join_name];
			//unionList = mUnionMap[joinList[0]];
			//if(unionList.size()>0)
			
			map<OmnString, vector<OmnString> >::iterator union_itr = mUnionMap.find(joinList[0]);
			if(union_itr != mUnionMap.end())
			{
				unionList = union_itr->second;
				//for union's select
				ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[0]], iiljoinList[1]);
				ser_conf << ",";
				ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[1]], iiljoinList[1]);
				ser_conf << ",";

				//for join'select key
				ser_conf = selectConfigStr(ser_conf, mSelectMap[joinList[1]], iiljoinList[0]);
				ser_conf << ",";
			}
			else
			{
				//for join'select key
				ser_conf = selectConfigStr(ser_conf, mSelectMap[joinList[0]], iiljoinList[0]);
				//ser_conf << ",";

				union_itr = mUnionMap.find(joinList[1]);
			
				if (union_itr != mUnionMap.end())
				{
					unionList = union_itr->second;
					aos_assert_rr(unionList.size() == 2, rdata, false);
					//for union's select
					ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[0]], iiljoinList[1]);
					//ser_conf << ",";
					ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[1]], iiljoinList[1]);
					//ser_conf << ",";
				}
				else
				{
					ser_conf = selectConfigStr(ser_conf, mSelectMap[joinList[1]], iiljoinList[1]);
				}
			}

			ser_conf = getStatFlow(join_name, ser_conf, iiljoinList);
			//ser_conf << "\"" << iiljoinList[0] << ".output\":[ \"" << mGroupByList[0] << ".input\"],"
			//	<< "\"" << iiljoinList[1] << ".output\":[ \"" << mGroupByList[0] << ".input\"],";
			if(idx < mStatMap.size() && !mInSelectList)
				ser_conf << ",";
			mInSelectList = false;
			idx++;
			itr++;
		}
	}
	else if(mJoinMap.size()>0 && mUnionMap.size()==0 && mIILJoinMap.size()>0)
	{
		map<OmnString, vector<OmnString> >::iterator itr;
		itr = mJoinMap.begin();
		while(itr != mJoinMap.end())
		{
			OmnString join_name = itr->first;
			vector<OmnString> joinList = mJoinMap[join_name];
			vector<OmnString> iiljoinList = mIILJoinMap[join_name];

			//for join'select key
			ser_conf = selectConfigStr(ser_conf, mSelectMap[joinList[0]], iiljoinList[0]);
			//if(!mInSelectList)
			//	ser_conf << ",";

			ser_conf = selectConfigStr(ser_conf, mSelectMap[joinList[1]], iiljoinList[1]);
			//if(!mInSelectList)
			//	ser_conf << ",";


			ser_conf = getStatFlow(join_name, ser_conf, iiljoinList);
			
			if(idx < mStatMap.size() && !mInSelectList)
				ser_conf << ",";
			mInSelectList = false;
			idx++;
			itr++;
		}
	}
	else if(mJoinMap.size()==0 && mUnionMap.size()>0)
	{
		map<OmnString, vector<OmnString> >::iterator itr;
		itr = mUnionMap.begin();
		OmnString union_name = itr->first;

		unionList = mUnionMap[union_name];
		//for union's select
		ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[0]], "");
		ser_conf << ",";
		ser_conf = selectConfigStr(ser_conf, mSelectMap[unionList[1]], "");
		ser_conf << ",";

		if(mGroupByList.size()>0)
			ser_conf = sampleFlowConf(ser_conf);
	}
	else if((mJoinMap.size()==0 && mUnionMap.size()==0 && mSelectMap.size()>0) || mDocMap.size()>0)
	{
		ser_conf << getIndexConf();
		if(ser_conf != "")
		{
			int length = ser_conf.length();
			int idx = ser_conf.findSubString(OmnString(","), length-6);
			if(idx >= 0)
				ser_conf = ser_conf.substr(0, idx-1);  
		}
		if(mGroupByList.size()>0)
			ser_conf = sampleFlowConf(ser_conf);
	}
	*/
	/*
	//for stat
	if(mGroupByList.size()>0)
	{
		map<OmnString, vector<OmnString> >::iterator itr;
		itr = mStatMap.begin();
		while(itr!=mStatMap.end())
		{
			OmnString name= itr->first;
		}
		ser_conf = dataFlowConf(ser_conf);
	}
	*/
	//rslt = insertSelectConf();
	mConfList = jimo_Service_raw->getConfList();
	ser_conf << ",\"datasets\":[ " << getAllDataset(jimo_Service_raw) << "],";
	ser_conf << "\"running\" : {";
	ser_conf << getDataProcConf();
	ser_conf << "} };";

	rslt = parseRun(rdata, ser_conf, jimo_Service_raw);
	if(!rslt)
		return false;

	OmnScreen << "Statements to run for Service: " << mServiceName << endl;
	cout << statements_str << endl;

	//aos_assert_rr(rslt, rdata, false);
	//OmnString Service_msg = "Service "; 
	//Service_msg << "'" <<  mServiceName << "' created successfully.";
	//rdata->setJqlMsg(Service_msg);
	return true;
}

bool 
AosJimoLogicCreateService::appendStatement(AosRundata *rdata,const AosJqlStatementPtr &stmt)
{
	if (!stmt)
	{
		AosLogError(rdata, true, "Missing_statement") << enderr;
		return false;
	}
	mStatements.push_back(stmt);
	return true;
}

	bool
AosJimoLogicCreateService::findSubString(
		AosRundata *rdata,
		OmnString str,
		vector<OmnString> &replace_parms)
{
	const char *data = str.data();
	int start_idx,end_idx;
	for(int i = 0; i < str.length(); i++)
	{

		if(data[i] == '$')
		{
			start_idx = i;
			for(int j = i ;j < str.length();j++)
			{
				if(data[j] == '"')
				{
					end_idx = j;
					i = j;
					break;
				}
			}

			replace_parms.push_back(OmnString(&data[start_idx],end_idx-start_idx));			
		}
	}
	return true;
}



AosJimoPtr 
AosJimoLogicCreateService::cloneJimo() const
{
	return OmnNew AosJimoLogicCreateService(*this);
}
/*
OmnString
AosJimoLogicCreateService::dataFlowConf(OmnString name, OmnString conf_str, vector<OmnString> stat_list)
{
	OmnString iilKeyName = "";
	conf_str <<"\"" << stat_list[0] << ".output\" :[\"" << stat_list[1] << ".input\"],"
		<<"\"" << stat_list[1] << ".output_vt2d\" :[\"" << stat_list[2] << ".input\"],"
		<<"\"" << stat_list[1] << ".output_statkey\" :[\"" << stat_list[3] << ".input\"],";

	for(u32 i=4; i<stat_list.size(); i++)
	{
		int namelen = name.length();
		int len = stat_list[i].length();
		iilKeyName = stat_list[i].substr(namelen, len);

		if(i == stat_list.size() -1)
		{
			conf_str <<"\"" << stat_list[1] << ".output_"
				<< iilKeyName << "\" :[\"" << stat_list[i] << ".input\"]";
		}
		else
		{
			conf_str <<"\"" << stat_list[1] << ".output_"
				<< iilKeyName << "\" :[\"" << stat_list[i] << ".input\"],";
		}
	}
	return conf_str;
}

OmnString
AosJimoLogicCreateService::selectConfigStr(OmnString conf_str, vector<OmnString> selectList, OmnString iilJoinName)
{
	OmnString join_name;
	mInSelectList = false;
	vector<OmnString> inList;
	inList.clear();
	if(selectList.size() >0)
	{
		for(u32 j=0; j<mIsInSelectList.size(); j++)
		{
			if(mIsInSelectList[j] == selectList[1])
				mInSelectList = true;
			else
				inList.push_back(selectList[1]);
		}
		if(mIsInSelectList.size() == 0)
			mIsInSelectList.push_back(selectList[1]);

		for(u32 k=0; k<inList.size(); k++)
			mIsInSelectList.push_back(inList[k]);
	}

	if(mInSelectList)
		return conf_str;

	if(mSelectInputMap.size()>0)
	{
		map<OmnString, vector<OmnString> >::iterator itr;
		itr = mSelectInputMap.find(selectList[1]);
		vector<OmnString> joinNameList;
		while(itr != mSelectInputMap.end())
		{
			joinNameList = itr->second;	
			for(u32 i=0; i<joinNameList.size(); i++)
			{
				join_name << ",\"" << joinNameList[i] << ".input\"";	
			}
			itr++;
		}
	}

	if(selectList.size() == 4)
	{
		conf_str << "\"" <<	selectList[0] << "\":[ \"" << selectList[1] << ".input\""
			<< getDataProcMap(selectList[0]) << getDocSelectConf(selectList[0]) << "]"
			<<",\"" << selectList[1] << ".output\" :[\"" << selectList[2] << ".input\"";
		if(mSelectInputMap.size() > 0)
			conf_str << join_name ;
		if(iilJoinName!= "")
		{
			conf_str <<",\"" << iilJoinName << ".input\"]";
		}
		else
			conf_str << "]";

		conf_str <<",\"" << selectList[2] << ".output\" :[\"" << selectList[3] << ".input\"]";
		// for map
		conf_str << "," << getDataProcMapIIL(selectList[0]);
		// for doc
		conf_str << getDocConf(selectList[0]);
	}
	else if(selectList.size() == 5)
	{
		conf_str << "\"" <<	selectList[0] << "\":[ \"" << selectList[1] << ".input\""
			<< getDataProcMap(selectList[0]) << getDocSelectConf(selectList[0]) << "]"
			<<",\"" << selectList[1] << ".output\" :[\"" << selectList[2] << ".input\"";
		if(mSelectInputMap.size() > 0)
			conf_str << join_name;
		if(iilJoinName !="")
		{
			// set iiljoin and doc
			conf_str << ",\"" << iilJoinName << ".input\"]";
		}
		else
		{
			conf_str <<",\"" << selectList[4] << ".input\"";
			conf_str << "]";
		}

		// for index 
		conf_str <<",\"" << selectList[2] << ".output\" :[\"" << selectList[3] << ".input\"]";

		// for map
		conf_str << "," << getDataProcMapIIL(selectList[0]);
		// for Doc
		conf_str << getDocConf(selectList[0]);
	}
	return conf_str;

}
	
OmnString
AosJimoLogicCreateService::getStatFlow(OmnString name, OmnString conf_str, vector<OmnString> iilJoinList)
{

	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mStatMap.begin();
	vector<OmnString> statList;
	while(itr != mStatMap.end())
	{
		OmnString join_name = itr->first;
		if(name == join_name)
		{
			statList = mStatMap[name];
			
			conf_str << "\"" << iilJoinList[0] << ".output\":[ \"" << statList[0] << ".input\"],"
				<< "\"" << iilJoinList[1] << ".output\":[ \"" << statList[0] << ".input\"],";
			
			//for stat data flow
			if(mGroupByList.size()>0)
				conf_str = dataFlowConf(name, conf_str, statList);
			break;
		}
		itr++;
	}
	return conf_str;

}

OmnString
AosJimoLogicCreateService::sampleFlowConf(OmnString conf_str)
{
	conf_str <<"\"" << mGroupByList[0] << ".output\" :[\"" << mStatJoinList[0] << ".input\"],"
		<<"\"" << mStatJoinList[0] << ".output_vt2d\" :[\"" << mStatDocList[0] << ".input\"],"
		<<"\"" << mStatJoinList[0] << ".output_statkey\" :[\"" << mStatKeyList[0] << ".input\"],";

	for(u32 i=0; i<mIILKeyList.size(); i++)
	{
		if(i == mIILKeyList.size() -1)
		{
			conf_str <<"\"" << mStatJoinList[0] << ".output_"
				<< mIILKeyList[i] << "\" :[\"" << mIILKeyList[i] << ".input\"]";
		}
		else
		{
			conf_str <<"\"" << mStatJoinList[0] << ".output_"
				<< mIILKeyList[i] << "\" :[\"" << mIILKeyList[i] << ".input\"],";
		}
	}
	return conf_str;

}
	
OmnString
AosJimoLogicCreateService::getDataProcMap(OmnString name)
{
	if(mDataProcMap.size() == 0)
	{
		return "";
	}

	OmnString conf_str = "";
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> mapList;
	mapList.clear();
	itr = mDataProcMap.begin();
	while(itr != mDataProcMap.end())
	{
		mapList = itr->second;
		for(u32 i=0; i<mapList.size(); i++)
		{
			if(mapList[i]==name)
			{
				conf_str << ",\"" << mapList[1] << ".input\"";
			}
		}
		itr++;
	}
	
	return conf_str;
}
	
OmnString
AosJimoLogicCreateService::getDataProcMapIIL(OmnString name)
{
	if(mDataProcMap.size() == 0)
	{
		return "";
	}
	OmnString conf_str = "";
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> mapList2;
	vector<OmnString> mapList3;
	mapList2.clear();
	mapList3.clear();
	u32 idx = 1;
	bool isMap = true;

	itr = mDataProcMap.begin();
	while(itr != mDataProcMap.end())
	{
		mapList2= (itr)->second;
		if((idx +1) <= mDataProcMap.size())
		{
			mapList3= (++itr)->second;
		}
		else
		{
			isMap = false;
			itr++;
		}

		if((mapList2[0] == name) && isMap)
		{
			conf_str << "\"" << mapList2[1] << ".output\" : "
				<<"[\"" << mapList2[2] << ".input\"],";
		}
		else if((mapList2[0] == name) && !isMap)
		{
			conf_str << "\"" << mapList2[1] << ".output\" : "
				<<"[\"" << mapList2[2] << ".input\"]";
		}
		idx++;
	}
	if(conf_str != "")
	{
		int length = conf_str.length();
		int idx = conf_str.findSubString(OmnString(","), length -1);
		if(idx < 0)
			conf_str << ",";
	}
	return conf_str;
}

OmnString
AosJimoLogicCreateService::getDocSelectConf(OmnString name)
{
	if(mDocMap.size() == 0)
	{
		return "";
	}

	OmnString conf_str = "";
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> docList;
	docList.clear();
	itr = mDocMap.begin();
	while(itr != mDocMap.end())
	{
		if((itr->first) == name)
		{
			docList = itr->second;
			if(docList.size()>0)
			{
				conf_str << ",\"" << docList[1] << ".input\"";
			}
		}
		itr++;
	}
	
	return conf_str;
}


OmnString
AosJimoLogicCreateService::getDocConf(OmnString name)
{
	if(mDocMap.size() == 0)
	{
		return "";
	}
	
	OmnString conf_str = "";
	u32 idx = 1;
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> docList2;
	vector<OmnString> docList3;
	docList2.clear();
	docList3.clear();
	itr = mDocMap.begin();
	bool isDoc = true;
	while(itr != mDocMap.end())
	{
		docList2 = (itr)->second;
		if((idx+1) <= mDocMap.size())
		{
			docList3 = (++itr)->second;
			if(	docList3[0] != name)
				isDoc = false;
		}
		else
		{
			isDoc = false;
			itr++;
		}

		if((docList2[0] == name) && isDoc)
		{
			conf_str << "\"" << docList2[1] << ".output\":"
				<<"[\"" << docList2[2] << ".input\"],";
		}
		else if((docList2[0] == name) && !isDoc)
		{
			conf_str << "\"" << docList2[1] << ".output\":"
				<<"[\"" << docList2[2] << ".input\"]";
		}
		idx++;
	}
	
	return conf_str;
}
*/
/*
OmnString
AosJimoLogicCreateService::getIndexConf()
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mIndexMap.begin();
	OmnString ser_conf = "";
	OmnString prev = "";
	OmnString sel_out = "";
	OmnString ds_out = "";
	vector<OmnString> selList;
	vector<OmnString> selectList;
	u32 idx =1;
	bool rslt = false;
	while(itr != mIndexMap.end())
	{
		OmnString select_name = itr->first;
		selectList = mIndexMap[select_name];
		rslt = isSameDataset(itr, idx);
		if(selectList[0] == prev || rslt)
		{
			prev = selectList[0];
			selList.push_back(selectList[1]);

			sel_out << "\"" << selectList[1] << ".output\" : ["
					<< "\"" << selectList[2] << ".input\"], "
					<< "\"" << selectList[2] << ".output\" : [ "
					<< "\"" << selectList[3] << ".input\"], ";
		}
		else
		{
			if(prev != "" && (selList.size()>0))
			{
				ds_out << "\"" << selectList[0] << ".output\" : [";
				for(u32 i=0; i<selList.size(); i++)
				{
					if(i==(selList.size()-1))
						ds_out << "\"" << selList[i] << ".input\"";
					else
						ds_out << "\"" << selList[i] << ".input\",";
				}

				//ds_out << getDataProcMap(prev) << getDocSelectConf(prev);
				//ds_out << "],";

				//sel_out << getDataProcMapIIL(prev) << getDocConf(prev);
			}
			selList.clear();
			prev = selectList[0];
			ds_out << "\"" << selectList[0] << ".output\" : ["
				<< "\"" << selectList[1] << ".input\"],";

			sel_out << "\"" << selectList[1] << ".output\" : ["
				<< "\"" << selectList[2] << ".input\" ], "
				<< "\"" << selectList[2] << ".output\" : [ "
				<< "\"" << selectList[3] << ".input\"], ";
		}
		itr++;
		idx++;
	}
	if(prev != "" && (selList.size()>0))
	{
		ds_out << "\"" << selectList[0] << ".output\" : [";
		for(u32 i=0; i<selList.size(); i++)
		{
			if(i==(selList.size()-1))
				ds_out << "\"" << selList[i] << ".input\"";
			else
				ds_out << "\"" << selList[i] << ".input\",";
		}
		//ds_out << getDataProcMap(prev) << getDocSelectConf(prev);
		//ds_out << "]";

		//sel_out << getDataProcMapIIL(prev) << getDocConf(prev);
	}
	ds_out << getDataProcMapConf() << getDocSelectConf();
	ds_out << "]";

	sel_out << getDataProcMapIILConf() << getDocConf();

	selList.clear();
	ds_out << "," << sel_out;
	ser_conf = ds_out;
	ser_conf << ",";
	return ser_conf;
}
*/
/*
OmnString
AosJimoLogicCreateService::getIndexConf()
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mIndexMap.begin();
	OmnString ser_conf = "";
	OmnString prev = "";
	OmnString sel_out = "";
	OmnString ds_out = "";
	vector<OmnString> selList;
	vector<OmnString> selectList;
	while(itr != mIndexMap.end())
	{
		OmnString select_name = itr->first;
		selectList = mIndexMap[select_name];
		//for set dataset --- dsname === selectname
		insertToDatasetMap(selectList[0],  selectList[1]);

		//for set select --- selectname === docname, indexname...
		insertToComSelMap(selectList[1],  selectList[2]);
		if(selectList.size() == 5)
		{
			insertToComSelMap(selectList[1],  selectList[4]);
		}
		sel_out	<< "\"" << selectList[2] << ".output\" : [ "
			<< "\"" << selectList[3] << ".input\"], ";

		itr++;
	}
	bool rslt = getDataProcMapConf();
	aos_assert_r(rslt, 0);

	rslt = getDocSelectConf();
	aos_assert_r(rslt, 0);

	ds_out << getCombineMap();
	sel_out << getDataProcMapIILConf();

	selList.clear();
	ds_out << sel_out;
	ser_conf = ds_out;
	return ser_conf;
}


bool
AosJimoLogicCreateService::isSameDataset(map<OmnString, vector<OmnString> >::iterator itr, u32 idx)
{
	OmnString name = itr->first;
	vector<OmnString> selList1 = itr->second;
	vector<OmnString> selList2;
 
	if((idx+1) <= mIndexMap.size())
	{
		name = (++itr)->first;
		selList2 = itr->second;
	}
	else
		return false;

	itr--;
	if(selList1[0] == selList2[0])
		return true;
	else
		return false;
}
	
	
bool
AosJimoLogicCreateService::getDataProcMapConf()
{
	if(mDataProcMap.size() == 0)
	{
		return "";
	}

	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> mapList;
	mapList.clear();
	itr = mDataProcMap.begin();
	while(itr != mDataProcMap.end())
	{
		mapList = itr->second;
		insertToDatasetMap(mapList[0], mapList[1]);
		itr++;
	}
	return true;
}

OmnString
AosJimoLogicCreateService::getDataProcMapIILConf()
{
	if(mDataProcMap.size() == 0)
	{
		return "";
	}
	OmnString conf_str = "";
	vector<OmnString> mapList;
	mapList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mDataProcMap.begin();
	while(itr != mDataProcMap.end())
	{
		mapList= itr->second;
		conf_str << "\"" << mapList[1] << ".output\" : "
			<<"[\"" << mapList[2] << ".input\"],";

		itr++;
	}

	return conf_str;
}
OmnString
AosJimoLogicCreateService::getDatasetAndMap(OmnString ds_name)
{
	OmnString conf = "";
	vector<OmnString> mapList;
	mapList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mDataProcMap.begin();
	while(itr != mDataProcMap.end())
	{
		mapList = itr->second;
		if(ds_name == mapList[0])
			conf << ",\"" << mapList[1] << ".input\"";
		itr++;
	}
	return conf;
}

bool
AosJimoLogicCreateService::getDocSelectConf()
{
	if(mDocMap.size() == 0)
	{
		return "";
	}

	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> docList;
	docList.clear();
	itr = mDocMap.begin();
	while(itr != mDocMap.end())
	{
		docList = itr->second;
		insertToDatasetMap(docList[0], docList[1]);

		// for set selcomMap
		insertToComSelMap(docList[1], docList[2]);
		itr++;
	}
	return true;
}

OmnString
AosJimoLogicCreateService::getDocConf()
{
	if(mDocMap.size() == 0)
	{
		return "";
	}
	
	map<OmnString, vector<OmnString> >::iterator itr;
	OmnString conf_str = "";
	vector<OmnString> docList;
	itr = mDocMap.begin();
	while(itr != mDocMap.end())
	{
		docList = itr->second;

		conf_str << "\"" << docList[1] << ".output\":"
			<<"[\"" << docList[2] << ".input\"],";

		itr++;
	}

	return conf_str;
}

bool
AosJimoLogicCreateService::insertToComSelMap(OmnString sel_name, OmnString idx_name)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mComSelMap.find(sel_name);
	vector<OmnString> idxList;
	idxList.clear();
	if(itr != mComSelMap.end())
	{
		idxList = mComSelMap[sel_name];
		idxList.push_back(idx_name);
		mComSelMap[sel_name] = idxList;
	}
	else
	{
		idxList.push_back(idx_name);
		mComSelMap[sel_name] = idxList;
	}

	return true;
}



OmnString
AosJimoLogicCreateService::getCombineMap()
{
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> selList;
	vector<OmnString> comList;
	selList.clear();
	OmnString ds_name = "";
	OmnString ser_conf = "";
	itr = mDatasetMap.begin();
	while(itr != mDatasetMap.end())
	{
		map<OmnString, vector<OmnString> >::iterator itr_idx;
		vector<OmnString> selList;
		selList = itr->second;
		ds_name = itr->first;	
		
		ser_conf << "\"" << ds_name << "\": [ \""
			<< selList[0] << ".input\"" 
			<< getDatasetAndMap(ds_name) << "], \""
			<< selList[0] << ".output\" : [";

		//for the same dataset
		mSelectList.push_back(selList[0]);
		for(u32 i=0; i<selList.size(); i++)
		{
			itr_idx = mComSelMap.find(selList[i]);
			if(itr_idx != mComSelMap.end())
			{
				comList = itr_idx->second;
				for(u32 j=0; j<comList.size(); j++)
				{
					if(j == (comList.size()-1))
					{
						ser_conf << "\"" << comList[j] << ".input\"";
					}
					else
					{
						ser_conf << "\"" << comList[j] << ".input\",";
					}
				}
				if(i != (selList.size()-1))
					ser_conf << ",";
			}
		}
		ser_conf << "],";
		itr++;
	}
	return ser_conf;
}

*/

OmnString
AosJimoLogicCreateService::getDataProcConf()
{

	OmnString conf = "";
	for(u32 j = 0; j < mConfList.size(); j++)
	{
		if(j == mConfList.size()-1)
			conf << mConfList[j];
		else
			conf << mConfList[j] << ",";
	}

	return conf;
}
/*
bool
AosJimoLogicCreateService::insertSelectConf()
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mSelectMap.begin();
	vector<OmnString> selList;
	selList.clear();
	OmnString name = "";
	OmnString conf = "";
	while(itr != mSelectMap.end())
	{
		name = itr->first;
		selList = mSelectMap[name];
		insertToDatasetMap(selList[0], selList[1]);
		++itr;
	}
	return true;
}


bool
AosJimoLogicCreateService::insertToDatasetMap(OmnString ds_name, OmnString sel_name)    
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mDatasetMap.find(ds_name);
	vector<OmnString> selList;
	selList.clear();
	if(itr != mDatasetMap.end())
	{
		selList = mDatasetMap[ds_name];
		selList.push_back(sel_name);
		mDatasetMap[ds_name] = selList;
	}
	else
	{
		selList.push_back(sel_name);
		mDatasetMap[ds_name] = selList;
	}

	return true;
}
*/

OmnString
AosJimoLogicCreateService::getAllDataset(AosJimoProgObj *prog)    
{
	set<OmnString> datasets = prog->getDatasetMap();
	set<OmnString>::iterator itr = datasets.begin();
	OmnString ds_conf = "";
	ds_conf << "\"" << *itr << "\"";
	++itr;
	while(itr != datasets.end())
	{
		ds_conf << ",\"" << *itr << "\"";
		++itr;
	}

	return ds_conf;
}


