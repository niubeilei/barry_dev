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
//
// Modification History:
// 2015/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicSchemaCSV.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicSchemaCSV_1(
			const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicSchemaCSV(version);
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


AosJimoLogicSchemaCSV::AosJimoLogicSchemaCSV(const int version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicSchemaCSV::~AosJimoLogicSchemaCSV()
{
}


bool 
AosJimoLogicSchemaCSV::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	create schema fixlen <schema_name>
	// 	(
	// 		charset: 			UTF8,
	// 		record_delimiter: 	CRLF,
	// 		field_delimiter: 	",",
	// 		text_quote: 		"DQM",
	// 		fields: 
	// 		[
	// 			(name: <name>, type: <type>, max_len: <ddd>),
	// 			(name: <name>, type: <type>, max_len: <ddd>),
	// 			...
	// 			(name: <name>, type: <type>, max_len: <ddd>),
	// 		]
	// 	);

	parsed = false;
	aos_assert_rr(mKeywords.size() >= 3, rdata, false);
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "schema", rdata, false);
	aos_assert_rr(mKeywords[2] == "csv", rdata, false);
	mErrmsg = "create schema csv ";
	// parse dataset name
	OmnString schema_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,schema_name,mErrmsg))
	{
		return false;
	}
	mErrmsg << schema_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_values;
	bool rslt = jimo_parser->getNameValueList(rdata, name_values);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mSchemaName = schema_name;
	mNameValueList = name_values;
	
	//get Fields
	rslt = parseFields(rdata, jimo_parser, "fields", mFieldDefs, name_values);
	if(!rslt)
	{
		setErrMsg(rdata,eMissingParm,"fields",mErrmsg);
		return false;
	}

	/*
		if(fieldValues.size() <= 0)
		{
			AosLogError(rdata, true, "there is no fields for this schema") << enderr;
			return false;
		}
		vector<AosExprObjPtr> childFieldValues;
		for(size_t i = 0; i < fieldValues.size(); i++)
		{
			childFieldValues = fieldValues[i]->getExprList();       
			stmt->mSchemaFields.push_back(childFieldValues);
			childFieldValues.clear();
		}
	}
	}
	*/
/*
	//parse  record_delimiter
	attrname = " record_delimiter";
	OmnString recordDelimiter = jimo_parser->getParmStr(rdata, attrname, name_values);
	stmt->mRecordDelimiter = recordDelimiter;

	//parse  field_delimiter
	attrname = "field_delimiter";
	OmnString fieldDelimiter = jimo_parser->getParmStr(rdata, attrname, name_values);
	stmt->mFieldDelimiter = fieldDelimiter;
*/
	
	// Parse Charset 
	if (!configStr(rdata, jimo_parser, "charset", "UTF8", mCharset, name_values)) return false;
	if (!configStr(rdata, jimo_parser, "record_delimiter", "CRLF", mRecordDelimiter, name_values)) return false;
	if (!configStr(rdata, jimo_parser, "field_delimiter", ",", mFieldDelimiter, name_values)) return false;
	if (!configStr(rdata, jimo_parser, "text_quote", "DQM", mTextQuote, name_values)) return false;
	if(!checkNameValueList(rdata, mErrmsg, name_values))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicSchemaCSV::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	// This function creates the schema. If the schema already exists, 
	// it is an error. 
	
	AosXmlTagPtr schemaDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, mSchemaName);
	aos_assert_r(schemaDoc.getPtr() == NULL, false);

	bool rslt = createSchemaDataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);
	
	return true;
}


AosJimoPtr 
AosJimoLogicSchemaCSV::cloneJimo() const
{
	return OmnNew AosJimoLogicSchemaCSV(*this);
}


bool
AosJimoLogicSchemaCSV::createSchemaDataproc(
		AosRundata *rdata,
		AosJimoProgObj* jimo_prog,
		OmnString &statements_str)
{
	OmnString fieldName;
	OmnString dataProcSchema_str = "create schema ";
	dataProcSchema_str << mSchemaName << " source type CSV";

	//generate fields
	dataProcSchema_str << " fields(";
	
	for(size_t i = 0; i < mFieldDefs.size(); i++)
	{
		dataProcSchema_str << "\"" << mFieldDefs[i].field_name << "\"";
		dataProcSchema_str << " type " << mFieldDefs[i].field_type;
		dataProcSchema_str << " max length " << mFieldDefs[i].max_len;
		if(i < mFieldDefs.size() -1) dataProcSchema_str << ",";
	}
	dataProcSchema_str << ")";
	
	//generate record delimiter
	dataProcSchema_str << " schema delimiter " << "\"" <<  mRecordDelimiter << "\"";
	dataProcSchema_str << " field delimiter " << "\"" <<  mFieldDelimiter << "\"";
	dataProcSchema_str << " text qualifier " << "\"" <<  mTextQuote << "\"";
 
	dataProcSchema_str << ";";
	
	OmnScreen << "RainQiu" << dataProcSchema_str << endl;
	statements_str << "\n" << dataProcSchema_str;
    
	bool rslt = parseRun(rdata, dataProcSchema_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}

