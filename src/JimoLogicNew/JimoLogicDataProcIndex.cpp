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
#include "JimoLogicNew/JimoLogicDataProcIndex.h"
#include "JQLStatement/JqlStatement.h"
#include "SEUtil/JqlTypes.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcIndex_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcIndex(version);
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


AosJimoLogicDataProcIndex::AosJimoLogicDataProcIndex(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcIndex::~AosJimoLogicDataProcIndex()
{
}


bool 
AosJimoLogicDataProcIndex::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc index data_proc_index_name
	// 	(
	//		inputs:[input_name],
	//		table:t1,
	//		indexes: ["<index_name>"...],
	//		conditions:<condition>,					//option
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "index", rdata, false);
	mErrmsg = "dataproc index ";
	// set service dft
	if(dft)
		mIsService = dft;

	// parse dataset name
	OmnString dataproc_index_name = jimo_parser->nextObjName(rdata);
	if(!isLegalName(rdata,dataproc_index_name,mErrmsg))
	{
		return false;
	}
	mErrmsg << dataproc_index_name;

	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = dataproc_index_name;
	mNameValueList = name_value_list;

	// parse table
	mTable = jimo_parser->getParmStr(rdata, "table", name_value_list);
	if (mTable == "")
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}

	// parse opr
	mOpr = jimo_parser->getParmStr(rdata, "opr", name_value_list);
	if (mOpr == "")  mOpr = "add";

	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTable);
	if (!table_doc)
	{
		setErrMsg(rdata, eNotExist, mTable,mErrmsg);
		return false;
	}
	if(mIsService)
	{
		OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
		aos_assert_rr(schema_objid != "", rdata, false);
		AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
		aos_assert_rr(schema_doc, rdata, false);
		AosXmlTagPtr record_doc = schema_doc->getFirstChild();
		record_doc = record_doc->getFirstChild();
		mSchemaName = record_doc->getAttrStr("zky_name");
	}
	
	// parse indexes
	vector<OmnString> indexes_name;
	rslt = jimo_parser->getParmArrayStr(rdata, "indexes",name_value_list,indexes_name);
	if (rslt)
	{
		if (indexes_name.size()<=0)
		{
			setErrMsg(rdata, eMissingIndex, "", mErrmsg);
			// AosLogError(rdata, true, "missing_indexes") << enderr;
			return false;
		}

		// Felicia, 2015/08/05, for bug JIMODB-252
		// Check whether there are duplicated index names.
		OmnString idx_name = checkInexNames(indexes_name);
		if (idx_name != "")
		{
			setErrMsg(rdata, eDuplicatedIndexName, idx_name, mErrmsg);	
			return false;
		}

		for(u32 i=0; i<indexes_name.size(); i++)
		{
			rslt = configKeysType(rdata, table_doc, indexes_name[i]);
			if (!rslt)
				return false;
		}

		mIndexes = indexes_name;
	}
	mIndexes.clear();

	if (indexes_name.size()<=0)
	{
		rslt = configKeysType(rdata, table_doc, "");
		if(!rslt)
			return false;
	}
	

	//parse parser
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	aos_assert_rr(schema_objid != "", rdata, false);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);
	aos_assert_rr(schema_doc, rdata, false);
	mParser = schema_doc->getAttrStr("zky_name","");

	// parse inputs
	mInput = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	if(mInput == "")
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	//parse shuffle_type
	//mShuffleType = jimo_parser->getParmStr(rdata, "shuffle_type", name_value_list);

	//parse shuffle_field
	//mShuffleField = jimo_parser->getParmStr(rdata, "shuffle_field", name_value_list);

	// Parse conditions
	mCond = jimo_parser->getParmExpr(rdata,"conditions",name_value_list);

	//parse split character
	//mKeysep = jimo_parser->getParmStr(rdata, "key_sep", name_value_list);

	
	if (schema_doc)
	{
		AosXmlTagPtr tag = schema_doc->getFirstChild("datarecord", "");
		aos_assert_r(tag, false);
		mRecordName = tag->getAttrStr("zky_name", "");
		tag = tag->getFirstChild("datafields", "");
		aos_assert_r(tag, false);
		rslt = getDataFields(mFields, tag, rdata);
		aos_assert_r(rslt, false);
	}
	
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcIndex::getDataFields(
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
AosJimoLogicDataProcIndex::run(
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
		//for load data
		mInputName = mInput;
		//AosLogError(rdata, true, "failed_retrieving_output_name")
			//<< AosFN("Input Name") << mInput << enderr;
		//return false;
	}

	//mDocIndexName = mDataProcName;
	//mDocIndexName << "_doc";

	//rslt = createDocBatachoprDataproc(rdata, prog, statement_str);
	//aos_assert_rr(rslt, rdata, false);

	//rslt = addDocTask(rdata, prog);
	//aos_assert_rr(rslt, rdata, prog);

	for(size_t  i = 0; i < mKeysType.size(); i++)
	{
		mDataProcIndexName = mDataProcName;
		for(size_t j = 0; j < mKeysType[i].second.size(); j++)
		{
			mDataProcIndexName << "_" << mKeysType[i].second[j];
		}
		
		if (mIsService)
		{
			mIndexList.clear();
			mIndexList.push_back(mInput);
			rslt = createSelectDataproc(rdata, prog, statement_str, i);
			aos_assert_rr(rslt, rdata, false);
		}

		mDataProcIILName = mDataProcIndexName;
		mDataProcIILName << "_reduce" ;

		rslt = createIndexDataproc(rdata, prog, statement_str,mKeysType[i]);
		aos_assert_rr(rslt, rdata, false);

		rslt = createIILBatchoprDataproc(rdata, prog, mKeysType[i].first,mIILName[i],statement_str);
		aos_assert_rr(rslt, rdata, false);
		
		if (!mIsService)
		{
			rslt = addIndexTask(rdata, prog);
			aos_assert_rr(rslt, rdata, prog);

			rslt = addIILTask(rdata,prog);	
			aos_assert_rr(rslt, rdata, prog);
			
			OmnString output_name;
			output_name << mDataProcIndexName << "_output";
			mOutputNames.push_back(output_name);
		}
	}
	return true;
}

