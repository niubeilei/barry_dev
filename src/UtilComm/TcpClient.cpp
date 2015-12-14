////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpClient.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpClient.h"

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

//
// Class variables
//
const int sgDefaultReconnTrialTimer = 10;



//
// Constructor
//
OmnTcpClient::OmnTcpClient(const OmnString &name,
						   const OmnIpAddr &remoteIpAddr,
						   const int remotePort,
						   const int numPorts,
						   const AosTcpLengthType lt)
:
OmnTcp(name, remoteIpAddr, remotePort, numPorts, lt),
mPortUsed(-1), 
mClientId(0),
mLengthIndicatorPos(0)
{
	//OmnScreen << "TcpClient Created: " << this << ":" << mSock << endl;

	if (!remoteIpAddr.isValid())
	{
		OmnAlarm << "Remote address invalid: " << remoteIpAddr.toString() << enderr;
	}

	if (remotePort <= 0)
	{
		OmnAlarm << "Remote port invalid: " << remotePort << enderr;
	}

	//mConnBuff = OmnNew OmnConnBuff();
	OmnTraceComm << "Create TCP Client: " << this
		<< " Remote: " << remoteIpAddr.toString() 
		<< ":" << remotePort
		<< ":" << numPorts 
		<< ". " << this
		<< endl;
		
}

OmnTcpClient::OmnTcpClient(const OmnString &name,
						   const OmnString &uPath,
						   const AosTcpLengthType lt)
:
OmnTcp(uPath, name, lt),
mPortUsed(-1), 
mClientId(0),
mLengthIndicatorPos(0)
{
	//OmnScreen << "TcpClient Created: " << this << ":" << mSock << endl;
	//mConnBuff = OmnNew OmnConnBuff();
	OmnTraceComm << "Create TCP Client: " << this;
}

OmnTcpClient::OmnTcpClient(const OmnIpAddr &remoteIpAddr,
		   				   const int remotePort,
		   				   const int remoteNumPorts,
		   				   const OmnIpAddr &localIpAddr,
		   				   const int localPort,
		   				   const int localNumPorts,
		   				   const OmnString &name,
		   				   const AosTcpLengthType lt)
:
OmnTcp(remoteIpAddr, remotePort, remoteNumPorts,
	   localIpAddr,  localPort,  localNumPorts,
	   name, lt),
mPortUsed(-1), 
mClientId(0)
{
	//OmnScreen << "TcpClient Created: " << this << endl;
	//mConnBuff = OmnNew OmnConnBuff();
	OmnTraceComm << "Create TCP Client: " << this << endl;
}


//
// Destructor
//
OmnTcpClient::~OmnTcpClient()
{
	//OmnScreen << "TcpClient deleted: " << this << ":" << mSock << endl;
	closeConn();
	mConnStatus = CLOSED;
	mPortUsed = -1;
}


OmnRslt
OmnTcpClient::connect(OmnString &err)
{
	// This function is used to create the connection.
	// Starting from the start port, it tries to create a connection
	// on every port in the range [mRemotePort, mRemotePort + mNumPorts].
	// If a connect is successful, it returns. If all fail, it returns
	// false.

	// Check whether the connection is already there. If yes,
	// simply return.

	// Chen Ding, 03/01/2012
	mLock->lock();
	err = "";
	if (OmnConn::isConnGood(mSock))
	{
		mLock->unlock();
		return true;
	}

	//
	// If the sock mSock is not 0, need to close it first
	//
	if (mSock > 0)
	{
		OmnCloseSocket(__FILE__, __LINE__, mSock);
		mSock = -1;
	}

	bool createSock = true;

	if (mSockType == eTypeTcp)
	{
		for (int i=0; i<mRemoteNumPorts; i++)
		{
			if (!tryToConnect(createSock, err))
			{
				mRemoteCrtPort++;
				if (mRemoteCrtPort >= mRemoteStartPort + mRemoteNumPorts)
				{
				 	mRemoteCrtPort = mRemoteStartPort;
				}
			}
			else
			{
				// Connection successful
				mLock->unlock();
				return true;
			}
	
			createSock = false;
		}
	}
	else if (mSockType == eTypeUnix)
	{
		if (tryToConnect(createSock, err))
		{
			mLock->unlock();
			return true;
		}

		createSock = false;
	}


	// Connection failed. The function has raised an alarm. We simply
	// return here.
    OmnCloseSocket(__FILE__, __LINE__, mSock);
	mSock = -1;
	mLock->unlock();
	return false;
}


