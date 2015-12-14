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
#include "JQLStatement/JqlStmtDataField.h"

#include "API/AosApi.h"
#include "AosConf/DataFieldExpr.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprMemberOpt.h"
#include "JQLExpr/ExprNameValue.h"
#include "JQLStatement/JqlStmtMap.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/JqlUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

std::map<OmnString,OmnString>  AosJqlStmtDataField::mFieldTypes;

AosJqlStmtDataField::AosJqlStmtDataField(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mIsVirtual = false;
	mFieldName = "";
	mFieldType = "";
	mIsVirtual = false;
	mMaxLen = 0;
	mOffset = "";
	mDefValue = "";
	mIsVirtual = false;
	mOp = JQLTypes::eOpInvalid;
	mMappingExprList = NULL;
}

AosJqlStmtDataField::AosJqlStmtDataField()
{
	mIsVirtual = false;
	mFieldName = "";
	mFieldType = "";
	mIsVirtual = false;
	mMaxLen = 0;
	mOffset = "";
	mDefValue = "";
	mIsVirtual = false;
	mMappingExprList = NULL;
	mOp = JQLTypes::eOpInvalid;
	initVector();
}


AosJqlStmtDataField::~AosJqlStmtDataField()
{
}


void
AosJqlStmtDataField::initVector()
{
	mFieldTypes.insert(make_pair("u64","u64"));
	mFieldTypes.insert(make_pair("int64","int64"));
	mFieldTypes.insert(make_pair("str","str"));
	mFieldTypes.insert(make_pair("expr","expr"));
	//Gavin 2015/07/31 JIMODB-153
	//mFieldTypes.insert(make_pair("number","number"));
	//mFieldTypes.insert(make_pair("bigint","int64"));
	mFieldTypes.insert(make_pair("double","double"));
	mFieldTypes.insert(make_pair("datetime","datetime"));
	//mFieldTypes.insert(make_pair("bin_u64","bin_u64"));
	//mFieldTypes.insert(make_pair("bin_int64","bin_int64"));
	//mFieldTypes.insert(make_pair("bin_double","bin_double"));
	//mFieldTypes.insert(make_pair("bin_datetime","bin_datetime"));
	//mFieldTypes.insert(make_pair("varchar","str"));
	//mFieldTypes.insert(make_pair("tinyblob","str"));
	//mFieldTypes.insert(make_pair("tinytext","str"));
	//mFieldTypes.insert(make_pair("blob","str"));
	//mFieldTypes.insert(make_pair("text","str"));
	//mFieldTypes.insert(make_pair("mediumblob","str"));
	//mFieldTypes.insert(make_pair("mediumtext","str"));
	mFieldTypes.insert(make_pair("string","str"));
	mFieldTypes.insert(make_pair("docid","docid"));
}


