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
#include "JimoLogicNew/JimoLogicDataProcUnion.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcUnion_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcUnion(version);
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


AosJimoLogicDataProcUnion::AosJimoLogicDataProcUnion(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcUnion::~AosJimoLogicDataProcUnion()
{
}


bool
AosJimoLogicDataProcUnion::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataset union datasetname
	// 	(
	// 		inputs: [<dataset_name>, <dataset_name> ...],
	// 		cache: true | false
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "union", rdata, false);
	mErrmsg = "dataproc union "; 

	// set service dft
	if(dft)
		mIsService = dft;
	
	// parse dataset name
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
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = dp_name;
	mNameValueList = name_value_list;

	// parse input
	OmnString attrname = "inputs";
	vector<OmnString> inputs ;
	rslt = jimo_parser->getParmArrayStr(rdata, attrname, name_value_list, inputs);
    if (!rslt)
    {
        mErrmsg << "[ERR] : The Union "<< mDataProcName << " missing \"inputs\"!";
        setErrMsg(rdata, eGenericError, "inputs", mErrmsg);
    }
	else if (inputs.size() < 2)
	{
		mErrmsg << "[ERR] : The Union "<< mDataProcName << " should have at least two inputs!";
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}
	mInputs = inputs;

	rslt = setInputNames();
	aos_assert_r(rslt, false);

	// parse cache
	AosExprObjPtr cache;
	attrname = "cache";
	cache = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (cache)
	{
		mCache = cache->dumpByNoQuote();
	}

	mOutputName = getOutputName();

	//set mIILName
	if(mSplitter.mType != "distribution_map")
	{
		mIILName << "_zt4i_" << mDataProcName << "_cache";
	}
	else
	{
		mIILName << "_zt4i_" << mDataProcName << "_cache_$group";
	}

	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcUnion::setInputNames()
{
	for (size_t i = 0; i < mInputs.size(); i++)
	{
		vector<OmnString> input_names;
		const char *sep = ".";
		int num = AosSplitStr(mInputs[i], sep, input_names, 3);
		aos_assert_r(num >= 1 && num <= 3, false);
		mInputNames.push_back(input_names[0]);
	}
	return true;
}


