////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GenThread.cpp
// Description:
//   This class provides a system time. It keeps a tick. Approximately
//  every second, it updates its tick. 
//
//  Singleton Dependency:
//  	OmnAlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestDrivers/TcpTrafficGen/GenThread.h"

#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Tracer/TraceEntry.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Thread/ThrdStatus.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Util1/Ptrs.h"
#include "Util1/Time.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"

/*
main()
{
	OmnTimeDriver test[100];

	for (int i=0; i<100; i++)
	{
		test[i].start();
	}
}
*/

static OmnMutex successLock;
static int success = 0;

OmnGenThread::OmnGenThread()
{
}


OmnGenThread::~OmnGenThread()
{
}



bool
OmnGenThread::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mSecTickThread = OmnNew OmnThread(thisPtr, "TimeThread", 0, true, true);
	mSecTickThread->start();

	return true;
}	


bool
OmnGenThread::stop()
{
	mSecTickThread->stop();
	return true;
}



bool 
OmnGenThread::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnString contents;
	OmnTcpClientPtr tcpClients;
	int lenStr;
	u32 sendBlockSize = 10000;
	extern int remotePort, numPorts;
	extern OmnIpAddr remoteAddr;

    OmnFile file("gethttp", OmnFile::eReadOnly);

    if(!file.isGood()&&file.openFile(OmnFile::eReadOnly))
    {
        cout << "Failed to read the file:"
             << "gethttp" << endl;
    }
    if(!file.readAll(contents))
    {
        cout << "Failed to read the file:"
             << "gethttp" << endl;
    }
    lenStr = strlen(contents.data());
    if(!file.closeFile())
    {
        cout << "Failed to close the file:"
             << "gethttp" << endl;
    }
	const char *data = contents.data();
	while (state == OmnThrdStatus::eActive)
	{
		// 1. Create a sock
		// 2. Connect
		// 3. Send request
		// 4. Read response
		// 5. Check the response
		// 6. Send the file
		// 7. Close the connection
		tcpClients = OmnNew OmnTcpClient("tcp", remoteAddr, remotePort, numPorts, OmnTcp::eNoLengthIndicator);
        if (!tcpClients)
        {
           OmnAlarm << "Failed to create tcpClient" << enderr;
           return -1;
        }
		OmnString err;
        if (!tcpClients->connect(err))
        {
           OmnAlarm << "Failed to connect: " << err << enderr;
        }
		int bytesSent = 0;

        while (bytesSent < lenStr)
        {
           //u32 sendLen = contentLen - bytesSent;
           u32 sendLen = lenStr - bytesSent;
           if (sendLen > sendBlockSize) sendLen = sendBlockSize;

           if (!tcpClients->writeTo(data + bytesSent, sendLen))
           {
              OmnAlarm << "Failed to send: " << enderr;
           }

           bytesSent += sendLen;
        }

		// 
		// We need to read the contents
		//

		OmnConnBuffPtr buff;
		bool connBroken;
		bool timeout = false;
		int bytesRead = 0;
		while (!timeout)
		{
    		if (!tcpClients->readFrom(buff, 30, timeout, connBroken))
			{
				// OmnAlarm << "Failed to read" << enderr;
				if (bytesRead == 102400275)
				{
					successLock.lock();
					success++;
					cout << "Total success: " << success << endl;
			//		cout << "Total receive: " << bytesRead << endl;
					successLock.unlock();
				}

				break;
			}

			if (timeout)
			{
				cout << "Timeout Read length: " << bytesRead << endl;
				break;
			}

			bytesRead += buff->getDataLength();		
		}
 
OmnSleep(120);

	}
	tcpClients->closeConn();
	return true;
}


bool 
OmnGenThread::signal(const int threadLogicId)
{
	return true;
}


bool
OmnGenThread::checkThread(OmnString &errmsg, const int tid) const 
{
	return true;
}


