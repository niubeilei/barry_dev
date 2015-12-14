////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicNew.h"

#include "JimoAPI/JimoParserAPI.h"
#include "API/AosApi.h"
#include "SEInterfaces/JimoParserObj.h"

#include "Debug/ErrorMsg.h" 

AosJimoLogicNew::AosJimoLogicNew()
:
AosJimoLogicObjNew()
{
}


AosJimoLogicNew::AosJimoLogicNew(const int version)
:
AosJimoLogicObjNew()
{
	mJimoVersion = version;
	mJimoType = AosJimoType::eJimoLogic;
}

/*
AosJimoLogicNew::AosJimoLogicNew(const AosJimoLogicNew &rhs)
:
AosGenericObj(rhs)
{
	mJimoName = rhs.mJimoName;
	mJob = rhs.mJob;

	AosExprObjPtr expr = NULL;
	for (size_t i = 0; i < rhs.mNameValueList.size(); i++)
	{
		expr = rhs.mNameValueList[i]->cloneExpr();
		mNameValueList.push_back(expr);
	}
	if (rhs.mInput)
		mInput = rhs.mInput->cloneExpr();
	mOrderFields = rhs.mOrderFields;
	mGroupByFields = rhs.mGroupByFields;
	mOrderType = rhs.mOrderType;
	mOutputNames = rhs.mOutputNames;
	mDsOutputNames = rhs.mDsOutputNames;
	mDocConf = rhs.mDocConf;
	mSplitter = rhs.mSplitter;
	mCond = rhs.mCond;
	mCache = rhs.mCache;
	mSaveDoc = rhs.mSaveDoc;
}
*/

AosJimoLogicNew::~AosJimoLogicNew()
{
}


bool
AosJimoLogicNew::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	return true;
}


bool 
AosJimoLogicNew::configStr(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		const OmnString &attrname,
		const OmnString &dft,
		OmnString &results,
		vector<AosExprObjPtr> &name_value_list)
{
	results = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	if (results == "") results = dft;
	return true;
}


bool
AosJimoLogicNew::configInt(                                                            
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		const OmnString &attrname,
		const int64_t &dft,
		int64_t &results,
		vector<AosExprObjPtr> &name_value_list)
{
	OmnString value = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	if (value == "") results = dft;
	else
	{
		results = value.toI64(0);
	}

	return true;
}


bool
AosJimoLogicNew::configStr(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		const OmnString &attrname,
		OmnString &results,
		vector<AosExprObjPtr> &name_value_list, 
		const OmnString &errmsg)
{
	results = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	if (results == "")
	{
		AosLogError(rdata, true, errmsg) << enderr;
		return false;
	}

	return true;
}


bool
AosJimoLogicNew::configExpr(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		const OmnString &attrname,
		AosExprObjPtr &results,
		vector<AosExprObjPtr> &name_value_list, 
		const bool mandatory, 
		const OmnString &errmsg)
{
	results = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (!results)
	{
		if (!mandatory) return true;
		AosLogError(rdata, true, errmsg) << enderr;
		return false;
	}
	return true;
}


bool
AosJimoLogicNew::parseFields(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		const OmnString &attrname,
		vector<Field> &fields,
		vector<AosExprObjPtr> &name_value_list)
{
	// Parse the fields
	bool rslt;
	fields.clear();

	vector<AosExprObjPtr> field_defs;
	rslt = jimo_parser->getParmArray(rdata, "fields", name_value_list, field_defs);
	if (!rslt) 
	{
		AosLogError(rdata, true, "failed_retrieve_value")
			<< AosFN("AttributeName") << attrname << enderr;
		return false;
	}

	u32 size = field_defs.size();
	for (u32 i=0; i<size; i++)
	{
		Field field;
		if (!parseField(rdata, field, field_defs[i])) return false;
		fields.push_back(field);
	}

	return true;
}


