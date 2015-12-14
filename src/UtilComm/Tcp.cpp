////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tcp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/Tcp.h"

#include "Alarm/Alarm.h"
#include "Debug/ExitHandler.h"
#include "Porting/GetErrnoStr.h"
#include "Util/StrSplit.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>

#ifdef OMN_PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

extern int gAosShowNetTraffic;

#include <signal.h>
#include <limits.h>

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Message/Msg.h"
#include "Porting/Itoa.h"
#include "Porting/GetHostName.h"
#include "Porting/Select.h"
#include "Porting/Socket.h"
#include "Porting/GetErrno.h"
#include "Porting/Sleep.h"
#include "Porting/SystemError.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Util/SerialTo.h"
#include "Util/SerialFrom.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommTypes.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"
#include <vector>
using namespace std;

const int sgMaxIgnoredPorts = 100;
static vector<int> sgIgnoredPorts;


void cleanup(int sig)
{
    OmnTrace << "cleaning up" << endl;
 
    OmnTrace << "closed all  files" << endl;
    switch (sig)
    {
    case SIGINT:
         OmnTrace << "signal:: Interrupt " << endl;
         break;

 	case SIGTERM:
	     OmnTrace << "signal:  SIGTERM received!" << endl;
	     break;

#ifdef FTT_PLATFORM_UNIX
    case SIGHUP:
         OmnTrace << "signal:: Hangup " << endl;
         break;
 
    case SIGQUIT:
         OmnTrace << "signal:: Quit " << endl;
         break;
#endif
 
    default:
         OmnTrace << "signal:: unexpected " << sig << endl;
    }

    //SntGetTcpMgr().closeAllConnections();
	OmnExit;
}
 

 
OmnTcp::OmnTcp(AosAddrDef &addr_def, 
		   AosPortDef &ports, 
		   const OmnString &name, 
		   const AosTcpLengthType length_type)
:
mSock(-1),
mSockType(eTypeTcp),
mName(name),
mLock(OmnNew OmnMutex(name)),
mRemoteCrtPort(-1),
mRemoteNumPorts(0),
mLengthType(length_type),
mIsBlocking(true)
{
	// Currently we only support single address.
	addr_def.reset();
	mLocalIpAddr = addr_def.nextAddr();
	aos_assert(addr_def.getNumAddrs() > 0);

	// Currently we only support one range
	aos_assert(ports.getNumRanges() > 0);
	int low, high;
	ports.reset();
	ports.nextRange(low, high);
	aos_assert(low > 0);
	aos_assert(low <= high);
	aos_assert(high < 65535);
	mLocalStartPort = low;
	mLocalNumPorts = high - low + 1;
	mLocalCrtPort = low;
}


//
// This constructor is for TCP server. It does not need to know
// remote information at this moment.
//
OmnTcp::OmnTcp(const OmnIpAddr &localIpAddr,
			   const int localPort,
			   const int localNumPorts,
			   const OmnString &name,
			   const AosTcpLengthType lt)
:
mSock(-1),
mSockType(eTypeTcp),
mName(name),
mLock(OmnNew OmnMutex(name)),
mLocalIpAddr(localIpAddr),
mLocalCrtPort(localPort),
mLocalStartPort(localPort),
mRemoteCrtPort(-1),
mRemoteNumPorts(0),
mLocalNumPorts(localNumPorts),
mLengthType(lt),
mIsBlocking(true)
{

	//OmnTrace << "In TCP constructor: " << mSockType << endl;
}

//
// for unix domain sock
//
OmnTcp::OmnTcp(const OmnString &uPath,
			   const OmnString &name,
			   const AosTcpLengthType lt)
:
mSock(-1),
mSockType(eTypeUnix),
mUnixPath(uPath),
mName(name),
mLock(OmnNew OmnMutex(name)),
mLengthType(lt),
mIsBlocking(true)
{
}

OmnTcp::OmnTcp(const OmnIpAddr &remoteIpAddr,
			   const int remotePort,
			   const int remoteNumPorts,
			   const OmnIpAddr &localIpAddr,
			   const int localPort,
			   const int localNumPorts,
			   const OmnString &name,
			   const AosTcpLengthType lt)
:
mSock(-1),
mSockType(eTypeTcp),
mName(name),
mLock(OmnNew OmnMutex(name)),
mLocalIpAddr(localIpAddr),
mLocalCrtPort(localPort),
mLocalStartPort(localPort),
mRemoteIpAddr(remoteIpAddr),
mRemoteCrtPort(remotePort),
mRemoteStartPort(remotePort),
mRemoteNumPorts(remoteNumPorts),
mLocalNumPorts(localNumPorts),
mLengthType(lt),
mIsBlocking(true)
{
	if (!remoteIpAddr.isValid())
	{
		OmnAlarm << "Remote address incorrect: " << remoteIpAddr.toString()
			<< ":" << remotePort << enderr;
	}

	if (remotePort <= 0)
	{
		OmnAlarm << "Remote port invalid: " << remotePort << enderr;
	}
}


