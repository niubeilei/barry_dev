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
#include "EmailServer/SmtpServer.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"

#include <python2.6/Python.h>

PyObject * AosSmtpServer::mSmtpServerModule = NULL;
static map<OmnString, OmnString> sgSmtpMap;
static map<OmnString, OmnString>::iterator itr;
static bool sgSmtpInitFlag = false;
static bool sgSmtpModuleInitFlag = false;


AosSmtpServer::AosSmtpServer(const AosXmlTagPtr &config)
:
mLock(OmnNew OmnMutex())
{
	if (!config) return;
	if (!sgSmtpInitFlag) svrInit(config);
	if (!sgSmtpModuleInitFlag) moduleInit(config);
}


AosSmtpServer::~AosSmtpServer()
{
	Py_Finalize();
}


void
AosSmtpServer::moduleInit(const AosXmlTagPtr &config)
{
	if (!config) return;
	AosXmlTagPtr smtpSvr;
	AosXmlTagPtr servers = config->getFirstChild(AOSTAG_SERVERS);
	if (!servers || !(smtpSvr = servers->getFirstChild(AOSTAG_SMTP_SERVER)))
	{
		OmnAlarm << "Configuration is wrong!" << enderr;
		return;
	}
	OmnString sPath = smtpSvr->getAttrStr(AOSTAG_MODULE_PATH);
	OmnString file = smtpSvr->getAttrStr(AOSTAG_MODULE_FILE);
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
		PyRun_SimpleString("sys.path.append('/usr/local/python/')");
		PyRun_SimpleString(path);
		mSmtpServerModule = PyImport_ImportModule(file);
		if (!mSmtpServerModule)
		{
			OmnAlarm << "Failed to load SmtpServer module! (check /usr/local/python/sendmail.py)" << enderr;
			exit(1);
		}
		mSendFunc = PyObject_GetAttrString(mSmtpServerModule, "test2");
		if (!mSendFunc)
		{
			OmnAlarm << "Failed to init send function" << enderr;
			exit(1);
		}
	}

	catch(...)
	{
		OmnAlarm << "Error to improt SmtpServer module!" << enderr;
		exit(1);
	}
	sgSmtpModuleInitFlag = true;
}


void
AosSmtpServer::svrInit(const AosXmlTagPtr &config)
{
	if (!config) return;
	AosXmlTagPtr smtpSvr;
	AosXmlTagPtr servers = config->getFirstChild(AOSTAG_SERVERS);
	if (!servers || !(smtpSvr = servers->getFirstChild(AOSTAG_SMTP_SERVER)))
	{
		OmnAlarm << "cofigration is wrong!" << enderr;
		return;
	}
	AosXmlTagPtr child = smtpSvr->getFirstChild();
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
		sgSmtpMap[svrname] = content;
		child = smtpSvr->getNextChild();
	}
	sgSmtpInitFlag = true;
}


bool
AosSmtpServer::send(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mSmtpServerModule && mSendFunc, false);
	aos_assert_r(data, false);

	//create log for sending message
	//bool log = AosSeLogClient::getSelf()->createLog(data, rdata);
	
	OmnString to = data->getAttrStr(AOSTAG_SMTP_TO);
	aos_assert_r(to != "", false);
	OmnString from = data->getAttrStr(AOSTAG_SMTP_FROM);
	aos_assert_r(from != "", false);
	OmnString server = getServer(from, rdata);
	aos_assert_r(server != "", false);
	OmnString senderpasswd = data->getAttrStr(AOSTAG_SMTP_SENDERPSWD);
	aos_assert_r(senderpasswd != "", false);
	OmnString subject = data->getAttrStr(AOSTAG_SMTP_SUBJECT);
	aos_assert_r(subject != "", false);
	OmnString count = data->getAttrStr(AOSTAG_SMTP_COUNT);
	aos_assert_r(count != "", false);
	AosXmlTagPtr child = data->getFirstChild();
	aos_assert_r(child, false);
	OmnString contents = child->getNodeText();
	aos_assert_r(contents != "", false);

	// Send email
	mLock->lock();
	try
	{
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		int iresult = 0;
		pArg = Py_BuildValue("(s,s,s,s,s,s,s)", 
			to.data(), from.data(), senderpasswd.data(), subject.data(), 
			contents.data(), server.data(), count.data());
		aos_assert_rl(pArg, mLock, false);
		result = PyEval_CallObject(mSendFunc, pArg);
		aos_assert_rl(result, mLock, false);
		
		PyArg_Parse(result,"i", &iresult);
		if (iresult  == 2)
		{
			mLock->unlock();
			rdata->setError() << "SMTP server login failed, please check the user name and password";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (iresult == 1)
		{
			mLock->unlock();
			rdata->setError() << "Connection unexpectedly closed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	catch(...)
	{
		OmnAlarm << "Failed SmtpServer mail!" << enderr;
		mLock->unlock();
		rdata->setError() << "Send mail failed (internal error)";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mLock->unlock();
	rdata->setOk();
	return true;
}

	
OmnString
AosSmtpServer::getServer(
			const OmnString &from, 
			const AosRundataPtr &rdata)
{
	// This function get server from map.
	aos_assert_r(from != "", false);
	
	OmnStrParser1 parser(from, "@");
	OmnString s = parser.nextWord();
	s = parser.nextWord();
	aos_assert_r(s != "", false);
	
	for(itr == sgSmtpMap.begin(); itr != sgSmtpMap.end(); itr ++)
	{
		if (s.findSubString(itr->first, 0, false) != -1)
		{
			return itr->second;
		}
	}

	rdata->setError() << "Can't get SMTP Server";
	OmnAlarm << rdata->getErrmsg() << enderr;
	//should nenver be here
	return "";
}
