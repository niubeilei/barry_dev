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
//
// Modification History:
// 05/31/2011	Created by Wynn 
////////////////////////////////////////////////////////////////////////////

#include "EmailServer/EmailServer.h" 

#include "Porting/Sleep.h"
#include "TransServer/TransServer.h"
#include "EmailServer/Pop3Server.h"
#include "EmailServer/SmtpServer.h"
#include "Rundata/Rundata.h"



AosEmailServer::AosEmailServer(const AosXmlTagPtr &config)
:
mPop3Svr(OmnNew AosPop3Server(config)),
mSmtpSvr(OmnNew AosSmtpServer(config))
{
}


AosEmailServer::~AosEmailServer()
{
}


bool
AosEmailServer::proc(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	aos_assert_r(data, false);

	AosXmlTagPtr doc = data->getFirstChild();
	aos_assert_r(doc, false);
	// 1. Get opr
	OmnString opr = doc->getAttrStr(AOSTAG_OPERATOR);
	aos_assert_r(opr != "", false);
	
	// Do something by opr 
	OmnString errmsg;
	if (opr == "login")
	{
		bool rslt = mPop3Svr->login(doc, rdata);
		if (!rslt)
		{
			errmsg = "Login to pop3 server failed!";
			rdata->setError() << errmsg;
			OmnAlarm << errmsg << enderr;
			rdata->setContents(errmsg);
			return false;
		}
	}
	else if (opr == "dele")
	{
		bool rslt = mPop3Svr->dele(doc, rdata);
		if (!rslt)
		{
			rdata->setError() << "Delete email failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else if (opr == "send")
	{
		bool rslt = mSmtpSvr->send(doc, rdata);
		if (!rslt)
		{
			rdata->setError() << "Send email failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else
	{
		rdata->setError() << "Unreconize Operation ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


