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
// 05/24/2007
////////////////////////////////////////////////////////////////////////////
#if 0
#include "UtilComm1/TcpClient.h"

#include "Porting/GetErrnoStr.h"
#include "Porting/Itoa.h"
#include "Porting/IPv6.h"
#include "Porting/Select.h"
#include "Porting/SockLen.h"
#include "Porting/BindErr.h"
#include "Thread/Mutex.h"

#include <sys/un.h>


int aos_tcp_client_connect(struct aos_tcp_client_t *conn)
{
	//
	// If the socket mSock is not 0, need to close it first
	//
	if (conn->mSock > 0)
	{
		OmnCloseSocket(conn->mSock);
		conn->mSock = -1;
	}

	bool createSock = true;
	if (conn->mSockType == eTypeUnix)
	{
		for (int i=0; i<conn->mRemoteNumPorts; i++)
		{
			if (!aos_tcp_client_try_connect(createSock))
			{
				conn->mRemoteCrtPort++;
				if (conn->mRemoteCrtPort >= 
						conn->mRemoteStartPort + conn->mRemoteNumPorts)
				{
			 		conn->mRemoteCrtPort = conn->mRemoteStartPort;
				}
			}
			else
			{
				//
				// Connection successful
				//
				return 0;
			}
	
			createSock = false;
		}
	}
	else if (mSockType == eTypeUnix)
	{
		if (aos_tcp_client_try_connect(createSock)) return 0;
	}


	//
	// Connection failed. The function has raised an alarm. We simply
	// return here.
	//
    OmnCloseSocket(conn->mSock);
	conn->mSock = -1;
	return -eAosRc_FailedConnect;
}


int aos_tcp_client_try_connect(const bool createSock, 
							   struct aos_tcp_client_t *conn)
{
	//
	// It establishes the connection. 
	// The caller should make sure that the connection is not there. 
	// This function will check no more. The caller should have locked
	// the connection.
	//
	bool bConnected = false;

