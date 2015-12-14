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
#include "JQLStatement/JqlStmtNickField.h"

#include "API/AosApi.h"
#include "AosConf/DataFieldMath.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "JQLStatement/JqlStmtMap.h"
#include "JQLStatement/JqlStmtTable.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace AosConf;
using boost::shared_ptr;
using boost::make_shared;

AosJqlStmtNickField::AosJqlStmtNickField()
{
	mFieldName = "";
	mFieldTypeStr = "math";
	mTableName = "";
	mOrigFieldName = "";
	mMaxLen = 50;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtNickField::~AosJqlStmtNickField()
{
}


bool
AosJqlStmtNickField::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataField(rdata);
	if (mOp == JQLTypes::eShow) return showDataFields(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataFields(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataField(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL VirtualField", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtNickField::createDataField(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString objid = getObjid(rdata, JQLTypes::eDataFieldDoc, mFieldName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		OmnString error;
		error << "Nick Field " << mFieldName << " already exists. "
			  << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(error);
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc) return false;

	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_VIRTUAL_FIELD_CONTAINER);   

	if ( !createDoc(rdata, JQLTypes::eDataFieldDoc, mFieldName, doc) )
	{
		OmnString error;
		error << "Failed to create Virtual Field doc." ;
		rdata->setJqlMsg(error);
		return false;
	}

	OmnString msg;
	msg << "Virtual Field " << mFieldName << " created successfully.";
	setPrintData(rdata, msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtNickField::convertToXml(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataFieldMath> fieldObj = boost::make_shared<DataFieldMath>();
	if (mConfParms)
	{
		return getConf(rdata, fieldObj, mConfParms);
	}

	fieldObj->setAttribute("type", "math");
	fieldObj->setAttribute("zky_name", mFieldName);
	fieldObj->setAttribute("zky_length", mMaxLen);
	fieldObj->setAttribute("zky_isconst", "true");
	fieldObj->setAttribute("zky_opr", "concat");
	fieldObj->setAttribute("zky_ignore_serialize", "true");
	fieldObj->setFieldName(mOrigFieldName);
	OmnString doc_str = fieldObj->getConfig();

	AosXmlTagPtr table = AosJqlStmtTable::getTable(mTableName, rdata);
	if (table)
	{
		// modify record doc
		OmnString nickfieldStr = "";
		nickfieldStr << "<datafield zky_name=\"" << mFieldName << "\"></datafield>";
		AosXmlTagPtr nickFieldNode = AosXmlParser::parse(nickfieldStr AosMemoryCheckerArgs);
		if (!nickFieldNode)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		OmnString schemaObjid = table->getAttrStr("zky_use_schema");
		AosXmlTagPtr schema_doc = getDocByObjid(rdata, schemaObjid); 
		if (!schema_doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr record_tag = schema_doc->getFirstChild("record");
		if (!record_tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		OmnString recordname = record_tag->getAttrStr("zky_name");
		AosXmlTagPtr record_doc = getDoc(rdata, JQLTypes::eDataRecordDoc, recordname);
		if (!record_doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr datafieldsNode = record_doc->getFirstChild("datafields");
		if (!datafieldsNode)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		datafieldsNode->addNode(nickFieldNode);

		bool rslt = modifyDoc(rdata, record_doc);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
	}

	return AosXmlParser::parse(doc_str AosMemoryCheckerArgs); 
}

bool
AosJqlStmtNickField::showDataFields(const AosRundataPtr &rdata)
{
	//OmnString msg;
	//vector<AosXmlTagPtr> dfields;
	//bool rslt = listVirtualFields(rdata, dfields);
	//if(!rslt)
	//{
	//	msg << "No Virtual Field Found! " ;
	//	rdata->setJqlMsg(msg);
	//	return true;
	//}
	//int total = dfields.size() ; 
	//OmnString content;
	//content << "<content total=\"" << total << "\">";
	//for(u32 i=0; i<dfields.size(); i++)
	//{
	//	OmnString vv = dfields[i]->getAttrStr("zky_name", "");
	//	if (vv != "")
	//	{
	//		content << "<record name=\"" << vv << "\"/>";
	//	}
	//}
	//content << "</content>";
	//rdata->setResults(content);
	//if (mContentFormat == "xml") 
	//{
	//	rdata->setResults(content);
	//	return true;
	//}
	//vector<OmnString> fields;
	//fields.push_back("name");
	//map<OmnString, OmnString> alias_name;
	//alias_name["name"] = "FieldName";

	//AosXmlParser parser;
	//AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	//msg = printTableInXml(fields, alias_name, doc);
	//setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtNickField::dropDataFields(const AosRundataPtr &rdata)
{
	//OmnString msg;
	//OmnString objid = getObjid(mFieldName);                   
	//if ( !deleteDocByObjid(objid) )
	//{                                                       
	//	msg << "Failed to drop nick field doc.";  
	//	rdata->setJqlMsg(msg);

	//	return false;                                       
	//}                                                       

	//msg << "Data Field " << mFieldName << " dropped successfully."; 
	//rdata->setJqlMsg(msg);

	return true;
}


bool
AosJqlStmtNickField::describeDataField(const AosRundataPtr &rdata)
{
	//if (!mFieldName != "")
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return false;
	//} 

	//OmnString msg;
	//string objid = getObjid(mFieldName);
	//if ( !getDocByObjid(objid) )
	//{
	//	msg << "Failed to display nick field " << mFieldName << " : not found.";
	//	rdata->setJqlMsg(msg);
	//	return false;
	//}

	//OmnString str_doc = mJmoRundata.str_value;
	//AosXmlTagPtr doc = AosXmlParser::parse(str_doc AosMemoryCheckerArgs);

	//vector<OmnString> fields;
	//map<OmnString, OmnString> alias_name;

	//fields.push_back("zky_name");
	//fields.push_back("type");
	//fields.push_back("zky_length");
	//fields.push_back("zky_offset");
	//fields.push_back("zky_map_name");

	//alias_name["zky_name"] = "Name";
	//alias_name["type"] = "Type";
	//alias_name["zky_length"] = "Length";
	//alias_name["zky_offset"] = "Offset";
	//alias_name["zky_map_name"] = "Map";

	//OmnString content = "<content>";
	//content << doc->toString() << "</content>";
	//if (mContentFormat == "xml")
	//{
	//	rdata->setResults(content);
	//	return true;
	//}

	//AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	//msg = printTableInXml(fields, alias_name, def);
	//setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtNickField::clone()
{
	return OmnNew AosJqlStmtNickField(*this);
}


void 
AosJqlStmtNickField::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtNickField::setName(OmnString name)
{
	mFieldName = name;
	AosJqlUtil::escape(mFieldName);
}


void 
AosJqlStmtNickField::setTable(OmnString name)
{
	mTableName = name;
}


void 
AosJqlStmtNickField::setOrigFieldName(OmnString name)
{
	mOrigFieldName = name;
}


void 
AosJqlStmtNickField::setMaxLen(int len)
{
	if (len <= 0) len = 50;
	mMaxLen = len;
}

