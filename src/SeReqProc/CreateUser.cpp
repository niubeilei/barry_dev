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
// The torturer is in SengTorturer/TesterCreateUserNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateUser.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static int sgMaxAccounts = 500;

AosCreateUser::AosCreateUser(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_USER, 
		AosSeReqid::eCreateUser, rflag)
{
}


bool 
AosCreateUser::proc(const AosRundataPtr &rdata)
{
	// This function creates a batch of user accounts. It assumes the
	//  request must be in the following format:
	//   <request>
	//       <objdef>
	//           <...>
	//               <cmd num="xxx" acct_root="xxx"/>
	//           </...>
	//       </objdef>
	//   </request>
	// 
	// If successful, it will return the accounts created
	//   <Contents>
	//       <accounts>
	//           <account username="xxx">passwd</account>
	//           <account username="xxx">passwd</account>
	//           ...
	//       </accounts>
	//   </Contents>

	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// 1. Retrieve the Object
	OmnString errmsg;
	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	AosXmlTagPtr userobj;
	if (!objdef || !(userobj = objdef->getFirstChild()))
	{
		AosSetError(rdata, "missing_userinfo");
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// Check whether it is to create multiple users through a
	// local file
	bool rslt;
	OmnString fname = userobj->getAttrStr("filename");
	int num_accounts = userobj->getAttrInt("num_accounts", -1);
	if (fname != "" || num_accounts >0)
	{
		// It is to create multiple users through a local file
		AOSLOG_LEAVE(rdata);
		rslt = createUserAccounts(rdata, root, userobj);
		if (!rslt)
		{
			rdata->setError() << "Create multiple user failed";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		AOSSYSLOG_CREATE(true, AOSREQIDNAME_CREATE_USER, rdata);
		AOSLOG_LEAVE(rdata);
		return true;
	}
	
	int num = -11;
	AosXmlTagPtr cmd = userobj->getFirstChild("cmd");
	if (cmd) num = cmd->getAttrInt("num", -1);
	if (num <= 0)
	{
		num = userobj->getAttrInt("tmp_num_create", -1);
	}
	if (num < 0)
	{
		rslt = createOneUserAcct(rdata, root, userobj);
		if (!rslt)
		{
			rdata->setError() << "Create one user failed";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		AOSSYSLOG_CREATE(true, AOSREQIDNAME_CREATE_USER, rdata);
		AOSLOG_LEAVE(rdata);
		return true;
	}
		
	if (num > sgMaxAccounts)
	{
		errmsg = "Too many accounts to create: ";
		errmsg << num;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
		
	OmnString username_base = userobj->getAttrStr("tmp_username_base", "user");
	userobj->removeAttr("acct_root", false, true);
	
	OmnString userobj_str = userobj->toString();
	OmnString resp;
	AosXmlParser parser;
	OmnString rslts = "<Contents><accounts>";

	for (int i=0; i<num; i++)
	{
		AosXmlTagPtr root = parser.parse(userobj_str, "" AosMemoryCheckerArgs);
		AosXmlTagPtr userdoc;
		if (!root || !(userdoc = root->getFirstChild()))
		{
			// This should never happen
			AosSetError(rdata, "internal_error");
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// Set the username
		OmnString username = username_base;
		username << "_" << i;
		userdoc->setAttr(AOSTAG_USERNAME, username);

		// Create the password
		OmnString passwd = AosGeneratePasswd();
		userdoc->setNodeText(AOSTAG_PASSWD, passwd, true);

		rslt = AosCreateUserAccount(rdata, userdoc,resp);

		if (!rslt)
		{
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		
		rslts << "<account username=\""
			<< userdoc->getAttrStr(AOSTAG_CLOUDID) << "\">"
			<< passwd << "</account>"
			<< "<record "
			<< AOSTAG_OBJID << "=\"" <<userdoc->getAttrStr(AOSTAG_OBJID) <<"\" "
			<< AOSTAG_DOCID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID) <<"\""
			<<"/>";
	}

	rslts << "</accounts></Contents>";
	rdata->setResults(rslts);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_CREATE_USER, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}


bool 
AosCreateUser::createUserAccounts(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &userobj)
{
	AOSLOG_ENTER_R(rdata, false);		
	
	OmnString errmsg;
	OmnString fname = userobj->getAttrStr("filename");
	userobj->removeAttr("filename", false, true);
	
	// Chen Ding, 2011/02/28
	int num_accounts = userobj->getAttrInt("num_accounts", -1);
	userobj->removeAttr("num_accounts");
	if (num_accounts > 0)
	{
		OmnString userobj_str1 = userobj->toString();
		OmnString resp;
		AosXmlParser parser;
		OmnString rslts = "<Contents><accounts>";
		for (int i=0; i<num_accounts; i++)
		{
			AosXmlTagPtr root = parser.parse(userobj_str1, "" AosMemoryCheckerArgs);
			AosXmlTagPtr userdoc1;
			if (!root || !(userdoc1 = root->getFirstChild()))
			{
				// This should never happen
				errmsg = "Failed to parse!";
				rdata->setError() << errmsg;
				AOSLOG_LEAVE(rdata);		
				return false;
			}

			// Create the password
			OmnString passwd = AosGeneratePasswd();
			userdoc1->setNodeText(AOSTAG_PASSWD, passwd, true);

			bool rslt = AosCreateUserAccount(rdata, userdoc1, resp);

			if (!rslt)
			{
				rdata->setError() << errmsg;
				AOSLOG_LEAVE(rdata);		
				return false;
			}

			rslts << "<account username=\""
				<< userdoc1->getAttrStr(AOSTAG_CLOUDID) << "\">"
				<< passwd << "</account>";
		}

		rslts << "</accounts></Contents>";

		rdata->setResults(rslts);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);		
		return true;
	}

	OmnString fields = userobj->getAttrStr("fields");
	if (fields == "")
	{
		errmsg = "Missing fields!";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);		
		return false;
	}
	userobj->removeAttr("fields", false, true);

	OmnString separator = userobj->getAttrStr("separator");
	if (separator == "")
	{
		errmsg = "Missing separator!";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);		
		return false;
	}
	userobj->removeAttr("separator", false, true);

	// Open the file
	OmnFile userfile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!userfile.isGood())
	{
		errmsg = "Failed to open the file: ";
		errmsg << fname;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);		
		return false;
	}

	OmnString fieldnames[eMaxFields];
	OmnString values[eMaxFields];
	AosStrSplit split;
	bool finished;
	int num_fields = split.splitStr(fields.data(), separator.data(), fieldnames, eMaxFields);

	OmnString userobj_str = userobj->toString();
	OmnString resp;
	bool rslt;
	AosXmlParser parser;
	OmnString rslts = "<Contents><accounts>";
	while (1)
	{
		OmnString vv = userfile.getLine(finished);
		if (vv == "") break;

		AosStrSplit split;
		int num_values = split.splitStr(vv.data(), separator.data(), values, eMaxFields);

		AosXmlTagPtr root = parser.parse(userobj_str, "" AosMemoryCheckerArgs);
		AosXmlTagPtr userdoc;
		if (!root || !(userdoc = root->getFirstChild()))
		{
			// This should never happen
			errmsg = "Failed to parse!";
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);		
			return false;
		}
		
		// Create the password
		OmnString passwd = AosGeneratePasswd();
		userdoc->setNodeText(AOSTAG_PASSWD, passwd, true);

		// Initialize the userdoc
		for (int i=0; i<num_fields; i++)
		{
			if (i < num_values)
			{
				userdoc->setAttr(fieldnames[i], values[i]);
			}
		}

		rslt = AosCreateUserAccount(rdata, userdoc,resp);

		if (!rslt)
		{
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);		
			return false;
		}
		
		rslts << "<account username=\""
			<< userdoc->getAttrStr(AOSTAG_CLOUDID) << "\">"
			<< passwd << "</account>";
	}
	rslts << "</accounts></Contents>";

	rdata->setResults(rslts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);		
	return true;
}