bool
AosJimoLogicDataProcUnion::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Find all the datasets from 'prog'. If not found, it is an error.
	// 2. Set the output name to the one defined in this dataset.
	// 3. Set the cache flag to all the unioned datasets.
	aos_assert_rr(prog, rdata, false);
	bool rslt;

	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcUnion*>(this);
	bool rslt_t = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt_t, rdata, false);

	// set outputname to map
	mOutputNames.push_back(mOutputName);
	mDsOutputNames[mDataProcName] = mOutputName;

	//if wo want to make union , wo should  get all Fields infomation by inputs
	//and retrive the original schema to fill it
	vector<AosExprObjPtr> fields;
	//vector<AosJimoLogicObjNew> jimologics;
	vector<JSONValue> fieldsJV;
	JSONValue fieldJV;
	JSONReader reader;
	vector<OmnString> field_strs;
	//OmnString field_str;

	mFieldConf.clear();
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
        // get inputs' jimo logic
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		if (!jimo_logic)
		{
			AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, mInputNames[i]);
			if (doc) continue;
			AosLogError(rdata, true, "unioned_dataset_not_found")
				<< AosFN("Dataset Name") << mInputNames[i] << enderr;
			return false;
		}

		if (jimo_logic)
		{
			AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
			if (type == AosJimoLogicType::eDataset)
			{
				OmnString ErrMgs ;
				ErrMgs << "[ERR] : input " << "\"" << mInputNames[i] << "\" "
								 << "should not be a dataset !";
				rdata->setJqlMsg(ErrMgs);
				return false;
			}
		}

        // set outputname
		jimo_logic->setOutputName(rdata, prog, mOutputName);
		OmnString name = mDsOutputNames[mInputNames[i]];

        // get inputs' fields
		// jimodb-753
		jimo_logic->getFieldStr(field_strs); 
		reader.parse(field_strs[i], fieldJV);
		//jimo_logic->getFieldStr(field_str);
		//reader.parse(field_str, fieldJV);

 		if (fieldJV.size()<=0)
		{
			OmnString ErrMsg;
			ErrMsg << "[ERR] : input " << "\"" << mInputNames[i] << "\" "
							 << "failed get select fields !";
			rdata->setJqlMsg(ErrMsg);
			return false;
		}
        if (i>0)
        {
            if(fieldJV.size() != fieldsJV[i-1].size())
            {
                mErrmsg << "[ERR] : the union input " << mInputNames[i] << " has " << fieldJV.size() << " field, "
                        << "which is different from the first unioned input: " << fieldsJV[i-1].size() << "!";
                setErrMsg(rdata,eGenericError,"inputs",mErrmsg);

                return false;

            }
        }
		fieldsJV.push_back(fieldJV);

	}

    // get union schema doc, if it does not exist,
    // config union schema and create doc
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, mDataProcName);
	if (doc)
	{
		mFieldConf.clear();
		bool rslt = getSchemaFromDoc(rdata, doc);
		if (!rslt) return false;
	}
	else
	{
		//bool rslt = configFields(rdata, prog, fieldsJV);
		bool rslt = getUnionFieldConf(rdata,prog,fieldsJV);
		aos_assert_r(rslt,false);

		rslt = config(rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	configFields(rdata,fieldsJV);

	for (u32 i = 0; i<mInputNames.size(), i<fieldsJV.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		if (!jimo_logic)
		{
			AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, mInputNames[i]);
			if (doc) continue;
			AosLogError(rdata, true, "unioned_dataset_not_found")
				<< AosFN("Dataset Name") << mInputNames[i] << enderr;
			return false;
		}

		//jimo_logic->setOutputName(rdata, prog, mOutputName);
		vector<OmnString> subfields;

		OmnString field_str = fieldsJV[i].toStyledString();
		jimo_logic->setFields(rdata, prog, field_str);

		mFieldStr.push_back(field_str);
		for(u32 j = 0; j<fieldsJV[i].size();j++)
		{
			if (fieldsJV[i][j]["alias"].asString() != "")
				subfields.push_back(fieldsJV[i][j]["alias"].asString());
			else
				subfields.push_back(fieldsJV[i][j]["fname"].asString());
		}
		jimo_logic->setSubFields(rdata, prog, subfields);
	}
	return true;
}


bool
AosJimoLogicDataProcUnion::getSchemaFromDoc(AosRundata *rdata, AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);

	AosXmlTagPtr schemas = doc->getFirstChild("union_schema");
	if (!schemas) return true;

	if (schemas)
	{
		AosXmlTagPtr schema = schemas->getFirstChild("schema");
		while(schema)
		{
			FieldConf conf;
			//conf.fname = schema->getAttrStr("fname");
			conf.type = schema->getAttrStr("type");
			conf.max_length = schema->getAttrStr("max_length");
			mFieldConf.push_back(conf);
			schema = schemas->getNextChild();
		}
	}
	return true;
}

