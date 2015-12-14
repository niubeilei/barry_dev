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
#include "JimoLogicNew/JimoLogicDatasetDir.h"
#include "JQLStatement/JqlStmtTable.h"

#include "SEInterfaces/NetFileCltObj.h"
#include "API/AosApi.h"
#include "Util/DirDesc.h"
#include "Util/CodeConvertion.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SEUtil/JqlTypes.h" 
#include "JQLStatement/JqlStatement.h"
#include "XmlUtil/XmlTag.h"
#include <dirent.h>

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDatasetDir_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDatasetDir(version);
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


AosJimoLogicDatasetDir::AosJimoLogicDatasetDir(const int version)
:
AosJimoLogicDataset(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDatasetDir::~AosJimoLogicDatasetDir()
{
}


bool 
AosJimoLogicDatasetDir::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//Dataset dir <dataset_name>
	//	(
	//	     dirs: [(server_id: <ddd>, dir: “<directory>”)...],
	//		 schema: <schema_name> | (<schema_definition>),
	//		 table: <table_name>,
	//		 splitter: (<splitter_spec>)
	//	);
    
	//we can get the schema whit table; and if table name is not Null, wo
	//would use table-schema and ignore the schema attribute field
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataset", rdata, false);
	aos_assert_rr(keywords[1] == "dir", rdata, false);
	mErrmsg = "dataset dir ";
	// parse dataset name
	OmnString dataset_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataset_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataset_name;

	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError, "",mErrmsg);
		return false;
	}

	mDatasetName = dataset_name;
	mErrmsg = mErrmsg;
	mNameValueList = name_value_list;

	// parse dirs
	OmnString attrname = "dirs";
	vector<AosExprObjPtr> values;
	
	rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, values);
	if (rslt)
	{
		if (values.size() <= 0)
		{
			setErrMsg(rdata,eMissingParm,"dirs",mErrmsg);
			return false;
		}
		vector<AosExprObjPtr> childValues;
		for(size_t i =0; i < values.size(); i++)
		{
			//arvin 2015.07.22
			//JIMODB-58:syntax check
			if(values[i]->getType() != AosExprType::eBrackets)
			{
				setErrMsg(rdata,eSyntaxError,"dirs",mErrmsg);
				return false;
			}
			childValues = values[i]->getExprList();
			mDirs.push_back(childValues);
			childValues.clear();
		}
	}
	else
	{
		setErrMsg(rdata,eMissingParm,"dirs",mErrmsg);
		return false;
	}
	//arvin 2015.07.31
	//JIMODB-152
	if(!parseDir(rdata))
	{
		return false;
	}

	//parse charset 
	mCharset = jimo_parser->getParmStr(rdata, "charset", name_value_list);
	if(mCharset == "") mCharset = "UTF-8";
	if (!CodeConvertion::isValid(mCharset))
	{
		OmnString errmsg;
		errmsg << " [ERR] : invalid charset '" << mCharset << "'!" ; 
		rdata->setJqlMsg(errmsg);
		return false;
	}
	
	//parse table name 
	attrname = "table";
	OmnString tableName = jimo_parser->getParmStr(rdata, attrname, name_value_list);
	mTableName = tableName;

	// parse schema 
	if(mTableName == "")
	{
		attrname = "schema";
		AosExprObjPtr schema_expr = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
		if (!schema_expr)
		{
			setErrMsg(rdata,eMissingParm,"schema",mErrmsg);
			return false;
		}

		// If schema_expr type is String. If it is an Bracket, it is defined here. Otherwise, 
		// it is an error. 
		if (schema_expr->getType() == AosExprType::eString)
		{
			mSchemaName = schema_expr->dumpByNoQuote();
			mSchemaName = schema_expr->dumpByNoQuote(); 
		//get schema by schema name	
			rslt = getSchemaBySchemaName(rdata, mSchemaName, mCharset);	
			if(!rslt)
			{
				setErrMsg(rdata, eNotExist, mSchemaName, mErrmsg);
				return false;
			}
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
			setErrMsg(rdata, eInvalidParm, mSchemaName, mErrmsg);
			return false;
		}
	}
	else
	{
	//get schema with tableName
		rslt = getSchemaByTableName(rdata, mTableName);	
		aos_assert_r(rslt, false);
	}

	//parse splitter
	attrname = "splitter";
	values.clear();
	rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, values);
	if (!rslt)
	{
		setErrMsg(rdata,eMissingParm,"splitter",mErrmsg);
		return false;
	}
	else
	{
		if (values.size() <= 0)
		{
			setErrMsg(rdata,eMissingParm,"splitter",mErrmsg);
			return false;
		}
		mSplitter = values; 
	}

	// jimodb-906, 2015.10.12
	// parse mIsSkipFirstLine
	mIsSkipFirstLine = jimo_parser->getParmStr(rdata, "skip_first_line", name_value_list); 
	
	//parse save doc
	//if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
	//{
	//	AosLogError(rdata, true, "missing_savedoc_attributes");
	//	return false;
	//}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDatasetDir::run(
		AosRundata *rdata, 
		AosJimoProgObj* prog,
		//const OmnString &verb_name,
		OmnString &statements_str,
		bool isparser) 
{
	//savedoc
	//if (mSaveDoc == "true")
	//{
		AosJqlStatementPtr stmt = dynamic_cast<AosJqlStatement*>(this);
		OmnString jql = stmt->getOrigStatement();
		bool rslt = createJimoLogicDoc(rdata, mDatasetName, jql);
		aos_assert_rr(rslt, rdata, false);
	//}

	rslt = createDirDataproc(rdata, prog, statements_str);
	aos_assert_r(rslt, false);

	return rslt;
}


