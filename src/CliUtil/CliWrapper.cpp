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
// 05/22/2007 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "CliUtil/CliWrapper.h"

#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"

#include <iostream>

static OmnTcpClientPtr	sgConn;

static const char EOC = 232;
static const char EOS = 231;
static const char EOU = 230;
static const char EOP = 252;

bool aos_run_cli(const OmnString &cmd, OmnString &rslt)
{
	if (!sgConn)	
	{
		OmnString errmsg;
		sgConn = OmnNew OmnTcpClient("CliConn", OmnIpAddr("127.0.0.1"), 
					4545, 1, OmnTcp::eNoLengthIndicator);
		if (!sgConn->connect(errmsg))
		{
			OmnAlarm << "Failed to connect to CLI Daemon: 127.0.0.1:4545"
				<< enderr;
			return false;
		}
	}

	OmnString msg;
	msg << "root" << EOU << "session" << EOS
		<< cmd << EOC;
	if (!sgConn->writeTo(msg.data(), msg.length()))
	{
		rslt = "Failed to send message";
		OmnAlarm << rslt << enderr;
		return false;
	}

OmnTrace << "Contents: " << msg.toHex() << endl;

	// 
	// Read the response
	//
	OmnConnBuffPtr buff;
	while (1)
	{
		buff = 0;
		bool connBroken;
		bool timeout = false;
		if (!sgConn->readFrom(buff, 3, timeout, connBroken, true))
		{
			OmnAlarm << "Failed to read" << enderr;
			return false;
		}

		if (timeout)
		{
			rslt = "Server not responding";
			return false;
		}

		OmnString tmp(buff->getData(), buff->getDataLength());
		OmnTrace << "Contents read: " << tmp.toHex() << endl;

		char *bb = buff->getData();
		char *p = (char *)memchr(bb, EOP, buff->getBufferLength());
		if (p)
		{
			// 
			// A whole response has been received. 
			// Retrieve the return code. The return code
			// is the four bytes prior to EOP.
			//
			if (buff->getDataLength() < 5)
			{
				OmnAlarm << "Received incorrect response: " 
					<< buff->getDataLength() << enderr;
				return false;
			}

			int *rc = (int *)(p - 4);
			if (*rc == 0)
			{
				// 
				// success
				//
				buff->setDataLength(0);
				return true;
			}

			// 
			// Command failed
			//
			rslt = bb;
			return false;
		}
	}

	return false;
}