bool
AosJimoLogicDataProcUnion::run(
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
		//add union to prog
		AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcUnion*>(this);
		prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
		//mUnionList.clear();

		OmnString tabName;
		AosJimoLogicObjNew *select = prog->getJimoLogic(rdata, mInputs[0]);
		tabName = select->getTableName();
		aos_assert_rr(rslt, rdata, false);
		
		AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdata,JQLTypes::eTableDoc, tabName);
		if (!table_doc)
		{
			//setErrMsg(rdata,3,stmt->mTableName, mErrmsg);
			return false;
		}
		OmnString db = table_doc->getAttrStr("zky_database","");
		mIILName = "";	
		mIILName << "_zt44_" << db << "_" << tabName
			<< "_" << mDataProcName;
			//<< "_" << mIndexKey;

		OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
		aos_assert_rr(schema_objid != "", rdata, false);
		AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdata);;
		aos_assert_rr(schema_doc, rdata, false);
		AosXmlTagPtr record_doc = schema_doc->getFirstChild();
		record_doc = record_doc->getFirstChild();
		mSchemaName = record_doc->getAttrStr("zky_name");
		
		vector<AosExprObjPtr> fields;
		//vector<AosJimoLogicObjNew> jimologics;
		vector<JSONValue> fieldsJV;
		JSONValue fieldJV;
		JSONReader reader;
		// jimodb-753
		//OmnString field_str;
		vector<OmnString> field_strs;

		mFieldConf.clear();
		for (u32 i = 0; i<mInputNames.size(); i++)
		{
			// get inputs' jimo logic
			AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
			if (!jimo_logic)
			{
				AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, mInputNames[i]);
				if (doc) continue;
				AosLogError(rdata, true, "unioned_dataset_not_found")
					<< AosFN("Dataset Name") << mInputNames[i] << enderr;
				return false;
			}

			if (jimo_logic)
			{
				AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
				if (type == AosJimoLogicType::eDataset)
				{
					OmnString ErrMgs ;
					ErrMgs << "[ERR] : input " << "\"" << mInputNames[i] << "\" "
						<< "should not be a dataset !";
					rdata->setJqlMsg(ErrMgs);
					return false;
				}
			}

			// get inputs' fields
			// jimodb-753
			//jimo_logic->getFieldStr(field_str);
			//reader.parse(field_str, fieldJV);
			jimo_logic->getFieldStr(field_strs); 
			reader.parse(field_strs[0], fieldJV);

			if (fieldJV.size()<=0)
			{
				OmnString ErrMsg;
				ErrMsg << "[ERR] : input " << "\"" << mInputNames[i] << "\" "
					<< "failed get select fields !";
				rdata->setJqlMsg(ErrMsg);
				return false;
			}
			if (i>0)
			{
				if(fieldJV.size() != fieldsJV[i-1].size())
				{
					mErrmsg << "[ERR] : the union input " << mInputNames[i] << " has " << fieldJV.size() << " field, "
						<< "which is different from the first unioned input: " << fieldsJV[i-1].size() << "!";
					setErrMsg(rdata,eGenericError,"inputs",mErrmsg);

					return false;

				}
			}
			fieldsJV.push_back(fieldJV);

		}

		// get union schema doc, if it does not exist,
		// config union schema and create doc
		bool rslt = getUnionFieldConf(rdata,prog,fieldsJV);
		aos_assert_r(rslt,false);
	}
	//proc inputs
	//bool rslt = processInput(rdata, prog, statements_str);
	//aos_assert_rr(rslt, rdata, false);
	
	if( mCache == "true")
	{
		bool rslt = createIndexDataproc(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);

		rslt = createIILBatchoprDataproc(rdata, prog, statements_str);
		aos_assert_rr(rslt, rdata, false);

		if (!mIsService)
		{
			//task for index
			rslt = addIndexTask(rdata, prog);
			aos_assert_rr(rslt, rdata, false);
		
			//task for iil
			rslt = addIILTask(rdata, prog);
			aos_assert_rr(rslt, rdata, false);
		}
	}
	return true;
}


//bool
//AosJimoLogicDataProcUnion::processInput(
//		AosRundata *rdata,
//		AosJimoProgObj *prog,
//		OmnString &statements_str)
//{
//	OmnString inputName = "";
//	for (u32 i = 0; i < mInputs.size(); i++)
//	{
//		bool rslt = AosJimoLogicNew::procInput(rdata, mInputs[i], prog, inputName, statements_str);
//		aos_assert_r(rslt, false);
//
//		if (inputName == "")
//		{
//			AosLogError(rdata, true, "failed_retrieving_output_name")
//				<< AosFN("Input Name") << mInputs[i] << enderr;
//			return false;
//		}
//		mRealInputNames.push_back(inputName);
//	}
//
//	if (mRealInputNames.size() != 2)
//	{
//		AosLogError(rdata, true, "joins_allows_two_table_joins_only") << enderr;
//		return false;
//	}
//	return true;
//}