bool
AosJimoLogicNew::setOutputName(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosJimoLogicNew::setUnionDatasetName(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosJimoLogicNew::parseField(
		AosRundata *rdata, 
		Field &field,
		const AosExprObjPtr &field_def)
{
	// The field definition should be in the following format:
	// 	(name: <name>, type: <type>, max_len: <ddd>)
	if (field_def->getType() != AosExprType::eBrackets)
	{
		AosLogError(rdata, true, "invalid_field_def")
			<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
		return false;
	}

	vector<AosExprObjPtr> exprs = field_def->getExprList();
	if (exprs.size() <= 0)
	{
		AosLogError(rdata, true, "invalid_field_def")
			<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
		return false;
	}

	for (u32 i=0; i<exprs.size(); i++)
	{
		if (exprs[i]->getType() != AosExprType::eNameValue)
		{
			AosLogError(rdata, true, "invalid_field_def")
				<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
			return false;
		}

		OmnString name = exprs[i]->getName().toLower();
		AosExprObjPtr value = exprs[i]->getValueAsExpr();
		if (name == "" || !value)
		{
			AosLogError(rdata, true, "invalid_name_value_pair")
				<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
			return false;
		}

		if (name == "field_name" || name == "fname")
		{
			field.field_name = value->getValue(rdata);
			if (field.field_name == "")
			{
				AosLogError(rdata, true, "field_name_empty")
					<< AosFN("FieldDef") << field_def->dumpByNoEscape() << enderr;
				return false;
			}
		}
		else if (name == "type")
		{
			field.field_type = value->getValue(rdata);
			if (field.field_type == "") field.field_type = "str";
		}
		else if (name == "max_len" || name == "max_length")
		{
			field.max_len = atoi(value->getValue(rdata).data());
		}
		else if (name == "offset")
		{
			field.offset = atoi(value->getValue(rdata).data());
		}
	}

	OmnString errmsg;
	if (!field.isValid(errmsg))
	{
		AosLogError(rdata, true, errmsg) << enderr;
		return false;
	}

	return true;
}


// Pay 2015-11-23 JIMODB-1236
// Add errmsg to configCommonAttrs()
bool
AosJimoLogicNew::configCommonAttrs(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		OmnString &errmsg)
{
	if (!configSplitter(rdata, jimo_parser, name_value_list,errmsg)) return false;
	if (!configConditions(rdata, jimo_parser, name_value_list,errmsg)) return false;
	if (!configCache(rdata, jimo_parser, name_value_list,errmsg)) return false;
	if (!configOrderByGroupBy(rdata, jimo_parser, name_value_list,errmsg)) return false;

	return true;
}


bool
AosJimoLogicNew::configConditions(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		OmnString &errmsg)
{
	// parse condition
    OmnString attrname = "conditions";
    AosExprObjPtr cond= jimo_parser->getParmExpr(rdata, attrname, name_value_list);
    if (cond)
    {
        mCond = cond->dumpByNoQuote();
	}
	return true;
}


// parse cache
bool
AosJimoLogicNew::configCache(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		OmnString &errmsg)
{
	AosExprObjPtr cache;
	OmnString attrname = "cache";
	cache = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (cache)
	{
		mCache = cache->dumpByNoQuote();
	}

	return true;
}


//parse save doc
bool
AosJimoLogicNew::configSaveDoc(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list)
{
	AosExprObjPtr save = jimo_parser->getParmExpr(rdata, "save_doc", name_value_list);
	if (save)
	{
		mSaveDoc = save->dumpByNoQuote();
	}
	return true;
}


bool
AosJimoLogicNew::createJimoLogicDoc(
		AosRundata *rdata,
		const OmnString &name,
		const OmnString &jql)
{
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, name);   
	if (doc)
	{
		AosLogError(rdata, true, "doc_already_exists")
			<< AosFN("Doc Name") << name << enderr;
		return true;
	}

	OmnString str= "";
	str	<< "<jimologicdataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
		<< " >"
		<<      "<![CDATA[" << jql << "]]>"
		<< "</jimologicdataproc>";

	doc = AosXmlParser::parse(str AosMemoryCheckerArgs);  
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;  
		OmnAlarm << __func__ << enderr;                                    
		return false;                                                      
	}

	if(!createDoc(rdata, JQLTypes::eJimoLogicDoc, name, doc))
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;  
		OmnAlarm << __func__ << enderr;                                    
		return false;                                                      
	}

	return true;
}