// 
// This constructor is for TCP Client. It does not need to know
// local information.
//
OmnTcp::OmnTcp(const OmnString &name,
			   const OmnIpAddr &remoteIpAddr,
			   const int remotePort,
			   const int remoteNumPorts,
			   const AosTcpLengthType lt)
:
mSock(-1),
mSockType(eTypeTcp),
mName(name),
mLock(OmnNew OmnMutex("OmnTcp")),
mLocalCrtPort(-1),
mLocalStartPort(0),
mRemoteIpAddr(remoteIpAddr),
mRemoteCrtPort(remotePort),
mRemoteStartPort(remotePort),
mRemoteNumPorts(remoteNumPorts),
mLocalNumPorts(1),
mLengthType(lt),
mIsBlocking(true)
{
	mLocalIpAddr = 0;
}


//
// Destructor
//
OmnTcp::~OmnTcp()
{
    OmnTraceComm << "OmnTcp deleted." << endl;
}


#if 0
// ice 2012/07/23
OmnRslt
OmnTcp::writeToSock(OmnIoBuff *buff, const int num_buffs)
{
	//
	// This function is called to write data to the sock. 
	// The data is stored in "data". If it failed
	// to write, it return false. Otherwise, it returns true.
	//
    //int bytesLeft = num_buffs;
	aos_assert_r(num_buffs >= 1 && num_buffs <= 3, false);

	for (int i=0; i<num_buffs; i++)
	{
		int bytesLeft = buff[i].iov_len;
		const char *data = (const char *)buff[i].iov_base;
    	while (bytesLeft > 0)
    	{
			int bytesWritten = OmnSendToSock(mSock, data, bytesLeft, mIsBlocking);
	
			if (bytesWritten <= 0)
			{
				// Failed to write. Check the errors and return.
   	       		if (EAGAIN == OmnGetErrno()||EWOULDBLOCK == OmnGetErrno())
   	       		{
					continue;
           		}
		
				if (mSock > 0)
				{
					OmnCloseSocket(mSock);
				}
				mSock = -1;
				return false;
			}

			data += bytesWritten;
			bytesLeft -= bytesWritten;
		}
	}
    return true;
}
#endif

OmnRslt
OmnTcp::writeToSock(OmnIoBuff *buff, const int num_buffs)
{
	//
	// This function is called to write data to the sock. 
	// The data is stored in "data". If it failed
	// to write, it return false. Otherwise, it returns true.
	//
    //int bytesLeft = num_buffs;
	// Chen Ding, 2013/01/09
	if (!mIsBlocking) return writeToSockNonBlock(buff, num_buffs);

	aos_assert_r(num_buffs >= 1 && num_buffs <= 3, false);
	int bytesLeft = 0;
	if (num_buffs == 2)
    	bytesLeft = buff[0].iov_len + buff[1].iov_len;
	else if (num_buffs == 1)
		bytesLeft = buff[0].iov_len;

	//const char *dataToWrite = data;
    int bytesWritten = 0;
    while (bytesLeft > 0)
    {
		try
		{
			bytesWritten = OmnWriteSock(mSock, buff, num_buffs);

			if (bytesWritten <= 0)
			{
				// Failed to write. Check the errors and return.
   	        	if(EAGAIN == OmnGetErrno()||EWOULDBLOCK == OmnGetErrno())
   	        	{
					continue;
            	}
			
				// Chen Ding, 08/19/2010
				// OmnRslt rslt = OmnWarn << "Failed to write TCP sock: " 
				// 	<< mLocalIpAddr.toString() << ":" << mLocalCrtPort << ", "
				// 	<< mSock << ", "
				// 	<< OmnGetStrError(OmnErrType::eSystem) << enderr;

				// handleWriteError(errcode);
				if (mSock > 0)
				{
					OmnCloseSocket(__FILE__, __LINE__, mSock);
				}
				mSock = -1;
				return false;
			}

			if (buff[0].iov_len == 0)
			{
				buff[1].iov_base =  (char*)buff[1].iov_base + bytesWritten;
				buff[1].iov_len -= bytesWritten;
			}
			else
			{
				if (buff[0].iov_len > (u32)bytesWritten)
				{
					buff[0].iov_base = (char*)buff[0].iov_base + bytesWritten;
					buff[0].iov_len -= bytesWritten;
				}
				else if (num_buffs == 2)
				{
					buff[1].iov_base = (char*)buff[1].iov_base + bytesWritten - buff[0].iov_len;
					buff[1].iov_len -= bytesWritten - buff[0].iov_len;
					buff[0].iov_len = 0;
				}
			}
     		bytesLeft -= bytesWritten;
    	}
		
		catch (const OmnExcept &e)
		{
			mSock = -1;
			OmnAlarm << "failed sending" << enderr;
			return false; 
		}
	}

	// Write successful
	// << mLocalIpAddr.toString() << ":" << mLocalCrtPort << ". Remote: "
	// << mRemoteIpAddr.toString() << ":" << mRemoteCrtPort <<endl;
    return true;
}