bool
AosJimoLogicDataProcUnion::parseRun(
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
AosJimoLogicDataProcUnion::createIndexDataproc(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog,
		OmnString &statements_str)
{
	if (mIsService)
	{
		OmnString dataprocIndex_str;
		OmnString dataprocName;
		OmnString mapname_str;
		mDataprocIndexName << "dpIndex_" << mInputs[0];
		dataprocIndex_str << "\"" << mDataprocIndexName << "\" : { "
			<< "\"docid\":\"docid\","
			<< "\"format\":\"" << mSchemaName << "\","
			<< "\"type\":\"dataprocindex\",";
		OmnString inName;
		for(size_t i=0; i<mInputs.size(); i++)
		{
			inName = mInputs[i];
			inName << ".output";
			jimo_prog->insertToDataFlowMap(inName, mDataprocIndexName);
			
			jimo_prog->insertToNameMap(mDataProcName,inName);
		}

		dataprocIndex_str << "\"record_type\":" << "\"buff\"" << ","
			<< "\"keys\":[";

		OmnString fname = "";
		for(u32 i=0; i< mFieldConf.size(); i++)
		{
			if(mFieldConf[i].alias != "")
				fname = mFieldConf[i].alias;
			else
				fname = mFieldConf[i].fname;
			if(i>0)
				dataprocIndex_str << ",";
			dataprocIndex_str <<  "\"" << fname << "\"" ;
		}
		dataprocIndex_str << "]}";

		OmnScreen << dataprocIndex_str << endl;
		statements_str << "\n" << dataprocIndex_str;

		jimo_prog->setConf(dataprocIndex_str);
		return true;
	}
	
	OmnString dataprocIndex_str = "create dataprocindex ";
	JSONValue indexJV;
	JSONValue keysJV;
	mDataprocIndexName << "_dsdp_" << mDataProcName << "_cache_map";
	dataprocIndex_str << mDataprocIndexName << " ";
	indexJV["type"] = "index";
    for(u32 i=0; i< mFieldConf.size(); i++)
    {
		OmnString name = mFieldConf[i].fname;
		//reader.parse(field,json);
		//OmnString fname = json["fname"].asString();
		keysJV.append(name.data());
    }
	indexJV["keys"] = keysJV;
	indexJV["record_type"] = "buff";
	indexJV["docid"] = "1";

	dataprocIndex_str << indexJV.toStyledString() << ";";

	OmnScreen << dataprocIndex_str << endl;
	statements_str << "\n" << dataprocIndex_str;

	if(mCache == "true")
	{
		bool rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}


bool
AosJimoLogicDataProcUnion::createIILBatchoprDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	if (mIsService)
	{
		OmnString dataprociil_str;
		OmnString dataprocIILName;

		mDataprocIILName << "dpIILBatch_" << mInputs[0];
		dataprociil_str << "\"" << mDataprocIILName << "\" : { "
			<< "\"iil_type\" : \"BigStr\"," 
			<< "\"type\" :" << "\"dataprociilbatchopr\",";
		OmnString inName = mDataprocIndexName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mDataprocIILName);

		dataprociil_str	<< "\"iilname\":" << "\"" << mIILName << "\"" << ","
			<< "\"opr\":" << "\"add\"";
		if(mSplitter.mShuffleType != "")
		{
			dataprociil_str << ",\"shuffle_type\":" << "\"" << mSplitter.mShuffleType << "\"";
		}
		dataprociil_str << "}";

		OmnScreen << dataprociil_str << endl;
		statements_str << "\n" << dataprociil_str;

		jimo_prog->setConf(dataprociil_str);
		return true;
	}

	OmnString dataprociil_str = "create dataprociilbatchopr ";
	OmnString dataprocIILName;
	mDataprocIILName << "_dsdp_" << mDataProcName << "_cache_reducer";

	dataprociil_str << mDataprocIILName << " ";
	JSONValue iilJV;
	iilJV["iilname"] = mIILName.data();
	iilJV["opr"] = "add";
	iilJV["iil_type"] = "BigStr";
	if(mSplitter.mShuffleType != "")
	{
		iilJV["shuffle_type"] = mSplitter.mShuffleType.data();
	}
	dataprociil_str << iilJV.toStyledString() << ";";
	OmnScreen << dataprociil_str << endl;
	statements_str << "\n" << dataprociil_str;

	if(	mCache == "true")
	{
		bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}


	bool
	AosJimoLogicDataProcUnion::addIndexTask(
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
	AosJimoLogicDataProcUnion::addIILTask(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog)
	{
		//aos_assert_rr(jimo_prog, rdata, false);
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


AosJimoPtr
AosJimoLogicDataProcUnion::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcUnion(*this);
}


bool
AosJimoLogicDataProcUnion::setOutputName(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		const OmnString &name)
{
	if (mOutputName == name) return true;

	mOutputName = name;
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		aos_assert_rr(jimo_logic, rdata, false);
		jimo_logic->setOutputName(rdata, prog, mOutputName);
	}

	return true;
}


bool
AosJimoLogicDataProcUnion::setCache(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		const OmnString &cache)
{
	if (mCache == cache) return true;

	mCache = cache;
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		aos_assert_rr(jimo_logic, rdata, false);
		jimo_logic->setCache(rdata, prog, cache);
	}

	return true;
}