OmnRslt
OmnTcpClient::tryToConnect(const bool createSock, OmnString &err)
{
	//
	// This is the key part of the class. It establishes the connection. 
	// The caller should make sure that the connection is not there. 
	// This function will check no more. The caller should have locked
	// the connection.
	//
	bool bConnected = false;

	if (createSock)
	{
		/*OmnTraceComm << "Create TCP Client: " << this 
			<< " to: " << mRemoteIpAddr.toString() 
			<< ":" << mRemoteCrtPort
			<< endl;*/

		if (mSockType == eTypeTcp)
		{
        	// Create the sock
        	if ((mSock = OmnCreateTcpSocket(__FILE__, __LINE__)) < 0)
        	{
				err = "Failed to create sock to: ";
				err << mRemoteIpAddr.toString() << ":" << mRemoteCrtPort
           	     << ". Error No.: " << OmnGetErrno();

            	return OmnAlarm << err << enderr;
         	}

			//OmnScreen << "Sock created: " << mSock << ":" << this << endl;

         	/*OmnTraceComm << "Socket created: " 
		 			<< mSock << ". Trying to connect!" << endl;*/

		 	bool bindstatus = false;
	 		struct sockaddr_in sockAddr;
	 		if (mLocalStartPort == 0)
	 		{
	 			// 
	 			// No need to loop. 
	 			//
	 			memset((char*)&sockAddr, 0, sizeof(sockAddr));

	 			sockAddr.sin_family = AF_INET;
	 			OmnSetIpAddr(&sockAddr, mLocalIpAddr);
	 			sockAddr.sin_port = 0;

	 			/*OmnTraceComm << "To bind TCP Client to: " << mSock
	 				<< ", " << mLocalIpAddr.toString() << ":" 
	 				<< 0 << endl;*/

	 			if (::bind(mSock, 
	 					(struct sockaddr*)&sockAddr, sizeof(sockAddr)) != 0)
	 			{
					// Chen Ding, 06/05/2012
	 				// err << "TCP failed to bind to: " 
	 				// 	<< mLocalIpAddr.toString() << ". Error: " 
	 				// 	<< OmnGetStrError(OmnErrType::eBind) << ". Check host name!!!";
 					// return OmnAlarm << err << enderr;
					return false;
 				}
	
 				//OmnTraceComm << "Bind successful" << endl;

 				//
 				// Retrieve the port
 				//
 				struct sockaddr_in sockAdd;
 				memset((char*) &sockAdd, 0, sizeof (sockAdd));
 				OmnSockLen addrLen = sizeof(sockAdd);
 				if (getsockname(mSock, (struct sockaddr*)&sockAdd, &addrLen) == 0)
 				{
 					mLocalCrtPort = ntohs(sockAdd.sin_port);
 				}
 				else
 				{
					// Chen Ding, 06/05/2012
					// Do not report error.
					// int err_no = errno;
					// OmnCommUtil::reportGetSockNameError(err_no, mSock);
 					// OmnWarn << "Failed to retrieve port" << enderr;
 				}
 			}
 			else
 			{
 				// 
 				// Need to bind to a local port. Will loop
 				//
 				for (int i=0; i<mLocalNumPorts; i++)
 				{
 					//
 					// Bind the sock to the specified local address and port
 					//
 					memset((char*)&sockAddr, 0, sizeof(sockAddr));
 	
 					sockAddr.sin_family = AF_INET;
 					OmnSetIpAddr(&sockAddr, mLocalIpAddr);
 					sockAddr.sin_port = htons(mLocalStartPort);
 	
 					OmnTraceComm << "To bind TCP Client to: " << mSock
 						<< ", " << mLocalIpAddr.toString() << ":" 
 						<< mLocalStartPort << endl;
 	
 					if (::bind(mSock, 
 						(struct sockaddr*)&sockAddr, sizeof(sockAddr)) != 0)
 					{
 						mLocalCrtPort++;
 						if (mLocalCrtPort >= mLocalStartPort + mLocalNumPorts)
 						{
 							mLocalCrtPort = mLocalStartPort;
 						}
 						continue;
 					}
 		
 					OmnTraceComm << "Bind successful" << endl;
 					bindstatus =true;
 					break;
 				}
 	
 				if (!bindstatus)
 				{
 					err << "TCP failed to bind to: " 
 						<< mLocalIpAddr.toString() << ":"
 						<< mLocalStartPort << ". Error: " 
 						<< OmnGetStrError(OmnErrType::eBind) << ". ";
 	
 					return OmnAlarm << err << enderr;
 				}
 			}
 		
 			//
 			// Bind successful. Retrieve the port used.
 			//
 			OmnSockLen addrLen = sizeof(sockAddr);
 			if (getsockname(mSock, (struct sockaddr*)&sockAddr, &addrLen) == 0)
 			{
 				mPortUsed = ntohs(sockAddr.sin_port);
 				//OmnTraceComm << "On port = " << mPortUsed << endl;
 			}
 			else
 			{
				// Chen Ding, 06/05/2012
				// DO not wait alarm.
 				// OmnWarn << OmnErrId::eWarnCommError
 				// 	<< "Failed to getsockname!" 
				// 	<< ". Remote: " << mRemoteIpAddr.toString()
				// 	<< ":" << mRemoteCrtPort << enderr;
 			}
 			// 
 			// To connect
 			//
   			struct sockaddr_in servAddr;
   			memset((char *)&servAddr, 0, sizeof(servAddr));
	
   			servAddr.sin_family = AF_INET;
   			OmnSetIpAddr(&servAddr, mRemoteIpAddr);
   			servAddr.sin_port = htons(mRemoteCrtPort);

   			//OmnTraceComm << "To connect to " 
  			//	<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort << endl;

			if (::connect(mSock, (struct sockaddr *)&servAddr, 
					sizeof(servAddr)) == 0)
				bConnected = true;
 		}
		else if (mSockType == eTypeUnix)
		{
      		//
       		// Create the sock
       		//
       	 	// if ((mSock = ::socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
       	 	if ((mSock = OmnCreateUnixSocket(__FILE__, __LINE__)) < 0)
        	{
				err = "Failed to create sock to: ";
				err << mUnixPath << ":" << ". Error No.: " << OmnGetErrno();

           		return OmnAlarm << err << enderr;
       		}

       		OmnTraceComm << "Socket created: " 
	 				<< mSock << ". Trying to connect!" << endl;

   			struct sockaddr_un servAddr;
   			memset((char *) &servAddr, 0, sizeof(servAddr));
	
   			servAddr.sun_family = AF_UNIX;
   			strcpy(servAddr.sun_path, mUnixPath.getBuffer());

   			OmnTraceComm << "To connect to " 
  				<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort << endl;

			if (::connect(mSock, (struct sockaddr *)&servAddr, 
					sizeof(servAddr)) == 0)
				bConnected = true;
		}
	}
 	
	//
	// Socket created successfully. Next need to connect
	//
	if (!bConnected)
	{
		// err << "Failed to connect to: " 
		// 	<< mRemoteIpAddr.toString() << ":" 
		// 	<< mRemoteCrtPort 
		// 	<< ". Error: " 
		// 	<< OmnGetStrError(OmnErrType::eConnect) << ". ";
		// OmnTrace << mName << "Failed on: " 
		// 	<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort << endl;
		//OmnScreen << "To close: " << this << ":" << mSock << endl;
		OmnCloseSocket(__FILE__, __LINE__, mSock);
		mSock = -1;
		return false;
	}

	/*OmnTraceComm << "Successfully connected to " 
		<< ". Socket = " << mSock 
		<< " to "
		<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort
		<< endl;*/

	struct linger ling;
   	ling.l_onoff = 0;
   	ling.l_linger = 0;

	//int set = 2;
	//setsockopt(mSock, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));
	
	// Ignore Broken Pipe
	signal(SIGPIPE, SIG_IGN);

	setsockopt(mSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
   	setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	// Below are some socket level options:
	// 1. When closing a sock, it goes through TIME_WAIT process. Below 
	//    forces a sock to close without going through TIME_WAIT.
	// bool bDontLinger = false; 
	// setsockopt(s，SOL_SOCKET，SO_DONTLINGER，(const char*)&bDontLinger，sizeof(bool));
	//
	// 2. Sending Timer: when sending data, if the socket is blocking, the sending 
	//    may be blocked. The following statements set the sending timer. 
	// int nNetTimeout=1000; //1 second
	// setsockopt(socket，SOL_S0CKET，SO_SNDTIMEO，(char *)&nNetTimeout，sizeof(int));
	//
	// 3. Receiving Timer: when reading a socket, it will wait until there are something.
	//    The statements below set the reading timer:
	// int nNetTimeout = 1000; // 1 second
	// setsockopt(socket，SOL_S0CKET，SO_RCVTIMEO，(char *)&amp;nNetTimeout，sizeof(int));

	return true;
}     


OmnRslt
OmnTcpClient::closeConn()
{
    if (mSock !=-1)
    {
     	//OmnTraceComm << "Closing sock: " << mSock << endl;
		//OmnScreen << "Close sock: " << this << ":" << mSock << endl;
     	OmnCloseSocket(__FILE__, __LINE__, mSock);
		mSock = -1;
		return true;
    }
  
	return true;
}


OmnString
OmnTcpClient::toString() const
{
	OmnString str;
	str << "TCP Client connection. Socket = " << OmnStrUtil::itoa(mSock)
		<< " Remote Ip Address = " << mRemoteIpAddr.toString()
		<< " Remote Crt Port = "	<< mRemoteCrtPort
		<< " Remote Start Port = "	<< mRemoteStartPort
		<< " NumPorts = "	<< mRemoteNumPorts;
	return str;
}


bool
OmnTcpClient::readFrom(OmnConnBuffPtr &buff, 
					   const int sec,
					   bool &timeout,
					   bool &connBroken,
					   bool appendBuff)
{
	// 
	// This is a timed reading. If it has something to read, it reads it
	// and return immediately. If there is nothing to read, it will wait
	// 'sec' number of seconds (minimum 1 second). If there is still noting
	// to read when timing out, it returns true and 'timeout' is set to true. 
	//

	if (!appendBuff)
	{
		buff = 0;
	}

	connBroken = false;
	if (mSock < 0 || mSock >= OmnCommTypes::eMaxSockToSelect)
	{
		OmnAlarm << "To read a TCP sock but the sock is not valid: "
			<< mSock
			<< ". " << toString() << enderr;
		return false;
	}

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mSock, &fd);
	
	int theSec = (sec>=0)?sec:1;

	timeval timer;
	timer.tv_sec = theSec;
	timer.tv_usec = 0;

	int rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, &timer);
	if (rslt < 0)
	{
		OmnAlarm << OmnErrId::eSocketSelectError
			<< "Selecting a TCP sock error: " << mSock
			<< ". " << toString() << enderr;
		return false;
	}

	if (rslt == 0)
	{
		// 
		// This is a timeout event.
		//
		timeout = true;
		return true;
	}

	timeout = false;
	//Modify 10/27/2010
	bool isEOF = false;
	bool rr = readFrom1(buff, connBroken, isEOF, appendBuff);
	if (isEOF) timeout = true;
	return rr;
	//end
}