bool
AosJimoLogicNew::configOrderType(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		const vector<OmnString> &fields)
{
	vector<AosExprObjPtr> field_names;	
	bool rslt = jimo_parser->getParmArray(rdata, "order_by_type", name_value_list, field_names);
	if (rslt)
	{
		for (u32 i=0; i<field_names.size(); i++)
		{
			OmnString fname = field_names[i]->dumpByNoQuote();
			mOrderType.push_back(fname);
		}
		return true;
	}

	AosExprObjPtr expr = jimo_parser->getParmExpr(rdata, "order_by_type", name_value_list);
	if (expr)
	{
		OmnString type = expr->getValue(rdata);
		if (type == "")
		{
			AosLogError(rdata, true, "order_by_type_invalid") << enderr;
			return false;
		}

		if (fields.size() == 1)
		{
			mOrderType.push_back(type);
		}
	}
	return true;
}


bool
AosJimoLogicNew::configOrderByGroupBy(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		OmnString &errmsg)
{
	// This function parses the "order_by|group_by" and "order_by_type" attributes:
	// 	(
	// 		order_by|group_by: <fieldname>,
	// 		order_by|group_by: [<fieldname>, ...],
	// 		order_by_type: <fieldname>
	// 		order_by_type: [<fieldname>, ...]
	vector<AosExprObjPtr> field_names;	
	bool rslt = jimo_parser->getParmArrayStr(rdata, "group_by", name_value_list, mGroupByFields);

	rslt = jimo_parser->getParmArrayStr(rdata, "order_by", name_value_list, mOrderFields);
	if (rslt)
	{
		vector<OmnString>::iterator itr;	
		if(mGroupByFields.size())
		{
			for(size_t i = 0; i < mOrderFields.size(); i++)
			{
				OmnString fname = mOrderFields[i];
				itr = find(mGroupByFields.begin(),mGroupByFields.end(),fname);
				if(itr == mGroupByFields.end())
				{
					OmnString msg = "";
					msg << errmsg << "The key of order by must be in the key of group by" << fname;
					rdata->setJqlMsg(msg);
					return false;
				}
			}
		}
		return configOrderType(rdata, jimo_parser, name_value_list, mOrderFields);
	}
	return true;
}


bool
AosJimoLogicNew::configSplitter(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		vector<AosExprObjPtr> &name_value_list,
		OmnString &errmsg)
{
	// This function parses the splitter attribute:
	// 	splitter : 
	// 	(
	// 		type: <type>,
	// 		cluster: xxx,
	// 		distribution: xxx,
	// 		timer: xxx,
	// 		min_size: xxx,
	// 		max_size: xxx,
	// 		...				// Addition attributes depending on the type.
	// 	),
	//
	// If 'flag' is true, the splitter is mandatory. Otherwise, it is optional.
	//
	// Retrieve the splitter
	vector<AosExprObjPtr> splitter;
	OmnString attrname  = "splitter";
	bool rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, splitter);
	if (rslt)
	{
		if(splitter.size() <= 0)
		{
			AosLogError(rdata, true, "missing_splitter") << enderr;
			return false;
		}
	}
	else return true;

	// parse splitter type
	attrname = "type"; 
	mSplitter.mType = jimo_parser->getParmStr(rdata, attrname, splitter);
	if (mSplitter.mType == "")
	{
		AosLogError(rdata, true, "missing_key_fields") << enderr;
		return false;
	}

	if (mSplitter.mType == "distribution_map")
	{
		//parse distribution_map
		attrname = "distribution_map"; 
		mSplitter.mDistributionMapName = jimo_parser->getParmStr(rdata, attrname, splitter);
		if (mSplitter.mDistributionMapName == "")
		{
	    	AosLogError(rdata, true, "missing_distribution_map_name") << enderr;
			return false;
		}

		// parse field_values
		attrname = "field_values";
		AosExprObjPtr expr = jimo_parser->getParmExpr(rdata, attrname, splitter);
		if (!expr)
		{
			AosLogError(rdata, true, "missing_field_values") << enderr;
			return false;
		}

		mSplitter.mSplitField = expr->dumpByNoQuote();
	}
	else
	{
		AosLogError(rdata, true, "type error") << enderr;
		return false;
	}

	// parse distribution(server/cube/..)
	attrname = "distribution"; 
	AosExprObjPtr locality = jimo_parser->getParmExpr(rdata, attrname, splitter);
	if (!locality)
	{
		mSplitter.mShuffleType = "none";
	}
	else
	{
		mSplitter.mShuffleType = locality->dumpByNoQuote();
	}

	// timer,
	// min_size,
	// max_size
	return true;
}