bool
AosJimoLogicDataProcUnion::setOrderFields(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		const vector<OmnString> &orderfields)
{
	if (orderfields == mOrderFields) return true;

	mOrderFields = orderfields;
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		aos_assert_rr(jimo_logic, rdata, false);
		jimo_logic->setOrderFields(rdata, prog, orderfields);
	}

	return true;
}

bool
AosJimoLogicDataProcUnion::setOrderType(
		AosRundata* rdata,
		AosJimoProgObj *prog,
		const vector<OmnString>& ordertype)
{
	if (ordertype == mOrderType) return true;

	mOrderType = ordertype;
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		aos_assert_rr(jimo_logic, rdata, false);
		jimo_logic->setOrderType(rdata, prog, ordertype);
	}

	return true;
}


bool
AosJimoLogicDataProcUnion::setFields(
		AosRundata* rdata,
		AosJimoProgObj *prog,
		const OmnString &fields_str)
{
	for (u32 i = 0; i<mInputNames.size(); i++)
	{
		AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mInputNames[i]);
		aos_assert_rr(jimo_logic, rdata, false);
		jimo_logic->setFields(rdata, prog,fields_str);
	}

	return true;
}

/*
bool
AosJimoLogicDataProcUnion::getFields(vector<AosExprObjPtr> &fields)
{
	fields = mFields;
	return true;
}
*/

bool
AosJimoLogicDataProcUnion::getInputV(vector<OmnString> &inputs)
{
	inputs = mInputs;
	return true;
}

bool 
AosJimoLogicDataProcUnion::getInputList(vector<OmnString> &inputs)
{
	inputs = mInputs;
	return true;
}


