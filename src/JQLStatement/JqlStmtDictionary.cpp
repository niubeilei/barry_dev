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
#include "JQLStatement/JqlStmtDictionary.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

AosJqlStmtDictionary::AosJqlStmtDictionary(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mKeys = NULL;
	mWhereCond = NULL;
}


AosJqlStmtDictionary::AosJqlStmtDictionary()
{
	mKeys = NULL;
	mWhereCond = NULL;
}

AosJqlStmtDictionary::~AosJqlStmtDictionary()
{
	OmnDelete(mKeys);
	delete mWhereCond;
	mWhereCond = NULL;
}


bool
AosJqlStmtDictionary::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDict(rdata);
	if (mOp == JQLTypes::eShow) return showDicts(rdata);
	if (mOp == JQLTypes::eDrop) return dropDict(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDict(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Dict", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDictionary::createDict(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDict, mDictName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		msg << "Dict " << mDictName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc->isRootTag())
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr("zky_objid", objid);

	if ( !createDoc(rdata, JQLTypes::eDict, mDictName, doc) )
	{
		msg << "Failed to create Dict doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Dict " << mDictName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDictionary::convertToXml(const AosRundataPtr &rdata)
{
	OmnString dbname = getCurDatabase(rdata);
	OmnString  dictIILName = "";
	dictIILName << "_zt44_dict_" << dbname << "_" << mTableName << "_" << mDictName;

	OmnString doc;
	doc << "<map zky_type=\"dict\" zky_name=\"" << mDictName <<"\"" << " zky_iilname=\"" << dictIILName << "\""
		<< " datatype=\"u64\">";
	doc << 	"<keys>";
	for (u32 i = 0; i < mKeys->size(); i++)
	{
		if ((*mKeys)[i])
			doc << "<key>" << (*mKeys)[i]->dumpByNoEscape() << "</key>";
	}
	doc << 	"</keys>";
	doc << 	"<values>";
	doc << 	"<value agrtype=\"set\" max_len=\"50\" type=\"expr\"><![CDATA[0]]></value>";
	doc << 	"</values>";
	if (mWhereCond)
		doc << "<cond><![CDATA[" << mWhereCond->dumpByNoEscape() << "]]></cond>";

	doc << "</map>";

	AosXmlTagPtr dict = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	if (!dict)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	dict->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	dict->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	dict->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_DICT_CONTAINER);   

	AosXmlTagPtr table = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if (table)
	{
		AosXmlTagPtr dicts = table->getFirstChild("maps");
		AosXmlTagPtr dict_tmp = dict->clone(AosMemoryCheckerArgsBegin);
		dicts->addNode(dict_tmp);

		bool rslt = modifyDoc(rdata, table);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
	}
	return dict;
}


bool
AosJqlStmtDictionary::showDicts(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> dfields;
	if ( !getDocs(rdata, JQLTypes::eDict, dfields) )
	{
		msg << "No Dict Found! " ;
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
	alias_name["name"] = "DictName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtDictionary::describeDict(const AosRundataPtr &rdata)
{
	if (mDictName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDict, mDictName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display dict " << mDictName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
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
AosJqlStmtDictionary::dropDict(const AosRundataPtr &rdata)
{
	//JIMODB-163 drop dict by Levi
	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDict, mDictName);
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete Dict doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop Dict " << mDictName << " scessfully";      
	rdata->setJqlMsg(msg);                                  
	return true;                                            
}


AosJqlStatement *
AosJqlStmtDictionary::clone()
{
	return OmnNew AosJqlStmtDictionary(*this);
}


void 
AosJqlStmtDictionary::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDictionary::setDictName(OmnString name)
{
	mDictName = name;
}

void                                            
AosJqlStmtDictionary::setTableName(OmnString name)
{
	mTableName = name;
}

void 
AosJqlStmtDictionary::setKeys(AosExprList *key_list)
{
	mKeys = key_list;
}

void 
AosJqlStmtDictionary::setWhereCond(AosExprObj *where_cond)
{
	mWhereCond = where_cond;
}

