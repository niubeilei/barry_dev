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
//  copied from TcpClient 
//
// Modification History:
// 2015/08/21 Created by White
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/TcpClientSingleThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Message/Msg.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/Itoa.h"
#include "Porting/IPv6.h"
#include "Porting/Socket.h"
#include "Porting/Select.h"
#include "Porting/SockLen.h"
#include "Porting/BindErr.h"
#include "Thread/Mutex.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/CommUtil.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpMsgReader.h"

#include <sys/un.h>
#include <sys/types.h>
#include <signal.h>

OmnTcpClientSingleThrd::OmnTcpClientSingleThrd(const OmnString &name,
						   const OmnIpAddr &remoteIpAddr,
						   const int remotePort,
						   const int numPorts,
						   const AosTcpLengthType lt)
:
OmnTcpClient(name, remoteIpAddr, remotePort, numPorts, lt)
{
}


OmnTcpClientSingleThrd::OmnTcpClientSingleThrd(const OmnString &name,
						   const OmnString &uPath,
						   const AosTcpLengthType lt)
:
OmnTcpClient(name, uPath, lt)
{
}

OmnTcpClientSingleThrd::OmnTcpClientSingleThrd(const OmnIpAddr &remoteIpAddr,
		   				   const int remotePort,
		   				   const int remoteNumPorts,
		   				   const OmnIpAddr &localIpAddr,
		   				   const int localPort,
		   				   const int localNumPorts,
		   				   const OmnString &name,
		   				   const AosTcpLengthType lt)
:
OmnTcpClient(remoteIpAddr, remotePort, remoteNumPorts, localIpAddr, localPort, localNumPorts, name, lt)
{
	//OmnScreen << "TcpClient Created: " << this << endl;
	//mConnBuff = OmnNew OmnConnBuff();
	OmnTraceComm << "Create TCP TcpClientSingleThrd: " << this << endl;
}


OmnTcpClientSingleThrd::~OmnTcpClientSingleThrd()
{
}


int
OmnTcpClientSingleThrd::smartRead(OmnConnBuffPtr &buff)
{
	int len;
	char *data;
	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
	case eAosTLT_FirstFourLow:
	case eAosTLT_LengthIndicator:
		 if (mSock <= 0) 
		 {
			 return -1;
		 }

		 len = OmnTcp::readLengthIndicator();
		 if (len <= 0)
		 {
			 // The remote side closed the connection gracefully. 
			 return 0;
		 }
		 //aos_assert_m(len > 0, -1, "len=%d", len);
		 if (len > OmnConnBuff::eMaxBuffSize)
		 {
			 OmnAlarm << "Buff too long: " << len << ":" << mLengthType 
				 << ". Remote: " << mRemoteIpAddr.toString() 
				 << ":" << mRemoteCrtPort << enderr;
			 if (buff) buff->setDataLength(0);
			 closeConn();
			 return -1;
		 }

		 try
		 {
		 	 buff = OmnNew OmnConnBuff((u32)len);
		 	 aos_assert_r(buff, -1);
		 	 data = buff->getBuffer();
		 	 if (!readFromSock(data, len))
			 {
				 OmnAlarm << "Failed to read: " << mSock << ":" << mRemoteCrtPort << enderr;
				 closeConn();
				 buff->setDataLength(0);
				 return false;
			 }

			 // OmnScreen << "-------- Read contents: " 
			 // << mLocalIpAddr.toString() << ":" << mLocalCrtPort
			 // << ":" << mRemoteIpAddr.toString() 
			 // << ":" << mRemoteCrtPort << ":" << buff->getData() << endl;

			 // Chen Ding, 11/02/2012 
			 // if (len == 13 && data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 13 &&
			 // 	(strncmp("<headrbea", &data[4], 9) == 0))
			 // {
			 // 	OmnAlarm << " find error" << enderr;
			 // 	int idx = 0;
			 // 	while (idx < 20)
			 //		{
			 //			char tmpbuff[30];
			 //			read(mSock, tmpbuff, 20);
			 //			idx++;
		     //		}
			 // }

		 	 buff->setDataLength(len);
		 }

		 catch (const OmnExcept &e)
		 {
			 OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
			 closeConn();
			 buff->setDataLength(0);
			 return -1;
		 }
		 return len;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // Chen Ding, 08/18/2010, ZKY-xxx
		 // It needs to handle the case that the package is larger than 
		 // the limit of two bytes. 
		 buff = 0;
		 len = 0;
		 while (1)
		 {
		 	int theln = readLengthIndicator();
		 	if (theln == 0)
		 	{
				if (len > 0)
				{
					// This means that there are multiple packages and 
					// this package is 0 length. 
					buff->setDataLength(len);
					return true;
				}

			 	// The remote side closed the connection gracefully. 
			 	return 0;
		 	}
			int start = len;
			len += theln;
		 	//aos_assert_m(theln > 0, -1, "theln=%d", theln);
			try
			{
		 		if (!buff) 
				{
					buff = OmnNew OmnConnBuff((u32)theln);
		 			aos_assert_r(buff, -1);
				}
				aos_assert_r(buff->determineMemory(len), -1);
		 		data = buff->getBuffer();
		 		aos_assert_r(readFromSock(&data[start], theln), -1);

				if (theln != eTwoByteMaxLen)
				{
					buff->setDataLength(len);
					return true;
				}
			}

			catch (const OmnExcept &e)
			{
			 	OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
			 	closeConn();
			 	buff->setDataLength(0);
			 	return -1;
			}
		 }
		 break;

	case eAosTLT_Xml:
	case eAosTLT_TermByZero:
	case eAosTLT_TermByNewLine:
		 OmnNotImplementedYet;
		 return -1;

	case eAosTLT_NoLengthIndicator:
		 //Jozhi 2014-08-06
		 try
		 {
		 	 buff = OmnNew OmnConnBuff();
		 	 aos_assert_r(buff, -1);
		 	 data = buff->getBuffer();
   		 	 int bytesRead = OmnReadSock(mSock, data, buff->getBufferLength()-1);
		 	 aos_assert_r(bytesRead >= 0, -1);
		 	 buff->setDataLength(bytesRead);
		 	 return bytesRead;
		 }

		 catch (const OmnExcept &e)
		 {
			 OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
			 closeConn();
			 buff->setDataLength(0);
			 return -1;
		 }
		 break;

	default:
		 OmnAlarm << "Invalid length type: " << mLengthType << enderr;
		 return -1;
	}

	return -1;
}