bool
AosJimoLogicNew::getDatasetOutput(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		const OmnString &input_name,
		vector<OmnString> &outputNames)
{
	/*
	OmnString statements_str = "";
	aos_assert_rr(prog, rdata, false);
	bool rslt = procInput(rdata, input_name, prog,outputNames, statements_str); 
	aos_assert_rr(rslt, rdata, false);
	*/
	
	AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, input_name);
	if (!jimo_logic)
	{
		//AosLogError(rdata, true, "incorrect_input_name")
		//	<< AosFN("Input Name") << input_name << enderr;
		//return false;
		outputNames.push_back(input_name);
		return true;
	}
	
	outputNames = jimo_logic->getOutputNames();

	/*
	jimo_logic->getOutputName(rdata, prog, outputName);
	if (outputName == "")
	{
		AosLogError(rdata, true, "failed_retrieving_output_name")
			<< AosFN("Input Name") << input_name << enderr;
			return false;
	}
	*/


	return true;
}


bool
AosJimoLogicNew::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);

	OmnString stmt_str;
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, stmt_str, false);
	}
	return true;
}


vector<OmnString> 
AosJimoLogicNew::getOutputNames()
{
	return mOutputNames;
}
	

bool
AosJimoLogicNew::procInput(
			AosRundata *rdata,
			const OmnString &input,
			AosJimoProgObj *prog,
			OmnString &input_name,
			OmnString &statements_str)
{
	// The 'input' format can be one of the following:
	// 	<input_name>
	// 	<input_name>.<output_name>, 
	// 	<input_name>.<output_name>.cache
	//
	// This function converts 'input' into 'input_name' as needed. 
	// The conversion rules are:
	// 1. If 'input' refers to a data proc that is not defined in 
	//    the current job, 'input' should refer to a data proc
	//    that is saved in the database. It loads the data proc
	//    into memory. If 'input' is in the form:
	//    		<input_name>.<output_name>.cache
	//    It should load the data proc identified by <input_name>
	//    and ask the data proc to get the IIL dataset and replace
	//    'input' with <dataset_name>. 

	vector<OmnString> inputs, outputs;
	input_name = "";
	const char *sep = ".";
	int num = AosSplitStr(input, sep, inputs, 4);
	aos_assert_rg(num <= 3, rdata, false, 
			AosErrMsg("invalid_input").field("Input", input));
	aos_assert_rg(num >= 1, rdata, false, AosErrMsg("internal_error"));

	AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, inputs[0]);
	if (!jimo_logic)
	{
		// The data proc/dataset is not in the current job. This means
		// it refers to a data proc or dataset that was created in a 
		// previous job. We need to read the data proc into memory. 
		AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, inputs[0]);
		if (!doc)
		{
		//for load data
			doc = getDoc(rdata, JQLTypes::eDatasetDoc, inputs[0]);
			if (!doc)
			{
				//AosLogError(rdata, true, "incorrect_input_name")
				//	<< AosFN("Input Name") << inputs[0] << enderr;
				//Gavin 2015/08/01 JIMODB-200
				OmnString errmsg;
				errmsg << " [ERR] : can not find the inputs '" << inputs[0] << "'!" ; 
				rdata->setJqlMsg(errmsg);
				return false;
			}
			else 
			{
				input_name = inputs[0];
				return true;
			}
		}

		OmnString jql = doc->getNodeText();
		jql << ";";
		bool rslt = addJimoLogicToProg(rdata, jql, prog);
		if (!rslt) return false;

		jimo_logic = prog->getJimoLogic(rdata, inputs[0]);
		if (!jimo_logic)
		{
			AosLogError(rdata, false, "failed_get_input")
				<< AosFN("Input Name") << inputs[0] << enderr;
			return false;
		}
	}

	AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
	if (num == 1)
	{
		if (type == AosJimoLogicType::eDataset)
		{
			input_name = inputs[0];
		}
		else
		{
			outputs = jimo_logic->getOutputNames();
			if (outputs.size() > 0)
				input_name = outputs[0];
		}
	}
	else if (num == 2)
	{
		if (type >= AosJimoLogicType::eDataProc && type <= AosJimoLogicType::eDataProcIndex)
		{
			//AosLogError(rdata, true, "incorrect_input_name_should_be_dataproc")
				//<< AosFN("Input Name") << inputs[0] << enderr;
			//return false;
			OmnString msg = "";
			msg << "[ERR]: " << inputs[0] << " doesn't allowed the following format " << "." << inputs[1];
			rdata->setJqlMsg(msg);
			return false;
		}

		outputs = jimo_logic->getOutputNames();
		if (inputs[1] == "output")
		{
			input_name = outputs[0];
		}
		else
		{
			size_t i = 0;
			for (; i < outputs.size(); i++)
			{
				if (inputs[1] == outputs[i])
					break;
			}
			if (i>=outputs.size())
			{
				AosLogError(rdata, true, "incorrect_input_name_")
					<< AosFN("Input Name") << inputs[1] << enderr;
				return false;
			}
			input_name = outputs[i];
		}
	}
	else
	{
		if (inputs[2] != "cache")
		{
			AosLogError(rdata, true, "incorrect_input_name_")
				<< AosFN("Input Name") << input << enderr;
			return false;
		}
		//1.create dataset iil
		//2.return dataset iil name
		OmnString jobname = prog->getJobname();
		input_name = "iilset_";
		input_name << inputs[0] << "_" << jobname;
		
		AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, input_name);
		if (!doc)
		{
			bool rslt = createDatasetIIL(rdata, input_name, jimo_logic, prog, statements_str);
			aos_assert_r(rslt, false);
		}
		return true;
	}

	return true;
}


