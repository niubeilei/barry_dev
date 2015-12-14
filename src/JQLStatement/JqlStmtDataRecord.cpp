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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtDataRecord.h"

#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"

#include "AosConf/DataRecord.h"
#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataRecordMulti.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtDataRecord::AosJqlStmtDataRecord(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mSchemaPicker = NULL;
	mName = "";
	mFieldNemes = 0;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtDataRecord::AosJqlStmtDataRecord()
{
	mSchemaPicker = NULL;
	mName = "";
	mFieldNemes = 0;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtDataRecord::~AosJqlStmtDataRecord()
{
	OmnDelete(mFieldNemes);
}


bool
AosJqlStmtDataRecord::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataRecord(rdata);
	if (mOp == JQLTypes::eShow) return showDataRecords(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataRecords(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataRecord(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL DataRecord", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDataRecord::createDataRecord(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataRecordDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		msg << "Data Record " << mName << " already exists. "
			  << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr(AOSTAG_OBJID, objid);
	doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_RECORD_CONTAINER);

	if ( !createDoc(rdata, JQLTypes::eDataRecordDoc, mName, doc) )
	{
		msg << "Failed to create DataRecord doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << mRecordType << " " << mName << " created successfully.";
	rdata->setJqlMsg(msg);

	return true;
}


AosXmlTagPtr 
AosJqlStmtDataRecord::convertMutilRecordConf(const AosRundataPtr &rdata)
{
	//<dataschema>
	//	<datafields>
	//		<datafield></datafield>
	//		<datafield></datafield>
	//		...
	//	</datafields>
	//	<datarecords>
	//		<datarecord> </datarecord>
	//		....
	//	</datarecords>
	//	<schema_picker> </schema_picker>
	//</dataschema>



	boost::shared_ptr<DataRecordMulti> recordObj = 
		boost::make_shared<DataRecordMulti> ();
	map<OmnString, OmnString> recordNames;
	// 1. datafields
	if (!mFieldNemes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	for (u32 i = 0; i < mFieldNemes->size(); i++)                    
	{                                                                     
		OmnString fieldName = (*mFieldNemes)[i]->getValue(rdata.getPtrNoLock());
		AosXmlTagPtr fieldDoc = getDoc(rdata, JQLTypes::eDataFieldDoc, fieldName);
		if (!fieldDoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		recordObj->setDataField(fieldDoc->toString());
		AosXmlTagPtr valueNode = fieldDoc->getFirstChild("value");
		while (valueNode)
		{
			OmnString recordname = valueNode->getAttrStr("zky_record_name");
			recordNames[recordname] = recordname;
			valueNode = fieldDoc->getNextChild();	
		}
	}

	// 2. datarecords
	vector<OmnString> records;
	for (map<OmnString, OmnString>::iterator itr = recordNames.begin(); 
			itr != recordNames.end(); itr++)
	{
		AosXmlTagPtr record_doc = getDoc(rdata, JQLTypes::eDataRecordDoc, itr->first);
		aos_assert_r(record_doc, NULL);
		recordObj->appendRecordPick(record_doc->toString());
	}

	// 3. schema picker
	if (mSchemaPicker)
	{
		string picker_str = mSchemaPicker->dump();
		recordObj->setSchemaPicker(picker_str);
	}

	recordObj->setAttribute("zky_name", mName);
	recordObj->setAttribute("type", mType);

	if (mRecordDelimiter != "") recordObj->setAttribute("zky_row_delimiter", mRecordDelimiter);
	if (mTextQualidier != "") recordObj->setAttribute("zky_text_qualifier", mTextQualidier);
	if (mFieldDelimiter != "") recordObj->setAttribute("zky_field_delimiter", mFieldDelimiter);
	if (mRecordLen !=  "") recordObj->setAttribute("zky_length", mRecordLen);

	return AosXmlParser::parse(recordObj->getConfig() AosMemoryCheckerArgs); 
}


AosXmlTagPtr
AosJqlStmtDataRecord::convertToXml(const AosRundataPtr &rdata)
{
	if (mType == "multi")
	{
		return convertMutilRecordConf(rdata);
	}

	boost::shared_ptr<DataRecord> recordObj;
	if (mType == "fixbin") 
	{
		recordObj = boost::make_shared<DataRecordFixbin>();
	}
	else 
	{
		recordObj = boost::make_shared<DataRecord>();
	}

	if (mConfParms)
	{
		return getConf(rdata, recordObj, mConfParms);
	}

	recordObj->setAttribute("zky_name", mName);
	recordObj->setAttribute("zky_type", "datarecord");
	recordObj->setAttribute("type", mType);
	recordObj->setAttribute("trim", mTrimCondition);

	if (mRecordDelimiter != "") recordObj->setAttribute("zky_row_delimiter", mRecordDelimiter);
	if (mTextQualidier != "") recordObj->setAttribute("zky_text_qualifier", mTextQualidier);
	if (mFieldDelimiter != "") recordObj->setAttribute("zky_field_delimiter", mFieldDelimiter);
	if (mRecordLen !=  "") recordObj->setAttribute("zky_length", mRecordLen);

	if (!mFieldNemes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	for (u32 i = 0; i < mFieldNemes->size(); i++)                    
	{                                                                     
		OmnString fieldName = (*mFieldNemes)[i]->getValue(rdata.getPtrNoLock());
		AosXmlTagPtr fieldDoc = getDoc(rdata, JQLTypes::eDataFieldDoc, fieldName);
		if (!fieldDoc)
		{
			//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			//OmnAlarm << __func__ << enderr;
			//Gavin 2015/08/05 JIMODB-263
			OmnString errmsg;                                         
			errmsg << " [ERR] :  Invalid field name '" << fieldName << "'!" ;    
			rdata->setJqlMsg(errmsg);                                 
			return 0;
		}
		boost::shared_ptr<DataFieldExpr> fieldObj = boost::make_shared<DataFieldExpr>(fieldDoc);
		recordObj->setField(fieldObj);
	}

	return AosXmlParser::parse(recordObj->getConfig() AosMemoryCheckerArgs); 
}


bool
AosJqlStmtDataRecord::showDataRecords(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> records;
	if( !getDocs(rdata, JQLTypes::eDataRecordDoc, records) )
	{
		msg << "Empty set";
		rdata->setJqlMsg(msg);
		return true;
	}
	int total = records.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<records.size(); i++)
	{
		OmnString vv = records[i]->getAttrStr("zky_name", "");
		if (vv != "") {
			content << "<record recordname=\"" << vv << "\"/>";
		}
	}

	content<<"</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("recordname");
	map<OmnString, OmnString> alias_name;
	alias_name["recordname"] = "NAME";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtDataRecord::dropDataRecords(const AosRundataPtr &rdata)
{
	//JIMODB-163 drop schema by Levi
	OmnString msg;
	vector<AosXmlTagPtr> records;
	if( !getDocs(rdata, JQLTypes::eDataRecordDoc, records) )
	{
		msg << "Empty set";
		rdata->setJqlMsg(msg);
		return true;
	}
	int total = records.size() ;
	
	OmnString objid = "";
	OmnString name = "";
	for(int i=0; i<total; i++)
	{
		name = records[i]->getAttrStr("zky_name");
		if(name == mName)
		{
			objid = records[i]->getAttrStr("zky_objid");
			break;
		}
	}

	//OmnString objid = getObjid(rdata, JQLTypes::eDataRecordDoc, mName);                 
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete data record doc.";
		rdata->setJqlMsg(msg);
		return false;                                       
	}                                                       
	msg << "Data Record " << mName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtDataRecord::describeDataRecord(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDataRecordDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display data record " << mName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;
	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back("trim");
	fields.push_back("zky_length");
	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["trim"] = "Trim";
	alias_name["zky_length"] = "Length";

	AosXmlTagPtr datafields= doc->getFirstChild("datafields");
	if (!datafields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr datafield = datafields->getFirstChild(true);
	int idx = 1;
	OmnString field_name;
	OmnString tmp_name;
	OmnString tag_name = "zky_name";
	while (datafield)
	{
		field_name = datafield->getAttrStr(tag_name);
		tmp_name << tag_name << idx;
		fields.push_back(tmp_name);
		alias_name[tmp_name] << "FieldName_" << idx;
		doc->setAttr(tmp_name, field_name);
		datafield = datafields->getNextChild();
		tmp_name = "";
		idx++;
	}

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtDataRecord::clone()
{
	return OmnNew AosJqlStmtDataRecord(*this);
}


void 
AosJqlStmtDataRecord::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDataRecord::setName(OmnString name)
{
	mName = name;
}

void 
AosJqlStmtDataRecord::setType(OmnString type)
{
	type.toLower();
	if (type.toLower() == "fixbin" || type.toLower() == "fixed")
	{
		mType = "fixbin";
	}
	else if (type.toLower() == "csv")
	{
		mType = "csv";
	}
	else if (type.toLower() == "multi")
	{
		mType = "multi";
	}
	else
	{
		mType = "invalid";
	}
}

void 
AosJqlStmtDataRecord::setRecordType(OmnString type)
{
	mRecordType = type;
}

void 
AosJqlStmtDataRecord::setRecordLength(int64_t len)
{
	if (len == 0) mRecordLen = "";
	else mRecordLen << len;
}

void 
AosJqlStmtDataRecord::setTrimCondition(OmnString name)
{
	mTrimCondition = name;
}

void 
AosJqlStmtDataRecord::setFieldDelimiter(OmnString name)
{
	mFieldDelimiter = name;
}

void 
AosJqlStmtDataRecord::setRecordDelimiter(OmnString name)
{
	mRecordDelimiter = name;
}

void 
AosJqlStmtDataRecord::setTextQualidier(OmnString name)
{
	mTextQualidier = name;
}

void 
AosJqlStmtDataRecord::setFieldNames(AosExprList *fieldnames)
{
	mFieldNemes = fieldnames;
}


void 
AosJqlStmtDataRecord::setScheamPicker(AosExprObj *exprobj)
{
	if (exprobj) mSchemaPicker = exprobj;
}