	if (createSock)
	{
		aos_eng_log(eAosMD_UtilComm, "Create TCP Client to: %s:%d\n",
			aos_ipaddr_dump(conn->mRemoteAddr), conn->mRemoteCrtPort);
		
		if (conn->mSockType == eTypeTcp)
		{
        	//
        	// Create the socket
        	//
        	if ((conn->mSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        	{
				aos_alarm("Failed to create socket to: %s:%d. Error: %d\n",
					aos_ipaddr_dump(conn->mRemoteIpAddr, conn->mRemoteCrtPort, 
					OmnGetErrno()));

            	return -eAosRc_FailedCreateSock;
         	}

         	aos_eng_log(eAosMD_UtilComm, 
				"Socket created: %d. Trying to connect!\n",
				conn->mSock);

		 	bool bindstatus = false;
	 		struct sockaddr_in sockAddr;
	 		if (conn->mLocalStartPort == 0)
	 		{
	 			// 
	 			// No need to loop. 
	 			//
	 			memset((char*)&sockAddr, 0, sizeof(sockAddr));

	 			sockAddr.sin_family = AF_INET;
	 			OmnSetIpAddr(&sockAddr, conn->mLocalIpAddr);
	 			sockAddr.sin_port = 0;

	 			aos_eng_log("To bind TCP Client to: %d, %s\n", conn->mSock,
					aos_ipaddr_dump(conn->mLocalIpAddr));

	 			if (bind(conn->mSock, 
	 					(struct sockaddr*)&sockAddr, sizeof(sockAddr)) != 0)
	 			{
	 				// 
	 				// Failed to find
	 				//
					aos_alarm("TCP failed to bind to: %s. Error: %d\n",
	 					aos_ipaddr_dump(conn->mLocalIpAddr), 
	 					OmnGetStrError(OmnErrType::eBind));
 	
 					return -eAosRc_FailedToBind;
 				}
	
 				aos_eng_log("Bind successful\n");

 				//
 				// Retrieve the port
 				//
 				struct sockaddr_in sockAdd;
 				memset((char*) &sockAdd, 0, sizeof (sockAdd));
 				OmnSockLen addrLen = sizeof(sockAdd);
 				if (getsockname(conn->mSock, (struct sockaddr*)&sockAdd, &addrLen) == 0)
 				{
 					conn->mLocalCrtPort = ntohs(sockAdd.sin_port);
 				}
 	
 				aos_eng_log("Bound to local port: %d\n", conn->mLocalCrtPort);
 			}
 			else
 			{
 				// 
 				// Need to bind to a local port. Will loop
 				//
 				for (int i=0; i<mLocalNumPorts; i++)
 				{
 					//
 					// Bind the socket to the specified local address and port
 					//
 					memset((char*)&sockAddr, 0, sizeof(sockAddr));
 	
 					sockAddr.sin_family = AF_INET;
 					OmnSetIpAddr(&sockAddr, conn->mLocalIpAddr);
 					sockAddr.sin_port = htons(conn->mLocalStartPort);
 	
 					aos_eng_log("To bind TCP Client to: %d, %s:%d\n", 
						conn->mSock, aos_ipaddr_dump(conn->mLocalIpAddr),
 						conn->mLocalStartPort);
 	
 					if (bind(conn->mSock, 
 						(struct sockaddr*)&sockAddr, sizeof(sockAddr)) != 0)
 					{
 						conn->mLocalCrtPort++;
 						if (conn->mLocalCrtPort >= 
								conn->mLocalStartPort + conn->mLocalNumPorts)
 						{
 							conn->mLocalCrtPort = conn->mLocalStartPort;
 						}
 						continue;
 					}
 		
 					aos_eng_log("Bind successful\n");
 					bindstatus =true;
 					break;
 				}
 	
 				if (!bindstatus)
 				{
					aos_alarm("TCP failed to bind to: %s:%d. Error: \n",
 						aos_ipaddr_dump(conn->mLocalIpAddr), 
 						conn->mLocalStartPort, 
 						OmnGetStrError(OmnErrType::eBind));
 	
 					return -eAosRc_FailedToBind;
 				}
 			}
 		
 			//
 			// Bind successful. Retrieve the port used.
 			//
 			OmnSockLen addrLen = sizeof(sockAddr);
 			if (getsockname(conn->mSock, 
					(struct sockaddr*)&sockAddr, &addrLen) == 0)
 			{
 				conn->mPortUsed = ntohs(sockAddr.sin_port);
 				aos_eng_log("On port = %d\n", mPortUsed);
 			}

 			// 
 			// To connect
 			//
   			struct sockaddr_in servAddr;
   			memset((char *)&servAddr, 0, sizeof(servAddr));
	
   			servAddr.sin_family = AF_INET;
   			OmnSetIpAddr(&servAddr, conn->mRemoteIpAddr);
   			servAddr.sin_port = htons(conn->mRemoteCrtPort);

   			aos_eng_log("To connect to %s:%d\n", 
  				aos_ipaddr_dump(conn->mRemoteIpAddr), 
				conn->mRemoteCrtPort);

			if (connect(conn->mSock, (struct sockaddr *)&servAddr, 
					sizeof(servAddr)) == 0)
			bConnected = true;
 		}
		else if (conn->mSockType == eTypeUnix)
		{
      		//
       		// Create the socket
       		//
       	 	if ((conn->mSock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
        	{
				aos_alarm("Failed to create socket to: %s. Error: %d\n",
					conn->mUnixPath, OmnGetErrno());

           		return -eAosRc_FailedCreateSocket;
       		}

       		aos_eng_log("Socket created: %d. Trying to connect!\n", conn->mSock);

   			struct sockaddr_un servAddr;
   			memset((char *) &servAddr, 0, sizeof(servAddr));
	
   			servAddr.sun_family = AF_UNIX;
   			strcpy(servAddr.sun_path, mUnixPath.getBuffer());

   			aos_en_log("To connect to %s:%d\n",
  				aos_ipaddr_dump(conn->mRemoteIpAddr), 
				conn->mRemoteCrtPort);

			if (connect(conn->mSock, (struct sockaddr *)&servAddr, 
					sizeof(servAddr)) == 0)
				bConnected = true;
		}
	}
 	
	//
	// Socket created successfully. Next need to connect
	//
	if (!bConnected)
	{
		aos_alarm("Failed to connect to: %s:%d. Error: %d\n",
			aos_ipaddr_dump(conn->mRemoteIpAddr),
			conn->mRemoteCrtPort,
			OmnGetStrError(OmnErrType::eConnect));

		OmnCloseSocket(conn->mSock);
		return -eAosRc_FailedToConn;
	}

	aos_eng_log("Successfully connected %d to %s:%d\n",
		conn->mSock,
		aos_ipaddr_dump(conn->mRemoteIpAddr), 
		conn->mRemoteCrtPort);

	struct linger ling;
   	ling.l_onoff = 0;
   	ling.l_linger = 0;

	setsockopt(conn->mSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
   	setsockopt(conn->mSock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	return 0;
}     


int aos_tcp_client_close_conn()
{
    if (mSock !=-1)
    {
     	OmnTraceComm << "Closing socket: " << mSock << endl;
     	OmnCloseSocket(mSock);
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
		OmnAlarm << "To read a TCP socket but the socket is not valid: "
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
	return readFrom1(buff, connBroken,appendBuff);
}



bool			
OmnTcpClient::readMsg(OmnConnBuffPtr &buff, const int sec, 
						bool &timeout, bool &connBroken)
{
	int secLeft = sec;
	int startTime = OmnTime::getCrtSec();
	if(mMsgReader.isNull())
	{
		return false;
	}

	
	while(timeout)
	{
		// 1 . If there is some data remain, check whether it contains a whole message
		if(mConnBuff->getDataLength() > 0)
		{
			OmnTcpClientPtr thisPtr(this, false);
			int msgLen = mMsgReader->nextMsg(mConnBuff,thisPtr);
			if(msgLen> 0)
			{
				buff = OmnNew OmnConnBuff(mConnBuff->getBuffer(),msgLen);
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
OmnTcpClient::readFrom1(OmnConnBuffPtr &buff, bool &connBroken,bool appendBuff)
{
	// 
	// !!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!
	// This function will new a OmnConnBuff. If the caller passed
	// a buffer, it will be thrown away. 
	//
	// This function will read the sock as much as there is up to the size
	// of the buffer. 
	//
	if(!appendBuff)
	{
		buff = OmnNew OmnConnBuff();
	}
	else
	{
		if (!buff) 
		{
			buff = mConnBuff;
		}
	}
	
	int length = buff->getBufferLength();
	int dataLength = buff->getDataLength();

	int bytesRead = OmnReadSock(mSock, buff->getBuffer() + dataLength, length - dataLength);

	OmnTrace << "Read: " << bytesRead << ":" << mSock
		<< " Local: " << mLocalIpAddr.toString() << ":" << mLocalCrtPort
		<< " Remote: " << mRemoteIpAddr.toString() << ":" << mRemoteCrtPort << endl;
	if (bytesRead > 0)
	{
		buff->setDataLength(dataLength + bytesRead);
		buff->setAddr(mLocalIpAddr, mLocalCrtPort, mRemoteIpAddr, mRemoteCrtPort);
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
}


OmnRslt
OmnTcpClient::readFromSock(char *data, int &bytesToRead)
{
	//
	// This function should be called when one wants to read 
	// a known number of bytes from the connection of this
	// instance. In case the message size is too big and can't be
	// read in one time, it will keep on reading until all 
	// bytes are read.
	//
	// This function uses the operating system call to retrieve
	// data from the connection. It will read as much data as 
	// available, up to the size of the buffer. If the connection
	// has more than 'bytesToRead', the remaining data
	// is retained in the connection, which can be retrieved by
	// calling the function again.
	//
	// If no data available in the connection, it will block
	// unless the socket is non-blocking. 
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
				<< ". Local address: " << mLocalIpAddr.toString() << ":" << mLocalCrtPort
				<< ". Remote address: " << mRemoteIpAddr.toString() << ":" << mRemoteCrtPort 
				<< enderr;
		}

		if (bytesRead == 0)
		{
			// 
			// This is still an error. 
			//
			return OmnAlarm << OmnErrId::eSocketReadError
				<< "Read 0 bytes: " << OmnGetStrError(OmnErrType::eRead)
				<< ". Local address: " << mLocalIpAddr.toString() << ":" << mLocalCrtPort
				<< ". Remote address: " << mRemoteIpAddr.toString() << ":" << mRemoteCrtPort 
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

	return writeTo(s.getData(), s.getDataLength());
}

OmnConnBuffPtr
OmnTcpClient::getDataBuff()
{
	return mConnBuff;
}
#endif