bool
AosJimoLogicNew::procOutput(AosRundata *rdata,
			const vector<OmnString> &outputs,
			AosJimoProgObj *prog,
			OmnString &output_name,
			OmnString &iilname)
{
	aos_assert_r(outputs.size() == 2, false);
	aos_assert_rg(outputs[1] == "output", rdata, false, 
			AosErrMsg("invalid_output").field("Output", outputs[1]));
	AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, outputs[0]);
	if (!jimo_logic)
	{
		// The data proc/dataset is not in the current job. This means
		// it refers to a data proc or dataset that was created in a 
		// previous job. We need to read the data proc into memory. 
		AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, outputs[0]);
		if (!doc)
		{
			AosLogError(rdata, true, "incorrect_output_name")
				<< AosFN("Output Name") << outputs[0] << enderr;
			return false;
		}

		OmnString jql = doc->getNodeText();
		jql << ";";
		bool rslt = addJimoLogicToProg(rdata, jql, prog);
		if (!rslt) return false;

		jimo_logic = prog->getJimoLogic(rdata, outputs[0]);
		if (!jimo_logic)
		{
			AosLogError(rdata, false, "failed_get_output")
				<< AosFN("Output Name") << outputs[0] << enderr;
			return false;
		}
	}

	jimo_logic->getOutputName(rdata, prog, output_name);
	iilname = jimo_logic->getIILName();

	return true;
}

