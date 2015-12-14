////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCltGrp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpCltGrp.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Select.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/Udp.h"


OmnTcpCltGrp::OmnTcpCltGrp()
:
mFdscnt(0), 
mLock(OmnNew OmnMutex()), 
mNumConns(0), 
mMaxConns(eDefaultMaxConns),
mIsRight(true)
{
	//
	// Set the bit string used by select().
	//
    FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);

	OmnString errmsg;
    mReader = OmnNew OmnUdp("TimerReaderUdp", OmnIpAddr("127.0.0.1"), 0);
    mWriter = OmnNew OmnUdp("TimerWriterUdp", OmnIpAddr("127.0.0.1"), 0);
	if (!mReader->connect(errmsg) || !mWriter->connect(errmsg))
	{
		mIsRight = false;
		OmnAlarm << "Failed to create pipe" << enderr;
	}
	else
	{
		FD_SET(mReader->getSock(), &mReadFds);
		mFdscnt = mReader->getSock() + 1;
	}
}


bool
OmnTcpCltGrp::resetFds()
{
	mLock->lock();
	FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);
	mClients.reset();
	mFdscnt = 0;
    while (mClients.hasMore())
    {
        OmnTcpClientPtr client = mClients.crtValue();
		int theSock = client->getSock();
		if (theSock <= 0 || theSock > eMaxSock)
		{
			// The connection is bad. Need to remove it. 
			mClients.eraseCrt();
			mNumConns--;
			if (mServer)
			{
				OmnTcpCltGrpPtr thisPtr(this, false);
				mServer->connClosed(thisPtr, client); 
			}
		}
		else
		{
			mClients.next();
        	FD_SET(theSock, &mReadFds); 
        	if (mFdscnt <= theSock)
        	{
            	mFdscnt = theSock + 1 ;
        	}
		}
	}
	mLock->unlock();
	return true;
}


OmnTcpCltGrp::~OmnTcpCltGrp()
{
    FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);

	closeConn();
	mNumConns = 0;
    mClients.clear();
}


bool
OmnTcpCltGrp::closeConn(const OmnTcpClientPtr &client)
{
	// To close a connection identified by 'client'. 
	// This function should be called when errors are found in the connection
	// or we need to remove the connection.
	removeConn(client);
	client->closeConn();
    return true;
}



OmnTcpClientPtr
OmnTcpCltGrp::getConn(const int sock)
{
	//
	// The server keeps a list of all active connections in 
	// mClients. This function will look up the list 
	// If it is found, its OmnTcpClient pointer is returned.
	// Otherwise, null is returned;
	//
	mLock->lock();
	mClients.reset();
    while (mClients.hasMore())
    {
        if (mClients.crtValue()->getSock() == sock)
        {
			OmnTcpClientPtr client = mClients.crtValue();
			mLock->unlock();
            return client;
        }

		mClients.next();
    }

	//
	// Not found
	//
	mLock->unlock();
    return 0;
}


bool
OmnTcpCltGrp::getConnEvent(int &sock, OmnTcpClientPtr &client)
{
	//
	// It assumes that events have been detected and recorded on 
	// mWorkingFds. This function searches the entries in mClients
	// to see which ones have events. The first one that has
	// the event is retrieved. The corresponding flags in mWorkingFds
	// are cleared so that the next time this function is called,
	// it will not be found again. Eventually, all responsible
	// connections will be processed. If false is returned, it means
	// that no one is responsible for the events recorded in _tmpReadfs.
	//

	mLock->lock();
	mClients.reset();
	int ret = 0;
    while (mClients.hasMore())
    {
        OmnTcpClientPtr c = mClients.crtValue();
		
		//
		// Check whether the connection is responsible for the events.
		//
		
		ret = c->checkEvent(&mWorkingFds);
        if(ret == 0)
		{
            client = c;
            sock = mClients.crtValue()->getSock();
			mLock->unlock();
            return true;
        }
		else if(ret == -1)
		{
			// the connection is broken , remove the sock
			mClients.eraseCrt();
			continue;
		}

		mClients.next();
    }

	mLock->unlock();
    return false;
}