bool
AosJimoLogicDataProcIndex::configKeysType(
		AosRundata* rdata,
		AosXmlTagPtr &table_doc,
		const OmnString &name)
{
	OmnString iilName,index_type;
	vector<OmnString> keys;
	mHaveColumn = false;
	
	//columns
	AosXmlTagPtr columns_doc = table_doc->getFirstChild("columns");
	if (columns_doc)
	{
		AosXmlTagPtr column_doc = columns_doc->getFirstChild("column");
		if (column_doc)
		{
			mHaveColumn = true;
		}
	}

	AosXmlTagPtr indexes_doc = table_doc->getFirstChild("indexes");
	aos_assert_rr(indexes_doc, rdata, false);
	AosXmlTagPtr index_doc = indexes_doc->getFirstChild();
	if(!index_doc)
	{
		mErrmsg << " : no indexes need to create on table "
			<< mTable;
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	
	bool flag = false;
	while(index_doc)
	{
		keys.clear();
		OmnString type;
		
		// if specify indexes
		if (name != "")
		{
			OmnString index_name = index_doc->getAttrStr("index_name","");
			if(index_name != name)
			{
				index_doc = indexes_doc->getNextChild();
				continue;
			}	
		}
		
		flag = true;

		index_type = index_doc->getAttrStr("zky_type","");
		iilName = index_doc->getAttrStr("zky_iilname","");
		if(index_type == "cmp")
		{
			type = "BigStr";
			bool rslt = getKeys(rdata, iilName,table_doc, keys, name);
			aos_assert_rr(rslt, rdata, false);
		}
		else
		{
			OmnString key = index_doc->getAttrStr("zky_name","");
			if(key == "")
			{
				AosLogError(rdata, true,"miss_key") << enderr;
				return false;
			}
			keys.push_back(key);
			
			type = getTypeFromKey(rdata, table_doc,key);
			if(type == "")
			{
				mErrmsg << " : can not find key \"" << key << "\"!"	;
				rdata->setJqlMsg(mErrmsg);
				return false;
			}
		
		}

		// VERSIONTHIRDONE-432
		//int len = getLen(rdata, table_doc,keys);
		//mMaxLen.push_back(len);

		mIILName.push_back(iilName);
		pair<OmnString,vector<OmnString> > pair(type,keys);
		//pair<pair<OmnString,int> > pair(type_len,keys);
		mKeysType.push_back(pair);
		index_doc = indexes_doc->getNextChild();
	}
	
	//Gavin 2015/08/01 JIMODB-200
	if (!flag)
	{
		OmnString err_msg;
		err_msg << " [ERR] : can not find the index '" << name << "'!" ;
		rdata->setJqlMsg(err_msg);
		return false;
	}

	return true;
}

bool
AosJimoLogicDataProcIndex::getKeys(
		AosRundata *rdata, 
		OmnString &iil_name,
		AosXmlTagPtr &table_doc,
		vector<OmnString> &keys,
		const OmnString &name)
{
	AosXmlTagPtr cmpindexes_doc = table_doc->getFirstChild("cmp_indexes");
	aos_assert_rr(cmpindexes_doc, rdata, false);
	AosXmlTagPtr cmpindex_doc = cmpindexes_doc->getFirstChild();
	while(cmpindex_doc)
	{
		// 2015/07/12
		if (name != "")
		{
			OmnString cmp_index_name = cmpindex_doc->getAttrStr("cmp_index_name", "");
			if (name != cmp_index_name)
			{
				cmpindex_doc = cmpindexes_doc->getNextChild();
				continue;
			}
		}
		OmnString iilname = cmpindex_doc->getAttrStr("zky_iilname","");
		if(iilname == iil_name)
		{
			AosXmlTagPtr tag = cmpindex_doc->getFirstChild();
			while(tag)
			{
				keys.push_back(tag->getAttrStr("zky_name",""));
				tag = cmpindex_doc->getNextChild();
			}
			return true;
		}
		cmpindex_doc = cmpindexes_doc->getNextChild();
	}
	OmnShouldNeverComeHere;
	return false;
}


OmnString
AosJimoLogicDataProcIndex::getTypeFromKey(
		AosRundata *rdata,
		AosXmlTagPtr &table_doc,
		OmnString &key)
{
	AosRundataPtr rdataPtr(rdata);
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	if(schema_objid == "") return "";
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
	if(!schema_doc) return "";

	AosXmlTagPtr datarecord_doc = schema_doc->getFirstChild("datarecord");
	if(!datarecord_doc) return "";
	AosXmlTagPtr datafields_doc = datarecord_doc->getFirstChild("datafields");
	if(!datafields_doc) return "";
	AosXmlTagPtr datafield_doc = datafields_doc->getFirstChild();
	while(datafield_doc)
	{
		OmnString name = datafield_doc->getAttrStr("zky_name","");
		if(name == key)
		{
			// VERSIONTHIRDONE-432
			//int len = datafield_doc->getAttrStr("zky_length","").toU64();

			OmnString type = datafield_doc->getAttrStr("data_type","");
			if (type == "")
			{
				type = datafield_doc->getAttrStr("type","");
			}
			//xiafan 2015.07.27
			//JIMODB-55
			type = convertType(rdata, type);
			if (type != "")
				return type;
		}
		datafield_doc = datafields_doc->getNextChild();
	}
	return "";
}


// VERSIONTHIRDONE-432
/*
int
AosJimoLogicDataProcIndex::getLen(
		AosRundata *rdata,
		AosXmlTagPtr &table_doc,
		vector<OmnString> &key)
{
	int max_len;
	AosRundataPtr rdataPtr(rdata);
	OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
	if(schema_objid == "") return "";
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);;
	if(!schema_doc) return "";

	AosXmlTagPtr datarecord_doc = schema_doc->getFirstChild("datarecord");
	if(!datarecord_doc) return "";
	AosXmlTagPtr datafields_doc = datarecord_doc->getFirstChild("datafields");
	if(!datafields_doc) return "";
	AosXmlTagPtr datafield_doc = datafields_doc->getFirstChild();
	for (u32 i=0; i<keys.size(); i++)
	{
		while(datafield_doc)
		{
			OmnString name = datafield_doc->getAttrStr("zky_name","");
			if(name == key)
			{
				int len = datafield_doc->getAttrStr("zky_length","").toU64();
				max_len += len;
				break;
			}
			datafield_doc = datafields_doc->getNextChild();
		}
	}
	return max_len;
}
*/
bool
AosJimoLogicDataProcIndex::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcIndex*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcIndex::createIndexDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str,
		pair<OmnString,vector<OmnString> >  &keys_type)
{
	if (mIsService)
	{
		OmnString dataprocIndex_str;
		OmnString inName = mSelectName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataProcIndexName);

		mIndexList.push_back(mDataProcIndexName);
		dataprocIndex_str << "\"" << mDataProcIndexName << "\" : {"
						  << "\"type\":\"dataprocindex\""
						  << ",\"record_type\":\"buff\""
						  << ",\"docid\":\"docid\","
						  << "\"format\":" << "\"" << mSchemaName << "\"";
		if(mShuffleType !="")
			dataprocIndex_str << ",\"shuffle_type\":\"" << mShuffleType.data() << "\"";
		if(mShuffleField != "")
			dataprocIndex_str << ",\"shuffle_field\":\"" << mShuffleField.data() << "\"";
		if(mCond)
			dataprocIndex_str << ",\"condition\":\"" << mCond->getValue(rdata).data() << "\"";

		dataprocIndex_str << ",\"keys\":[";
		for(size_t i = 0;i < keys_type.second.size(); i++)
		{
			if (i < keys_type.second.size() -1)
				dataprocIndex_str << "\"" << keys_type.second[i].data() << "\",";
			else
				dataprocIndex_str << "\"" << keys_type.second[i].data() << "\"";
		}
		dataprocIndex_str << "]}";

		OmnScreen << dataprocIndex_str << endl;
		statement_str << "\n" << dataprocIndex_str;

		jimo_prog->setConf(dataprocIndex_str);
		return true;
	}

	OmnString dataprocIndex_str = "create dataprocindex ";
	dataprocIndex_str << mDataProcIndexName << " ";

	//arvin 2015.08.5
	//JIMODB-285
	JSONValue json;
	json["type"]= "index";
	json["record_type"]= "buff";
	if(mShuffleType !="")
		json["shuffle_type"]=mShuffleType.data();
	if(mShuffleField != "")
		json["shuffle_field"]=mShuffleField.data();
	if(mCond)
		json["condition"]=mCond->getValue(rdata).data();
	OmnString docid;
	docid << "getDocid(\'" << mParser << "\')";
	json["docid"] = docid.data();
	JSONValue keyJson;
	for(size_t i = 0;i < keys_type.second.size(); i++)
	{
		keyJson.append(JSONValue(keys_type.second[i].data()));
	}
	json["keys"]= keyJson;
	dataprocIndex_str << json.toStyledString();
	dataprocIndex_str << ";";

	OmnScreen << dataprocIndex_str << endl;
	statement_str << "\n" << dataprocIndex_str;
    
	bool rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicDataProcIndex::createSelectDataproc(
		AosRundata *rdata, 
		AosJimoProgObj *jimo_prog, 
		OmnString &statement_str,
		int idx)
{
	OmnString dataprocSelect_str = "";
	//OmnString dataprocName = "dpSelect_index_";
	//dataprocName << mInput << idx;
	OmnString dataprocName = "dpSelect_";
	dataprocName << mInput;
	mSelectName = dataprocName;
	if(jimo_prog->hasSelect(mSelectName))
	{
		return true;
	}
	
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
			OmnAlarm << mInput << " is not a jimo logic ! " << enderr;
		}
		
	}
	mIndexList.push_back(dataprocName);
	
	dataprocSelect_str << "\"" << dataprocName << "\"" << " : { "
					   << "\"type\":" << "\"" << "dataprocselect" << "\""
					   << ",\"assign_docid\": \"true\""
					   << ",\"format\": \"" << mRecordName << "\""
					   << ", \"docid\":" << "\"getDocid('" << mParser << "')\""
					   << ", \"record_type\":" << "\"buff\"";

	if(mCond)
	{
		dataprocSelect_str << ",\"condition\":" << "\"" << mCond->getValue(rdata).data() << "\"" ;
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
/*
	if (mDistinct.size() > 0)
	{
		dataprocSelect_str << ",\"distinct\":" << "[";
		if ( mFieldStr != "")
		{
			dataprocSelect_str << "\"all\"" << "]" << ",\"fields\":" << mFieldStr;
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
		dataprocSelect_str << ",\"fields\":" << mFieldStr;
		// dataprocSelect_str << ",\"fields\":" << "[";
		// for(u32 i=0; i< mFields.size(); i++)
		// {
		// 	    if(i>0) dataprocSelect_str << ",";
		// 		    dataprocSelect_str <<  "\"" << mFields[i]->dumpByNoQuote() << "\"" ;
		// }
	}
	*/
	dataprocSelect_str << "}";

	OmnScreen <<"qqqqqqqqqqqqqqqqqqq" <<  dataprocSelect_str << endl;
	statement_str << "\n" << dataprocSelect_str;
	jimo_prog->setConf(dataprocSelect_str);
	jimo_prog->setSelectMap(mSelectName);
	return true;
}

