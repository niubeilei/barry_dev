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
// A Map Task is defined as:
// 	Create map map-name
// 	(
//     	table="<table-name>",
//      Inputs="<input-data>",
//      KeyFields=[<expression-list>],
//      value=<value>,
//      condition=<condition>
//  );
//
// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcMap.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcMap_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcMap(version);
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


AosJimoLogicDataProcMap::AosJimoLogicDataProcMap(const int version)
:
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcMap::~AosJimoLogicDataProcMap()
{
}


AosJimoPtr
AosJimoLogicDataProcMap::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcMap(*this);
}


bool
AosJimoLogicDataProcMap::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	Create map <map-name>
	// 	(
	// 		name-value-list
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "map", rdata, false);
	mErrmsg = "dataproc map ";

	parsed = false;
	OmnString map_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,map_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << map_name;
	
	if(dft)
		mIsService = dft;

	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);	
		return false;
	}

	mMapName = map_name;
	mNameValueList = name_value_list;
		
	// Parse table
	mTableName = jimo_parser->getParmStr(rdata, "table", name_value_list);
	if(mTableName == "")
	{
		setErrMsg(rdata, eGenericError,"table",mErrmsg);	
		return false;
	}
	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTableName);
	if (!table_doc)
	{
		setErrMsg(rdata, eNotExist, mTableName, mErrmsg);	
		return false;
	}

	//parse parser
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	aos_assert_rr(schema_objid != "", rdata, false);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
	aos_assert_rr(schema_doc, rdata, false);
	mParser = schema_doc->getAttrStr("zky_name","");
	
	//config virtual field
	rslt = createVfNameValueMap(schema_doc);
	aos_assert_r(rslt,false);
	
	//config mKeyFields ,value , cond(option)
	rslt = 	configKeysValueCond(rdata, table_doc);
	aos_assert_r(rslt,false);


	//Parse Input
	mInput = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	if(mInput == "")
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);	
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
AosJimoLogicDataProcMap::createVfNameValueMap(const AosXmlTagPtr &schema_doc)
{
	AosXmlTagPtr tag = schema_doc->getFirstChild("datarecord", "");
	aos_assert_r(tag, false);
	tag = tag->getFirstChild("datafields", "");
	aos_assert_r(tag, false);

	AosXmlTagPtr field = tag->getNextChild("datafield");
	OmnString fieldname, value;
	while(field)
	{
		OmnString type = field->getAttrStr("type", "");
		if (type == "expr")
		{
			fieldname = field->getAttrStr("zky_name", "");
			aos_assert_r(fieldname != "", false);

			AosXmlTagPtr expr = field->getNextChild("expr");
			aos_assert_r(expr, false);
			value= expr->getNodeText();
		}
		mVfFieldNameValueMap.insert(make_pair(fieldname, value));
		field = tag->getNextChild();
	}
	return true;
}


bool 
AosJimoLogicDataProcMap::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	aos_assert_rr(prog, rdata, false);
	bool rslt = procInput(rdata, mInput, prog, mInputName, statements_str);
	if (!rslt)
	{
		OmnString msg = "dataproc map ";
		msg << mMapName << " missing input : " << mInput << ".";
		rdata->setJqlMsg(msg);
		return false;
	}
	
	if(mIsService)
	{
		mJobName = prog->getJobname();
	}

	//if (mInputName == "")
	//{
	//	AosLogError(rdata, true, "failed_retrieving_output_name")
	//		<< AosFN("Input Name") << mInput << enderr;
	//	return false;
	//}

	// This function creates an AosJimoLogicDataProcMap.
	for(size_t i = 0; i < mKeyFields.size(); i++)
	{
		aos_assert_r(i < mDataType.size(), false);
		mDataprocMap = "";
		mDataprocMap << mJobName << "_dp_map_" << mTableName << "_" << mMapName << i;
		mDataprocDoc = "";
		mDataprocDoc << mJobName << "_dp_map_" << mTableName << "_" << mMapName << i << "_doc";
		mMapIILName = "";
		mMapIILName << mJobName << "_dp_map_iil_" << mTableName << "_" << mMapName << i;

		OmnString datatype = mDataType[i];
		if (datatype == "str" || datatype == "string" || datatype == "datetime")
		{
			rslt = createStr2StrMapDataproc(rdata, prog, statements_str, i);
			aos_assert_rr(rslt, rdata, false);
		}
		else
		{
			rslt = createStr2NumMapDataproc(rdata, prog, statements_str, i, datatype);
			aos_assert_rr(rslt, rdata, false);
		}

		/*rslt = createDocBatchoprDataproc(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);*/
	
		rslt = createIILDataproc(rdata, prog, statements_str, i);
		aos_assert_rr(rslt, rdata, false);

		if(!mIsService)
		{
			rslt = addMapTask(rdata, prog, i);
			aos_assert_rr(rslt, rdata, false);

		//rslt = addDocTask(rdata, prog, i);
		//aos_assert_rr(rslt, rdata, false);

			rslt = addIILTask(rdata, prog, i);
			aos_assert_rr(rslt, rdata, false);
		}
		else
		{
			vector<OmnString> mapList;
			mapList.clear();
			mapList.push_back(mInput);
			mapList.push_back(mDataprocMap);
			mapList.push_back(mMapIILName);
		}
	}

	return true;
}


