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
#include "JQLStatement/JqlStmtVirtualField.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "JQLStatement/JqlStmtMap.h"
#include "JQLStatement/JqlStmtTable.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

AosJqlStmtVirtualField::AosJqlStmtVirtualField(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mFieldName = "";
	mFieldTypeStr = "";
	mMaxLen = "";
	mOffset = "";
	mFields = 0;
	mOp = JQLTypes::eOpInvalid;
	mIsInfoField = false;
}


AosJqlStmtVirtualField::AosJqlStmtVirtualField()
{
	mFieldName = "";
	mFieldTypeStr = "";
	mMaxLen = "";
	mOffset = "";
	mFields = 0;
	mOp = JQLTypes::eOpInvalid;
	mIsInfoField = false;
}

AosJqlStmtVirtualField::~AosJqlStmtVirtualField()
{
	OmnDelete(mFields);
}

bool
AosJqlStmtVirtualField::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
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

	if (mOp == JQLTypes::eCreate) return createDataField(rdata);
	if (mOp == JQLTypes::eShow) return showDataFields(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataField(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataField(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL VirtualField", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtVirtualField::createDataField(const AosRundataPtr &rdata)
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
	OmnString objid = getObjid(rdata, JQLTypes::eVirtualField, mFieldName);
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
AosJqlStmtVirtualField::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc_str;
	doc_str << "<datafield zky_name=\"" << mFieldName << "\"";
	doc_str << " zky_isinfofield=\"" << mIsInfoField << "\"";
	if (mMaxLen != "") doc_str << " zky_length=\"" << mMaxLen << "\"";
	if (mOffset != "") doc_str << " zky_offset=\"" << mOffset << "\"";

	// Add by Young, for number type
	if (mDataType == "number")
	{
		doc_str << " zky_precision=\"" << mDataTypeInfo->getFirst() << "\"";
		doc_str << " zky_scale=\"" << mDataTypeInfo->getSecond() << "\"";
	}

	doc_str << " zky_otype=\"datafield\" type=\"" << mFieldTypeStr << "\""
		<< " zky_map_name=\""<< getObjid(rdata, JQLTypes::eMap, mMapName) << "\" virtual_field=\"true\""  
		<< " >";
	doc_str << "<keys>";
	for (u32 i =0;  i < mFields->size(); i++)
	{
		doc_str << "<key>" << (*mFields)[i]->dump() << "</key>";
	}
	doc_str << "</keys>";
	doc_str << "</datafield>";
	AosXmlTagPtr table = AosJqlStmtTable::getTable(mTableName, rdata);
	if (table)
	{
		table->setAttr("zky_record_len", (table->getAttrU32("zky_record_len", 0) + mMaxLen.toInt()));
		AosXmlTagPtr lf_column;
		AosXmlTagPtr map_conf = getDoc(rdata, JQLTypes::eMap, mMapName);
		if (!map_conf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}                                                     
		map_conf->removeMetadata();
		//map_conf->setAttr("datatype", mFieldTypeStr);   
		map_conf->setAttr("datatype", mDataType);   
		if (mDataType == "number")
		{
			map_conf->setAttr("zky_precision", mDataTypeInfo->getFirst());
			map_conf->setAttr("zky_scale", mDataTypeInfo->getSecond());
		}

		AosXmlTagPtr columns = table->getFirstChild("columns");
		if (!columns)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr column = columns->getFirstChild(true);
		if (!column)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		while(column)
		{
			if (column->getAttrStr("name").toUpper() == "LF")
			{
				lf_column = column->clone(AosMemoryCheckerArgsBegin);
				columns->removeNode(column);
				mOffset = lf_column->getAttrStr("offset");
				u32 offset = mOffset.toInt() + mMaxLen.toInt();
				lf_column->setAttr("offset", offset);
				continue;
			}
			column = columns->getNextChild();
		}

		//int precision = -1;
		//int scale = -1;
		//if (mDataTypeInfo)
		//{
		//	if (mDataType == "number")
		//	{
		//		precision = mDataTypeInfo->getFirst();
		//		scale = mDataTypeInfo->getSecond();
		//	}
		//}


		OmnString column_str;
		column_str << "<column name=\"" << mFieldName
		<< "\" datatype=\"" << mFieldTypeStr 
		<< "\" zky_valdftvalue=\"" << mDefault 
		<< "\" size=\"" << mMaxLen 
		<< "\" offset=\"" << mOffset
		<< "\" type=\"virtual\"";
		// Add by Young, 2015/03/03
		if (mDataType == "number")
		{
			column_str << " zky_precision=\"" << mDataTypeInfo->getFirst() << "\"";
			column_str << " zky_scale=\"" << mDataTypeInfo->getSecond() << "\"";
		}
		column_str << ">";

		column_str << "<keys>";
		for (u32 i =0;  i < mFields->size(); i++)
		{
			column_str << "<key>" << (*mFields)[i]->dump() << "</key>";
		}
		column_str << "</keys>";
		column_str << map_conf->toString();
		column_str << "</column>";
		column =  AosXmlParser::parse(column_str AosMemoryCheckerArgs);
		columns->addNode(column);
		if (lf_column)
			columns->addNode(lf_column);

		bool rslt = modifyDoc(rdata, table);
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
AosJqlStmtVirtualField::showDataFields(const AosRundataPtr &rdata)
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
AosJqlStmtVirtualField::dropDataField(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eVirtualField, mFieldName);
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
AosJqlStmtVirtualField::describeDataField(const AosRundataPtr &rdata)
{
	if (mFieldName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eVirtualField, mFieldName);
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


AosJqlStatement *
AosJqlStmtVirtualField::clone()
{
	return OmnNew AosJqlStmtVirtualField(*this);
}


void 
AosJqlStmtVirtualField::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtVirtualField::setName(OmnString name)
{
	mFieldName = name;
	AosJqlUtil::escape(mFieldName);
}


void
AosJqlStmtVirtualField::setType(OmnString type)
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


//void
//AosJqlStmtVirtualField::setDataType(OmnString type)
//{
//	mDataType = type;
//}


void
AosJqlStmtVirtualField::setDataType(AosJQLDataFieldTypeInfo* type)
{
	if (type)
	{
		mDataTypeInfo = type;
		mDataType = type->getType();
	}
	
	//mDataType = type;
}


void 
AosJqlStmtVirtualField::setMaxLen(int64_t len)
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
AosJqlStmtVirtualField::setOffset(int64_t len)
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
AosJqlStmtVirtualField::setDefault(OmnString dft)
{
	mDefault = dft;
}

void 
AosJqlStmtVirtualField::setTable(OmnString name)
{
	mTableName = name;
}


void
AosJqlStmtVirtualField::setMapName(OmnString name)
{
	mMapName = name;
}


void 
AosJqlStmtVirtualField::setFields(AosExprList *expr_list)
{
	mFields = expr_list;
}

