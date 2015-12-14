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
#include "JQLStatement/JqlStmtUserMgr.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"


AosJqlStmtUserMgr::AosJqlStmtUserMgr()
{
	mUserName = "";
	mPwd = "";
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtUserMgr::AosJqlStmtUserMgr(const OmnString errmsg)
:
mErrmsg(errmsg)
{
}


AosJqlStmtUserMgr::~AosJqlStmtUserMgr()
{
}


bool
AosJqlStmtUserMgr::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false; 
	}


	if (mOp == JQLTypes::eCreate) return create(rdata);
	if (mOp == JQLTypes::eShow) return show(rdata);
	if (mOp == JQLTypes::eDrop) return drop(rdata);
	if (mOp == JQLTypes::eDescribe) return describe(rdata);
	if (mOp == JQLTypes::eAlter) return alter(rdata);
	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL UserMgr", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool AosJqlStmtUserMgr::alter(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosXmlTagPtr doc = getDocByKey(rdata,"zky_sysuser", "zky_uname", mUserName);
	if(!doc)
	{
		msg << "User " << mUserName << " is not exist.";
		rdata->setJqlMsg(msg);
		return false;
	}
	AosXmlTagPtr pwd_tag = doc->getFirstChild("zky_passwd__n");
	aos_assert_r(pwd_tag, false);
	pwd_tag->setNodeText(mPwd,true);
	AosModifyDoc(doc,rdata);
	rdata->setJqlMsg(OmnString("pwd identified"));
	return true;
}

bool
AosJqlStmtUserMgr::create(const AosRundataPtr &rdata)
{
	//OmnString objid = getObjid(mUserName);
	OmnString msg;

	//AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	AosXmlTagPtr doc = getDocByKey(rdata,"zky_sysuser", "zky_uname", mUserName); 
	if (doc)
	{
		msg << "User " << mUserName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc) return false;
	//doc->setAttr(AOSTAG_OBJID, objid); 

	if ( !createUser(rdata, doc) )
	{
		msg << "Failed to create User doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "User \"" << mUserName << "\" created successfully.";
	rdata->setJqlMsg(msg);

	return true;
}


AosXmlTagPtr
AosJqlStmtUserMgr::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	doc << "<embedobj zky_sdocemail=\"sdmcl_userregistemail\" "
		<< " zky_uname=\"" << mUserName << "\""
		<< " zky_usrctnr=\"zky_sysuser\" zky_pctrs=\"zky_sysuser\">"
		<< "	<zky_passwd__n1><![CDATA[" << mPwd << "]]></zky_passwd__n1>"
		<< "	<zky_passwd__n><![CDATA[" << mPwd <<"]]></zky_passwd__n>"
		<< "</embedobj>";
	return AosXmlParser::parse(doc AosMemoryCheckerArgs); 
}

bool
AosJqlStmtUserMgr::show(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> users;
	//bool rslt = false; // = listUserMgrs(rdata, users);
	bool rslt = getDocs(rdata, JQLTypes::eUserDoc,users);
	if(!rslt)
	{
		OmnString error;
		error << "No UserMgr Found. ";
		rdata->setJqlMsg(error);

		//OmnCout << "No UserMgr Found. " << endl;
		return true;
	}
	OmnString content = "<content>";
	for(u32 i=0; i<users.size(); i++)
	{
		OmnString vv = users[i]->getAttrStr("zky_uname", "");
		if (vv != "")
		{
			content << "<record username=\"" << vv << "\"/>";
		}
	}

	content << "</content>";
	rdata->setResults(content);

	vector<OmnString> fields;
	fields.push_back("username");

	map<OmnString, OmnString> alias_name;
	alias_name["username"] = "UserName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
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
AosJqlStmtUserMgr::drop(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosXmlTagPtr doc = getDocByKey(rdata,"zky_sysuser", "zky_uname", mUserName); 
	if (!doc)
	{
		msg << "User " << mUserName << " is not exist.";
		rdata->setJqlMsg(msg);
		return false;
	}

	//OmnString objid = getObjid(mUserName);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	if (objid == "")
	{                                                               
		msg << "user doc's objid is null.";
		rdata->setJqlMsg(msg);
		return false;                                               
	}             

	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                               
		msg << "Failed to delete user doc.";
		rdata->setJqlMsg(msg);
		return false;                                               
	}                                                               
	msg << "User " << mUserName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtUserMgr::describe(const AosRundataPtr &rdata)
{
	if (mUserName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 
	OmnString msg;
	//string objid = getObjid(mUserName);
	//AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	AosXmlTagPtr doc = getDocByKey(rdata,"zky_sysuser", "zky_uname", mUserName); 
	if (!doc)
	{
		msg << "Failed to display user " << mUserName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	OmnString content = "<content>";
	content << doc->toString() << "</content>";

	vector<OmnString> fields;
	fields.push_back("zky_uname");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_uname"] = "UserName";

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	if (mContentFormat == "xml")
	{
		rdata->setResults(def->toString());
		return true;
	}
	setPrintData(rdata, msg);

	return true;
}


OmnString
AosJqlStmtUserMgr::getObjid(const OmnString name)
{
	OmnString objid = "";

	objid << AOSZTG_USER_CTNR << "_" << name;
	return objid;
}

AosJqlStatement *
AosJqlStmtUserMgr::clone()
{
	return OmnNew AosJqlStmtUserMgr(*this);
}


void 
AosJqlStmtUserMgr::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtUserMgr::setUserName(OmnString name)
{
	mUserName = name;
}


void
AosJqlStmtUserMgr::setPwd(OmnString pwd)
{
	mPwd = pwd; 
}