bool
OmnTcp::isConnGood() const
{
	//return OmnConn::isConnGood(mSock);
	return mSock != -1;
}

   
int
OmnTcp::checkEvent(fd_set *fdMask)
{
	if(mSock < 0)
	{
		return -1;
	}
	return FD_ISSET(mSock, fdMask)?0:1;
}


int 
OmnTcp::readLengthIndicator()
{
	// If successful, it returns the length indicator (an integer).
	// If the remote closed the connection, it returns -2. Otherwise,
	// it returns -1.
	//
	// This function assumes there is something to read. 
	
	unsigned char lenEncoded[5];
	char c;
	int nread;
	// int noTries = 0;
	const char SPECIAL_CHAR = (const char)0xff;
	int tries = 0;
	
	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
	case eAosTLT_FirstFourLow:
		 // 
		 // The length is the first four bytes, with the first byte the high byte 
		 // (eFirstFourHigh) or first byte the low byte (eFirstFourLow).
		 //
		 {
			 unsigned char bb[4];
			 int bytesLeft = 4;
			 int start = 0;
			 while (bytesLeft > 0)
			 {
			 	nread = OmnReadSock(mSock, &bb[start], bytesLeft);
				if (nread <= 0) return -1;
				aos_assert_r(nread <= bytesLeft, -1);
				start += nread;
				bytesLeft -= nread;
			 }

			 // Read two types.
			 if (mLengthType == eAosTLT_FirstFourHigh) 
			 {
				 return ((bb[0]) << 24) + 
					 (bb[1] << 16) + 
					 (bb[2] << 8) + bb[3];
			 }
			 
			 return ((bb[3]) << 8) + 
			 	(bb[2] << 16) + 
				(bb[1] << 8) + bb[0];
		 }
		 break;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // 
		 // The length is the first two bytes, with the first byte the high byte 
		 // (eFirstWordHigh) or first byte the low byte (eFirstWordLow).
		 //
		 {
			 unsigned char bb[2];
			 nread = OmnReadSock(mSock, &bb, 2);
			 if (nread < 2) return nread;

			 // Read two types.
			 if (mLengthType == eAosTLT_FirstWordHigh) 
				 return ((bb[0]) << 8) + bb[1];
			 return ((bb[1]) << 8) + bb[0];
		 }
		 break;

	case eAosTLT_LengthIndicator:
		 //
		 // Length indicator is in the format:
		 //	0xff 0xHH 0xLL 0xff
		 // where 0xHH is the higher byte of the length and 0xLL is the lower
		 // byte of the length. 
		 //
		 break;

	default:
		 OmnAlarm << "Length indicator error: " << mLengthType << enderr;
		 return -1;
	}

	while (1)	
	{
    	nread = OmnReadSock(mSock, &c, 1);
        if(nread <= 0)
        {
        	int eno = OmnGetErrno();
            if (eno == EINTR)
            {
				if (!isConnGood() || tries >= 3)
				{
					OmnTrace << "Conn broken" << endl;
					return -2;
				}

				 tries++;
               	 continue;
             }

			 if (nread == 0 && eno == 0)
			 {
				 // 
				 // This is a good indication that the remote side
				 // has closed the connection. It will not raise 
				 // alarms. It's up to the caller to determine whether
				 // to raise an alarm.
				 //
				 OmnTrace << "Conn broken" << endl;
				 return -2;
			 }

			 OmnWarn << "Read sock error. Return state" << enderr;
             return -1;
        }

		if (c != SPECIAL_CHAR)
		{
			OmnWarn << "Incorrect beginning of length indicator. Ignored!"
				 << enderr;
			continue;
		}

		//
		// Read the two bytes length
		//
		// noTries = 0;
		nread = OmnReadSock(mSock, (char *)lenEncoded, 2);
        if(nread <= 0)
        {
         	int eno = OmnGetErrno();
            if (eno == EINTR)
            {
				if (!isConnGood())
				{
					OmnTrace << "Conn broken" << endl;
					return -2;
				}

                //
                // It is interrupted system call. This is not an error.
                // There may be something going on which causes the system
                // interruption call. We simply ignore the call.
                //
                OmnWarn << "Interrupted system call detected. Ignored: " 
					<< mName << enderr;
                OmnSleep(1);
                continue;
            }
 
            if (nread == 0 && eno == 0)
            {
				// 
				// This is a good indication that the remote side
				// has closed the connection. It will not raise 
				// alarms. It's up to the caller to determine whether
				// to raise an alarm.
				//
				return -2;
            }
 
            OmnWarn << "Read sock error. Return: " 
				<< mName << enderr;
            return -1;
        }

		
		//
		// Read the second 0xff
		//
		// noTries = 0;
        nread = OmnReadSock(mSock, &c, 1);
        if(nread <= 0)
        {
            int eno = OmnGetErrno();
            if (eno == EINTR)
            {
				if (!isConnGood())
				{
					OmnTrace << "Conn broken" << endl;
					return -2;
				}

                //
                // It is interrupted system call. This is not an error.
                // There may be something going on which causes the system
                // interruption call. We simply ignore the call.
                //
                OmnWarn << "Interrupted system call detected. Ignored!"
					<< enderr;
                OmnSleep(1);
                continue;
            }

            if (nread == 0 && eno == 0)
            {
				// 
				// This is a good indication that the remote side
				// has closed the connection. It will not raise 
				// alarms. It's up to the caller to determine whether
				// to raise an alarm.
				//
				return -2;
            }

            OmnWarn << "Read sock error. Return state" << enderr;
            return -1;
        }

        if (c != SPECIAL_CHAR)
        {
            OmnWarn << "Incorrect beginning of length indicator. Ignored!"
				<< enderr;
            continue;
        }

		//
		// mod by lxx 08/20/2003 2003-0112
		//
		// int length = (lenEncoded[0] << 8) + lenEncoded[1];
		int length = (lenEncoded[1] << 8) + lenEncoded[0];

		// 
		// lxx, 08/17/2003, 2003-0112
		// 
		// if (length < 0 || length > 1000)
		if (length < 0 || length > eMaxTcpLength)
		{
			OmnWarn << "Tcp Packet length too long. Ignored: "
				<< length << enderr;
			continue;
		}

		return length;
	}
}


