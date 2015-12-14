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
#include "Python/Python.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"

#include <python2.6/Python.h>


OmnSingletonImpl(AosPythonSingleton,
                 AosPython,
                 AosPythonSelf,
                "AosPython");

AosPython::AosPython()
:
	mLock(OmnNew OmnMutex())
{
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
		PyRun_SimpleString("sys.path.append('/home/cding/AOS/src/UserMgmt/')");
		//mPythonModule = PyImport_ImportModule("sendemail");
		//if (!mPythonModule)
		//{
		//	OmnAlarm << "Failed to load Python mail! (check /usr/local/python/sendmail.py)" << enderr;
		//	exit(1);
		//}
	}

	catch(...)
	{
		OmnAlarm << "Error to improt Python mail!" << enderr;
		exit(1);
	}

}


AosPython::~AosPython()
{
	Py_Finalize();
}


bool      	
AosPython::start()
{
	return true;
}


bool        
AosPython::stop()
{
	return true;
}


bool
AosPython::config(const AosXmlTagPtr &def)
{
	return true;
}


bool
AosPython::sendEmail(
		const OmnString &to,
		const OmnString &from,
		const OmnString &senderpasswd,
		const OmnString &subject,
		const OmnString &body,
		const OmnString &server,
		const OmnString &count, 
		const AosRundataPtr &rdata)
{
	if (to == "")
	{
		rdata->setError() << "Missing receiver address";
		return false;
	}

	if (from == "")
	{
		rdata->setError() << "Missing sender address!";
		return false;
	}

	//aos_assert_r(mPythonModule, false);
	mLock->lock();
	try
	{
		PyObject *pFunc = NULL;
		PyObject *pArg = NULL;
		PyObject *result = NULL;
		int iresult = 0;
		PyObject * pModule = PyImport_ImportModule("sendemail");
		if (!pModule)
		{
			OmnAlarm << "Failed to load Python mail! (check /usr/local/python/sendmail.py)" << enderr;
		 	mLock->unlock();
		 	return false; 
		}
		//pFunc = PyObject_GetAttrString(mPythonModule, "test2"); 
		pFunc = PyObject_GetAttrString(pModule, "test2"); 
		aos_assert_rl(pFunc, mLock, false);
		pArg = Py_BuildValue("(s,s,s,s,s,s,s)", 
			to.data(), from.data(), senderpasswd.data(), subject.data(), body.data(), server.data(), count.data());
		aos_assert_rl(pArg, mLock, false);
		result = PyEval_CallObject(pFunc, pArg);

		if (result == NULL)
		{
			mLock->unlock();
			rdata->setError() << "Failed sending email!";
			return false;
		}
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
		OmnAlarm << "Failed Python mail!" << enderr;
		mLock->unlock();
		rdata->setError() << "Send mail failed (internal error)";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mLock->unlock();
	rdata->setOk();
	return true;
}