bool
AosJimoLogicNew::createDatasetIIL(
		AosRundata *rdata,
		const OmnString &input_name,
		AosJimoLogicObjNew *logic,
		AosJimoProgObj *prog,
		OmnString &statements_str)
{
	OmnString iilname = logic->getIILName();
	vector<OmnString> fnams = logic->getSubFields();
	OmnString str = "dataset iil ";
	str << input_name << " ( "
		<< "type: \"iil\", "
		<< "iilname: \"" << iilname << "\", "
		<< "conditions: true, "
		<< "schema: ( type : \"iil\", "
		<< "fields: [ ( type: \"str\", "
		<< "name: \"" << iilname << "_new\", "
		<< "max_length: 100, "
		<< "sub_fields: [ ";
	for (size_t i = 0; i < fnams.size(); i++)
	{
		if (i>0)
			str << ", ";
		str << "\"" << fnams[i] << "\"";
	}
	str << "] ), ( type:\"str\", "
		<< "name:\"docid\", "
		<< "max_length:8 ) ] ), "
		<< "splitter: ( type:\"cube\", block_size:10000 ) );";

	OmnScreen << str << endl;
	statements_str << "\n" << str;

    bool rslt = parseRun(rdata, str, prog);
    aos_assert_rr(rslt,rdata,false);

	rslt = createJimoLogicDoc(rdata, input_name, str);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicNew::getFields(vector<AosExprObjPtr> &)
{
	return false;
}


bool
AosJimoLogicNew::getDistinct(vector<AosExprObjPtr> &)
{
	return false;
}


bool 
AosJimoLogicNew::setFields(
		AosRundata* rdata, 
		AosJimoProgObj* prog, 
		const OmnString& fields_str)
{
	return false;
}


bool 
AosJimoLogicNew::getOutputsFromDataProc(
		AosRundata* rdata,
		AosJimoProgObj *jimo_prog,
		const vector<OmnString> &procNames,
		vector<OmnString> &outputs)
{
	aos_assert_rr(procNames.size()>0, rdata, false);
	for(size_t i = 0; i < procNames.size();i++)
	{
		AosJimoLogicObjNew* logic = jimo_prog->getJimoLogic( rdata, procNames[i]);
		if(!logic)
		{
		//	AosLogError(rdata, true, "please_checkout_data_proc_name is Exist");
			return false;
		}
		vector<OmnString> output_names = logic->getOutputNames();
		outputs.insert(outputs.end(),output_names.begin(),output_names.end());
	}
	return true;
}

bool 
AosJimoLogicNew::getInputV(vector<OmnString> &inputs)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosJimoLogicNew::getInputList(vector<OmnString> &inputs)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosJimoLogicNew::getSchemaName(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		vector<OmnString> &schema_name)
{
	vector<OmnString> inputs;
	bool rslt = getInputV(inputs);
	aos_assert_rr(rslt,  rdata, false);
	for(size_t i = 0; i < inputs.size(); i++)
	{
		AosJimoLogicObjNew *logic = prog->getJimoLogic(rdata, inputs[i]);
		if(!logic)
		{
			AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, inputs[i]);
			if (doc)
			{
				OmnString jql = doc->getNodeText();
				jql << ";";
				bool rslt = addJimoLogicToProg(rdata, jql, prog);
				aos_assert_rr(rslt,  rdata, false);

				logic = prog->getJimoLogic(rdata, inputs[i]);
				if (logic)
					logic->getSchemaName(rdata, prog,schema_name);
				else
				{
					AosLogError(rdata, true, "miss_jimo_logic");
					return false;
				}
			}
			else
			{
				doc = getDoc(rdata, JQLTypes::eDatasetDoc, inputs[i]);
				if (doc)
				{
					doc = doc->getFirstChild("dataschema");
					if (doc)
					{
						//get schema
						OmnString name = doc->getAttrStr(AOSTAG_NAME, "");
						doc = getDoc(rdata, JQLTypes::eSchemaDoc, name);
						if (doc)
						{
							//get schema name
							doc = doc->getFirstChild("datarecord");
							if (doc)
							{
								name = doc->getAttrStr(AOSTAG_NAME, "");
								schema_name.push_back(name);
								return true;
							}
						}
					}
				}
				//AosLogError(rdata, true, "miss_jimo_logic") << inputs[i] << enderr;
				OmnString msg = "could not find inputs: ";
				msg << "\"" << inputs[i] << "\"";
				rdata->setJqlMsg(msg);
				return false;
			}
		}
		logic->getSchemaName(rdata, prog,schema_name);
	}
	return true;	
}

bool
AosJimoLogicNew::addJimoLogicToProg(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);

	OmnString stmt_str;
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->compileJQL(rdata, jimo_prog);
		//statements[i]->run(rdata, 0, stmt_str);
	}
	return true;
}