bool
AosJimoLogicDataProcIndex::createDocBatachoprDataproc(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &statement_str)
{
	OmnString doc_str = "create dataprocdocbatchopr ";
	doc_str << mDocIndexName << " {"
		<< "\"type\":\"docbatchopr\""
		<< ", \"opr\":\"batch_insert\"";
	if (!mHaveColumn)
	{
		doc_str << ", \"format\":\"" << mParser << "\""
			    << ", \"fields\":[";
		for (size_t i = 0; i < mFields.size(); i++)
		{
			if (i>0)
				doc_str << ", ";
			doc_str << "\"" << mFields[i] << "\"";
		}
		doc_str << "] ";
	}
	doc_str << ", \"docid\":" << "\"getDocid('" << mParser << "')\"};";

	OmnScreen << doc_str << endl;
	statement_str << "\n" << doc_str;
    
	bool rslt = parseRun(rdata,doc_str,prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}

bool
AosJimoLogicDataProcIndex::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &type,
		OmnString &iilname,
		OmnString &statement_str)
{
	OmnString dataprociil_str;
	if (!mIsService)
	{
		dataprociil_str << "create dataprociilbatchopr "
						<< mDataProcIILName << " { ";
	}
	else
	{
		OmnString inName = mDataProcIndexName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataProcIILName);

		dataprociil_str << "\"" << mDataProcIILName << "\" : {"
						<< "\"type\":\"dataprociilbatchopr\",";
		mIndexList.push_back(mDataProcIILName);
	}

	dataprociil_str	<< "\"iilname\":" << "\"" << iilname << "\"" << ","
					<< "\"record_type\":" << "\"buff\","
					<< "\"opr\":" << "\""<< mOpr << "\"" << ","
					//<< "\"record_type\":" << "\"fixbin\","
					//<< "\"opr\":" << "\""<< type <<"add\"" << ","
					<< "\"iil_type\":" << "\""<< type << "\"" << ","
			        << "\"build_bitmap\":" << "\"true\"}";
	if(!mIsService)
		dataprociil_str	<< ";";

	OmnScreen << dataprociil_str << endl;
	statement_str << "\n" << dataprociil_str;

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
AosJimoLogicDataProcIndex::addIndexTask(
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
AosJimoLogicDataProcIndex::addDocTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	OmnString add_doc_str;
	OmnString taskname;
	taskname << "task_index_" << mDocIndexName;
	add_doc_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDocIndexName << "\","
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


bool
AosJimoLogicDataProcIndex::addIILTask(
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
		<< "\"physicalid\":" << "\"" << AosGetIdealSvrIdByCubeId(AosGetCubeId(mDataProcIILName)) << "\"," 
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

OmnString
AosJimoLogicDataProcIndex::checkInexNames(const vector<OmnString> &index_names)
{
	OmnString idx_name = "";
	set<OmnString> idx_names_set;
	set<OmnString>::iterator itr;
	for(size_t i=0; i<index_names.size(); i++)
	{
		itr = idx_names_set.find(index_names[i]);
		if (itr != idx_names_set.end())
		{
			idx_name = index_names[i];
			return idx_name;
		}

		idx_names_set.insert(index_names[i]);
	}

	return idx_name;
}


AosJimoPtr 
AosJimoLogicDataProcIndex::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcIndex(*this);
}


OmnString 
AosJimoLogicDataProcIndex::getInput() const
{
	return mInput;
}

OmnString 
AosJimoLogicDataProcIndex::getTableName() const
{
	return  mTable;
}

bool 
AosJimoLogicDataProcIndex::isExist(const OmnString &name,const OmnString &parm)
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