bool
AosJimoLogicDataProcUnion::getDataFieldsDocs(
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

// union schema
bool
AosJimoLogicDataProcUnion::config(AosRundata *rdata)
{
	/*
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eJimoLogicDoc, mDataProcName);
	if (!doc)
	{
		AosLogError(rdata, true, "doc_dose_not_exist")
			<< AosFN("Doc Name") << mDataProcName  << enderr;
		return false;
	}
	*/
	OmnString union_schema_str = "<union_schema></union_schema>";
	AosXmlTagPtr union_schema = AosXmlParser::parse(union_schema_str AosMemoryCheckerArgs);
	for (u32 i=0; i<mFieldConf.size(); i++)
	{
		OmnString schemaConf = "<schema ";

		schemaConf << "fname =" << "\"" << mFieldConf[i].fname << "\" "
				   << "type =" << "\"" << mFieldConf[i].type << "\" "
				   << "/>";
		AosXmlTagPtr schema = AosXmlParser::parse(schemaConf AosMemoryCheckerArgs);
		union_schema->addNode(schema);
	}

	mDocConf.push_back(union_schema);

	return true;
}

/*
bool
AosJimoLogicDataProcUnion::configFields(
				AosRundata *rdata,
				AosJimoProgObj *prog,
				vector<JSONValue> &fieldsJV)
{
	bool rslt;
	//vector<JSONValue> fieldsJV;
	//vector<pair<string,string> > infos;

	rslt = getUnionFieldConf(rdata, prog, fieldsJV);
	if (!rslt)
	{
		return false;
	}

	rslt = configSelectFields(rdata,fieldsJV);
	if (!rslt)
	{
		return false;
	}
	return true;

}
*/

bool
AosJimoLogicDataProcUnion::getUnionFieldConf(
				AosRundata *rdata,
				AosJimoProgObj *prog,
				vector<JSONValue> fieldsJV)
{

	vector<OmnString> types;
	vector<OmnString> names;
	bool rslt = getFieldsInfo(rdata,fieldsJV,types,names);
	if (!rslt)
	{
		return false;
	}
	for (u32 i=0; i<types.size(); i++)
	{
		FieldConf conf;
		conf.type = types[i];
		conf.fname = names[i];
        //conf.alias = names[i].second;
		mFieldConf.push_back(conf);
	}
	return true;
}

// check inputs' fields: name,type
bool
AosJimoLogicDataProcUnion::getFieldsInfo(
			AosRundata *rdata,
			vector<JSONValue> fieldsJV,
			vector<OmnString> &types,
			vector<OmnString> &names)
{
	OmnString type;
	OmnString name;
    //OmnString alias;

	for(u32 i=0; i<fieldsJV.size(); i++)
	{
		for (u32 j=0; j<fieldsJV[i].size();j++)
		{
			type = fieldsJV[i][j]["type"].asString();
			aos_assert_rr(type != "", rdata,false);


			if(i==0)
			{
				types.push_back(type);
                name = fieldsJV[i][j]["alias"].asString();
                if (name == "")
				    name = fieldsJV[i][j]["fname"].asString();
                //alias = fieldsJV[i][j]["alias"].asString();
				aos_assert_rr(name!= "",rdata,false);
				//names.push_back(make_pair(name,alias));
                names.push_back(name);
			}

			else
			{
				if(type != types[j]&& type != "")
				{
					//type = compareType(type,infos[k].first);
				    OmnString errMsg = "The data type for " ;
                    errMsg << mInputNames[i] << "." << fieldsJV[i][j]["fname"].asString()
                           << " is " << type << ",which is incompatible with the data type \""
                           << types[j] << "\" of the field " << mInputNames[0] << "." << names[j] << "!";

					setErrMsg(rdata, eGenericError,"" ,errMsg);
					return false;
				}
			}
		}
	}
	return true;
}


// set inputs' config
bool
AosJimoLogicDataProcUnion::configFields(
			AosRundata *rdata,
			vector<JSONValue> &fieldsJV)
{
	for (u32 j=0; j<fieldsJV.size();j++)
	{
		for (u32 i=0; i<mFieldConf.size(); i++)
		{
			fieldsJV[j][i]["type"] = mFieldConf[i].type.data();

			if(j!=0)
			{
                OmnString alias = fieldsJV[j][i]["alias"].asString();
                OmnString fname = fieldsJV[j][i]["fname"].asString();
                //if (fieldsJV[j][i]["alias"].asString() == ""
                //    && fieldsJV[j][i]["fname"].asString()!= string(mFieldConf[i].fname))
                if (alias != mFieldConf[i].fname && fname!= string(mFieldConf[i].fname))
                {
                    fieldsJV[j][i]["alias"] = mFieldConf[i].fname.data();
                }
			}
		}
	}
	return true;
}

OmnString
AosJimoLogicDataProcUnion::getLogicName() const
{
	const OmnString name = "union";
	return name;
}
// jimodb-753
bool
AosJimoLogicDataProcUnion::getFieldStr(vector<OmnString> &field_strs)
{
	field_strs = mFieldStr;
	return true;
}
