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
#include "JQLStatement/JqlStmtSchema.h"
#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"

#include "AosConf/DataSchema.h"
#include "AosConf/DataSchemaMultiRecord.h"
#include "AosConf/DataRecordFixbin.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtSchema::AosJqlStmtSchema(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mName = "";
	mLineBreak = "LF";
	mRecord = "";
	mComment = "";
	mSchemaNames = 0;
	mPickers = 0;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtSchema::AosJqlStmtSchema()
{
	mName = "";
	mLineBreak = "LF";
	mRecord = "";
	mComment = "";
	mSchemaNames = 0;
	mPickers = 0;
	mRecordNames = 0;
	mMaxRecordLen = 0;
	mIsIgnoreSubPattern = false;
	mIsSkipInvalidRecords = false;
	mSchemaPos = 0;
	mSchemaLen = 0;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtSchema::~AosJqlStmtSchema()
{
	OmnDelete mSchemaNames;

	if (mPickers) 
	{
		delete mPickers;
		mPickers= NULL;
	}

	if (mRecordNames)
	{
		delete mRecordNames;
		mRecordNames = NULL;
	}
}

bool
AosJqlStmtSchema::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
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

	if (mOp == JQLTypes::eCreate) return createSchema(rdata);
	if (mOp == JQLTypes::eShow) return showSchemas(rdata);
	if (mOp == JQLTypes::eDrop) return dropSchema(rdata);
	if (mOp == JQLTypes::eDescribe) return describeSchema(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Schema", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtSchema::createSchema(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eSchemaDoc, mName);
	if ( getDocByObjid(rdata, objid) )
	{
		msg << "Schema '" << mName << "' is already exist.";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr doc = convertToXml(rdata);
	if (!doc)
	{
		return false;
	}
	doc->setAttr(AOSTAG_OBJID, objid);

	if ( !createDoc(rdata, JQLTypes::eSchemaDoc, mName, doc) )
	{
		msg << "Failed to create Schema doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << mRecordType << " " << mName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtSchema::generateMultiRecordConf(const AosRundataPtr &rdata)
{
	aos_assert_r(mPickers, NULL);
//	aos_assert_r(mRecordNames->size() == mPickers->size(), NULL);
	boost::shared_ptr<DataSchemaMultiRecord> dsch = boost::make_shared<DataSchemaMultiRecord>(); 
	dsch->setAttribute(AOSTAG_PUBLIC_DOC, "true");   
	dsch->setAttribute(AOSTAG_CTNR_PUBLIC, "true");
	dsch->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_SCHEMA_CONTAINER);
	dsch->setAttribute("jimo_objid", "dataschema_fixedlen_jimodoc_v0");
	dsch->setAttribute("zky_name", mName);
	dsch->setAttribute("zky_dataschema_type", mSchemaType);
	dsch->setAttribute("zky_otype", "dataschema");
	dsch->setAttribute("pattern", mDelimiter);
	dsch->setAttribute("last_entry_with_nopattern", "true");
	dsch->setAttribute("skip_invalid_records", "true");

	OmnString tmp;
	u64 max_len = 0;
	tmp << mSchemaPos;
	string pos = tmp.data();
	tmp = "";
	tmp << mSchemaLen;
	string len = tmp.data();

	dsch->setSchemaPicker(pos, len);
	AosXmlTagPtr dataRecordDoc;
	boost::shared_ptr<DataRecord> record;

	for (u32 i = 0; i < mPickers->size(); i++)
	{
		dataRecordDoc = getDoc(rdata, JQLTypes::eDataRecordDoc, (*mPickers)[i]->record);
		record = boost::make_shared<DataRecordFixbin>(dataRecordDoc);
		if (((*mPickers)[i]->offset) > (int)max_len) max_len = (*mPickers)[i]->offset;
		tmp = "";
		tmp <<  ((*mPickers)[i]->offset);
		// appendRecordPick(len, ken, record);
		dsch->appendRecordPick(tmp, (*mPickers)[i]->matchStr, record);
	}

	tmp = ""; 
	tmp << max_len;

	dsch->setAttribute("max_record_length", tmp);
	return AosXmlParser::parse(dsch->getConfig() AosMemoryCheckerArgs); 
}

AosXmlTagPtr
AosJqlStmtSchema::convertToXml(const AosRundataPtr &rdata)
{	
	//Gavin 2015/08/03 JIMODB-228
	mType.toLower();
	if (!(mType == "fixedlength" || mType == "univariable" || mType == "unilength" || mType == "rcd"))
	{
		OmnString errmsg;
		errmsg << " [ERR] : invalid TYPE '" << mType << "'!" ;
		rdata->setJqlMsg(errmsg); 
		return NULL;

	}
	if (mType == "fixedlength") {
		return generateMultiRecordConf(rdata);
	}

	OmnString jimoObjid = "";
	boost::shared_ptr<DataSchema> dsch = boost::make_shared<DataSchema>(); 
	dsch->setAttribute(AOSTAG_PUBLIC_DOC, "true");   
	dsch->setAttribute(AOSTAG_CTNR_PUBLIC, "true");
	dsch->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_SCHEMA_CONTAINER);

	if (mSchemaType == "static") {
		jimoObjid = "dataschema_univariable_jimodoc_v0";
	}

	if (mType == "unilength" || mType == "UNILENGTH") {
		jimoObjid = "dataschema_unilength_jimodoc_v0";
	}

	if (mType == "rcd" || mType == "RCD")
	{
		jimoObjid = "dataschema_record_jimodoc_v0";
	}

	dsch->setAttribute("zky_name", mName);
	dsch->setAttribute("zky_dataschema_type", mSchemaType);
	dsch->setAttribute("zky_otype", "dataschema");
	dsch->setAttribute("jimo_objid", jimoObjid);

	// Modify by Young, 2014/10/14
	AosXmlTagPtr dataRecordDoc = getDoc(rdata, JQLTypes::eDataRecordDoc, mRecord);
	if (!dataRecordDoc)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//OmnAlarm << __func__ << enderr;
		OmnString errMsg = "";
		errMsg << "\"" << mRecord << "\"" << " doesn't exsit!";
		rdata->setJqlMsg(errMsg);
		return NULL;
	}
	dsch->setRecord(dataRecordDoc->toString());
	return AosXmlParser::parse(dsch->getConfig() AosMemoryCheckerArgs); 
}

bool
AosJqlStmtSchema::showSchemas(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> schemas;
	if ( !getDocs(rdata, JQLTypes::eSchemaDoc, schemas) )
	{
		OmnString error;
		error << "No Data Schema Found. ";
		rdata->setJqlMsg(error);

		OmnCout << "No Data Schema Found. " << endl;
		return true;
	}

	int total = schemas.size() ;
	OmnString content;
	content << "<content total=\"" << total << "\"></content>"; 
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	for(u32 i=0; i<schemas.size(); i++)
	{
		AosXmlTagPtr schema = schemas[i];
		doc->addNode(schema);
	}
	rdata->setResults(content);

	vector<OmnString> fields;
	fields.push_back("zky_name");
	fields.push_back("type");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_name"] = "SchemaName";
	alias_name["zky_dataschema_type"] = "SchemaType";

	OmnString msg = printTableInXml(fields, alias_name, doc);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}
	setPrintData(rdata, msg);

	return true;
}