bool 
AosJimoLogicNew::setErrMsg(
		AosRundata *rdata,
		const ErrorType status, 
		const OmnString &parm, 
		OmnString &msg)
{
	OmnString errmsg = "[ERR] : ";
	errmsg << msg;
	OmnString err = rdata->getJqlMsg();

	switch (status)
	{
	case eGenericError:
		 if(err != "") errmsg << " : " << err;
		 break;
	case eMissingParm:
		errmsg << " : \"" << parm << "\",miss parameter!";
		break;
	case eInvalidParm:	
		errmsg << " : \"" << parm << "\",invalid parameter!";
		break;
	case eNotExist:
		errmsg << " : \"" << parm << "\",doesn't exist!";
		break;
	case eSyntaxError:
		errmsg << " : \"" << parm << "\",syntax error!";
		break;
	case eFilePathNotExist:
		errmsg << " : filepath \"" << parm << "\" doesn't exist!";
		break;
	case eUnknowParm:	
		errmsg << " : \"" << parm << "\",unknown parameter!";
		break;
	case eMissingIndex:
		errmsg << " : \"" << parm << "\",missing index!";
		break;
	case eDuplicatedIndexName:
		errmsg << " : \"" << parm << "\" is duplicated index name!";
		break;
	case eIILegalName:
		errmsg << " :\"" << parm << "\" is a iilegal name!";
	default:
		break;
	}
	rdata->setJqlMsg(errmsg);
	aos_assert_r(0,false);
	return true;
}

bool 
AosJimoLogicNew::checkNameValueList(AosRundata *rdata,OmnString &errmsg,vector<AosExprObjPtr> &list)
{
	if(list.size() <= 0)return true;
	AosExprObjPtr expr = list[0];
	setErrMsg(rdata,eUnknowParm,expr->getName(),mErrmsg);
	return false;
}

bool 
AosJimoLogicNew::setUserVarValue(AosRundata *rdata, const AosValueRslt &value)
{
	OmnNotImplementedYet;		
	return true;
}


AosValueRslt 
AosJimoLogicNew::getUserVarValue(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return AosValueRslt();
}


bool 
AosJimoLogicNew::isLegalName(
		AosRundata *rdata, 
		const OmnString &name,
		//AosJimoProgObj *prog,
		OmnString &errmsg)
{
	// This function checks whether 'name' is valid. 'name' is valid if and only if:
	// 1. It is not empty
	// 2. It is not used by other data procs
	OmnString msg = rdata->getJqlMsg();
	if(msg != "") 
	{
		OmnString err = "[ERR] : ";
		err << errmsg  << name << " : " << name << ", " <<msg;
		rdata->setJqlMsg(err);
		return false;
	}

	if (name == "")
	{
		OmnString err = "[ERR] : ";
		err << "Object Name is empty";
		rdata->setJqlMsg(err);
		return false;
	}

	/*
	if (!prog->addObjectName(rdata, name))
	{
		OmnString err = "[ERR] : ";
		err << "Object Name is already used: " << name;
		rdata->setJqlMsg(err);
		return false;
	}
	*/

	return true;
}


bool 
AosJimoLogicNew::setSubFields(
						AosRundata *rdata, 
						AosJimoProgObj* prog, 
						const vector<OmnString> &subfields)
{
	return false;
}

OmnString 
AosJimoLogicNew::convertType(AosRundata *rdata, OmnString &type)
{
	if (type == "number" || type == "double")
	{
		type = "BigD64";
	}

	// jimodb-869
	if (type == "str" || type == "string")
	{
		type = "BigStr";
	}

	if (type == "u64")
	{
		type = "BigU64";
	}

	if (type == "int64" || type == "datetime")
	{
		type = "BigI64";
	}

	if (type != "BigStr" && type != "BigU64" && type != "BigD64" && type != "BigI64")
	{
		AosLogError(rdata, true, "invalid_type") << enderr;
		return "";
	}
	
	return type;

}

OmnString 
AosJimoLogicNew::getInput() const
{
	OmnShouldNeverComeHere;
	return "";
}

OmnString 
AosJimoLogicNew::getTableName() const
{
	OmnShouldNeverComeHere;
	return  "";
}