bool
AosJqlStmtDataField::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		//AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		//OmnAlarm << __func__ << enderr;
		rdata->setJqlMsg(mErrmsg);
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataField(rdata);
	if (mOp == JQLTypes::eShow) return showDataFields(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataField(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataField(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL DataField", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDataField::createDataField(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString objid = getObjid(rdata, JQLTypes::eDataFieldDoc, mFieldName);
	AosXmlTagPtr doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr(AOSTAG_OBJID, objid);
	if ( !createDoc(rdata, JQLTypes::eDataFieldDoc, mFieldName, doc) )
	{
		//OmnString error;
		//error << "Failed to create DataField doc." ;
		//rdata->setJqlMsg(error);
		return false;
	}

	//OmnString msg;
	//msg << "Data Field " << mFieldName << " created successfully.";
	//setPrintData(rdata, msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDataField::convertToXml(const AosRundataPtr &rdata)
{
	OmnString fieldConf = getFieldConf(rdata);
	AosXmlTagPtr doc = AosXmlParser::parse(fieldConf AosMemoryCheckerArgs); 
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	if (mMappingExprList)
	{
		for (size_t i=0; i<mMappingExprList->size(); i++)
		{
			AosExprObjPtr expr = (*mMappingExprList)[i];	
			aos_assert_r(expr->getType() == AosExprType::eMemberOpt, 0);
			AosExprMemberOptPtr member = dynamic_cast<AosExprMemberOpt*>(expr.getPtr());
			OmnString recordname = member->getMember2();
			OmnString fieldname = member->getMember3();
			OmnString value_str = "";
			value_str << "<value zky_field_name=\"" << fieldname << "\" zky_record_name=\"" << recordname << "\"></value>";
			AosXmlTagPtr v = AosXmlParser::parse(value_str AosMemoryCheckerArgs);
			doc->addNode(v);
		}
		doc->setAttr("type", "virtual");
		doc->setAttr("data_type", mFieldType);
	}
	return doc;
}

bool
AosJqlStmtDataField::showDataFields(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	if( !getDocs(rdata, JQLTypes::eDataFieldDoc, dfields) )
	{
		msg << "No Data Field Found! " ;
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
	alias_name["name"] = "DataFieldName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);

	return true;
}

bool 
AosJqlStmtDataField::dropDataField(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataFieldDoc, mFieldName);                   
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to drop data field doc.";  
		rdata->setJqlMsg(msg);
		return false;                                       
	}                                                       

	msg << "Data Field " << mFieldName << " dropped successfully."; 
	rdata->setJqlMsg(msg);

	return true;
}


bool
AosJqlStmtDataField::describeDataField(const AosRundataPtr &rdata)
{
	if (mFieldName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 
	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDataFieldDoc, mFieldName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display data field " << mFieldName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back("zky_length");
	fields.push_back("zky_offset");
	fields.push_back("idfamily");
	//fields.push_back("zky_def_value");
	fields.push_back(AOSTAG_VALUE_DFT_VALUE);

	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["zky_length"] = "Length";
	alias_name["zky_offset"] = "Offset";
	alias_name["idfamily"] = "IdFamily";
	//alias_name["zky_def_value"] = "DefValue";
	alias_name[AOSTAG_VALUE_DFT_VALUE] = "DefValue";

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
AosJqlStmtDataField::clone()
{
	return OmnNew AosJqlStmtDataField(*this);
}


void 
AosJqlStmtDataField::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtDataField::setName(OmnString name)
{
	mFieldName = name;
	AosJqlUtil::escape(mFieldName);
}


void
AosJqlStmtDataField::setType(OmnString type)
{
	type.toLower();
	if (type == "join") 
	{
		mFieldType = "iilmap";
	}
	else
	{
		mFieldType = type;
	}
}


bool 
AosJqlStmtDataField::setType(AosJQLDataFieldTypeInfo *typeInfo)
{
	mFieldTypeInfo = typeInfo;
	mFieldType = typeInfo->getType();
	mFieldType.toLower();
	map<OmnString, OmnString>::iterator itr = mFieldTypes.find(mFieldType);
	//for(size_t i = 0; i < 22; i++)
	//{
	//	if(mFieldTypes[i] == mFieldType) break;
	//	if(i == 21)
	if(itr == mFieldTypes.end())
	{
		//Gavin 2015/07/31 JIMODB-153
		mErrmsg << "[ERR] : datafield '" << mFieldName <<  "' has illegal datatype '" << mFieldType << "'";	
		return false;
	}
	mFieldType = itr->second;
	mFieldType = AosJType2DType(mFieldType);
	if (mFieldType == "join") mFieldType = "iilmap";
	else if (mFieldType == "str" || mFieldType == "string" || 
			mFieldType == "text" || mFieldType == "longtext")
		mFieldType = "str";	
	else if (mFieldType == "bit" || mFieldType == "tinyint" 
			|| mFieldType == "smallint" || mFieldType == "mediumint" 
			|| mFieldType == "int")
		mFieldType = "int32";
	else if (mFieldType == "bigint") mFieldType = "int64";
	else if (mFieldType == "float") mFieldType = "double";
	else if (mFieldType == "decimal") mFieldType = "number";
	return true;
}


void 
AosJqlStmtDataField::setMaxLen(int64_t len)
{
	if (len >= 0) mMaxLen = len;
}


void 
AosJqlStmtDataField::setIdfamily(OmnString idfamily)
{	
	mIdfamily = idfamily;
}


void
AosJqlStmtDataField::setOffset(int64_t len)
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
AosJqlStmtDataField::setDefValue(OmnString def_value)
{
	mDefValue = def_value;
}

void 
AosJqlStmtDataField::setFromFieldName(OmnString fieldname)
{
	mFromFieldName = fieldname;
}

void 
AosJqlStmtDataField::setFromName(OmnString name)
{
	mFromName = name;
}


void 
AosJqlStmtDataField::setToName(OmnString name)
{	
	mToName = name;
}

void 
AosJqlStmtDataField::setNOTNULL(OmnString notnull)
{	
	mNOTNULL = notnull;
}

void 
AosJqlStmtDataField::setIILName(OmnString iilname)
{
	mIILName = iilname;
}
void 
AosJqlStmtDataField::setNeedSplit(bool flag)
{
	mNeedSplit = "false";
	if (flag) mNeedSplit = "true";
}
void 
AosJqlStmtDataField::setNeedSwap(bool flag)
{
	mNeedSwap = "false";
	if (flag) mNeedSwap = "true";
}

void 
AosJqlStmtDataField::setUseKeyAsValue(bool flag)
{
	mUseKeyAsValue = "false";
	if (flag) mUseKeyAsValue = "true";
}


void 
AosJqlStmtDataField::setTable(OmnString name)
{
	mTableName = name;
}


void
AosJqlStmtDataField::setMapName(OmnString name)
{
	mMapName = name;
}


void 
AosJqlStmtDataField::isVirtual()
{
	mIsVirtual = true;
}

OmnString 
AosJqlStmtDataField::getIndexName(
		const AosRundataPtr &rdata, 
		const OmnString name)
{
	OmnString index_objid = "";
	index_objid << AOSZTG_INDEXMGR_IIL << "_" << name;

	AosXmlTagPtr doc = getDocByObjid(rdata, index_objid);
	//if (!doc)
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return "";
	//}

	OmnString iilname = doc->getAttrStr(AOSTAG_IILNAME);
	if (iilname == "")
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	return iilname;
}


u64 
AosJqlStmtDataField::getFieldLen(const OmnString &type)
{
	//if (mMaxLen > 0 && type == "str") return mMaxLen;
	if (mMaxLen > 0) return mMaxLen;
	if (type == "u64" || type == "int64" || type == "bin_u64") return sizeof(u64);
	if (type == "u32" || type == "int32" || type == "int") return sizeof(u32);
	if (type == "double") return sizeof(double);
	if (type == "number") return sizeof(double);
	if (type == "float") return sizeof(float);

	return 50;
}


OmnString
AosJqlStmtDataField::getFieldConf(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataFieldExpr> dfield = boost::make_shared<DataFieldExpr>();
	dfield->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_FIELD_CONTAINER);   
	dfield->setAttribute("zky_name", mFieldName);
	dfield->setAttribute("zky_otype", "datafield");
	dfield->setAttribute("type", mFieldType);
	dfield->setAttribute("zky_isinfofield", mIsInfoField);
	if (mFieldType == "number")
	{
		dfield->setAttribute("zky_precision", mFieldTypeInfo->getFirst());
		dfield->setAttribute("zky_scale", mFieldTypeInfo->getSecond());
	}
	if (mFieldType == "expr")
	{
		dfield->setAttribute("data_type", mDataType);
		dfield->setExpr(mValue);
	}
	else
	{
		dfield->setAttribute("zky_offset", mOffset);
	}

	if (mIsVirtual)
	{
		string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eMap, mMapName);
		if (!getDocByObjid(rdata, objid))
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		dfield->setAttribute("zky_map_name", objid);
		dfield->setAttribute("virtual_field", "true");
		return dfield->getConfig();
	}

	u64 fieldLen = getFieldLen(mFieldType);	
	if (mFieldType == "number")
	{
		//fieldLen = mFieldTypeInfo->getFirst() + 3; // "-" "." "\0"
		if (mMaxLen > 0) fieldLen = mMaxLen;	// Young, 2015/04/17, hard code from fixbin 
	}

	if (fieldLen > 0 && mFieldType != "docid") dfield->setAttribute("zky_length", fieldLen);

	dfield->setAttribute("idfamily", mIdfamily);
	dfield->setAttribute(AOSTAG_VALUE_DFT_VALUE, mDefValue);
	if (mFieldType == "iilmap")
	{
		dfield->setAttribute("zky_iilname", getIndexName(rdata, mIILName));
		dfield->setAttribute("zky_needswap", mNeedSwap);
		dfield->setAttribute("zky_needsplit", mNeedSplit);
		dfield->setAttribute("zky_use_key_as_value", mUseKeyAsValue);
		dfield->setAttribute("zky_sep", "0x01");
	}

	AosJqlUtil::escape(mFromFieldName);
	dfield->setAttribute("zky_value_from_field", mFromFieldName);
	dfield->setAttribute("zky_origtmfmt", mFromName);
	dfield->setAttribute("zky_targettmfmt", mToName);
	if (mFieldType == "str") dfield->setAttribute("zky_datatooshortplc", "cstr");
	if (mNOTNULL == "true") dfield->setAttribute("zky_notnull", "true");
	if (mFieldFormater != "") dfield->setAttribute("format", mFieldFormater);

	return dfield->getConfig();
}


void
AosJqlStmtDataField::setValue(OmnString value)
{
	mValue = value; 
}

void 
AosJqlStmtDataField::setValue(AosExprObj *expr)
{
	aos_assert(expr);
	mValue = expr->dump();
	if (mValue.length() > 0 && mValue[0] == '@')
	{
		AosValueRslt v;
		bool rslt = expr->getValue(OmnApp::getRundata().getPtr(), 0, v);
		aos_assert(rslt);
		mValue = v.getStr();
	}
}


void 
AosJqlStmtDataField::setDataType(AosJQLDataFieldTypeInfo *type)
{
	if (type)
	{
		mDataType = type->getType();
	}
}


void 
AosJqlStmtDataField::setMapping(AosExprList *expr)
{
	if (expr) mMappingExprList = expr;
}


void 
AosJqlStmtDataField::setFormater(OmnString formater)
{
	mFieldFormater = formater;
}