bool
AosJimoLogicDatasetDir::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
   //AosJimoLogicDatasetDirPtr jimologic = dynamic_cast<AosJqlStatementPtr>(this);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDatasetDir*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDatasetName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDatasetDir::createDirDataproc(
		AosRundata *rdata,
		AosJimoProgObj* jimo_prog,
		OmnString &statements_str)
{
	OmnString dataProcDir_str = "create datasetdir ";
	dataProcDir_str << mDatasetName << " { "
		<< "\"type\":" << "\"" << "dir" << "\"";

	//generate the dirs field
	if(mDirs.size() <= 0) return false;
	dataProcDir_str << ",\"dirs\":" << "[" << mDirStr << "]";

	// jimodb-906
	if (mIsSkipFirstLine != "")
		dataProcDir_str << ",\"skip_first_line\": " << mIsSkipFirstLine;

	//generate the split field
	if(mSplitter.size() > 0)
	{
		dataProcDir_str << ",\"split\":" << "{";
		dataProcDir_str << "\"type\":\"dirfile\"";
		for(size_t i=0; i < mSplitter.size(); i++)
		{
			dataProcDir_str << ",";  
			if(mSplitter[i]->getName() == "block_size")
			{
				//arvin 2015.8.1
				//JIMODB-194::check block_size
				OmnString size_str = mSplitter[i]->dumpByNoQuote();
				u64 size = size_str.toU64(0);
				if(size <= 0 || size > UINT_MAX)
				{
					OmnString msg = "[ERR] : ";
					msg << mErrmsg << " \"block_size\" must range from 1 to " << UINT_MAX <<" (unit is million)!";
					rdata->setJqlMsg(msg);
					return false;
				}
				dataProcDir_str << "\"block_size\":" << mSplitter[i]->dumpByNoQuote();
			}
			else
			{
				setErrMsg(rdata,eUnknowParm,mSplitter[i]->getName(),mErrmsg);
				return false;
			}

		}
		dataProcDir_str << "}";
	}
	else
	{
		dataProcDir_str << "\"split\":{"
			<< "\"type\":\"dirfile\","
			<< "\"block_size\":10000000}";
	}

	//generate the schema field
	if(mTableName == "" && mSchemaName == "")
	{
		if(mNormalSchemaField.size() < 0) return false;
		dataProcDir_str << ",\"schema\": {";
		for(size_t i = 0; i < mNormalSchemaField.size(); i++)
		{
			dataProcDir_str << "\"" << mNormalSchemaField[i]->getName() << "\": " << "\""
				<< mNormalSchemaField[i]->dumpByNoQuote() << "\"";
			if(i < mNormalSchemaField.size() - 1) dataProcDir_str << ",";
		}
		if(mSchemaFields.size() < 0) return false;

		aos_assert_r(mSchemaName != "", false);
		AosXmlTagPtr schemaDoc = AosGetDocByObjid(AosObjid::getObjidByJQLDocName(JQLTypes::eSchemaDoc, mSchemaName), rdata);
		aos_assert_r(schemaDoc, false);
		AosXmlTagPtr recordDoc = schemaDoc->getFirstChild("datarecord");
		aos_assert_r(recordDoc, false);
		int recordLen = recordDoc->getAttrInt("zky_length", 0);
		if (recordLen > 0) dataProcDir_str << ",\"zky_length\": " << recordLen;

		dataProcDir_str << ",\"fields\": [";
		for(size_t i = 0; i < mSchemaFields.size(); i++)
		{
			dataProcDir_str << "{";
			for(size_t j = 0; j < mSchemaFields[i].size(); j++)
			{
				if(mSchemaFields[i][j]->getName() == "max_length")
				{
					dataProcDir_str << "\"max_length\": " << mSchemaFields[i][j]->dumpByNoQuote();
					if(j < mSchemaFields[i].size() -1) dataProcDir_str << ",";
					continue;
				}
				dataProcDir_str << "\"" << mSchemaFields[i][j]->getName() << "\": " << "\""
					<< mSchemaFields[i][j]->dumpByNoQuote() << "\"";
				if(j < mSchemaFields[i].size() -1) dataProcDir_str << ",";
			}
			dataProcDir_str << "}";
			if(i < mSchemaFields.size() - 1) dataProcDir_str << ",";
		}
		dataProcDir_str << "]}";
	}
	else
	{
		dataProcDir_str << mStrSchema;	
	}

	
	
	dataProcDir_str << "};";
	
	OmnScreen << "RainQiu" << dataProcDir_str << endl;
	statements_str << "\n" << dataProcDir_str;
    
	bool rslt = parseRun(rdata, dataProcDir_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


AosJimoPtr 
AosJimoLogicDatasetDir::cloneJimo() const
{
	return OmnNew AosJimoLogicDatasetDir(*this);
}


bool 
AosJimoLogicDatasetDir::getSchemaByTableName(
				AosRundata*		rdata, 
				const OmnString &name)
{
	//<dataschema jimo_objid=\"dataschema_record_jimodoc_v0\" zky_dataschema_type=\"static\" zky_name=\"t1_parser\" zky_otype=\"dataschema\" zky_pctrs=\"zky__zt_schemac		tnr\" zky_objid=\"_zt4g_schema_t1_parser\" zky_siteid=\"100\" zky_crtor=\"50003\" zky_ctime=\"05-30-2015 09:24:10\" zky_mtime=\"05-30-2015 09:24:10\" zky_ctmepo__      d=\"1432949050\" zky_mtmepo__d=\"1432949050\" zky_docid=\"5084\">
	//		<datarecord type=\"csv\" zky_field_delimiter=\",\" zky_name=\"t1_schema\" zky_row_delimiter=\"LF\" zky_text_qualifier=\"DQM\" zky_type=\"datarecord\" zky_ob				jid=\"zt4g_data_record_t1_schema\" zky_pctrs=\"zky__zt_record\" zky_otype=\"zky_unknown\" zky_siteid=\"100\" zky_crtor=\"50003\" zky_ctime=\"05-30-2015 				09:24:09\"zky_mtime=\"05-30-2015 09:24:09\" zky_ctmepo__d=\"1432949049\" zky_mtmepo__d=\"1432949049\" zky_docid=\"5082\">
	//			<datafields>
	//				<datafield type=\"str\" zky_datatooshortplc=\"cstr\" zky_length=\"20\" zky_name=\"NETWEIGHT\" zky_objid=\"_zt4g_data_field_NETWEIGHT\" zky_otype=\"d						atafield\" zky_pctrs=\"zky__zt_field\"></datafield>
	//				<datafield type=\"u64\" zky_length=\"8\" zky_name=\"OWNER_ID\" zky_objid=\"_zt4g_data_field_OWNER_ID\" zky_otype=\"datafield\" zky_pctrs=\"zky__zt_f						ield\"></datafield>
	//			</datafields>
	//		</datarecord>
	//</dataschema>	
	//get ObjId 
	//use ObjId to find the doc
	OmnString schema;
	AosXmlTagPtr tableDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eTableDoc, name);
	AosXmlTagPtr schemaDoc = tableDoc->getNextChild("schema_docs");
	if(!schemaDoc) return false;
	AosXmlTagPtr dataSchema = schemaDoc->getNextChild("dataschema");
	if(!dataSchema) return false;
	AosXmlTagPtr dataRecordDoc = dataSchema->getFirstChild(true);
	OmnString docid = dataRecordDoc->getAttrStr(AOSTAG_DOCID, "");
	OmnString type = dataRecordDoc->getAttrStr("type", "");
	if(type == "") return false;
	schema << ",\"schema\": " << "{";
	schema << "\"type\": " << "\"" << type << "\"";

	int recordlen = dataRecordDoc->getAttrInt("zky_length", 0);
	if (recordlen > 0) schema << ",\"zky_length\": " << recordlen;

	OmnString trim = dataRecordDoc->getAttrStr("trim", "");
	schema << ",\"trim\": " << "\"" << trim << "\"";

	OmnString record_delimiter = dataRecordDoc->getAttrStr("zky_row_delimiter", "");
	if(record_delimiter == "" && type == "csv") return false;
	if(record_delimiter != "")
	{
		schema << ",\"record_delimiter\": " << "\"" << record_delimiter << "\"";
	}
	OmnString field_delimiter = dataRecordDoc->getAttrStr("zky_field_delimiter", "");
	if(field_delimiter == "" && type == "csv") return false;
	if(field_delimiter != "")
	{
		schema << ",\"field_delimiter\": " << "\"" << field_delimiter << "\"";
	}
	OmnString text_qualifier = dataRecordDoc->getAttrStr("zky_text_qualifier", "");
	if(text_qualifier == "" && type =="csv") return false;
	if(text_qualifier != "")
	{
		schema << ",\"text_qualifier\": " << "\"" << text_qualifier << "\"";
	}
	schema << ",\"schema_docid\": " << "\"" << docid << "\"";
	
	//get Fields
	AosXmlTagPtr dataFieldsDoc = dataRecordDoc->getNextChild("datafields");
	//get datafield
	AosXmlTagPtr dataFieldDoc = dataFieldsDoc->getNextChild();
	if(!dataFieldDoc) return false;
	schema << ",\"fields\": [";
	
	int i = 0;
	while(dataFieldDoc)
	{
		if (i != 0)
			schema << ",";
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

		if (fieldType == "expr")
		{
			AosXmlTagPtr expr = dataFieldDoc->getFirstChild("expr");               
			aos_assert_r(expr, false);
			OmnString value = expr->getNodeText();
			aos_assert_r(value != "", false);
			schema << ",\"value\": \"" << value << "\"";
		}
		else if(fieldType == "number")
		{
			OmnString zky_precision = dataFieldDoc->getAttrStr("zky_precision", "");
			schema << ",\"zky_precision\": " << zky_precision;
			OmnString zky_scale = dataFieldDoc->getAttrStr("zky_scale", "");
			schema << ",\"zky_scale\": " << zky_scale;
		}

		// rain , 2015/07/27
		if (fieldType == "datetime")
		{
			schema << ",\"format\": " << dataFieldDoc->getAttrStr("format");
		}
		
		dataFieldDoc = dataFieldsDoc->getNextChild();
		schema << "}";
		i++;
		//if(!dataFieldDoc) break; 
		//schema << ",";
	}
	//append docid
	//schema << "{\"type\": \"docid\", \"name\": \"docid\"}";

	schema << "]}";
	mStrSchema = schema;
	return true;
}

/*
bool
AosJimoLogicDatasetDir::getOutputName(
		        AosRundata *rdata, 
				AosJimoProgObj *prog, 
		        OmnString &outputName)
{
	outputName = mDatasetName;
	return true;
}
*/
//this is for JimoLogicNew::getDatasetOutput
vector<OmnString> 
AosJimoLogicDatasetDir::getOutputNames()
{
	mOutputNames.push_back(mDatasetName);
	return mOutputNames;
}

bool 
AosJimoLogicDatasetDir::getSchemaBySchemaName(
				AosRundata*		rdata, 
				const OmnString &name,
				const OmnString	&charset)
{
	OmnString schema;
	AosXmlTagPtr dataRecordDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, name);
	//AosXmlTagPtr dataRecordDoc = dataSchemaDoc->getFirstChild(true);
	if(!dataRecordDoc) return false;

	dataRecordDoc->removeAttr("zky_type");
	dataRecordDoc->removeAttr("zky_objid");
	dataRecordDoc->removeAttr("zky_pctrs");
	dataRecordDoc->removeAttr("zky_otype");
	dataRecordDoc->removeAttr("zky_siteid");
	dataRecordDoc->removeAttr("zky_crtor");
	dataRecordDoc->removeAttr("zky_ctime");
	dataRecordDoc->removeAttr("zky_mtime");
	dataRecordDoc->removeAttr("zky_ctmepo__d");
	dataRecordDoc->removeAttr("zky_mtmepo__d");
	dataRecordDoc->removeAttr("zky_name");
	AosXmlTagPtr dataFieldsDoc = dataRecordDoc->getFirstChild("datafields");
	aos_assert_r(dataFieldsDoc, NULL);
	OmnString strXml = dataFieldsDoc->toString();
	dataRecordDoc->removeNode("datafields", false, true);

	vector<pair<OmnString, OmnString> > attrs;
	attrs = dataRecordDoc->getAttributes();
	vector<pair<OmnString, OmnString> >::iterator itr = attrs.begin();
	schema << ",\"schema\": " ;
	

	JSONValue jsonSchema;
	JSONValue fields;
	while(itr != attrs.end())
	{
		if(itr->first == "zky_length")
		{
			jsonSchema["zky_length"] = (int)itr->second.toU64();
		}
		else
		{
			jsonSchema[itr->first] = itr->second.data();
		}
		itr++;
	}
	jsonSchema["charset"] = charset.data(); 
	
	// jimodb-906
	jsonSchema["skip_first_line"] = mIsSkipFirstLine.data();

	dataFieldsDoc = AosXmlParser::parse(strXml AosMemoryCheckerArgs);
	AosXmlTagPtr dataFieldDoc = dataFieldsDoc->getNextChild();
	if(!dataFieldDoc) return false;

	while(dataFieldDoc)
	{
		JSONValue field;
		OmnString fieldType = dataFieldDoc->getAttrStr("type", "");
		if(fieldType == "") return false;
		field["type"] = fieldType.data();
		OmnString fieldName = dataFieldDoc->getAttrStr("zky_name", "");
		if(fieldName == "") return false;
		field["name"] = fieldName.data();
		OmnString maxLength = dataFieldDoc->getAttrStr("zky_length", "");
		if(maxLength == "") return false;
		field["max_length"] = (int)maxLength.toU64();
		if (fieldType == "expr")
		{
			AosXmlTagPtr expr = dataFieldDoc->getFirstChild("expr");               
			aos_assert_r(expr, false);
			OmnString value = expr->getNodeText();
			aos_assert_r(value != "", false);
			field["value"] = value.data();
		}
		else if(fieldType == "number")
		{
			OmnString zky_precision = dataFieldDoc->getAttrStr("zky_precision", "");
			field["zky_precision"] = zky_precision.data();
			OmnString zky_scale = dataFieldDoc->getAttrStr("zky_scale", "");
			field["zky_scale"] = zky_scale.data();
		}

		// rain , 2015/07/27
		if (fieldType == "datetime")
		{
			field["format"] = dataFieldDoc->getAttrStr("format").data();
		}

		fields.append(field);
		dataFieldDoc = dataFieldsDoc->getNextChild();
	}
	jsonSchema["fields"] = fields;
	schema << jsonSchema.toStyledString();
	mStrSchema = schema;
	return true;
}