bool
OmnTcpCltGrp::addConn(const OmnTcpClientPtr &client)
{
	//
	// When new connection is detected and accepted successfully,
	// an instance of OmnTcpClient is created. This function
	// is called to add the connection to mClients, to set
	// mReadFds, mFdscnt. It will also check whether the connection
	// is already there. If it is, it is an error. 
	//

	int theSock = client->getSock();

	if (mNumConns >= mMaxConns)
	{
		// 
		// Too many connections. Do not add
		//
		return false;
	}

	//
	// Find the index of the element in mClients that 
	// corresponds to theSock.
	//
    OmnTcpClientPtr c = getClient(theSock);

    if (c.isNull())
    {
		//
		// This means that the connection doesn't exist.
		// Set the flag, add the connection to the connection list
		// and return.
		//

		mLock->lock();

        FD_SET(theSock, &mReadFds); 
		mClients.append(client);
		mNumConns++;

        if (mFdscnt <= theSock)
        {
            mFdscnt = theSock + 1 ;
        }

		int bytes = mWriter->writeTo("hb", 2, 
			OmnIpAddr("127.0.0.1"), mReader->getLocalPort());
		if (bytes <= 0)
		{
			OmnAlarm << "Failed to write" << enderr;
		}

		mLock->unlock();
        // OmnTrace << "AddConnection: " << theSock
        //        << " successful!" 
		// 	   << ". Total connections: " 
		// 	   << mClients.entries() << endl;

// OmnString errmsg;
// if (!consistencyCheck(errmsg))
// {
// 	OmnAlarm << "Consistency check failed: " << errmsg << enderr;
// 	exit(0);
// }

        return true;
    }

	OmnWarn << OmnErrId::eWarnCommError
		<< "Connection already exist!" << enderr;

// OmnString errmsg;
// if (!consistencyCheck(errmsg))
// {
// 	OmnAlarm << "Consistency check failed: " << errmsg << enderr;
// 	exit(0);
// }

    return false;
}


bool
OmnTcpCltGrp::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter TcpCltGrp::threadFunc." << endl;

	OmnTcpClientPtr client;
	OmnConnBuffPtr buff = 0;
    while ( state == OmnThrdStatus::eActive )
    {
		client = 0;
		checkReading(client);

		if (client.isNull()) continue;

		// Found a valid client that has something to read
		buff = 0;

		int nread = client->smartRead(buff);
	    if (nread < 0)
		{
			// 
			// Either the remote has closed the connection or something
			// wrong with the connection. We will close the connection.
			//
			OmnAlarm << "Reading TCP client error: " 
				<< client->toString() 
				<< ". Connection closed!" << enderr;
			closeConn(client);
		}
		else if (nread > 0)
		{
			//
			// Successfully read something. 
			//
			if (!mListener.isNull())
			{
				OmnTcpCltGrpPtr thisPtr(this, false);
				mListener->msgRecved(thisPtr, buff, client);
			}
			else
			{
				OmnAlarm << "Read a message but no message processor" << enderr;
			}
		}
		else
		{
			// Remote side closed the connection gracefully.
			closeConn(client);
		}
	}

	return true;
}