bool			
OmnTcpClient::readMsg(OmnConnBuffPtr &buff, const int sec, 
						bool &timeout, bool &connBroken, const bool)
{
	int secLeft = sec;
	int startTime = OmnTime::getCrtSec();
	if(mMsgReader.isNull())
	{
		return false;
	}
	
	while(timeout)
	{
		// 1 . If there is some data remain, check whether it contains 
		// a whole message
		if(mConnBuff->getDataLength() > 0)
		{
			OmnTcpClientPtr thisPtr(this, false);
			int msgLen = mMsgReader->nextMsg(mConnBuff,thisPtr);
			if(msgLen> 0)
			{
				try
				{
					buff = OmnNew OmnConnBuff(mConnBuff->getBuffer(),msgLen,false);
				}
				catch(...)
				{
					closeConn();
					return false;
				}
				mConnBuff->removeHeader(msgLen);
				return true;
			}
		}
		
		if(!readFrom(mConnBuff,secLeft,timeout,connBroken,true))
		{
			return false;
		}

		secLeft =  sec - (OmnTime::getCrtSec() - startTime);
	}

	return false;
}

bool
OmnTcpClient::truncateBuff(const int len)
{
	return mConnBuff->removeHeader(len);
}


void			
OmnTcpClient::setMsgReader(const OmnTcpMsgReaderPtr	msgReader)
{
	mMsgReader = msgReader;
}

