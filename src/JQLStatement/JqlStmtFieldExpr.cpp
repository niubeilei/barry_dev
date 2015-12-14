
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
#include "JQLStatement/JqlStmtFieldExpr.h"

#include "API/AosApi.h"
#include "AosConf/DataFieldExpr.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using AosConf::DataFieldExpr;
using boost::shared_ptr;
using boost::make_shared;


AosJqlStmtFieldExpr::AosJqlStmtFieldExpr()
{
	mFieldName = "";
	mFieldTypeStr = "";
	mMaxLen = "";
	mOffset = "";
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtFieldExpr::~AosJqlStmtFieldExpr()
{
}

bool
AosJqlStmtFieldExpr::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataField(rdata);
	if (mOp == JQLTypes::eShow) return showDataFields(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataField(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataField(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL VirtualField", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtFieldExpr::createDataField(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eVirtualField, mFieldName);
	if (doc)
	{
		msg << "Virtual field '" << mFieldName << "' is already exists.";
		rdata->setJqlMsg(msg);
		return true;
	}
	OmnString objid = getObjid(mFieldName);
	doc = convertToXml(rdata); 
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_VIRTUAL_FIELD_CONTAINER);   

	if (!createDoc(rdata, JQLTypes::eVirtualField, mFieldName, doc))
	{
		msg << "Failed to create Virtual Field doc." ;
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Virtual Field " << mFieldName << " created successfully.";
	setPrintData(rdata, msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtFieldExpr::convertToXml(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataFieldExpr> fieldObj = boost::make_shared<DataFieldExpr>();
	fieldObj->setAttribute("zky_name", mFieldName);
	fieldObj->setAttribute("virtual_field", "true");
	fieldObj->setExpr(mExprStr);
	OmnString doc_str = fieldObj->getConfig();

	AosXmlTagPtr tableDoc = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if (tableDoc)
	{
		tableDoc->setAttr("zky_record_len", (tableDoc->getAttrU32("zky_record_len", 0) + mMaxLen.toInt()));
		AosXmlTagPtr lf_column;

		OmnString schemaObjid = tableDoc->getAttrStr("zky_use_schema");
		AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
		aos_assert_r(schemaDoc, NULL);
		AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
		aos_assert_r(recordNode, NULL);

		AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");
		aos_assert_r(fieldsNode, NULL);

		AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
		while(fieldNode)
		{
			if (fieldNode->getAttrStr("name").toUpper() == "LF")
			{
				lf_column = fieldNode->clone(AosMemoryCheckerArgsBegin);
				fieldsNode->removeNode(fieldNode);
				mOffset = lf_column->getAttrStr("offset");
				u32 offset = mOffset.toInt() + mMaxLen.toInt();
				lf_column->setAttr("offset", offset);
				continue;
			}
			fieldNode = fieldsNode->getNextChild();
		}
		OmnString column_str;
		column_str << "<column name=\"" << mFieldName
		<< "\" zky_name=\"" << mFieldName
		<< "\" datatype=\"" << mFieldTypeStr 
		<< "\" sdatatype=\"" << mDataType
		<< "\" size=\"" << mMaxLen 
		<< "\" offset=\"" << mOffset
		<< "\" type=\"" << "expr" 
		<< "\" zky_otype=\"" << "datafield" 
		<< "\" >";
		//column_str << "<keys>";
		//column_str << "</keys>";
		column_str << "<expr><![CDATA[";
		column_str << mExprStr;
		column_str << "]]></expr>";
		column_str << "</column>";

		AosXmlTagPtr columns = tableDoc->getFirstChild("columns");
		aos_assert_r(columns, NULL);
		AosXmlTagPtr column =  AosXmlParser::parse(column_str AosMemoryCheckerArgs);
		columns->addNode(column);
		if (lf_column) columns->addNode(lf_column);

		// Modify dataschema node
		//AosXmlTagPtr v_field = AosXmlParser::parse(doc_str AosMemoryCheckerArgs); 
		//aos_assert_r(v_field, 0);

		//OmnString schemaObjid = tableDoc->getAttrStr("zky_use_schema");
		//AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
		//aos_assert_r(schemaDoc, 0);
		//AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
		//AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");

		// convert to real field
		OmnString r_field = "";
		r_field << "<datafield type=\"" << mDataType << "\" "
			<< "zky_length=\"" << mMaxLen << "\" " 
			<< "zky_name=\"" << mFieldName << "\" " 
			<< "offset=\"" << mOffset << "\"></datafield>";

		AosXmlTagPtr v_field = AosXmlParser::parse(r_field AosMemoryCheckerArgs); 
		aos_assert_r(v_field, 0);

		fieldsNode->addNode(v_field);
		if (schemaDoc->getAttrStr("is_new_scheam") == "true")
		{
			modifyDoc(rdata, schemaDoc);	
		}
		else
		{
			OmnString name = schemaDoc->getAttrStr("zky_name");
			OmnString new_schema_name = "_ztdada_";
			new_schema_name << mTableName << "_" << name;

			schemaDoc->setAttr("is_new_scheam", "true");
			schemaDoc->setAttr("zky_name", new_schema_name);
			createDoc(rdata, JQLTypes::eSchemaDoc, new_schema_name, schemaDoc);
			tableDoc->setAttr("zky_use_schema", AosObjid::getObjidByJQLDocName(JQLTypes::eSchemaDoc, new_schema_name));  
		}

		bool rslt = modifyDoc(rdata, tableDoc);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}
	}
	return AosXmlParser::parse(doc_str AosMemoryCheckerArgs); 
}

bool
AosJqlStmtFieldExpr::showDataFields(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	if ( !getDocs(rdata, JQLTypes::eVirtualField, dfields) )
	{
		msg << "No Virtual Field Found! " ;
		rdata->setJqlMsg(msg);
		return true;
	}
	int total = dfields.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<dfields.size(); i++)
	{
		OmnString vv = dfields[i]->getAttrStr("zky_name", "");
		if (vv != "")
		{
			content << "<record name=\"" << vv << "\"/>";
		}
	}
	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml") 
	{
		rdata->setResults(content);
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("name");
	map<OmnString, OmnString> alias_name;
	alias_name["name"] = "FieldName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtFieldExpr::dropDataField(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(mFieldName);                   
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to drop virtual field doc.";  
		rdata->setJqlMsg(msg);

		return false;                                       
	}                                                       

	msg << "Data Field " << mFieldName << " dropped successfully."; 
	rdata->setJqlMsg(msg);

	return true;
}


bool
AosJqlStmtFieldExpr::describeDataField(const AosRundataPtr &rdata)
{
	if (mFieldName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(mFieldName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if ( !doc ) 
	{
		msg << "Failed to display virtual field " << mFieldName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back("zky_length");
	fields.push_back("zky_offset");
	fields.push_back("zky_map_name");

	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["zky_length"] = "Length";
	alias_name["zky_offset"] = "Offset";
	alias_name["zky_map_name"] = "Map";

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


OmnString
AosJqlStmtFieldExpr::getObjid(const OmnString name)
{
	OmnString objid = "";
	objid << AOSZTG_VIRTUAL_FIELD_IIL << "_" << name;
	return objid;
}

AosJqlStatement *
AosJqlStmtFieldExpr::clone()
{
	return OmnNew AosJqlStmtFieldExpr(*this);
}


void 
AosJqlStmtFieldExpr::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtFieldExpr::setName(OmnString name)
{
	mFieldName = name;
	AosJqlUtil::escape(mFieldName);
}


void
AosJqlStmtFieldExpr::setType(OmnString type)
{
	if (type == "join") 
	{
		mFieldTypeStr = "iilmap";
	}
	else
	{
		mFieldTypeStr = type;
	}
}

void
AosJqlStmtFieldExpr::setDataType(AosJQLDataFieldTypeInfo* type)
{
	if (type) mDataType = type->getType();
}

void 
AosJqlStmtFieldExpr::setMaxLen(int64_t len)
{
	if (len == 0)
	{
		mMaxLen = "";
	}
	else
	{
		mMaxLen << len;
	}
}


void
AosJqlStmtFieldExpr::setOffset(int64_t len)
{
	if (len == -1)
	{
		mOffset = "";
	}
	else
	{
		mOffset << len;
	}
}


void 
AosJqlStmtFieldExpr::setTable(OmnString name)
{
	mTableName = name;
}


void 
AosJqlStmtFieldExpr::setExpr(AosExprObj* expr)
{
	mExprStr = expr->dump();
}