/*
OmnRslt
OmnTcp::writeTo(const char *data, const int nbytes)
{
	//
	// It sends the data "data" to the remote side. If the length type is 
	// eLengthIndicator, it first sends the data length and then the data itself. 
	//
	char bytes[2];
	OmnRslt rslt;
	switch (mLengthType)
	{
	case eAosTLT_NoLengthIndicator:
    case eAosTLT_TermByNewLine:
		 mLock->lock();
		 rslt = writeToSock(data, nbytes);
		 mLock->unlock();
		 return rslt;

    case eAosTLT_FirstWordHigh:
    case eAosTLT_FirstWordLow:
		 // Need to send the length bytes first
		 if (eAosTLT_FirstWordHigh)
		 {
		 	bytes[1] = (char)nbytes;
		 	bytes[0] = (char)(nbytes >> 8);
		 }
		 else
		 {
		 	bytes[0] = (char)nbytes;
		 	bytes[1] = (char)(nbytes >> 8);
		 }

		 mLock->lock();
		 if (!writeToSock(bytes, 2))
		 {
			 mLock->unlock();
			 OmnAlarm << "Failed to send the length bytes" << enderr;
			 return false;
		 }
		 if (!writeToSock(data, nbytes))
		 {
			 OmnAlarm << "Failed to send the data: " << nbytes << enderr;
			 mLock->unlock();
			 return false;
		 }
		 mLock->unlock();
		 return true;

    case eAosTLT_LengthIndicator:
		 break;

	default:
		 return OmnAlarm << "Invalid length type: " << mLengthType << enderr;
 	}
		 
	mLock->lock();
	//
	// The length is sent in the form of:
	//
	//		0xff
	//		higher byte
	//		lower byte
	//		0xff
	//
	char lengthBuf[5];
	lengthBuf[(size_t)0] = (char)0xff;
	lengthBuf[(size_t)3] = (char)0xff;

	//
	// Write the length. Note that the maximum length should never be more 
	// than two bytes long.
	//

	//
	// mod by lxx 08/20/2003 2003-0112
	//
	lengthBuf[(size_t)1] = (char)nbytes;
	lengthBuf[(size_t)2] = (char)(nbytes >> 8);

	//
	// If write fails, it reconnect and then write again
	// until it succeeds.
	//
	if (!writeToSock((const char *)lengthBuf, 4))
	{
		OmnAlarm << "Tcp failed to write length." << enderr;
		mLock->unlock();
		return false;
	}

	//
	// Now it is the time to write the data itself.
	// If write fails, it reconnects and then goes back to the 
	// beginning of the outer loop to write the length indicator
	// again.
	//
	if (!writeToSock(data, nbytes))
	{
		OmnAlarm << "Failed to write the data: " << nbytes << enderr;
		mLock->unlock();
		return false;
	}

	mLock->unlock();
	return true;
}
*/