bool
OmnTcpClient::readFrom1(OmnConnBuffPtr &buff, 
						bool &connBroken,
						bool &isEndOfFile,
						bool appendBuff)
{
	OmnNotImplementedYet;
	return true;
	/*
	// This function will read the sock as much as there is up to the size
	// of the buffer. 
	isEndOfFile = false;
	if(!appendBuff)
	{
		// Need to throw 'buff' away and create a new one
		buff = OmnNew OmnConnBuff();
	}
	else
	{
		if (!buff) buff = mConnBuff;
	}
	
	int length = buff->getBufferLength();
	int dataLength = buff->getDataLength();
	int remaining_len = length - dataLength;
	if (remaining_len <= 0) 
	{
		buff->expandMemory();
		length = buff->getBufferLength();
	}

	char *data_buff = buff->getBuffer();
	// int bytesRead = OmnReadSock(mSock, buff->getBuffer() + dataLength, 
	// 		length - dataLength);
	int bytesRead = OmnReadSock(mSock, &data_buff[dataLength], length - dataLength);

//	OmnTrace << "Read: " << bytesRead << ":" << mSock
//		<< " Local: " << mLocalIpAddr.toString() << ":" << mLocalCrtPort
//		<< " Remote: " << mRemoteIpAddr.toString() 
//		<< ":" << mRemoteCrtPort << endl;

	if (bytesRead > 0)
	{
		buff->setDataLength(dataLength + bytesRead);
		buff->setAddr(mLocalIpAddr, mLocalCrtPort, mRemoteIpAddr, 
				mRemoteCrtPort);
		return true;
	}
	else if (bytesRead == 0)
	{
		isEndOfFile = true;
		return true;
	}
	else
	{
		if (!appendBuff)
		{
			buff = 0;
		}
		return false;
	}

	return true;
	*/
}


