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
#include "JimoLogicNew/JimoLogicDatasetFile.h"
#include "JQLStatement/JqlStmtTable.h"


#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SEUtil/JqlTypes.h" 
#include "JQLStatement/JqlStatement.h"
#include "Util/File.h"
#include "Util/CodeConvertion.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDatasetFile_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDatasetFile(version);
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


AosJimoLogicDatasetFile::AosJimoLogicDatasetFile(const int version)
:
AosJimoLogicDataset(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDatasetFile::~AosJimoLogicDatasetFile()
{
}


bool 
AosJimoLogicDatasetFile::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//Dataset file <dataset_name>
	//	(
	//	     charset:UTF8
	//	     filename:"xxxxxxxxx",
	//	     serverid: serverid,
	//	      |  files: [(server_id: <ddd>, file: “<directory>”)...],
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
	aos_assert_rr(keywords[1] == "file", rdata, false);
	mErrmsg = "dataset file ";

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
		setErrMsg(rdata,eGenericError, "",mErrmsg);
		return false;
	}

	mDatasetName = dataset_name;
	mNameValueList = name_value_list;

	// parse multiple Files
	/*
	OmnString attrname = "files";
	vector<AosExprObjPtr> values;
	
	rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, values);
	if (rslt)
	{
		//if size = 1  signal file
		if (values.size() <= 0)
		{
			AosLogError(rdata, true, "there is no this data files") << enderr;
			return false;
		}
		vector<AosExprObjPtr> childValues;
		for(size_t i =0; i < values.size(); i++)
		{
			childValues = values[i]->getExprList();
			stmt->mFiles.push_back(childValues);
			childValues.clear();
		}
	}
	else
	{
		AosLogError(rdata, true, "there is no such data files") << enderr;
		return false;
	}
*/
	//parse signal file
	//format is:
	//files:(filename:"/home/xxx",serverid:0)

	//parse filename
	OmnString filename = jimo_parser->getParmStr(rdata, "filename", name_value_list);
	//parse serverid
	int serverid = jimo_parser->getParmInt(rdata, "serverid", name_value_list);

	if(filename == "" && serverid >= 0)
	{
		setErrMsg(rdata,eMissingParm, "filename",mErrmsg);
		return false;
	}
	mFileName = filename;
	mServerId = serverid;

	if(serverid < 0 && mFileName != "")
	{
		setErrMsg(rdata, eInvalidParm, "serverid",mErrmsg);
		return false;
	}
	vector<AosExprObjPtr> values;
	if(mFileName == "")
	{
		OmnString attrname = "files";
		rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, values);
		if(rslt)
		{
			if(values.size() != 2)
			{
				//			AosLogError(rdata, true, "there is no this data files") << enderr;
				setErrMsg(rdata, eInvalidParm, "files",mErrmsg);
				return false;
			}
			mFile = values;	
		}
		else
		{
			//		AosLogError(rdata, true, "there is no such data files") << enderr;
			setErrMsg(rdata, eMissingParm, "files",mErrmsg);
			return false;
		}
	}
	//parse table name 
	OmnString tableName = jimo_parser->getParmStr(rdata, "table", name_value_list);
	mTableName = tableName;
	if(tableName != "")
	{
		AosRundataPtr rdataPtr(rdata);
		AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr, JQLTypes::eTableDoc, mTableName);
		if (!table_doc)
		{
			setErrMsg(rdata, eNotExist, mTableName, mErrmsg);
			return false;
		}
	}

	//parse charset 
	//Gavin 2015/08/03 JIMODB-225 
	mCharset = jimo_parser->getParmStr(rdata, "charset", name_value_list);

	if (!CodeConvertion::isValid(mCharset))
	{
		OmnString errmsg;
		errmsg << " [ERR] : invalid charset '" << mCharset << "'!" ; 
		rdata->setJqlMsg(errmsg);
		return false;
			
	}
	if(mCharset == "") mCharset = "UTF-8";
	
	// parse schema 
	if(mTableName == "")
	{
		OmnString attrname = "schema";
		AosExprObjPtr schema_expr = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
		if (!schema_expr)
		{
			setErrMsg(rdata, eMissingParm, "schema", mErrmsg);
			return false;
		}

		// If schema_expr type is String. If it is an Bracket, it is defined here. Otherwise, 
		// it is an error. 
		if (schema_expr->getType() == AosExprType::eString)
		{
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
//			AosLogError(rdata, true, "invalid schema") << enderr;
			setErrMsg(rdata, eInvalidParm, "invalid", mErrmsg);
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
	values.clear();
	rslt = jimo_parser->getParmArray(rdata, "splitter", name_value_list, values);
	if (!rslt)
	{
		mErrmsg << " [ERR] : missing or invalid attribute \"splitter\"!";
		setErrMsg(rdata, eGenericError,"",mErrmsg);
		return false;
	}
	
	mSplitter = values; 

	// jimodb-906, 2015.10.12
	// parse mIsSkipFirstLine
	mIsSkipFirstLine = jimo_parser->getParmStr(rdata, "skip_first_line", name_value_list); 
	//parse save doc
//	if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
//	{
//		AosLogError(rdata, true, "missing_savedoc_attributes");
//		return false;
//	}
	if(!checkNameValueList(rdata,mErrmsg,name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDatasetFile::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString& statements_str,
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
	
//	if(mIsService)
//		prog->setDataset(mDatasetName);
	rslt = createFileDataproc(rdata, prog, statements_str);
	//aos_assert_r(rslt, false);

	return rslt;
}


bool
AosJimoLogicDatasetFile::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDatasetFile*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDatasetName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	//mDataprocSelectName << "_dsdp_" << mDatasetName;
	//mOutputName << mDataprocSelectName << "_output";
	//setOutputNames(rdata, prog, mOutputName);
	return true;
}


bool
AosJimoLogicDatasetFile::createFileDataproc(
		AosRundata *rdata,
		AosJimoProgObj* jimo_prog,
		OmnString &statements_str)
{
	OmnString dataProcFile_str = "create datasetfile ";
	dataProcFile_str << mDatasetName << " { "
		<< "\"type\":" << "\"" << "file" << "\"";

	// jimodb-906
	if (mIsSkipFirstLine != "")
		dataProcFile_str << ",\"skip_first_line\": " << mIsSkipFirstLine;

	//generate the files field
	/*if(mFiles.size() <= 0) return false;
	//multiple files
	
	dataProcFile_str << ",\"files\":" << "[";
	for(size_t i =0; i < mFiles.size(); i++)
	{
		dataProcFile_str << "{";
		for(size_t j =0; j < mFiles[i].size(); j++)
		{
			if(mFiles[i][j]->getName() == "filename")
				dataProcFile_str << "\"filename\": " << "\"" << mFiles[i][j]->dumpByNoQuote() << "\"";
			else if(mFiles[i][j]->getName() == "serverid")
				dataProcFile_str << "\"serverid\": " << mFiles[i][j]->dumpByNoQuote();
			else return false;

			if(j < mFiles[i].size() - 1) dataProcFile_str << ",";
		}
		dataProcFile_str << "}";
		if(i < mFiles.size() - 1) dataProcFile_str << ",";
	}
	dataProcFile_str << "]";
	*/

	//singal file format
	dataProcFile_str << ",\"files\":" << "{";
	if(mFileName != "")
	{
		//arvin 2015.07.29
		//JIMODB-152:check filepath is exist?
		/*if(!OmnFile::fileExist(mFileName))
		  {
		  setErrMsg(rdata,eUnknowParm,mFileName,mErrmsg);
		  return false;
		  }*/
		int fd = open(mFileName.data(), O_RDONLY);
		if(fd == -1)
		{
			mErrmsg << "file doesn't exist";
			rdata->setJqlMsg(mErrmsg);
			return false;
		}
		struct stat st;
		lstat(mFileName.data(), &st);
		int tt = S_ISREG(st.st_mode);
		if(!tt)
		{
			//setErrMsg(rdata,eUnknowParm,mFileName,mErrmsg);
			mErrmsg << " isn't a regular file";
			rdata->setJqlMsg(mErrmsg);
			return false;
		}
		dataProcFile_str << "\"filename\": " << "\"" << mFileName << "\"";
		dataProcFile_str << ",\"serverid\": " << mServerId;
	}
	else
	{
		for(size_t i =0; i < mFile.size(); i++)
		{
			if(mFile[i]->getName() == "filename")
			{
				//arvin 2015.07.29
				//JIMODB-152:check filepath is exist?
				/*		OmnString filename = mFile[i]->dumpByNoQuote();
						if(!OmnFile::fileExist(filename))
						{
						setErrMsg(rdata,eUnknowParm,filename,mErrmsg);
						return false;
						}*/
				OmnString filename = mFile[i]->dumpByNoQuote();
				int fd = open(filename.data(), O_RDONLY);
				if(fd == -1)
				{
					mErrmsg << "file doesn't exist";
					rdata->setJqlMsg(mErrmsg);
					return false;
				}
				struct stat st;
				lstat(filename.data(), &st);
				int tt = S_ISREG(st.st_mode);
				if(!tt)
				{
					//setErrMsg(rdata,eUnknowParm,mFileName,mErrmsg);
					mErrmsg << " isn't a regular file";
					rdata->setJqlMsg(mErrmsg);
					return false;
				}
				dataProcFile_str << "\"filename\": " << "\"" << filename << "\"";
			}		
			else if(mFile[i]->getName() == "serverid")
			{	
				dataProcFile_str << "\"serverid\": " << mFile[i]->dumpByNoQuote();
				//Gavin 2015/08/14 JIMODB-391
				OmnString  server_id = mFile[i]->dumpByNoQuote();
				int serverid = server_id.toInt();
				if (serverid < 0 || serverid > 16777216)
				{
					OmnString err_msg;
					err_msg << "[ERR] : 'serverid' must range from 0 to 1677216!";
					rdata->setJqlMsg(err_msg);
					return false;
				}
			}
			else
			{
				OmnString msg="";
				setErrMsg(rdata,eInvalidParm,mFile[i]->getName(), msg);
				return false;
			}
			if(i < mFile.size() - 1) dataProcFile_str << ",";
		}
	}
	dataProcFile_str << "},";
	
	//generate the split field
	if(mSplitter.size() > 0) 
	{
		dataProcFile_str << "\"split\":" << "{";
		//Gavin 2015/08/13 JIMODB-391
		dataProcFile_str << "\"type\":\"dirfile\"";
		for(size_t i=0; i < mSplitter.size(); i++)
		{
			OmnString name = mSplitter[i]->getName();
			//bool flag = false;
			dataProcFile_str << ",";  
			if(name == "block_size")
			{
				//Gavin 2015/08/06                                                                  
				//JIMODB-286::check block_size                                                    
				OmnString size_str = mSplitter[i]->dumpByNoQuote();                               
				u64 size = size_str.toU64(0);                                                     
				if(size <= 0 || size > UINT_MAX)                                                  
				{                                                                                 
					OmnString msg = "[ERR] : ";                                                   
					msg << mErrmsg << " \"block_size\" must range from 1 to " << UINT_MAX <<" (unit is million)!";  
					rdata->setJqlMsg(msg);                                                        
					return false;                                                                 
				}                                                                                 

				//flag = true;
				dataProcFile_str << "\"block_size\":" << mSplitter[i]->dumpByNoQuote();
			}
			else 
			{
				//Gavin 2015/08/14 JIMODB-391
				setErrMsg(rdata,eUnknowParm,mSplitter[i]->getName(),mErrmsg);
				return false;
			}
		}
		dataProcFile_str << "}";
	}
	else
	{
		dataProcFile_str << "\"split\":{"
			<< "\"type\":\"dirfile\","
			<< "\"block_size\":10000000}";
	}
	//generate the schema field
	if(mTableName == "" && mSchemaName == "")
	{
		if(mNormalSchemaField.size() < 0) 
		{
			mErrmsg << "; no fields in define schema!";
			setErrMsg(rdata, eGenericError,"",mErrmsg);
			return false;
		}
		dataProcFile_str << ",\"schema\": {";
		for(size_t i = 0; i < mNormalSchemaField.size(); i++)
		{
			dataProcFile_str << "\"" << mNormalSchemaField[i]->getName() << "\": " << "\""
				<< mNormalSchemaField[i]->dumpByNoQuote() << "\"";
			if(i < mNormalSchemaField.size() - 1) dataProcFile_str << ",";
		}
		if(mSchemaFields.size() < 0) return false;
		dataProcFile_str << ",\"fields\": [";
		for(size_t i = 0; i < mSchemaFields.size(); i++)
		{
			dataProcFile_str << "{";
			for(size_t j = 0; j < mSchemaFields[i].size(); j++)
			{
				if(mSchemaFields[i][j]->getName() == "max_length")
				{
					dataProcFile_str << "\"max_length\": " << mSchemaFields[i][j]->dumpByNoQuote();
					if(j < mSchemaFields[i].size() -1) dataProcFile_str << ",";
					continue;
				}
				dataProcFile_str << "\"" << mSchemaFields[i][j]->getName() << "\": " << "\""
					<< mSchemaFields[i][j]->dumpByNoQuote() << "\"";
				if(j < mSchemaFields[i].size() -1) dataProcFile_str << ",";
			}
			dataProcFile_str << "}";
			if(i < mSchemaFields.size() - 1) dataProcFile_str << ",";
		}
		dataProcFile_str << "]}";
	}
	else
	{
		dataProcFile_str << mStrSchema;	
	}
	
	
	dataProcFile_str << "};";
	
	OmnScreen << "RainQiu" << dataProcFile_str << endl;
	statements_str << "\n" << dataProcFile_str;    

    bool rslt = parseRun(rdata, dataProcFile_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


AosJimoPtr 
AosJimoLogicDatasetFile::cloneJimo() const
{
	return OmnNew AosJimoLogicDatasetFile(*this);
}


bool 
AosJimoLogicDatasetFile::getSchemaByTableName(
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

	OmnString record_delimiter = dataRecordDoc->getAttrStr("zky_row_delimiter", "");
	if(record_delimiter == "") return false;
	schema << ",\"record_delimiter\": " << "\"" << record_delimiter << "\"";

	OmnString field_delimiter = dataRecordDoc->getAttrStr("zky_field_delimiter", "");
	if(field_delimiter == "") return false;
	schema << ",\"field_delimiter\": " << "\"" << field_delimiter << "\"";

	OmnString text_qualifier = dataRecordDoc->getAttrStr("zky_text_qualifier", "");
	if(text_qualifier == "") return false;
	schema << ",\"text_qualifier\": " << "\"" << text_qualifier << "\"";
	schema << ",\"schema_docid\": " << "\"" << docid << "\"";
	
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
			//arvin 2015.07.31
			//JIMODB-183 ,timeformat need doublequotes
			schema << ",\"format\": \"" << dataFieldDoc->getAttrStr("format") << "\"";
		}
		
		dataFieldDoc = dataFieldsDoc->getNextChild();
		schema << "}";
		//if(!dataFieldDoc) break; 
		if(dataFieldDoc) schema << ",";
	}
	//append docid
	//schema << "{\"type\": \"docid\", \"name\": \"docid\"}";

	schema << "]}";
	mStrSchema = schema;
	return true;
}

	
bool 
AosJimoLogicDatasetFile::getSchemaBySchemaName(
				AosRundata*			rdata, 
				const OmnString 	&name,
				const OmnString 	&charset)
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
AosJimoLogicDatasetFile::getSchemaName(
				AosRundata *rdata,
				AosJimoProgObj *prog,
				vector<OmnString> &schema_name)
{
	schema_name.push_back(mSchemaName);
	return true;
}

vector<OmnString> 
AosJimoLogicDatasetFile::getOutputNames()
{
	mOutputNames.push_back(mDatasetName);
	return mOutputNames;
}
bool 
AosJimoLogicDatasetFile::getInputV(vector<OmnString> &inputs)
{
	inputs.push_back(mDatasetName);
	return true;
}
