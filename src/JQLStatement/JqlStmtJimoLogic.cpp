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
#include "JQLStatement/JqlStmtJimoLogic.h"

#include "JQLStatement/JqlStmtTable.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

AosJqlStmtJimoLogic::AosJqlStmtJimoLogic()
{
	mLogicName = "";
	mAsName = "";
	mTableName = "";
	mEndPoint = "";
	mTime = "";
	mCheckPoint = "";
	mMatrixName = "";
	mMatrix2Name = "";
	mResultsName = "";
}

AosJqlStmtJimoLogic::~AosJqlStmtJimoLogic()
{
}


bool
AosJqlStmtJimoLogic::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createLogic(rdata);
	if (mOp == JQLTypes::eShow) return showLogics(rdata);
	if (mOp == JQLTypes::eDrop) return dropLogic(rdata);
	if (mOp == JQLTypes::eDescribe) return describeLogic(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Logic", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtJimoLogic::createLogic(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	AosXmlTagPtr tableDoc = AosJqlStmtTable::getTable(mTableName, rdata);
	if (!tableDoc)
	{
		msg << "Table '" << mTableName << "' not exist";
		rdata->setJqlMsg(msg);
		return true;
	}
	
	// 1. add node on table 
	OmnString logicStr = "";
	OmnString objid = getObjid(mLogicName);
	logicStr 
		<< "<matrix zky_name=\"" << mLogicName << "\" zky_objid=\"" << objid << "\" zky_iilname=\"_zt44_idx_" << mLogicName << "\" zky_type=\"iilpatternopr\">"
		<< 	"<columns>"
		<< 		"<endpoint><![CDATA[" << mEndPoint << "]]></endpoint>"
		<< 		"<time><![CDATA[" << mTime << "]]></time>"
		<< 		"<checkpoint><![CDATA[" << mCheckPoint << "]]></checkpoint>"
		<< 	"</columns>"
		<< 	"<matrix><![CDATA[" << mMatrixName << "]]></matrix>";

	// felicia, 2014/10/17, for overspeed
	if(mMatrix2Name != "")
	{
		logicStr <<	"<matrix2><![CDATA[" << mMatrix2Name << "]]></matrix2>";
	}

	logicStr 
		<< "<results><![CDATA[" << mResultsName << "]]></results>"
		<< "</matrix>";
	AosXmlTagPtr doc = AosXmlParser::parse(logicStr AosMemoryCheckerArgs);
	if (!doc) 
	{
		msg << "Jimo logic '" << mLogicName << "' create failed";
		rdata->setJqlMsg(msg);
		return false;
	}
	AosXmlTagPtr doc_clone = doc->clone(AosMemoryCheckerArgsBegin);
	tableDoc->addNode(doc);

	bool rslt = modifyDoc(rdata, tableDoc);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	// 2. save doc
	doc_clone->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_MAP_CONTAINER);   
	if ( !createDocByObjid(rdata, doc_clone, objid) )
	{
		msg << "Failed to create Logic doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Logic " << mLogicName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtJimoLogic::showLogics(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	//bool rslt = listLogics(rdata, dfields);
	bool rslt = true;
	if(!rslt)
	{
		msg << "No Logic Found! " ;
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
	alias_name["name"] = "LogicName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtJimoLogic::describeLogic(const AosRundataPtr &rdata)
{
	if (mLogicName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(mLogicName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if ( !doc )
	{
		msg << "Fail to get lgoic '" << mLogicName << "' doc";
		rdata->setJqlMsg(msg);
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("zky_type");
	fields.push_back("datatype");

	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["datatype"] = "DataType";

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


bool
AosJqlStmtJimoLogic::dropLogic(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	string objid = getObjid(mLogicName);                    
	if ( !deleteDocByObjid(rdata, mLogicName) )
	{                                                       
		msg << "Failed to delete Logic doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop Logic " << mLogicName << " scessful";      
	rdata->setJqlMsg(msg);                                  
	return true;                                            
}



OmnString
AosJqlStmtJimoLogic::getObjid(const OmnString name)
{
	OmnString objid = "";
	objid << AOSZTG_JIMO_LOGIC_IIL << "_" << name;
	return objid;
}

AosJqlStatement *
AosJqlStmtJimoLogic::clone()
{
	return OmnNew AosJqlStmtJimoLogic(*this);
}


void 
AosJqlStmtJimoLogic::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtJimoLogic::setName(OmnString name)
{
	mLogicName = name;
}


void 
AosJqlStmtJimoLogic::setTableName(OmnString name)
{
	mTableName = name;
}

void 
AosJqlStmtJimoLogic::setAsName(OmnString name)
{
	mAsName = name;
}


void 
AosJqlStmtJimoLogic::setEndPoint(OmnString name)
{
	mEndPoint = name;
}


void 
AosJqlStmtJimoLogic::setTime(OmnString name)
{
	mTime = name;
}


void 
AosJqlStmtJimoLogic::setCheckPoint(OmnString name)
{
	mCheckPoint = name;
}


void 
AosJqlStmtJimoLogic::setMatrixName(OmnString name)
{
	mMatrixName = name;
}

void 
AosJqlStmtJimoLogic::setSecondName(OmnString name)
{
	mMatrix2Name = name;
}


void 
AosJqlStmtJimoLogic::setResultsName(OmnString name)
{
	mResultsName = name;
}