/*
OmnRslt
OmnTcpClient::readFromSock(char *data, int &bytesToRead)
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
   	 	int bytesRead = OmnReadSock(mSock, &data[index], bytesRemaining);
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
*/


/*
OmnRslt
OmnTcpClient::sendMsgTo(const OmnMsgPtr &msg)
{
	OmnSerialTo s;
	if (!msg->serializeTo(s))
	{
		return OmnAlarm << "Failed to serialize the message" << enderr;
	}

	OmnTraceComm << "Send a msg to: "
		<< mRemoteIpAddr.toString() << ":" << mRemoteCrtPort
		<< ". Content: \n" << s.getData() << endl;

	return writeTo((const char *)s.getData(), s.getDataLength());
}
*/


OmnConnBuffPtr
OmnTcpClient::getDataBuff()
{
	return mConnBuff;
}


// 2013/01/09, Ice
int
OmnTcpClient::smartRead(OmnConnBuffPtr &buff)
{
	// This function reads in one message. The message may be incomplete
	// in the connection, or it may have multiple messages.
	if (mIsBlocking)
	{
		int bytes_read = OmnTcp::smartRead(buff);
		mConnBuff = buff;
		return bytes_read;
	}
	
	mLock->lock();

	if (mSock <= 0) 
	{
		mLock->unlock();
		return -1;
	}
	int rslt = 1;
	if (mLengthIndicatorPos != 4)
	{
		rslt = readLengthIndicator();
	}

	if (rslt == -1)
	{
		mLock->unlock();
		return -1;
	}

	if (rslt == 0)
	{
		mLock->unlock();
		return 0;
	}

	rslt = readBody();

	mLock->unlock();
	return rslt;
}


int 
OmnTcpClient::readLengthIndicator()
{
	int bytesread  = OmnReadSock(mSock, &mLength[mLengthIndicatorPos], 4-mLengthIndicatorPos);
	if (bytesread == -1 && errno == EAGAIN)
	{
		if (errno == EAGAIN)
			return 0;
		else
			return -1;
	}
	if (bytesread == 0)
	{
		return -1;
	}
	mLengthIndicatorPos += bytesread;
	if (mLengthIndicatorPos == 4)
	{
		try
		{
			int length = getDataLength();
			aos_assert_r(length >=0, -1);
			mConnBuff = OmnNew OmnConnBuff((u32)length);
		}
		catch(...)
		{
			mConnBuff = 0;
			return -1;
		}

		return 1;
	}
	else
		return 0;
}