bool 
AosJimoLogicNew::isExist(const OmnString &name,const OmnString &parm)
{
	OmnShouldNeverComeHere;
	return false;
}

// jimodb-753
bool AosJimoLogicNew::getFieldStr(vector<OmnString> &field_str)
{
	return false;
}

// jimodb-753
bool
AosJimoLogicNew::getDataFieldsDocs(
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


bool
AosJimoLogicNew::configFields(
		AosRundata *rdata,
		const vector<AosXmlTagPtr> &datafields_docs,
		vector<OmnString> &field_strs)
{
	JSONValue fieldsJV;
	JSONValue fieldJV;
	for(size_t j = 0; j < datafields_docs.size(); j++)
	{
		AosXmlTagPtr data_field_doc = datafields_docs[j]->getFirstChild();
		while(data_field_doc)
		{
			OmnString fname = data_field_doc->getAttrStr("zky_name","");
			if (fname == "")
			{
				AosLogError(rdata, true, "miss_field_type") << enderr;
				return false;
			}
			
			OmnString ftype,flength;
			ftype = data_field_doc->getAttrStr("type","");
			if(ftype == "")
			{
				AosLogError(rdata, true, "miss_field_type") << enderr;
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
				else if(ftype == "i64") ftype = "i64";
				else if(ftype == "double") ftype = "double";
				else 
				{
					OmnAlarm << "error" << enderr;
					return false;
				}
			}
			
			fieldJV["fname"] = fname.data();
			fieldJV["type"] = ftype.data();

			fieldsJV.append(fieldJV);
			data_field_doc = datafields_docs[j]->getNextChild();
		}
	}
	field_strs.push_back(fieldsJV.toStyledString());
	//mFieldsJV = fieldJV;
	return true;
}

// jimodb-753 , 2015/9/18
// get schema from input dataset
bool 
AosJimoLogicNew::getInputSchema(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		vector<OmnString> &field_strs)
{
	JSONReader read;
	JSONValue fieldsJV;
	JSONValue fieldJV;
	
	vector<OmnString> inputs;
	bool rslt = getInputV(inputs);
	//JSONValue fieldsJV;
	aos_assert_rr(rslt,  rdata, false);
	for(size_t i = 0; i < inputs.size(); i++)
	{
		AosJimoLogicObjNew *logic = prog->getJimoLogic(rdata, inputs[i]);
		if(!logic)
		{
			AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, inputs[i]);
			if (doc)
			{
				OmnString jql = doc->getNodeText();
				jql << ";";
				bool rslt = addJimoLogicToProg(rdata, jql, prog);
				aos_assert_rr(rslt,  rdata, false);

				logic = prog->getJimoLogic(rdata, inputs[0]);
				if (logic)
				{
					//OmnString tmp;
					u32 count = field_strs.size();
					logic->getFieldStr(field_strs);
					if (field_strs.size() == count)
					{
						return logic->getInputSchema(rdata, prog, field_strs);
					}
					continue;
				}
			}
			else
			{
				getFieldsConf(rdata, prog, field_strs);
				continue;
			}
		}
		if (logic)
		{

			if (logic->getJimoLogicType() == AosJimoLogicType::eDataset)
			{
				getFieldsConf(rdata, prog, field_strs);
				continue;
			}
			else
			{
				u32 count = field_strs.size();
				logic->getFieldStr(field_strs);
				if (field_strs.size() == count)
				{
					return logic->getInputSchema(rdata, prog, field_strs);
				}
				continue;
			}
		}
	}
	return true;
}

bool
AosJimoLogicNew::getFieldsConf(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		vector<OmnString> &field_strs)
{
	vector<OmnString> schema_names;
	bool rslt = getSchemaName(rdata, prog, schema_names);
	aos_assert_rr(rslt, rdata, false);

	vector<AosXmlTagPtr> datafields_docs;
	rslt = getDataFieldsDocs(rdata, schema_names, datafields_docs);
	aos_assert_rr(rslt, rdata, false);

	rslt = configFields(rdata, datafields_docs, field_strs);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool 
AosJimoLogicNew::semanticsCheck(
		AosRundata *rdata,
		AosJimoProgObj *prog)
{
	return true;
}