bool
OmnTcpCltGrp::checkReading(OmnTcpClientPtr &theTcp)
{
    //
    // This member function determines whether there is any event on
    // any connection this class manages, or whether there is any
    // new connection request.
    //
    // If there is at least one event, it finds which connId and TCP
    // the event occurs and returns true.
    //
    // Whether there is event or not is indicated by "mWorkingFds", which
    // was set by the most recent call of "select(...)".
	//
    //
    // If there is no event, it goes to a while(1) loop, which first
    // reset "mWorkingFds" to "mReadFds", and then call the "select(...)".
    // Two things can cause the call return. One is there is at least
    // one event on one of the connections and the other is that there
    // is a new connection request.
    //
    // Once returned from "select(...)", it checks new connection first.
    // If there is new connection, it creates a new instance of TCP, and
    // adds the pair (sock, TCP) to mClients.
    //
    // It then checks whether there is any new events. If no, it goes back
    // to the beginning of the loop. Otherwise, it returns the connId and
    // TCP.
    //

	//
	// It checks whether there is any events on mWorkingFds. This may
	// be the events it received the last time and haven't been processed
	// yet. If it finds, it gets the connId and conn, clear the flag,
	// and then return.
	//
	int theSock;
    if (getConnEvent(theSock, theTcp))
    {
		// 
		// Events are found. So we need to clear the flag and return.
		//
        FD_CLR((size_t)theSock, &mWorkingFds);
        return true;
    }

	int clientsNum;
    while (1)
    {
		//
		// In this loop, it first go 'select'. When it is waken up,
		// it checks whether the select was successful. If failed,
		// it can be something intermident (EINTR or EINVAL). Or
		// it is possible that the server connection is not okay.
		//
        mWorkingFds = mReadFds;

		timeval tval;
		tval.tv_sec = 1;
		tval.tv_usec = 0;
		int ret = OmnSocketSelect(mFdscnt, &mWorkingFds, 0, 0, &tval);
		if(ret == 0)
		{
			// timeout 
			theTcp =0;
			return true;
		}
		
		if (ret < 0)
        {
			mLock->lock();
			clientsNum = mClients.entries();
			mLock->unlock();
			if(clientsNum <= 0)
			{
				return false;
			}
		
			// OmnAlarm << OmnErrId::eWarnCommError
			// 	<< "Failed to select" << enderr;

            switch (OmnGetErrno())
            {
            case EINTR:
            case EINVAL:
                 continue;

            case EBADF:
				 // OmnAlarm << "Bad connections found!" << enderr;
                 resetFds();
                 continue;

            default:
				 //
			 	 // It failed the selection. This should never happen.
				 //
				 OmnAlarm << "Something is wrong in select: " 
					<< OmnGetErrno() << enderr;
                 return false; 
            }

        }

		//
        // This means the select is successful.
		//
		if (FD_ISSET(mReader->getSock(), &mWorkingFds))
		{
			char data[100];
			int length = 100;

			FD_CLR(mReader->getSock(), &mWorkingFds);
			OmnReadSock(mReader->getSock(), data, length);
			continue;
		}

        if (getConnEvent(theSock, theTcp))
        {
			//
			// It means that there are reading requests.
			// Clear the bit so that the next time, we do not get the same
			// event again.
			//
            FD_CLR((size_t)theSock, &mWorkingFds);
            break;
        }
    }

	//
	// If it ever gets to this point, it means a reading request has 
	// be detected.
	//
    return true;
}


OmnTcpClientPtr
OmnTcpCltGrp::getClient(const int theSock) 
{
	mLock->lock();
    mClients.reset();
	while (mClients.hasMore())
	{
		if ((mClients.crtValue())->getSock() == theSock)
        {
			OmnTcpClientPtr client = mClients.crtValue();
			mLock->unlock();
			return client;
        }

		mClients.next();
    }

	mLock->unlock();
    return 0;
}


int
OmnTcpCltGrp::checkConns()
{
    //
    // This function is called when a bad file descriptor is found.
    // All connections are saved in mClients. This function simply
    // go over all the connections in mClients. If a connection is
    // bad, it removes it.
	//
	// It returns the number of connections being removed.
    //

	mLock->lock();
	mClients.reset();
	int numRemoved = 0;
    while (mClients.hasMore())
    {
        if (!(mClients.crtValue())->isConnGood())
        {
            OmnScreen << "Bad connection found: "
				<< (mClients.crtValue())->getSock() << endl;

            FD_CLR((size_t)(mClients.crtValue())->getSock(), &mReadFds);
            mClients.eraseCrt();
			numRemoved++;
			mNumConns--;
        }
		else
		{
			mClients.next();
		}
    }

	mLock->unlock();
	return numRemoved;
/*
OmnString errmsg;
if (!consistencyCheck(errmsg))
{
	OmnAlarm << "Consistency check failed: " << errmsg << enderr;
	exit(0);
}
*/
}


bool
OmnTcpCltGrp::closeConn()
{
	//
	// Need to close all member connections
	//
	mLock->lock();
    mClients.reset();
    while (mClients.hasMore())
    {
        (mClients.crtValue())->closeConn();

		mClients.next();
    }

	mLock->unlock();
	return true;
}


/*
OmnRslt     
OmnTcpCltGrp::writeTo(const char *data, const int length, const int sock)
{
	OmnTcpClientPtr client = getConn(sock);
	if (client.isNull())
	{
		return OmnAlarm << "Failed to write: connection is broken!" << enderr;
	}
	
	return client->writeTo(data, length);
}
*/