int 
OmnTcpClient::readBody()
{
	int bytesleft = mConnBuff->getBufferLength() - mConnBuff->getDataLength();
	char *data = mConnBuff->getBuffer() + mConnBuff->getDataLength();
	int bytesread  = OmnReadSock(mSock, data, bytesleft);
	if (bytesread == -1 && errno == EAGAIN)
	{
		if (errno == EAGAIN)
			return 0;
		else
			return -1;
	}
	if (bytesread == 0)
	{
		return -1;
	}
	mConnBuff->setDataLength(bytesread + mConnBuff->getDataLength());
	return bytesread;
}


int 
OmnTcpClient::getDataLength()
{
	switch (mLengthType)
	{
		case eAosTLT_FirstFourHigh:
			 return ((mLength[0]) << 24) + 
				((mLength[1] << 16)) + 
				((mLength[2] << 8)) + mLength[3];
		default:
			OmnAlarm << "Invalid length type: " << mLengthType << enderr;
			return -1;
	}
	return -1;
}


bool
OmnTcpClient::clearBuffedData()
{
	mLock->lock();
	mConnBuff = 0;
	mLengthIndicatorPos = 0;
	mLock->unlock();
	return true;
}


/*
bool
OmnTcpClient::smartSend(const char *data, const int len)
{
	// This function will send the given number of bytes. It may need to 
	// send length indicator as needed. 
	char bytes[5];
	switch (mLengthType)
	{
	case eAosTLT_FirstFourHigh:
		 bytes[0] = (len >> 24);
		 bytes[1] = (len >> 16);
		 bytes[2] = (len >> 8);
		 bytes[3] = len;
		 if (!writeToSock(bytes, 4)) return false;
		 if (len > 0) return writeToSock(data, len);
		 return true;

	case eAosTLT_FirstFourLow:
		 bytes[3] = (len >> 24);
		 bytes[2] = (len >> 16);
		 bytes[1] = (len >> 8);
		 bytes[0] = len;
		 if (!writeToSock(bytes, 4)) return false;
		 if (len > 0) return writeToSock(data, len);
		 return true;

	case eAosTLT_FirstWordHigh:
	case eAosTLT_FirstWordLow:
		 // Chen Ding, 08/18/2010
		 {
			 int bytesLeft = len;
			 int start = 0;
			 while (bytesLeft > 0)
			 {
				 if (bytesLeft < eTwoByteMaxLen)
				 {
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
		 			return writeToSock(bytes, 2) && writeToSock(&data[start], bytesLeft);
				 }

				 if (bytesLeft == eTwoByteMaxLen)
				 {
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
		 			bool rslt = writeToSock(bytes, 2);
					rslt &= writeToSock(&data[start], bytesLeft);

					bytes[0] = 0;
					bytes[1] = 0;
		 			rslt &= writeToSock(bytes, 2);
					return rslt;
				 }

				 // Too big to be sent in one package
				 int ln = eTwoByteMaxLen;
				 if (mLengthType == eAosTLT_FirstWordHigh)
				 {
		 		 	bytes[0] = (ln >> 8);
		 		 	bytes[1] = ln;
				 }
				 else
				 {
		 		 	bytes[1] = (ln >> 8);
		 		 	bytes[0] = ln;
				 }
		 		 bool rslt = writeToSock(bytes, 2);
				 rslt &= writeToSock(&data[start], ln);
				 start += eTwoByteMaxLen;
				 bytesLeft -= eTwoByteMaxLen;
			 }
			 OmnShouldNeverComeHere;
			 return false;
		 }
		 break;

	case eAosTLT_LengthIndicator:
	case eAosTLT_TermByNewLine:
		 OmnNotImplementedYet;
		 return -1;

	case eAosTLT_NoLengthIndicator:
		 return writeToSock(data, len);

	default:
		 OmnAlarm << "Invalid length type: " << mLengthType << enderr;
		 return -1;
	}

	return -1;
}
*/