bool
AosJimoLogicDataProcMap::configKeysValueCond(
		AosRundata *rdata, 
		AosXmlTagPtr &table_doc)
{
	AosXmlTagPtr maps_doc = table_doc->getFirstChild("maps");
	aos_assert_rr(maps_doc, rdata ,false);
	AosXmlTagPtr map_doc = maps_doc->getFirstChild("map");
	//JIMODB-1026
	//arvin 2015.10.27
	if(!map_doc)
	{
		mErrmsg << " : no maps need to create on table "
			<< mTableName << "!";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	while(map_doc)
	{
		//get keyfields
		vector<OmnString> keys;
		keys.clear();
		AosXmlTagPtr keys_doc = map_doc->getFirstChild("keys");
		aos_assert_rr(keys_doc, rdata, false);
		AosXmlTagPtr key_doc = keys_doc->getFirstChild("key");
		while(key_doc)
		{
			OmnString key = key_doc->getNodeText();
			keys.push_back(key);
			key_doc = keys_doc->getNextChild();
		}
		mKeyFields.push_back(keys);
		//get iil_name
		OmnString iilname = map_doc->getAttrStr("zky_iilname","");
		mIILNames.push_back(iilname);


		// get map type, Young, 2015/11/25
		OmnString maptype = map_doc->getAttrStr("zky_type");		
		mMapTypes.push_back(maptype);

		//get value
		vector<OmnString> value_strs;
		value_strs.clear();
		AosXmlTagPtr values_doc = map_doc->getFirstChild("values");
		aos_assert_rr(values_doc, rdata, false);
		AosXmlTagPtr value_doc = values_doc->getFirstChild("value");
		aos_assert_r(value_doc, false);
		//get value datatype
		OmnString datatype = map_doc->getAttrStr(AOSTAG_DATAGEN_TYPE, "");
		mDataType.push_back(datatype);

//		while(value_doc)
//		{
		OmnString value_str = value_doc->getNodeText();
		map<OmnString, OmnString>::iterator itr = mVfFieldNameValueMap.find(value_str);
		if (itr != mVfFieldNameValueMap.end())
			value_str = itr->second;
		//			value_strs.push_back(value_str);
		//			value_doc = values_doc->getNextChild();
		//		}
		mValues.push_back(value_str);
		//get value Aggr_opr 
		mValueAggrOpr = value_doc->getAttrStr("agrtype", "");
		//get MaxKeyLen
		mMaxLens.push_back(value_doc->getAttrInt("max_len",0));
		//get Condition;
		OmnString cond = "";
		AosXmlTagPtr cond_doc = map_doc->getFirstChild("cond");
		if(cond_doc)
			cond = cond_doc->getNodeText();
		mConds.push_back(cond);
		map_doc = maps_doc->getNextChild();
	}
	return true;
}

bool
AosJimoLogicDataProcMap::createStr2StrMapDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		int opt)
{
	OmnString dataprocMap_str = "";
	if(mIsService)
	{
		AosJimoLogicObjNew * jimo_logic = jimo_prog->getJimoLogic(rdata, mInput);
		if (jimo_logic)
		{
			AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
			if (type == AosJimoLogicType::eDataset)
			{
				jimo_prog->insertToDataFlowMap(mInput, mDataprocMap);
				jimo_prog->setDatasetMap(mInput);
			}
			else
			{
				OmnString inName = mInput;
				inName << ".output";
				jimo_prog->insertToDataFlowMap(inName, mDataprocMap);
			}
		}
		else
		{
			OmnAlarm << "ddddddddddd" << enderr;
		}
		
		dataprocMap_str << "\"" << mDataprocMap << "\" : { "
					<< "\"type\":" << "\"" << "dataprocstr2strmap" << "\"" << ",";
	}
	else
	{
		dataprocMap_str = "create dataprocstr2strmap ";
		mJobName = jimo_prog->getJobname();
		dataprocMap_str << mDataprocMap << " { "
					<< "\"type\":" << "\"" << "str2strmap" << "\"" << ",";
	}
  	dataprocMap_str << "\"record_type\":" << "\"buff\"" << ",";
	if(mIsService)
		dataprocMap_str << "\"docid\":\"docid\",";
	else
		dataprocMap_str << "\"docid\":\"getDocid(\'" << mParser <<"\')\",";

	dataprocMap_str<< "\"key_fields\":" << "[" ;
	for(u32 i=0; i< mKeyFields[opt].size(); i++)
	{
		if(i>0)
			dataprocMap_str << ",";
		dataprocMap_str << "\"" << mKeyFields[opt][i] << "\"";
	}
	dataprocMap_str << "],";
    dataprocMap_str << "\"value_field\":\"" << mValues[opt] << "\",";
	if(mConds[opt] != "")
		dataprocMap_str << "\"condition\":\"" << mConds[opt] << "\",";
	if(mIsService)
		dataprocMap_str << "\"max_length\":" << mMaxLens[opt] << "}";
	else
		dataprocMap_str << "\"max_length\":" << mMaxLens[opt] << "};";

	if (mIsService)
		jimo_prog->setConf(dataprocMap_str);
	else
	{
		OmnScreen << dataprocMap_str << endl;
		statements_str << "\n" << dataprocMap_str;
		bool rslt = parseRun(rdata,dataprocMap_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}


bool
AosJimoLogicDataProcMap::createStr2NumMapDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		int opt,
		const OmnString &datatype)
{
	OmnString dataprocMap_str = "";
	if(mIsService)
	{
		AosJimoLogicObjNew * jimo_logic = jimo_prog->getJimoLogic(rdata, mInput);
		if (jimo_logic)
		{
			AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
			if (type == AosJimoLogicType::eDataset)
			{
				jimo_prog->insertToDataFlowMap(mInput, mDataprocMap);
				jimo_prog->setDatasetMap(mInput);
			}
			else
			{
				OmnString inName = mInput;
				inName << ".output";
				jimo_prog->insertToDataFlowMap(inName, mDataprocMap);
			}
		}
		else
		{
			OmnAlarm << "ddddddddddd" << enderr;
		}
		
		dataprocMap_str << "\"" << mDataprocMap << "\" : { "
					<< "\"type\":" << "\"" << "dataprocstr2nummap" << "\"" << ",";
	}
	else
	{
		dataprocMap_str = "create dataprocstr2nummap ";
		mJobName = jimo_prog->getJobname();
		dataprocMap_str << mDataprocMap << " { "
					<< "\"type\":" << "\"" << "str2nummap" << "\"" << ",";
	}

  	dataprocMap_str << "\"record_type\":" << "\"buff\"" << ","
		<< "\"key_fields\":" << "[" ;
	for(u32 i=0; i< mKeyFields[opt].size(); i++)
	{
		if(i>0)
			dataprocMap_str << ",";
		dataprocMap_str << "\"" << mKeyFields[opt][i] << "\"";
	}
	dataprocMap_str << "],"
					<< "\"data_type\": \"" << datatype << "\", "
    				<< "\"value_field\":\"" << mValues[opt] << "\",";
	if(mValueAggrOpr != "") 
		dataprocMap_str << "\"aggr_opr\": \"" << mValueAggrOpr <<  "\",";
	if(mConds[opt] != "")
		dataprocMap_str << "\"condition\":\"" << mConds[opt] << "\",";
	if(mIsService)
		dataprocMap_str << "\"max_length\":" << mMaxLens[opt] << "}";
	else
		dataprocMap_str << "\"max_length\":" << mMaxLens[opt] << "};";


	if (mIsService)
		jimo_prog->setConf(dataprocMap_str);
	else
	{
		OmnScreen << dataprocMap_str << endl;
		statements_str << "\n" << dataprocMap_str;
		bool rslt = parseRun(rdata,dataprocMap_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}

/*
//arvin 2015.06.11
bool
AosJimoLogicDataProcMap::createDocBatchoprDataproc(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &statement_str)
{
	OmnString doc_str = "create dataprocdocbatchopr ";
	doc_str << mDataprocDoc << " {"
		<< "\"type\":\"docbatchopr\","
		<< "\"opr\":\"batch_insert\","
		<< "\"format\":\"" << mParser << "\","
		<< "\"docid\":\"getDocid(\'" << mParser << "\')\"};";

	OmnScreen << doc_str << endl;
	statement_str << "\n" << doc_str;
    
	bool rslt = parseRun(rdata,doc_str,prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}
*/

//create dataprociilbatchopr
bool
AosJimoLogicDataProcMap::createIILDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		int opt)
{

	OmnString dataprociil_str = "";
	if (!mIsService)
	{
		dataprociil_str = "create dataprociilbatchopr ";
		dataprociil_str << mMapIILName << " { ";
	}
	else
	{
		OmnString inName = mDataprocMap;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mMapIILName);

		dataprociil_str << "\"" << mMapIILName << "\" : {"
						<< "\"type\":\"dataprociilbatchopr\",";
	}

	//for(u32 i=0; i<mKeyFields.size(); i++)
//	{
		//mIILName << "_" << mKeyFields[0];
//	}
	dataprociil_str << "\"iilname\":" << "\"" <<mIILNames[opt] << "\"" << ","
  	    << "\"record_type\":" << "\"buff\"" << ","
		<< "\"iil_type\":" << "\"BigStr\",";
	
	//if (mIILNames[opt].indexOf("dictionary", 0) != -1)
	if (mMapTypes[opt] == "dict")
	{
		dataprociil_str << "\"opr\":" << "\"inc\",";
		dataprociil_str << "\"inc_type\":" << "\"dict\"";
	}
	else
	{
		dataprociil_str << "\"opr\":" << "\"add\"";
	}

	if(mIsService)
	{
		dataprociil_str << "}";
		jimo_prog->setConf(dataprociil_str);
	}
	else
	{
		dataprociil_str << "};";
		OmnScreen << dataprociil_str << endl;
		statements_str << "\n" << dataprociil_str;
		bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}

/*
 *{
 *   "name": "xxxx",
 *   "dataengine_type":"dataengine_scan2",
 *   "dataprocs":
 *   [
 *       {
 *       "dataproc": "dp_map_tablename_mapname",
 *       "inputs": ["input"],
 *       "outputs": ["dp_map_tablename_mapname_output"]
 *       }
 *   ]
 * }
 *
 */
bool
AosJimoLogicDataProcMap::addMapTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		int opt)
{

	OmnString addmap_str;
	OmnString output;
	OmnString TaskName;
	TaskName << "task_map_" << mTableName << "_" << mMapName << opt;
	output << mDataprocMap << "_output";
	//addgroupby_str << jobname;
	addmap_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocMap << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"],"
		<< "\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	if(jimo_prog)
	{
		jimo_prog->addTask(rdata, addmap_str, "");
		jimo_prog->appendStatement(rdata, "tasks",addmap_str);
	
	}
	//	mTasks.push_back(addmap_str);
	OmnScreen << addmap_str << endl;
    //bool rslt = parseRun(rdata,addmap_str,jimo_prog);
    //aos_assert_rr(rslt,rdata,false);
	return true;
}
/*
bool
AosJimoLogicDataProcMap::addDocTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		int opt)
{
	OmnString add_doc_str;
	OmnString TaskName;
	TaskName << "task_doc_" << mTableName << "_" << mMapName << opt;
	add_doc_str << " { "
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocDoc << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"";
	add_doc_str <<"]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks",add_doc_str);
	OmnScreen << add_doc_str << endl;
	return true;

}
*/


bool
AosJimoLogicDataProcMap::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		int opt)
{
	OmnString addiil_str;
 	OmnString input;
	OmnString output;
	OmnString TaskName;
	TaskName << "task_map_iil_" << mTableName << "_" << mMapName << opt;
	input << mDataprocMap << "_output";
	//addgroupby_str << jobname;
	addiil_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mMapIILName << "\","
		<< "\"inputs\":" << "[\"" << input << "\"" << "]}]}";
	
	if(jimo_prog)
	{
		jimo_prog->addTask(rdata, addiil_str, "");
		jimo_prog->appendStatement(rdata, "tasks",addiil_str);
		
	}
		//mTasks.push_back(addiil_str);
	OmnScreen << addiil_str << endl;
    //bool rslt = parseRun(rdata,addiil_str,jimo_prog);
    //aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcMap::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 

	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcMap*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mMapName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