bool 
AosJqlStmtSchema::dropSchema(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eSchemaDoc, mName);
	bool rslt = deleteDocByObjid(rdata, objid);              
	if (!rslt)
	{                                                        
		msg << "Failed to delete the schema " << mName;
		rdata->setJqlMsg(msg);

		OmnCout << "Failed to delete the schema " << mName << endl;
		return false;                                        
	}                                                        
	msg << "Schema " << mName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	OmnCout << "Schema " << mName << " dropped successfully." << endl; 
	return true;
}

bool
AosJqlStmtSchema::describeSchema(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString objid = getObjid(rdata, JQLTypes::eSchemaDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);              
	if (!doc)
	{
		OmnString error;
		error << "Failed to display schema " << mName << " : not found.";
		rdata->setJqlMsg(error);
	
		OmnCout << "Failed to display schema " << mName << " : not found." << endl;
		return false;
	}

	mSchemaType = doc->getAttrStr("zky_dataschema_type");
	//output schema info based on static or dynamic 
	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;
	AosXmlParser parser;

	if (mSchemaType == "static")
	{
		AosXmlTagPtr record = doc->getFirstChild("datarecord");
		if (!record)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
	} 
	else
	{
		AosXmlTagPtr pickers = doc->getFirstChild("record_picker");
		if (!pickers)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}

		//output the record picker list
		cout << "Record Pickers: " << endl;
		fields.push_back("record");
		fields.push_back("offset");
		fields.push_back("match_str");
		alias_name["record"] = "RecordName";
		alias_name["offset"] = "Offset";
		alias_name["match_str"] = "MatchString";

		OmnString msg = printTableInXml(fields, alias_name, pickers);
		if (mContentFormat == "xml")
		{
			rdata->setResults(pickers->toString());
			return true;
		}
		setPrintData(rdata, msg);
	}

	return true;
}


AosJqlStatement *
AosJqlStmtSchema::clone()
{
	return OmnNew AosJqlStmtSchema(*this);
}


void 
AosJqlStmtSchema::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtSchema::setSchemaType(OmnString type)
{
	mSchemaType = type;
}


void 
AosJqlStmtSchema::setType(OmnString type)
{
	mType = type;
}


void 
AosJqlStmtSchema::setRecordType(OmnString type)
{
	mRecordType = type;
}

void 
AosJqlStmtSchema::setName(OmnString name)
{
	mName = name;
}

void 
AosJqlStmtSchema::setRecord(OmnString record)
{
	mRecord = record;
}

void 
AosJqlStmtSchema::setLineBreak(OmnString lineBreak)
{
	mLineBreak = lineBreak;
}

void 
AosJqlStmtSchema::setPickers(vector<AosJqlRecordPickerPtr> *pickers)
{
	mPickers = pickers;
}

void 
AosJqlStmtSchema::setComment(OmnString comment)
{
	mComment = comment;
}


void 
AosJqlStmtSchema::setRecordNames(AosExprList *names)
{
	mRecordNames = names;
}

void 
AosJqlStmtSchema::setRecordLen(u32 len)
{
	mMaxRecordLen = len;
}

void
AosJqlStmtSchema::setRecordDelimiter(OmnString delimiter)
{
	mDelimiter = delimiter;
}

void
AosJqlStmtSchema::setIgnoreSubPattern(bool rslt)
{
	mIsIgnoreSubPattern = rslt;
}

void
AosJqlStmtSchema::setSkipInvalidRecords(bool rslt)
{
	mIsSkipInvalidRecords = rslt;
}

void 
AosJqlStmtSchema::setSchemaPos(u32 pos)
{
	mSchemaPos = pos;
}

void 
AosJqlStmtSchema::setSchemaLen(u32 len)
{
	mSchemaLen = len;
}