bool
OmnTcpCltGrp::removeConn(const OmnTcpClientPtr &client)
{
    // It removes the connection 'client' from mClients. It then calls
	// mServer->connClosed(...). 
	int sock = client->getSock();
	mLock->lock();
    mClients.reset();
    while (mClients.hasMore())
    {
        if ((mClients.crtValue())->getSock() == sock)
		{
			if (sock != -1)
				FD_CLR(sock, &mReadFds);
			mClients.eraseCrt();
			mNumConns--;
			mLock->unlock();

			OmnTcpCltGrpPtr thisPtr(this, false);
			if (mServer)
			{
				mServer->connClosed(thisPtr, client); 
			}
			return true;
		}

		mClients.next();
	}

	mLock->unlock();
	OmnWarn << "To remove connection: " << sock 
		<< " but not found!" << enderr;
    return false;
}


void
OmnTcpCltGrp::setListener(const OmnTcpCltGrpListenerPtr &listener)
{
	if (!mListener.isNull())
	{
		OmnAlarm << "To set TCP Server msg processor, but there is already one!" 
			<< enderr;
	}

	mListener = listener;
}


void
OmnTcpCltGrp::startReading()
{
	if (mListener.isNull())
	{
		OmnAlarm << "To start reading a TCP Server but no message processor" 
			<< enderr;
		return;
	}

	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "TcpCltGrp", 0, false, true, __FILE__, __LINE__);
	}

	mThread->start();
}	


void
OmnTcpCltGrp::stopReading()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return;
	}

	mThread->stop();
}	

void
OmnTcpCltGrp::closeReading()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return;
	}

	mThread->exitThread();

	//need to signal reading thread
	if(mWriter)
	{
		mWriter->writeTo("hb", 2, 
			OmnIpAddr("127.0.0.1"), mReader->getLocalPort());
	}
	mReader->closeConn();
    mWriter->closeConn();
}	

bool 
OmnTcpCltGrp::signal(const int threadLogicId)
{
	return true;
}


void
OmnTcpCltGrp::heartbeat()
{
	//
	// Currently not managed by ThreadMgr. Do nothing.
	//
}


//
// E007, 07/13/2003, 2003-0073
//
bool
OmnTcpCltGrp::checkThread(OmnString &) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
    return true;
}


bool
OmnTcpCltGrp::isCriticalThread() const
{
    return false;
}


OmnRslt		
OmnTcpCltGrp::readFrom(OmnConnBuffPtr &buffer, OmnTcpClientPtr &client)
{
	bool connClosed;
	bool isEndOfFile;
	while (1)
	{
    	checkReading(client);

		//
		// So found a connection which has something to be read. 
		// Go read it.
		//
		if (!client->readFrom1(buffer, connClosed, isEndOfFile))
    	{
			// 
			// The connection is broken. This is not necessarily
			// an error. Simply close it and go back to read again
			//
			if (connClosed)
			{
				OmnTrace << "Remote closed the connection" << endl;
			}
			else
			{
				OmnAlarm << "Read failed! Connection " 
					<< " is removed." << enderr;
			}
			closeConn(client);
		}
		else
		{
			if (isEndOfFile)
			{
				closeConn(client);
			}

			//
			// Otherwise, read successful
			//
			return true;
		}
	}

    return true;
}


bool 
OmnTcpCltGrp::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
OmnTcpCltGrp::consistencyCheck(OmnString &errmsg)
{
	mLock->lock();
	mClients.reset();
	fd_set fds = mReadFds;
    FD_CLR(mReader->getSock(), &fds);
	bool rslt = true;
	int numClients = 0;

	while (mClients.hasMore())
	{
		OmnTcpClientPtr client = mClients.next();
		int sock = client->getSock();
		numClients++;

		if (!FD_ISSET(sock, &fds))
		{
			errmsg << "Sock: " << sock << " not set. ";
			rslt = false;
		}
		else
		{
			FD_CLR(sock, &fds);
		}
	}

	for (int i=0; i<1000; i++)
	{
		if (FD_ISSET(i, &fds))
		{
			errmsg << "Sock: " << i << " unexpected. ";
			rslt = false;
		}
	}

	if (mNumConns != numClients)
	{
		errmsg << "mNumClients not match: " << numClients << ":" << mNumConns;
		rslt = false;
	}

	mLock->unlock();
	return rslt;
}


void 
OmnTcpCltGrp::setServer(const OmnTcpServerGrpPtr &p) 
{
	mServer = p;
}

bool		
OmnTcpCltGrp::isStopped()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return true;
	}

	mThread->isStopped();
	return true;
}

bool		
OmnTcpCltGrp::isClosed()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return true;
	}

	bool ret = mThread->isExited();
	return ret;
}


