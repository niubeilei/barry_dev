////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Wait.cpp
// Description:
//	This is a singleton. Singleton dependency:
//		OmnAlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/Wait.h" 

#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/ExitHandler.h"
#include "Porting/Select.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/IpAddr.h"
#include "Util1/Time.h"
#include "UtilComm/CommUtil.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(OmnWaitSingleton,
                 OmnWait,
                 OmnWaitSelf,
                "OmnWait");

OmnWait::OmnWait()
:
mTheSock(-1)
{
}


OmnWait::~OmnWait()
{
}


bool
OmnWait::start()
{
	// 
	// We will use the loopback address
	//
	OmnIpAddr localIpAddr("127.0.0.1");

	int localPort = 0;
	OmnString err;
	OmnRslt rslt = OmnCommUtil::createUdpSock(
			mTheSock, localIpAddr, localPort, err, false);
	if (!rslt)
	{
		cout << "<" << __FILE__ << ":" << __LINE__ << "> **********"
			<< "Failed To Create OmnWait Socket: " << err.data() << endl;

		OmnExitHandler::handleExit(OmnFileLine, 
			OmnErrId::eFailedToCreateTimerSocket);
		return false;
	}

	//OmnTrace << "Wait IP: " << localIpAddr.toString() << endl;
	//OmnTrace << "Wait sock: " << mTheSock << endl;
	cout << "Wait IP: " << localIpAddr.toString() << endl;
	cout << "Wait sock: " << mTheSock << endl;

    FD_ZERO(&mReadfds);
    FD_SET(mTheSock, &mReadfds);
    mNfds = mTheSock + 1;

	return true;
}


bool
OmnWait::stop()
{
	return true;
}


bool
OmnWait::wait(const int theSec, const int theUsec)
{
	//
	// It uses ::select(...) to wait for the specified msec and then return.
	// This is a block call. 
	//
	// Since this select is used for timer, we do not have to put anything
	// in the read, write, or error parameters. But Windows require that
	// there shall be at least one. So we will add the read parameter. 
	//
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(mTheSock, &fd);

	// The wait shall be at least 10ms. Otherwise, it is an error.
	int sec = theSec;
	int usec = theUsec;
	if (sec < 0 || usec < 0 || (sec == 0 && usec < eMinUsec))
	{
		sec = 0;
		usec = eMinUsec;
	}
		
	timeval t;
	t.tv_sec  = sec;
	t.tv_usec = usec;
	uint crtsec, crtusec;
	uint waitMs = sec * 1000 + usec / 1000;
	OmnTime::getRealtime(crtsec, crtusec);

	while (1)
	{
		int rslt = OmnSocketSelect(mNfds, &fd, 0, 0, &t);
		if (rslt == 0)
		{
			// 
			// Check whether it has waited enough
			//
			uint crtsec1, crtusec1;
			OmnTime::getRealtime(crtsec1, crtusec1);
			uint actual = (crtsec1 - crtsec) * 1000 + 
						  (crtusec1 - crtusec) / 1000;
			if (actual >= waitMs - 5)
			{
				return true;
			}

			// 
			// This means it hasn't waited enough
			//
			sec -= (crtsec1 - crtsec);			
			usec -= (crtusec1 - crtusec);			
			if(usec < 0)			
			{				
				sec --;				
				usec += 1000000;			
			}	
			else if(usec >= 1000000)
			{
				sec ++;				
				usec -= 1000000;			
			}
			
			t.tv_sec  = sec;			
			t.tv_usec = usec;			
			continue;
			
		}
		else
		{
			// 
			// Failed the selection. If the alarm is on, we need to raise the
			// alarm. Otherwise, just print the error message.
			//
			if (OmnAlarmMgr::isAlarmOn())
			{
				OmnAlarm << "Failed to select: " 
					<< OmnGetStrError(OmnErrType::eSelect) 
					<< ". Sec: " << theSec << ":" << theUsec 
					<< ":" << mTheSock << ":" << rslt << enderr;
			}
			else
			{
				cout << "<" << __FILE__ << ":" << __LINE__ 
					<< "> ********** Failed to select: " 
					<< OmnGetStrError(OmnErrType::eSelect).data() << endl;
			}
			return false;
		}
	}
}
	
	
bool
OmnWait::config(const AosXmlTagPtr &def)
{
	return true;
}