/*
OmnRslt     
OmnTcp::writeTo(const char *data,
                const int length,
                const OmnIpAddr &recvIpAddr,
                const int recvPort)
{
	//
	// If the remote IP address and port does not match this class's
	// it is an error.
	//
	if (recvIpAddr != mRemoteIpAddr || recvPort != mRemoteCrtPort)
	{
		return OmnWarn << "Use a datagram through a TCP connection. It seems to be an"
			<< " program error. The intented receiver: " 
			<< recvIpAddr.toString() << ":" << recvPort
			<< " but the TCP connection is: " 
			<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort << enderr;
	}

	return writeTo(data, length);
}
*/

void
OmnTcp::handleWriteError(const OmnErrId::E errcode)
{
	//
	// A write error occured. 'errcode' is the current error code. This 
	// function prints an error message, and then determines what to do
	// to handle the error.
	//
	//
	// Close the sock
	// 
	if (mSock > 0)
	{
		OmnCloseSocket(__FILE__, __LINE__, mSock);
	}
	mSock = -1;
	return;
}
	

bool
OmnTcp::hasPacketToRead() const
{
    if (mSock <= 0 || mSock >= OmnCommTypes::eMaxSockToSelect)
    {
        //
        // The sock is not correct
        //
        return false;
    }

    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(mSock, &fd);

    timeval timer;
    timer.tv_sec = 0;
    timer.tv_usec = 0;
    int rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, &timer);
    return (rslt > 0);
}