bool 
AosCreateUser::createOneUserAcct(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &userobj)
{
	AOSLOG_ENTER_R(rdata, false);		

	OmnString errmsg;

	OmnString userobj_str = userobj->toString();
	OmnString resp;
	bool rslt;
	AosXmlParser parser;
	OmnString rslts = "<Contents><accounts>";

	AosXmlTagPtr userdoc = parser.parse(userobj_str, "" AosMemoryCheckerArgs);
	if (!userdoc)
	{
		// This should never happen
		AosSetError(rdata, "internal_error");
		AOSLOG_LEAVE(rdata);		
		return false;
	}

	//Ketty for account	2011/05/06
	OmnString loginPass = userdoc->getAttrStr("login_pass_pw", "");
	userdoc->removeAttr("login_pass_pw");
	if(loginPass != "12345")
	{
//		userdoc->setAttr(AOSTAG_REGIST_PAY_STATUS, "no_level");
	}
	rslt = AosCreateUserAccount(rdata, userdoc, resp );

	if (!rslt)
	{
		rdata->setError();
		AOSLOG_LEAVE(rdata);		
		return false;
	}

	rslts << "<account "
		<< AOSTAG_DOCID<< "=\"" << userdoc->getAttrStr(AOSTAG_DOCID) << "\" "
		<< AOSTAG_OBJID<< "=\"" << userdoc->getAttrStr(AOSTAG_OBJID) << "\" "
		<< AOSTAG_USERNAME << "=\"" << userdoc->getAttrStr(AOSTAG_USERNAME) << "\" "
		<< AOSTAG_CLOUDID << "=\"" << userdoc->getAttrStr(AOSTAG_CLOUDID) << "\"/>"
		<< "</accounts></Contents>";
	rdata->setOk();
	rdata->setResults(rslts);
	AOSLOG_LEAVE(rdata);		
	return true;
}