bool 
AosJimoLogicDatasetDir::parseDir(
		AosRundata *rdata)
{
	for(size_t i =0; i < mDirs.size(); i++)
	{
		mDirStr << "{";
		//arvin 2015.08.13
		//JIMODB-395
		if(mDirs[i].size() < 2)
		{
			setErrMsg(rdata,eMissingParm,"dirname or serverid",mErrmsg);
			return false;
		}

		OmnString dirname = "";
		int server_id = -1;

		for(size_t j =0; j < mDirs[i].size(); j++)
		{
			if(mDirs[i][j]->getName() == "dirname")
			{
				dirname = mDirs[i][j]->dumpByNoQuote();
				//// 1. maybe is a uservar
				//if (!OmnDirDesc::dirExist(dirname))
				//{
				//	OmnString msg = "";
				//	OmnString expr_str = "";
				//	expr_str << dirname << ";";
				//	AosExprObjPtr expr = AosParseExpr(expr_str, msg, rdata);

				//	AosValueRslt v;
				//	if (expr)
				//	{
				//		bool rslt = expr->getValue(rdata, 0, v);
				//		aos_assert_r(rslt, false);
				//		dirname = v.getValueStr1();
				//	}
				//}
				mDirStr << "\"dirname\": " << "\"" << dirname<< "\"";
			}
			else if(mDirs[i][j]->getName() == "serverid")
			{

				OmnString serverid = mDirs[i][j]->dumpByNoQuote();
				server_id = serverid.toInt();
				if(server_id < 0 || server_id > 1677216)
				{
					OmnString err_msg;
					err_msg << "[ERR] : 'serverid' must range from 0 to 1677216!"; 
					rdata->setJqlMsg(err_msg);
					return false;
				}
				mDirStr << "\"serverid\": " << mDirs[i][j]->dumpByNoQuote();
			}
			else
			{
				mErrmsg << " invalid parameter " << "\"" << mDirs[i][j]->getName() << "\"";
				rdata->setJqlMsg(mErrmsg);
				return false;
			}

			if(j < mDirs[i].size() - 1) mDirStr << ",";
		}
		bool rslt = AosNetFileCltObj::dirIsGoodStatic(dirname, server_id, false, rdata);
		if ( !rslt )
		{
			OmnString err_msg;
			err_msg << "[ERR] : please check dirname(" << dirname << "), serverid(" << server_id << ")";
			rdata->setJqlMsg(err_msg);
			return false;
		}
		mDirStr << "}";
		if(i < mDirs.size() - 1) mDirStr << ",";
	}
	return true;
}

bool 
AosJimoLogicDatasetDir::getSchemaName(
				AosRundata *rdata, 
				AosJimoProgObj *prog,
				vector<OmnString> &schema_name)
{
	schema_name.push_back(mSchemaName);
	return true;
}
bool 
AosJimoLogicDatasetDir::getInputV(vector<OmnString> &inputs)
{
	inputs.push_back(mDatasetName);
	return true;
}