bool
OmnTcp::smartSend(const char *data, const int len)
{
	// This function will send the given number of bytes. It may need to 
	// send length indicator as needed. 
	if (gAosShowNetTraffic > 1) 
	{
		bool ignored = false;
		for (u32 i=0; i<sgIgnoredPorts.size(); i++)
		{
			if (sgIgnoredPorts[i] == mRemoteCrtPort)
			{
				ignored = true;
				break;
			}
		}

		if (!ignored)
		{
			OmnScreen << "To send: " << mRemoteIpAddr.toString() << ":" 
				<< mRemoteCrtPort << ":" << len << endl;
		}
	}
	aos_assert_r(len > 0, false);
	char bytes[5];
	OmnRslt rslt;

	OmnIoBuff iobuff[2];

	iobuff[0].iov_base = bytes;

	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
		 bytes[0] = (len >> 24);
		 bytes[1] = (len >> 16);
		 bytes[2] = (len >> 8);
		 bytes[3] = len;

		 mLock->lock();
		 iobuff[0].iov_len = 4;
		 iobuff[1].iov_base = (void*)data;
		 iobuff[1].iov_len= len;

		 rslt = writeToSock(iobuff, 2);
		 mLock->unlock();
		 return rslt;

	case eAosTLT_FirstFourLow:
		 bytes[3] = (len >> 24);
		 bytes[2] = (len >> 16);
		 bytes[1] = (len >> 8);
		 bytes[0] = len;
		 mLock->lock();
		 iobuff[0].iov_len = 4;

		 iobuff[1].iov_base = (void*)data;
		 iobuff[1].iov_len= len;

		 rslt = writeToSock(iobuff, 2);
		 mLock->unlock();
		 return rslt;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // Chen Ding, 08/18/2010
		 {
			 int bytesLeft = len;
			 aos_assert_r(bytesLeft < eTwoByteMaxLen, false);
			 mLock->lock();
			 int start = 0;
			 if (mLengthType == eAosTLT_FirstWordHigh)
			 {
		 		bytes[0] = (bytesLeft >> 8);
		 		bytes[1] = bytesLeft;
			 }
			 else
			 {
		 		bytes[1] = (bytesLeft >> 8);
		 		bytes[0] = bytesLeft;
			 }
			 iobuff[0].iov_len = 2;

			 iobuff[1].iov_base = (void*)&data[start];
			 iobuff[1].iov_len= bytesLeft;

			 rslt = writeToSock(iobuff, 2);
			 mLock->unlock();
			 return rslt;
		 }
		 break;

	case eAosTLT_LengthIndicator:
	case eAosTLT_TermByNewLine:
		 OmnNotImplementedYet;
		 return false;

	case eAosTLT_NoLengthIndicator:
		 mLock->lock();
		 iobuff[0].iov_base = (void*)data;
		 iobuff[0].iov_len= len;
		 rslt = writeToSock(iobuff, 1);
		 mLock->unlock();
		 return rslt;

	default:
		 OmnAlarm << "Invalid length type: " << mLengthType << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
OmnTcp::smartSend(
		const char *data1, 
		const int len1, 
		const char *data2,
		const int len2)
{
	// This function will send the given number of bytes. It may need to 
	// send length indicator as needed. 
	if (gAosShowNetTraffic > 1)
	{
		bool ignored = false;
		for (u32 i=0; i<sgIgnoredPorts.size(); i++)
		{
			if (sgIgnoredPorts[i] == mRemoteCrtPort)
			{
				ignored = true;
				break;
			}
		}

		if (!ignored)
		{
			OmnScreen << "To send: " << mRemoteIpAddr.toString() << ":" 
				<< mRemoteCrtPort << ":" << len1 << ":" << len2 << endl;
		}
	}
	int len = len1 + len2;
	aos_assert_r(len > 0, false);
	char bytes[5];
	OmnRslt rslt;

	OmnIoBuff iobuff[3];

	iobuff[0].iov_base = bytes;

	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
		 bytes[0] = (len >> 24);
		 bytes[1] = (len >> 16);
		 bytes[2] = (len >> 8);
		 bytes[3] = len;

		 mLock->lock();
		 iobuff[0].iov_len = 4;
		 iobuff[1].iov_base = (void*)data1;
		 iobuff[1].iov_len= len1;
		 rslt = writeToSock(iobuff, 2);
		 aos_assert_rl(rslt, mLock, false);

		 iobuff[0].iov_base = (void*)data2;
		 iobuff[0].iov_len= len2;
		 rslt = writeToSock(iobuff, 1);
		 mLock->unlock();
		 return rslt;

	case eAosTLT_FirstFourLow:
		 bytes[3] = (len >> 24);
		 bytes[2] = (len >> 16);
		 bytes[1] = (len >> 8);
		 bytes[0] = len;
		 mLock->lock();
		 iobuff[0].iov_len = 4;

		 iobuff[1].iov_base = (void*)data1;
		 iobuff[1].iov_len= len1;
		 rslt = writeToSock(iobuff, 2);
		 aos_assert_rl(rslt, mLock, false);

		 iobuff[0].iov_base = (void*)data2;
		 iobuff[0].iov_len= len2;

		 rslt = writeToSock(iobuff, 1);
		 mLock->unlock();
		 return rslt;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // Chen Ding, 08/18/2010
		 {
			 aos_assert_r(len < eTwoByteMaxLen, false);
			 mLock->lock();
			 if (mLengthType == eAosTLT_FirstWordHigh)
			 {
		 		bytes[0] = (len>> 8);
		 		bytes[1] = len;
			 }
			 else
			 {
		 		bytes[1] = (len>> 8);
		 		bytes[0] = len;
			 }
			 iobuff[0].iov_len = 2;

			 iobuff[1].iov_base = (void*)data1;
			 iobuff[1].iov_len= len1;
		 	rslt = writeToSock(iobuff, 3);
		 	aos_assert_rl(rslt, mLock, false);

			 iobuff[0].iov_base = (void*)data2;
			 iobuff[0].iov_len= len2;

			 rslt = writeToSock(iobuff, 1);
			 mLock->unlock();
			 return rslt;
		 }
		 break;

	case eAosTLT_LengthIndicator:
	case eAosTLT_TermByNewLine:
		 OmnNotImplementedYet;
		 return false;

	case eAosTLT_NoLengthIndicator:
		 OmnNotImplementedYet;
		 return false;
		 // mLock->lock();
		 // iobuff[0].iov_base = (void*)data;
		 // iobuff[0].iov_len= len;
		 // rslt = writeToSock(iobuff, 1);
		 // mLock->unlock();
		 // return rslt;

	default:
		 OmnAlarm << "Invalid length type: " << mLengthType << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


int
OmnTcp::smartRead(OmnConnBuffPtr &buff)
{
	// This function will read a package based on the length type. 
	// If there is no length type, it will report errors. 
	// Note that this function will create a new buff. 
	if (gAosShowNetTraffic > 1)
	{
		bool ignored = false;
		u32 size = sgIgnoredPorts.size();
		for (u32 i=0; i<size; i++)
		{
			if (sgIgnoredPorts[i] == mRemoteCrtPort)
			{
				ignored = true;
				break;
			}
		}

		if (!ignored)
		{
			OmnScreen << "To send: " << mRemoteIpAddr.toString() << ":" 
				<< mRemoteCrtPort << endl;
		}
	}
	int len;
	char *data;
	//int bytesRead;
	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
	case eAosTLT_FirstFourLow:
	case eAosTLT_LengthIndicator:
		 mLock->lock();
		 if (mSock <= 0) 
		 {
			 mLock->unlock();
			 return -1;
		 }

		 len = readLengthIndicator();
		 if (len <= 0)
		 {
			 // The remote side closed the connection gracefully. 
		 	 mLock->unlock();
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
		 	 mLock->unlock();
			 return -1;
		 }

		 try
		 {
		 	 buff = OmnNew OmnConnBuff((u32)len);
		 	 aos_assert_rl(buff, mLock, -1);
		 	 data = buff->getBuffer();
		 	 if (!readFromSock(data, len))
			 {
				 OmnAlarm << "Failed to read: " << mSock << ":" << mRemoteCrtPort << enderr;
				 closeConn();
				 buff->setDataLength(0);
		 	 	 mLock->unlock();
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
		 	 mLock->unlock();
			 return -1;
		 }
		 mLock->unlock();
		 return len;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // Chen Ding, 08/18/2010, ZKY-xxx
		 // It needs to handle the case that the package is larger than 
		 // the limit of two bytes. 
		 buff = 0;
		 len = 0;
		 mLock->lock();
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
		 			mLock->unlock();
					return true;
				}

			 	// The remote side closed the connection gracefully. 
		 		mLock->unlock();
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
		 			aos_assert_rl(buff, mLock, -1);
				}
				aos_assert_rl(buff->determineMemory(len), mLock, -1);
		 		data = buff->getBuffer();
		 		aos_assert_rl(readFromSock(&data[start], theln), mLock, -1);

				if (theln != eTwoByteMaxLen)
				{
					buff->setDataLength(len);
		 			mLock->unlock();
					return true;
				}
			}

			catch (const OmnExcept &e)
			{
			 	OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
			 	closeConn();
			 	buff->setDataLength(0);
		 		mLock->unlock();
			 	return -1;
			}
		 }
	     mLock->unlock();
		 break;

	case eAosTLT_Xml:
	case eAosTLT_TermByZero:
	case eAosTLT_TermByNewLine:
		 OmnNotImplementedYet;
		 return -1;

	case eAosTLT_NoLengthIndicator:
		 //Jozhi 2014-08-06
	     mLock->lock();
		 try
		 {
		 	 buff = OmnNew OmnConnBuff();
		 	 aos_assert_rl(buff, mLock, -1);
		 	 data = buff->getBuffer();
   		 	 int bytesRead = OmnReadSock(mSock, data, buff->getBufferLength()-1);
		 	 aos_assert_rl(bytesRead >= 0, mLock, -1);
		 	 buff->setDataLength(bytesRead);
	     	 mLock->unlock();
		 	 return bytesRead;
		 }

		 catch (const OmnExcept &e)
		 {
			 OmnAlarm << "Exception caught: " << e.getErrmsg() << enderr;
			 closeConn();
			 buff->setDataLength(0);
	     	 mLock->unlock();
			 return -1;
		 }
	     mLock->unlock();
		 break;

	default:
		 OmnAlarm << "Invalid length type: " << mLengthType << enderr;
		 return -1;
	}

	return -1;
}


