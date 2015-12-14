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
// 03/18/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "EmailServer/Pop3Server.h"

#include "Alarm/Alarm.h"
#include "SearchEngine/DocServerCb.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILClient/IILClient.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/Objid.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/StrParser.h"
#include "Util/SecUtil.h"

#include <python2.6/Python.h>

PyObject * AosPop3Server::mPop3ServerModule = NULL;
static map<OmnString, OmnString> sgPop3Map;
static map<OmnString, OmnString>::iterator itr;
static bool sgPop3SvrInitFlag = false;
static bool sgPop3ModuleInitFlag = false;

static AosXmlParser sgXmlParser;

AosPop3Server::AosPop3Server(const AosXmlTagPtr &config)
:
mLock(OmnNew OmnMutex()),
mLoginFunc(0),
mRetrFunc(0),
mDeleFunc(0),
mGetLenFunc(0),
mGetUidlFunc(0)
{
	if (!config) return;
	if (!sgPop3SvrInitFlag) svrInit(config);
	if (!sgPop3ModuleInitFlag) moduleInit(config);
}


AosPop3Server::~AosPop3Server()
{
	Py_Finalize();
}

	
void
AosPop3Server::moduleInit(const AosXmlTagPtr &config)
{
	if (!config) return;
	AosXmlTagPtr pop3Server;
	AosXmlTagPtr servers = config->getFirstChild(AOSTAG_SERVERS);
	if (!servers || !(pop3Server = servers->getFirstChild(AOSTAG_POP3_SERVER)))
	{
		OmnAlarm << "Configuration is wrong!" << enderr;
		return;
	}
	OmnString sPath = pop3Server->getAttrStr(AOSTAG_MODULE_PATH);
	OmnString file = pop3Server->getAttrStr(AOSTAG_MODULE_FILE);
	if (sPath == "" && file == "")
	{
		OmnAlarm << "Configuration is wrong!" << enderr;
		return;
	}
	OmnString path = "sys.path.append('";
	path << sPath << "')";
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		OmnAlarm <<"init failed"<< enderr;
		exit(1);
	}

	try
	{
		PyRun_SimpleString("import sys");
		PyRun_SimpleString(path);
		mPop3ServerModule = PyImport_ImportModule(file);
		if (!mPop3ServerModule)
		{
			OmnAlarm << "Failed to load Pop3Server Module! " << enderr;
			exit(1);
		}
		
		mLoginFunc = PyObject_GetAttrString(mPop3ServerModule, "login"); 
		mGetLenFunc = PyObject_GetAttrString(mPop3ServerModule, "getlen");
		mRetrFunc = PyObject_GetAttrString(mPop3ServerModule, "rec");
		mGetUidlFunc = PyObject_GetAttrString(mPop3ServerModule, "getuidlbyidx");
		mDeleFunc = PyObject_GetAttrString(mPop3ServerModule, "dele"); 
		
		if (!(mLoginFunc && mGetLenFunc && mRetrFunc && mGetUidlFunc && mDeleFunc))
		{
			OmnAlarm << "Failed to init function module!" << enderr;
			exit(1);
		}
	}

	catch(...)
	{
		OmnAlarm << "Error to improt Pop3Server Module!" << enderr;
		exit(1);
	}
	sgPop3ModuleInitFlag = true;
}


void
AosPop3Server::svrInit(const AosXmlTagPtr &config)
{
	if (!config) return;
	AosXmlTagPtr pop3Svr;
	AosXmlTagPtr servers = config->getFirstChild(AOSTAG_SERVERS);
	if (!servers || !(pop3Svr = servers->getFirstChild(AOSTAG_POP3_SERVER)))
	{
		OmnAlarm << "cofigration is wrong!" << enderr;
		return;
	}
	AosXmlTagPtr child = pop3Svr->getFirstChild();
	while (child)
	{
		OmnString svrname = child->getAttrStr("name");
		if (svrname == "")
		{
			OmnAlarm << "configration is wrong!" << enderr;
			return;
		}
		OmnString content = child->getNodeText();
		if (content == "")
		{
			OmnAlarm << "configration is wrong!" << enderr;
			return;
		}
		sgPop3Map[svrname] = content;
		child = pop3Svr->getNextChild();
	}
	sgPop3SvrInitFlag = true;
}


