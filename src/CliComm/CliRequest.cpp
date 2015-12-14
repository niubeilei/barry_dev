////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliRequest.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliComm/CliRequest.h"

#include "AosAlarm/AosAlarm.h"
#include "KernelInterface/CliProc.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"


static char *sgHeadTag = "<CliCommRequest>";
static char *sgHeadCloseTag = "</CliCommRequest>";

AosCliRequest::AosCliRequest(const OmnTcpClientPtr &client)
:
mTcpClient(client),
mBuffLen(0)
{
}


bool
AosCliRequest::procMsg(const OmnConnBuffPtr &buff)
{
	if (mBuffLen == 0)
	{
		if (buff->getDataLength() + mBuffLen >= eBuffLen)
		{
			OmnString err;
			err << "Data too long. Buffered: " << mBuffLen
				<< ", received: " << buff->getDataLength();
			sendResponse(eDataTooLong, err);
			return true;
		}

		memcpy(mBuff, buff->getBuffer(), buff->getDataLength());
		mBuffLen += buff->getDataLength();
	}	

	while (mBuffLen)
	{
		if (!parseRequest())
		{
			// 
			// It either failed to parse the message (a response is sent
			// and the connection is closed) or the buffered does not
			// contain a complete message.
			//
			return false;
		}

		// 
		// Now, we got the CLI command.
		//
		OmnTrace << "Received CLI command: " << mCmd << endl;

		OmnString rslt;
		// if (!OmnCliProc::getSelf()->runCli(mCmd, rslt))
		if (!OmnCliProc::getSelf()->runCli("app proxy add app1 http 192.168.1.81 5000 ssl", rslt))
		{
			cout << "Failed to run the command: " << mCmd << endl;
			sendResponse(eCmdError, rslt);
			return true;
		}

		cout << "The command successful: " << mCmd << endl;
		sendResponse(e200Ok, rslt);
	}

	return true;	
}


bool
AosCliRequest::sendResponse(const AosCliRequest::ReturnCode code, 
						 const OmnString &err)
{
	OmnString resp;

	if (!mTcpClient)
	{
		// 
		// The connection is not set. This should never happen.
		//
		aos_progerr(eAosAMD_CLI, eAosAlarm_NullPointer, 
			("TcpClient is null"));
		return false;
	}

	if (!mTcpClient->isConnGood())
	{
		// 
		// The connection is no longer good.
		//
		return false;
	}

	resp << "<CliCommResponse>"
		<<   "<ResponseCode>"
		<< 		mRequestId
		<<   "</ResponseCode>"
		<<   "<Results>"
		<<      err
		<<   "</Results>"
		<< "</CliCommResponse>";

	OmnTrace << "To send response: " << resp << endl;
	return mTcpClient->writeTo(resp.data(), resp.length());
}


bool
AosCliRequest::parseRequest()
{
	//
	// A CLI request is defined as:
	// 	<CliCommRequest>
	//		<RequestId>an-integer</RequestId>
	//		<Command>the-command</Command>
	//	</CliCommRequest>
	//
	// The request is stored in mBuff. Note that mBuff may contain
	// only partial contents of a request. The data length is mBuffLen.
	// 
	unsigned int index = 0;
	unsigned int start = 0;
	
	if (mBuffLen < eRequestMinLen)
	{
		// 
		// The buffer is too short to contain a complete message. 
		// 
		return false;
	}

	if (strncmp(&mBuff[index], "<CliCommRequest>", eHeadTagLen) != 0)
	{
		// 
		// Syntax error. Send a response and close the connection.
		//
		OmnString err;
		err << "Command syntax error: not started with <CliCommRequest>";
		sendResponse(eSyntaxError, err);
				
		// 
		// Close the connection.
		//
		if (mTcpClient)
		{
			mTcpClient->closeConn();
		}

		mBuffLen = 0;
		return false;
	}

	// 
	// Look for the end of the message.
	// 
	index = eHeadTagLen;
	start = index;
	while (index < mBuffLen)
	{
		if (mBuff[index] == '<')
		{
			if (mBuffLen - index < eHeadTagLen+1)
			{
				// 
				// The buffer is not long enough. 
				// 
				return false;
			}

			if (strncmp(&mBuff[index], sgHeadCloseTag, eHeadTagLen+1) == 0)
			{
				// 
				// Found the end of the message. 
				//
				index += eHeadTagLen+1;

				OmnXmlItem item = OmnXmlItem(sgHeadTag, &mBuff[start], index-1);

				mRequestId = item.getStr("RequestId", "000000");
				mCmd = item.getStr("Command", "");
				if (mCmd.length() <= 0)
				{
					// 
					// Failed to retrieve the command.
					//
					OmnString err;
					err << "Command syntax error: No Command";
					sendResponse(eSyntaxError, err);
					
					// 
					// Close the connection.
					//
					if (mTcpClient)
					{
						mTcpClient->closeConn();
					}

					mBuffLen = 0;
					return false;
				}
		
				if (mBuffLen > index)
				{
					memcpy(mBuff, &mBuff[index], mBuffLen - index);
				}
				mBuffLen -= index;
				return true;
			}
		}

		index++;
	}

	// 
	// Did not find the closing tag. 
	//
	return false;
}