OmnRslt
OmnTcp::readFromSock(char *data, int &bytesToRead)
{
	//
	// This function should be called when one wants to read 
	// a known number of bytes from the connection of this
	// instance. In case the message size is too big and can't be
	// read in one time, it will keep on reading until all 
	// bytes are read. THe caller needs to make sure 'data'
	// has at least 'bytesToRead' number of bytes of memory.
	//
	// This function uses the operating system call to retrieve
	// data from the connection. It will read as much data as 
	// available, up to the size of the buffer. If the connection
	// has more than 'bytesToRead', the remaining data
	// is retained in the connection, which can be retrieved by
	// calling the function again.
	//
	// If no data available in the connection, it will block
	// unless the sock is non-blocking. 
	//
	// If the remote side shuts down gracefully, and all data
	// has been read, calling this function will return 0. 
	//
	// If the read function returns a positive number, we consider
	// it successful. Set the buffer and return.
	//
	// If it returns 0, if it is Microsoft, it means the remote
	// side has gracefully shut down the connection. (For Unix 
	// part, we need to check). Set the buffer accordingly and 
	// return false.
	//
	// Otherwise, it is an error. If it is Microsoft, it returns
	// SOCKET_ERROR and the actual error is retrieved through 
	// OmnGetErrno(), which will retrieve the last error code 
	// occured in this thread. 
	//
	// If reading is successful, 'data' is set properly. If 
	// failed, 'data' is set to fail, fail code, and fail msg.
	// false is returned.
	//

	//
	// Read the data
	//
	int bytesRemaining = bytesToRead;
	int index = 0;
	while (bytesRemaining > 0)
	{
		int ssize = 32767;
		int readsize = bytesRemaining>ssize?ssize:bytesRemaining;
		//int readsize = bytesRemaining>SSIZE_MAX?SSIZE_MAX:bytesRemaining;
   	 	int bytesRead = OmnReadSock(mSock, &data[index], readsize);
//OmnScreen << "bytesRead" <<(int)this <<":" << bytesRead <<":" << &data[index] <<endl;
/*
for(int i = 0 ;i <bytesRead; i++)
{
	if (i%10 == 0) cout << "\t";
	if (i%40 == 0) cout << "\n";
	cout << (int)data[index+i] << " " ;
}
cout << endl;*/
		if (bytesRead < 0)
		{
			//
			// Reading failed. Check the error conditions
			//
			if (OmnIsTcpConnectionShutDown(bytesRead))
			{
				//
				// The remote side has gracefully shut down the connection. This
				// is an error.
				//
				OmnAlarm << OmnErrId::eSocketReadError
					<< "Failed to read TCP connection because remote side shut down. "
					<< "Connection Info" << enderr;
				return false;
			}

			//
			// Read failed. 
			// 
			return OmnAlarm << OmnErrId::eSocketReadError
				<< "Failed to read: " << OmnGetStrError(OmnErrType::eRead)
				<< ". Local address: " << mLocalIpAddr.toString() 
				<< ":" << mLocalCrtPort
				<< ". Remote address: " << mRemoteIpAddr.toString() 
				<< ":" << mRemoteCrtPort 
				<< enderr;
		}

		if (bytesRead == 0)
		{
			// 
			// This is still an error. 
			//
			return OmnAlarm << OmnErrId::eSocketReadError
				<< "Read 0 bytes: " << OmnGetStrError(OmnErrType::eRead)
				<< ". Local address: " << mLocalIpAddr.toString() 
				<< ":" << mLocalCrtPort
				<< ". Remote address: " << mRemoteIpAddr.toString() 
				<< ":" << mRemoteCrtPort 
				<< enderr;
		}

		//
		// Reading successful. Set the buffer and return.
		//
		bytesRemaining -= bytesRead;
		index += bytesRead;
	}
	return true;
}


