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
// 2015/05/19 Worked on by Rain
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDatasetIIL.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLExpr/ExprNameValue.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SEUtil/JqlTypes.h" 
#include "JQLStatement/JqlStatement.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDatasetIIL_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDatasetIIL(version);
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


AosJimoLogicDatasetIIL::AosJimoLogicDatasetIIL(const int version)
:
AosJimoLogicDataset(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDatasetIIL::~AosJimoLogicDatasetIIL()
{
}


bool 
AosJimoLogicDatasetIIL::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//Dataset IIL <dataset_name>
	//	(
	//	     iilname: "_zt4k_db_unicom_stat_unicom_stat_model_internal_0__0",
	//		 splitter: (<splitter_spec>),
	//		 conditions: true,
	//		 schema: <schema_name> | (<schema_def>)
	//	);
    
	//we can get the schema whit table; and if table name is not Null, wo
	//would use table-schema and ignore the schema attribute field
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataset", rdata, false);
	aos_assert_rr(keywords[1] == "iil", rdata, false);
	mErrmsg = "dataset iil ";

	// parse dataset name
	OmnString dataset_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataset_name,mErrmsg))
	{
		// This is not what it expects.
		return true;
	}
	mErrmsg << dataset_name;
	

	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDatasetName = dataset_name;
	mNameValueList = name_value_list;

	//parse iilname
	OmnString attrname = "iilname";
	OmnString iilname = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	if (iilname == "")
	{
	//	AosLogError(rdata, true, "missing_iilname") << enderr;
		mErrmsg << " : miss parameter \"iilname\"!";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	mIILName = iilname;

	//parse splitter
	attrname = "splitter";
	vector<AosExprObjPtr> values;
	rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, values);
	if (!rslt)
	{
	//	AosLogError(rdata, true, "missing_splitter") << enderr;
				
		return false;
	}
	else
	{
		if (values.size() <= 0)
		{
			AosLogError(rdata, true, "missing_splitter") << enderr;
			return false;
		}
		mSplitter = values; 
	}

	//parse conditions 
	attrname = "conditions";
	OmnString conditions = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	mCondition = conditions;

	// parse schema 
	attrname = "schema";
	AosExprObjPtr schema_expr = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (!schema_expr)
	{
		AosLogError(rdata, true, "missing_schema") << enderr;
		return false;
	}

	// If schema_expr type is String. If it is an Bracket, it is defined here. Otherwise, 
	// it is an error. 
	if (schema_expr->getType() == AosExprType::eString)
	{
		mSchemaName = schema_expr->dumpByNoQuote(); 
		//get schema by schema name	
		rslt = getSchemaBySchemaName(rdata, mSchemaName);	
		aos_assert_r(rslt, false);
	}
	else if(schema_expr->getType() == AosExprType::eBrackets)
	{
		//user define 
		mDefineSchema = schema_expr;
		//parse schema to the vector
		vector<AosExprObjPtr> normalFields;
		AosExprObjPtr fields;
		normalFields = schema_expr->getExprList();
		OmnString fieldName;
		for(size_t i=0; i < normalFields.size(); i++)
		{
			fieldName = normalFields[i]->getName();
			if(fieldName == "fields")
			{
				fields = normalFields[i];
				normalFields.erase(normalFields.begin() + i);
			}
		}
		mNormalSchemaField = normalFields;

		//get fields expr
		vector<AosExprObjPtr> fieldFields= fields->getExprList();
		vector<AosExprObjPtr> nameValues;
		for(size_t i=0; i < fieldFields.size(); i++)
		{
			nameValues = fieldFields[i]->getExprList();
			mSchemaFields.push_back(nameValues);
			nameValues.clear();
		}
	}
	else
	{
		AosLogError(rdata, true, "invalid schema") << enderr;
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicDatasetIIL::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString& statements_str,
		bool isparser) 
{

	bool rslt = createIILDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDatasetIIL::compileJQL(AosRundata *rdata, AosJimoProgObj* prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDatasetIIL*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDatasetName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	//mDataprocSelectName << "_dsdp_" << mDatasetName;
	//mOutputName << mDataprocSelectName << "_output";
	//setOutputNames(rdata, prog, mOutputName);
	return true;
}


bool
AosJimoLogicDatasetIIL::createIILDataproc(
		AosRundata *rdata,
		AosJimoProgObj* jimo_prog,
		OmnString &statements_str)
{
	OmnString dataProcIIL_str = "create datasetiil ";
	dataProcIIL_str << mDatasetName << " { "
		<< "\"type\":" << "\"" << "iil" << "\"";

	//iilname
	if (mIILName == "") return false;
	dataProcIIL_str << ",\"iilname\":" << "\"" << mIILName << "\"";

	//conditions
	if(mCondition != "") 
	{
		dataProcIIL_str << ",\"conditions\": " << mCondition;
	}
	else
	{
		dataProcIIL_str << ",\"conditions\": " << "true";
	}

	//generate the split field
	if(mSplitter.size() <0) return false;
	dataProcIIL_str << ",\"split\":" << "{";
	for(size_t i=0; i < mSplitter.size(); i++)
	{
		if(mSplitter[i]->getName() == "block_size")
		{
			dataProcIIL_str << "\"block_size\":" << mSplitter[i]->dumpByNoQuote();
			continue;
		}
		dataProcIIL_str <<  "\"" << mSplitter[i]->getName() <<"\": " << "\"" << mSplitter[i]->dumpByNoQuote()<< "\"";
		if(i < mSplitter.size() - 1) dataProcIIL_str << ",";  
	}
	dataProcIIL_str << "}";

	//generate the schema field
	if(	mSchemaName == "")
	{
		//if(mNormalSchemaField.size() < 0) return false;
		dataProcIIL_str << ",\"schema\": {";
		//for(size_t i = 0; i < mNormalSchemaField.size(); i++)
		//{
		//	dataProcIIL_str << "\"" << mNormalSchemaField[i]->getName() << "\": " << "\""
		//		<< mNormalSchemaField[i]->dumpByNoQuote() << "\"";
		//	if(i < mNormalSchemaField.size() - 1) dataProcIIL_str << ",";
		//}
		dataProcIIL_str << "\"type\": " << "\"iil\"";
		if(mSchemaFields.size() < 0) return false;
		dataProcIIL_str << ",\"fields\": [";
		for(size_t i = 0; i < mSchemaFields.size(); i++)
		{
			dataProcIIL_str << "{";
			for(size_t j = 0; j < mSchemaFields[i].size(); j++)
			{
				if(mSchemaFields[i][j]->getName() == "max_length")
				{
					dataProcIIL_str << "\"max_length\": " << mSchemaFields[i][j]->dumpByNoQuote();
					if(j < mSchemaFields[i].size() -1) dataProcIIL_str << ",";
					continue;
				}
				AosExprNameValue* subField = dynamic_cast<AosExprNameValue*>(mSchemaFields[i][j].getPtr());
				AosExprObjPtr subValue = subField->getValue();
				if(subValue->getType() == AosExprType::eBrackets)
				{
					vector<AosExprObjPtr> values = mSchemaFields[i][j]->getExprList();
					dataProcIIL_str << "\"" << mSchemaFields[i][j]->getName() << "\": [";
					for(size_t k = 0; k < values.size(); k++)
					{
						dataProcIIL_str << "\"" << values[k]->dumpByNoQuote() << "\"";
						if(k < values.size() - 1) dataProcIIL_str << ",";
					}
					dataProcIIL_str << "]";
				}
				else
				{
					dataProcIIL_str << "\"" << mSchemaFields[i][j]->getName() << "\": " << "\""
						<< mSchemaFields[i][j]->dumpByNoQuote() << "\"";
				}
				if(j < mSchemaFields[i].size() -1) dataProcIIL_str << ",";
			}
			dataProcIIL_str << "}";
			if(i < mSchemaFields.size() - 1) dataProcIIL_str << ",";
		}
		dataProcIIL_str << "]}";
	}
	else
	{
		dataProcIIL_str << mStrSchema;	
	}
	
	
	dataProcIIL_str << "};";
	
	OmnScreen << "RainQiu" << dataProcIIL_str << endl;
	statements_str << "\n" << dataProcIIL_str;    

    bool rslt = parseRun(rdata, dataProcIIL_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}

/*
bool AosJimoLogicDatasetIIL::setOutputName(
		    AosRundata *rdata,
			const AosJimoProgObjPtr &prog,
			const OmnString &name) 
{
	mOutputName = name; 
	return true;
}
*/
/*
bool AosJimoLogicDatasetIIL::setUnionDatasetName( 
			AosRundata *rdata, 
			const AosJimoProgObjPtr &prog,
			const OmnString &name)
{
	mUnionDatasetName = name;
	return true;
}
*/
AosJimoPtr 
AosJimoLogicDatasetIIL::cloneJimo() const
{
	return OmnNew AosJimoLogicDatasetIIL(*this);
}


bool 
AosJimoLogicDatasetIIL::getSchemaBySchemaName(
				AosRundata*		rdata, 
				const OmnString &name)
{
	OmnString schema;
	OmnString type;
	AosXmlTagPtr dataRecordDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, name);
	//AosXmlTagPtr dataRecordDoc = dataSchemaDoc->getFirstChild(true);
	if(!dataRecordDoc) return false;
	//OmnString type = dataRecordDoc->getAttrStr("type", "");
	//if(type == "") return false;
	schema << ",\"schema\": " << "{";
	schema << "\"type\": " << "\"" << "iil" << "\"";

	//OmnString record_delimiter = dataRecordDoc->getAttrStr("zky_row_delimiter", "");
	//if(record_delimiter == "") return false;
	//schema << ",\"record_delimiter\": " << "\"" << record_delimiter << "\"";

	//OmnString field_delimiter = dataRecordDoc->getAttrStr("zky_field_delimiter", "");
	//if(field_delimiter == "") return false;
	//schema << ",\"field_delimiter\": " << "\"" << field_delimiter << "\"";

	//OmnString text_qualifier = dataRecordDoc->getAttrStr("zky_text_qualifier", "");
	//if(text_qualifier == "") return false;
	//schema << ",\"text_qualifier\": " << "\"" << text_qualifier << "\"";
	
	//get Fields
	AosXmlTagPtr dataFieldsDoc = dataRecordDoc->getNextChild("datafields");
	//get datafield
	AosXmlTagPtr dataFieldDoc = dataFieldsDoc->getNextChild();
	if(!dataFieldDoc) return false;
	schema << ",\"fields\": [";
	
	while(dataFieldDoc)
	{
		schema << "{";
		OmnString fieldType = dataFieldDoc->getAttrStr("type", "");
		if(fieldType == "") return false;
		schema << "\"type\": " << "\"" << fieldType << "\"";
		
		OmnString fieldName = dataFieldDoc->getAttrStr("zky_name", "");
		if(fieldName == "") return false;
		schema << ",\"name\": " << "\"" << fieldName << "\"";
		
		OmnString maxLength = dataFieldDoc->getAttrStr("zky_length", "");
		if(maxLength == "") return false;
		schema << ",\"max_length\": " << maxLength ;
		
		dataFieldDoc = dataFieldsDoc->getNextChild();
		schema << "}";
		//if(!dataFieldDoc) break; 
		schema << ",";
	}
	//append docid
	schema << "{\"type\": \"bin_u64\", \"name\": \"docid\"}";

	schema << "]}";
	mStrSchema = schema;
	return true;
}
