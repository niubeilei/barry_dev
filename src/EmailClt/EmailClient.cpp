////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/05/26	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "EmailClt/EmailClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "EmailClt/Ptrs.h"
#include "Rundata/Rundata.h"
#include "sys/stat.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"

#include <python2.6/Python.h>

static AosXmlParser     sgXmlParser;
int sgTimeout = 60;


OmnSingletonImpl(AosEmailClientSingleton,
                 AosEmailClient,
                 AosEmailClientSelf,
                "AosEmailClient");

AosEmailClient::AosEmailClient()
{
}


AosEmailClient::~AosEmailClient()
{
}


bool
AosEmailClient::start()
{
	return true;
}


bool
AosEmailClient::stop()
{
	return true;
}


bool
AosEmailClient::config(const AosXmlTagPtr &config)
{
	if (!config) 
	{
		OmnAlarm << "Can't get config!" << enderr;
		return false;
	}
	AosXmlTagPtr trans = config->getFirstChild("transactions");
	if (!trans) 
	{
		OmnAlarm << "Can't get config!" << enderr;
		return false;
	}
	AosXmlTagPtr email_trans = trans->getFirstChild("EmailClt");
	if (!email_trans) 
	{
		OmnAlarm << "Can't get config!" << enderr;
		return false;
	}
	AosXmlTagPtr trans_file = email_trans->getFirstChild();
	if (!trans_file) 
	{
		OmnAlarm << "Can't get config!" << enderr;
		return false;
	}

	return true;
}


bool
AosEmailClient::login(
		const OmnString &email_addr,
		const OmnString &passwd,
		const OmnString &email_dir,
		const AosRundataPtr &rdata)
{
	// This function login to remote pop3 server
	aos_assert_rr(email_addr != "" && passwd != "" 
			&& email_dir != "", rdata, false);

	// 2. set doc
	OmnString str = "<login ";
	str << AOSTAG_OPERATOR << "=\"login\" "
		<< AOSTAG_POP3_EMAILADDR << "=\"" << email_addr << "\" "
		<< AOSTAG_POP3_EMAILDIR << "=\"" << email_dir << "\" "
		<< AOSTAG_POP3_PASSWD << "=\"" << passwd << "\" />";
	
	// Ketty 2013/02/22
	//AosXmlTagPtr doc = sgXmlParser.parse(str, "" AosMemoryCheckerArgs);
	//aos_assert_rr(doc, rdata, false);

	// 3. Send it to emailserver
	//AosXmlTagPtr resp;
	bool timeout;
	AosBuffPtr resp_buff;
	// Ketty 2013/03/25
	OmnNotImplementedYet;
	bool rslt;// = mTransClient->addTrans(rdata, mRobin->routeReq(0), str.data(),
			//str.length(), true, 0, timeout, resp_buff);
	aos_assert_rr(rslt, rdata, false);

	// 4. if timeout
	if (timeout)
	{
		rdata->setError() << "Login timeout";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	rdata->setOk();
	return true;
}


bool 
AosEmailClient::retr(
		const int start_idx, 
		const int psize,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// This function retrieve the email user who wanted,
	// All emails of a user have been converted into doc
	// in our system. The action of retrieve just retrieve
	// our doc in system.
	// DocClient's function
	return true;
}


bool
AosEmailClient::dele(
		const OmnString &index,
		const OmnString &deleOpr,
		const AosRundataPtr &rdata)
{
	// This function delete the index of email,
	// if index if 'all', we will retr all email,
	// else we will retr the index defined.
	aos_assert_rr(index != "", rdata, false);
	aos_assert_rr(deleOpr != "", rdata, false);

	OmnString str = "<dele ";
	str << AOSTAG_OPERATOR << "=\"dele\" "
		<< AOSTAG_DELEMAIL_OPR << "=\"" 
		<< deleOpr << "\" "
		<< AOSTAG_DELETE_INDEX << "=\"" << index << "\" />";
	// Ketty 2013/02/22
	//AosXmlTagPtr doc = sgXmlParser.parse(str, "" AosMemoryCheckerArgs);
	//aos_assert_rr(doc, rdata, false);
	
	//AosXmlTagPtr resp;
	bool timeout;
	AosBuffPtr resp_buff;
	// Ketty 2013/03/25
	OmnNotImplementedYet;
	bool rslt;// = mTransClient->addTrans(rdata, mRobin->routeReq(0), str.data(),
			//str.length(), true, 0, timeout, resp_buff);
	aos_assert_rr(rslt, rdata, false);

	// 4. if timeout
	if(timeout)
	{
		rdata->setError() << "Delete timeout";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


bool
AosEmailClient::send(
			const OmnString &to,
			const OmnString &from,
			const OmnString &senderpasswd,
			const OmnString &subject,
			const OmnString &body,
			const OmnString count,
			const AosRundataPtr &rdata)
{
	// This function send a email to remote pop3 server
	OmnString str = "<send ";
	str << AOSTAG_OPERATOR << "=\"send\" "
		<< AOSTAG_SMTP_TO << "=\"" << to << "\" "
		<< AOSTAG_SMTP_FROM << "=\"" << from << "\" "
		<< AOSTAG_SMTP_SENDERPSWD << "=\"" << senderpasswd << "\" "
		<< AOSTAG_SMTP_SUBJECT << "=\"" << subject << "\" "
		<< AOSTAG_SMTP_COUNT << "=\"" << count << "\" >"
		<< "<content>" << body << "</content>"
		<< "</send>";
	// Ketty 2013/02/22
	//AosXmlTagPtr doc = sgXmlParser.parse(str, "" AosMemoryCheckerArgs);
	//aos_assert_rr(doc, rdata, false);
	
	//AosXmlTagPtr resp;
	bool timeout;
	AosBuffPtr resp_buff;
	// Ketty 2013/03/25
	OmnNotImplementedYet;
	bool rslt;// = mTransClient->addTrans(rdata, mRobin->routeReq(0), str.data(),
			//str.length(), true, 0, timeout, resp_buff);
	aos_assert_rr(rslt, rdata, false);

	// 4. if timeout
	if(timeout)
	{
		rdata->setError() << "send timeout";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	rdata->setOk();
	return true;
}