// Chen Ding, 06/11/2012
void
OmnTcp::setIgnoredPorts(const OmnString &portstr)
{
	// Ports are represented as:
	// 		port,port,...
	vector<OmnString> ports;
	sgIgnoredPorts.clear();
	AosStrSplit::splitStrByChar(portstr, ",", ports, 100);
	for (u32 i=0; i<ports.size(); i++)
	{
		sgIgnoredPorts.push_back(atoi(ports[i].data()));
	}
}


// Chen Ding, 2013/01/09
bool
OmnTcp::writeToSockNonBlock(OmnIoBuff *buff, const int num_buffs)
{
	// This function is called from 'writeToSock(...)'. The only difference
	// is this function assumes the socket is non-blocking.
	aos_assert_r(!mIsBlocking, false);
	aos_assert_r(num_buffs >= 1 && num_buffs <= 3, false);
	int bytesLeft = 0;
	if (num_buffs == 2)
    	bytesLeft = buff[0].iov_len + buff[1].iov_len;
	else if (num_buffs == 1)
		bytesLeft = buff[0].iov_len;

	//const char *dataToWrite = data;
    int bytesWritten = 0;
    while (bytesLeft > 0)
    {
		try
		{
			bytesWritten = OmnWriteSock(mSock, buff, num_buffs);
			if (bytesWritten <= 0)
			{
				// Failed to write. Check the errors and return.
				int eno = OmnGetErrno();
				fd_set fds;
				if (eno == EAGAIN || eno == EWOULDBLOCK)
				{
					// This means that the write would block. Need to wait until
					// it is ready to write. 
					FD_ZERO(&fds);
					FD_SET(mSock, &fds);
					if (OmnSocketSelect(mSock+1, 0, &fds, 0, 0) < 0)
					{
						eno = OmnGetErrno();
						OmnCloseSocket(__FILE__, __LINE__, mSock);
						mSock = -1;
						return false;
					}

					// It can write again.
					continue;
				}

				// The connectino is broken. 
				OmnCloseSocket(__FILE__, __LINE__, mSock);
				mSock = -1;
				return false;
			}

			if (buff[0].iov_len == 0)
			{
				buff[1].iov_base =  (char*)buff[1].iov_base + bytesWritten;
				buff[1].iov_len -= bytesWritten;
			}
			else
			{
				if (buff[0].iov_len > (u32)bytesWritten)
				{
					buff[0].iov_base = (char*)buff[0].iov_base + bytesWritten;
					buff[0].iov_len -= bytesWritten;
				}
				else if (num_buffs == 2)
				{
					buff[1].iov_base = (char*)buff[1].iov_base + bytesWritten - buff[0].iov_len;
					buff[1].iov_len -= bytesWritten - buff[0].iov_len;
					buff[0].iov_len = 0;
				}
			}
     		bytesLeft -= bytesWritten;
    	}
		
		catch (const OmnExcept &e)
		{
			mSock = -1;
			OmnAlarm << "failed sending" << enderr;
			return false; 
		}
	}

    return true;
}