bool 
AosPop3Server::login(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	// This function login to remote pop3 server
	aos_assert_r(data, false);

	// Get user, password, emaildir and server
	OmnString user = data->getAttrStr(AOSTAG_POP3_EMAILADDR);
	aos_assert_r(user != "", false);
	OmnString passwd = data->getAttrStr(AOSTAG_POP3_PASSWD);
	aos_assert_r(passwd != "", false);
	OmnString emldir = data->getAttrStr(AOSTAG_POP3_EMAILDIR);
	aos_assert_r(emldir != "", false);
	mServer = getServer(user,  rdata);
	aos_assert_r(mServer != "", false);
	aos_assert_r(mPop3ServerModule, false);
	
	mLock->lock();
	
	// login to server
	bool loginFlag = loginToPopSvr(user, passwd, mServer, emldir, rdata);
	aos_assert_rl(loginFlag, mLock, false);

	// Get the number of emails
	int len = getEmlLen(mServer, rdata);
	aos_assert_rl(len != 0, mLock, false);

	//Get python function of retr email and uidl
	mRetrFunc = PyObject_GetAttrString(mPop3ServerModule, "rec");
	mGetUidlFunc = PyObject_GetAttrString(mPop3ServerModule, "getuidlbyidx");
	
	for (int i=0; i<len; i++)
	{
		OmnString uidl = getEmlUIDL(i, user, mGetUidlFunc, rdata);
		if (uidl == "")
		{
			//mLock->unlock();
			rdata->setError() << "Error: udil is empty";
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		bool found = checkExist(uidl, mServer, rdata);
		if (!found)
		{
			OmnString email = retrEmail(i, user, mRetrFunc, rdata);
			if (email == "")
			{
			//	mLock->unlock();
				rdata->setError() << "Failed to retrieve email";
				OmnAlarm << rdata->getErrmsg() << enderr;
				//return false;
				continue;
			}
			bool create = setXml(email, user, mServer, i, uidl, rdata);
			//aos_assert_rl(create, mLock, false);
			if (!create) 
			{
				OmnAlarm << "Failed to create doc" << enderr;
				continue;
			}
		}
	}
	mLock->unlock();
	rdata->setOk();
	return true;
}


bool 
AosPop3Server::loginToPopSvr(
			const OmnString &user,
			const OmnString &passwd,
			const OmnString &server,
			const OmnString &emldir,
			const AosRundataPtr &rdata)
{
	// This function login to pop3 server.
	
	OmnString rtnState;
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;

		aos_assert_r(mLoginFunc, false);
		
		pArg = Py_BuildValue("(s,s,s,s)",
			server.data(), user.data(), passwd.data(), emldir.data());
		aos_assert_r(pArg, false);
		result = PyEval_CallObject(mLoginFunc, pArg);
		if (result == NULL)
		{
			rdata->setError() << "Failed login to server : " 
				<< server << "user : " << user;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		Py_DECREF(result);
		OmnString reml  = PyString_AsString(result);
		aos_assert_r(reml != "", false);
		bool rslt = checkState(reml, rtnState, rdata);	
		if(!rslt)
		{
			rdata->setError() << "Failed login to server : " 
				<< server << "user : " << user;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	catch(...)
	{
		rdata->setError() << "Failed login to server : " 
			<< server << "user : " << user;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


int 
AosPop3Server::getEmlLen(
			const OmnString &server,
			const AosRundataPtr &rdata)
{
	// This function get email's number.
	OmnString rtnState;
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		
		mGetLenFunc = PyObject_GetAttrString(mPop3ServerModule, "getlen");
		aos_assert_r(mGetLenFunc, 0);
		pArg = Py_BuildValue("(s)", server.data());
		aos_assert_r(pArg, 0);
		result = PyEval_CallObject(mGetLenFunc, pArg);
		if (result == NULL)
		{
			rdata->setError() << "Get email number failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		Py_DECREF(result);
		OmnString rtnLen  = PyString_AsString(result);
		aos_assert_r(rtnLen != "", 0);
		checkState(rtnLen, rtnState, rdata);	
		int num = rtnState.toInt();
		if (num == 0)
		{
			rdata->setError() << "Failed to get number of email";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		return num;
	}
	catch(...)
	{
		rdata->setError() << "Failed to get email's len : "
			<< "server : " << server;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	// Should never be here.
	return 0;
}


OmnString 
AosPop3Server::getEmlUIDL(
			const int idx,
			const OmnString &user,
			PyObject * uFunc,  
			const AosRundataPtr &rdata)
{
	// This function get email's uidl
	aos_assert_r(uFunc, "");
	OmnString rtnState;
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		
		pArg = Py_BuildValue("(i)", idx);
		aos_assert_r(pArg, "");
		result = PyEval_CallObject(uFunc, pArg);
		//aos_assert_r(result, "");
		if (!result) return "";
		OmnString uidl = PyString_AsString(result);
		aos_assert_r(uidl != "", "");
		bool rslt = checkState(uidl, rtnState, rdata);
		if (!rslt) return "";
		// if (!rslt)
		// {
		// 		rdata->setError() << "Get UIDL failed";
		// 		OmnAlarm << rdata->getErrmsg() << enderr;
		// 		return "";
	// }
		return rtnState;
	}
	catch(...)
	{
		rdata->setError() << "Failed to retrieve uidl : " << idx 
			<< " : user : " << user;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}
	return "";
}


OmnString 
AosPop3Server::retrEmail(
			const int idx,
			const OmnString &user,
			PyObject * rFunc,
			const AosRundataPtr &rdata)
{
	// This function retrieve email form server.
	aos_assert_r(rFunc, "");
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		
		pArg = Py_BuildValue("(i)", idx+1);
		//aos_assert_r(pArg, "");
		if (!pArg) return "";
		result = PyEval_CallObject(rFunc, pArg);
		//aos_assert_r(result, "");
		if (!result) return "";
		OmnString html = PyString_AsString(result);
		//aos_assert_r(html != "", "");
		if (html == "") return "";
		return html;
	}
	catch(...)
	{
		rdata->setError() << "Failed to retrieve email : " << idx
			<< " : user : " << user;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}

	return "";
}


bool 
AosPop3Server::dele(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	// This function delete a email
	aos_assert_r(mPop3ServerModule, false);
	aos_assert_r(data, false);

	OmnString deleOpr = data->getAttrStr(AOSTAG_DELEMAIL_OPR);
	aos_assert_r(deleOpr != "", false);
	OmnString uidl = data->getAttrStr(AOSTAG_DELEMAIL_UIDL, "");
	if (deleOpr == "deleforever") return deleForever(rdata);
	if (deleOpr == "setdeleflag") return setDeleFlag(uidl, rdata);
	else
	{
		rdata->getErrmsg() << "Unrecongnise Operation";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosPop3Server::setDeleFlag(
				const OmnString &uidl,
				const AosRundataPtr &rdata)
{
	// This function retreive uidl and find the uidl in map, set map value true.
	OmnString key = uidl;
	aos_assert_r (key != "", false);
	key << mServer;
	if ((mDeleMapItr = mDeleMap.find(key)) != mDeleMap.end())
	{
		mDeleMapItr->second = true;
		return true;
	}
	return false;
}

bool
AosPop3Server::deleForever(const AosRundataPtr &rdata)
{
	// IMPORTANT : This function should be 
	// used when the user login out.
	// This function search map,if find true 
	// delete it, remove it form map.
	mLock->lock();
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		
		aos_assert_rl(mDeleFunc, mLock, false);
		
		for (mDeleMapItr = mDeleMap.begin();
				mDeleMapItr != mDeleMap.end(); mDeleMapItr ++)
		{
			if (mDeleMapItr->second)
			{
				int idx = mUIDLMap.find(mDeleMapItr->first)->second;
				pArg = Py_BuildValue("", idx);
				aos_assert_rl(pArg, mLock, false);
				result = PyEval_CallObject(mDeleFunc, pArg);
				aos_assert_rl(result, mLock, false);
				OmnString rslt;
				PyArg_Parse(result,"s", &rslt);
				bool rr = checkState("delete", rslt, rdata);
				if (!rr)
				{
					rdata->setError() << "Failed to delete :" << idx;
					OmnAlarm << rdata->getErrmsg() << enderr;
					mLock->unlock();
					return false;
				}
				mDeleMap.erase(mDeleMapItr->first);
			}
		}
	}
	catch(...)
	{
		OmnAlarm << "Someting error!" << enderr;
		mLock->unlock();
		rdata->setError() << "receive mail failed (internal error)";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mLock->unlock();
	rdata->setOk();
	return true;
}


bool
AosPop3Server::setXml(
				const OmnString &html,
				const OmnString &user,
				const OmnString &server,
				const int idx,
				OmnString &key,
				const AosRundataPtr &rdata)
{
 	aos_assert_r(html != "", false);
	AosXmlTagPtr doc = sgXmlParser.parse(html, "");
	aos_assert_r(doc, false);
	
	key << "_" << server;
	aos_assert_r(key != "", false);

	doc->setAttr(AOSTAG_EMAIL_USER, user);
	doc->setAttr(AOSTAG_EMAIL_SERVER, server);
	doc->setAttr(AOSTAG_EMAIL_ID, key);
	doc->setAttr(AOSTAG_PARENTC, user);
	doc->setAttr(AOSTAG_EMAIL_INDEX, idx);

	// A user's all email doc  has a unified attribute:
	// AOSTAG_EMAIL_CTNR. 18/08/2011 Brian Zhang
	u64 userid = rdata->getUserid();
	if (userid == 0)
	{
		rdata->setError() << "Missing userid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString uCtnr = "email_";
	uCtnr << userid;
	doc->setAttr(AOSTAG_EMAIL_CTNR, uCtnr);
	// ?????????????????
	// Now we put a account's emails(zhyawshhz@163.com) into 
	// container called servername(zkyawshhz@163.com), but if somebody has 
	// some email account(zkyawshhz@163.com ,453757465@qq.com),we can't put 
	// all email into a container.
	//
	// objid?
	//
	
	OmnString cid = rdata->getCid();
	bool crslt = AosDocClientObj::getDocClient()->createDoc1(rdata,
			        0, cid, doc, true, 0, 0, false);
	if (!crslt)
	{
		rdata->setError() << "Failed to create doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosPop3Server::checkState(
			const OmnString &content,
			OmnString &state,
			const AosRundataPtr &rdata)
{
	aos_assert_r(content != "", false);

	AosXmlTagPtr doc = sgXmlParser.parse(content, "");
	aos_assert_r(doc, false);

	bool rslt = doc->getAttrBool("state");
	if (!rslt)
	{
		OmnString errmsg = doc->getAttrStr("errmsg");
		aos_assert_r(errmsg, false);
		rdata->setError() << errmsg;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	state = doc->getAttrStr("content");
	
	rdata->setOk();
	return true;
}


OmnString
AosPop3Server::getServer(
				const OmnString &email_addr,
				const AosRundataPtr &rdata)
{
	// This function get server from map.
	aos_assert_r(email_addr != "", false);
	
	OmnStrParser1 parser(email_addr, "@");
	OmnString s = parser.nextWord();
	s = parser.nextWord();
	aos_assert_r(s != "", false);
	
	for(itr = sgPop3Map.begin(); itr != sgPop3Map.end(); itr ++)
	{
		if (s.findSubString(itr->first, 0, false) != -1)
		{
			return itr->second;
		}
	}

	rdata->setError() << "Can't get POP3 server";
	OmnAlarm << rdata->getErrmsg() << enderr;
	//should nenver be here
	return "";
}


bool
AosPop3Server::checkExist(
				const OmnString &key,
				const OmnString &server,
				const AosRundataPtr &rdata)
{
	// This function check email of pop3 server is new or not,
	// if new, it will retrieve.
	OmnString iilname = AosIILName::composeAttrIILName(AOSTAG_EMAIL_ID); 
	aos_assert_r(iilname != "", false);

	aos_assert_r(key != "" && server != "", false);
	OmnString iilkey = "";
	iilkey << key << "-" << server;
	aos_assert_r(key != "", false);
	OmnString id = AosSecUtil::signValue(iilkey);

	u64 docid;
	bool is_unique;
	bool rslt = AosIILClient::getSelf()->getDocid(
					iilname, id, docid, is_unique, rdata);
	if (!is_unique) 
	{
		rdata->setError() << "Inter Error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (!rslt) return false;	
	if (docid == 0) return false;
	
	return true;
}

