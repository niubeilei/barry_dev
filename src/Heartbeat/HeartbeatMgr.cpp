////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HeartbeatMgr.cpp
// Description:
//	This class is used by a module to maintain a heartbeat connection
//	with one or more others so that it knows who is up and down.
//	
//  This class has a TCP Server. Anyone that wants to establish a
//	heartbeat connection to this application should make a TCP 
//  connection to this server. Anything received by this TCP server 
//  is automatically bounced back. The sender can use these connections 
//  to determine whether this application is up and running. 
// 
//  When bouncing back a heartbeat request, this class will ask the
//  ThreadMgr to determine whether this unit is running healthy.
//  ThreadMgr knows whether threads are running healthy. If not,
//  it will either not return the call or return an error message.
//
//  On the other hand, this class maintain a list of client TCP
//	connections to other units, called Monitored Units. It constantly 
//  sends heartbeat messages to the monitored units. If it does not 
//  receive response, or receive negative response, or the connection 
//  breaks, the remote unit is considered down. It will call its 
//  callback to take actions.   
//
// Modification History:
//		It appeared that the heartbeat fails incorrectly. It may be
//		because after sending heartbeat, we somehow are waken up 
//		by someone. Added checking the time to make sure there is
//		enough time for programs to respond to heartbeat requests. 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Heartbeat/HeartbeatMgr.h"

#include "Alarm/Alarm.h"
#include "Heartbeat/HbMonitor.h"
#include "Heartbeat/HbServer.h"
#include "Heartbeat/DbHeartbeat.h"
#include "Logger/Log.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlItem.h"


extern OmnTimer *	OmnTimerSelf;
extern OmnTime *	OmnTimeSelf;

OmnSingletonImpl(OmnHeartbeatMgrSingleton,
				 OmnHeartbeatMgr,
				 OmnHeartbeatMgrSelf,
				 "OmnHeartbeatMgr");


OmnHeartbeatMgr::OmnHeartbeatMgr()
:
mStatus(eIdle),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mHbTimerSec(eDefaultHbTimerSec)
{
	for (int i=0; i<eMaxMonitors; i++)
	{
		mCallbacks[i] = 0;
	}
}


bool
OmnHeartbeatMgr::start()
{
	OmnTraceCP << "Start HeartbeatMgr" << endl;
	if (mStatus != eIdle)
	{
		OmnAlarm << "Heartbeat Mgr is not idle: " << mStatus << enderr;
		return false;
	}

	if (!mHbServer.isNull() && !mHbServer->start())
	{
		return false;
	}

	if (!mHbServer2.isNull() && !mHbServer2->start())
	{
		return false;
	}

	if (!mDbHeartbeat.isNull())
	{
		mDbHeartbeat->start();
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "HeartbeatMgr", 0, true, __FILE__, __LINE__);
	mThread->start();

	startMonitor();

	return true;
}


bool
OmnHeartbeatMgr::stop()
{
	if (!mHbServer.isNull())
	{
		mHbServer->stop();
	}

	if (!mHbServer2.isNull())
	{
		mHbServer2->stop();
	}

	mThread->stop();
	return true;
}


OmnHeartbeatMgr::~OmnHeartbeatMgr()
{
}


OmnRslt
OmnHeartbeatMgr::config(const OmnXmlParserPtr &parser)
{
	//
	// It assumes
	// 		<HeartbeatConfig>
	// 		 	<DbHeartbeatFreq>
	//			<HbTimerSec>
	//			<HbServers>
	//				<HbServer>
	//					<IpAddr>
	//					<LocalPort>
	//					<NumPorts>
	//					<Name>
	//				</HbServer>
	//				<HbServer>		// Optional
	//					<IpAddr>
	//					<LocalPort>
	//					<NumPorts>
	//					<Name>
	//				</HbServer>
	//			</HbServers>
	//			<Monitors>
	//				<Monitor>
	//					<Name>
	//					<MonitorId>
	//					<Conn1>
	//						<Name>
	//						<RemoteIpAddr>
	//						<RemotePort>
	//						<NumberOfPorts>
	//						<LocalIpAddr>
	//						<LocalPort>
	//					</Conn1>
	//					<Conn2>		// Optional for redundancy
	//					...
	//					</Conn2>
	//				<Monitor>
	//				...
	//			</Monitors>
	//		</HeartbeatConfig>
	//

	if (!parser)
	{
		// 
		// No configuration. 
		// 
		OmnAlarm << "Heartbeat must be configured but no configuraiton is passed in!"
			<< enderr;
		return false;
	}

	def = def->tryItem(OmnXmlItemName::eHeartbeatConfig);
	if (def.isNull())
	{
		OmnAlarm << "Heartbeat must be configured but no configuraiton is passed in!"
			<< enderr;
		return false;
	}

	// 
	// Check whether it needs to start 'OmnDbHeartbeat'
	//
	int dbHeartbeatFreq = def->getInt(OmnXmlItemName::eDbHeartbeatFreq, 0);
	if (dbHeartbeatFreq > 0)
	{
		mDbHeartbeat = OmnNew OmnDbHeartbeat(dbHeartbeatFreq);
	}

	mHbTimerSec = def->getInt(OmnXmlItemName::eHbTimerSec, eDefaultHbTimerSec);
	
	OmnXmlItemPtr hbservers = def->tryItem(OmnXmlItemName::eHbServers);
	if (!hbservers.isNull())
	{
		hbservers->reset();
		if (!hbservers->hasMore())
		{
			OmnAlarm << OmnErrId::eAlarmConfigError
				<< "Missing HeartbeatServer: " 
				<< def->toString() << enderr;
			return false;
		}

		OmnXmlItemPtr hbserver = hbservers->next();
		mHbServer = OmnNew OmnHbServer();
		if (!mHbServer->config(hbserver))
		{
			return false;
		}

		if (hbservers->hasMore())
		{
			// 
			// There is a second heartbeat server
			//
			hbserver = hbservers->next();
			mHbServer2 = OmnNew OmnHbServer();
			if (!mHbServer2->config(hbserver))
			{
				return false;
			}
		}
	}

	OmnXmlItemPtr monitors = def->tryItem(OmnXmlItemName::eMonitors);
	if (monitors.isNull())
	{
		return true;
	}

	monitors->reset();
	while (monitors->hasMore())
	{
		OmnXmlItemPtr monitor = monitors->next();
		try
		{
			OmnHbMonitorPtr m = OmnNew OmnHbMonitor(this, monitor);

			int monitorId = m->getMonitorId();
			if (monitorId < 0 || monitorId >= eMaxMonitors)
			{
				OmnAlarm << OmnErrId::eAlarmConfigError
					<< "Invalid monitor ID: "
					<< monitorId << ". Maximum: " 
					<< eMaxMonitors << ". "
					<< monitor->toString() << enderr;
				return false;
			}

			mMonitors[monitorId] = m;
		}

		catch (const OmnExcept &e)
		{
			OmnAlarm << OmnErrId::eAlarmConfigError
				<< "Failed to create heartbeat monitor: " 
				<< e.toString() << enderr;
		}
	}

	return true;
}

	
bool
OmnHeartbeatMgr::threadFunc(OmnThrdStatus::E &state, 
							const OmnThreadPtr &thread)
{
    OmnTrace << "Enter OmnHeartbeatMgr::threadFunc." << endl;
	OmnTimerObjPtr thisPtr(this, false);

	bool needToSend = true;		// Chen Ding, 05/18/2003, Change-0001
	int lastSendTick;			// Chen Ding, 05/18/2003, Change-0001

    while (state == OmnThrdStatus::eActive)
    {

		//
		// Chen Ding, 05/18/2003
		//
        // mLock->lock();

		// 
		// Chen Ding, 05/18/2003, Change-0001
		// 
		if (needToSend)
		{

			//
			// Send heartbeat messages to all peers.
			//
			for (int i=0; i<eMaxMonitors; i++)
			{
				if (!mMonitors[i].isNull())
				{
					mMonitors[i]->sendHeartbeat();
				}
			}

			needToSend = false;
			lastSendTick = OmnTimeSelf->getSecTick();
        }

		//
		// Then wait a while
		//
		OmnTimerSelf->startTimer("HeartbeatMgr", mHbTimerSec, 
			0, thisPtr, 0);

		mThreadStatus = true;

		// 
		// Chen Ding, 05/18/2003
		//
		mLock->lock();
		mCondVar->wait(mLock);
		mLock->unlock();

		mThreadStatus = true;

		// 
		// Chen Ding, 05/18/2003, Change-0001
		//
		if (OmnTimeSelf->getSecTick() - lastSendTick < mHbTimerSec-1)
		{
			//
			// Someone woke us up.
			//
			OmnWarn << OmnErrId::eWarnProgError
				<< "Someone woke heartbeat up too soon: " 
				<< OmnTimeSelf->getSecTick() << ":" 
				<< lastSendTick << ":"
				<< mHbTimerSec << enderr;
		}
		else
		{
			//
			// Then it's the time to check whether all returned
			// the messages.
			//
			for (int i=0; i<eMaxMonitors; i++)
			{
				if (!mMonitors[i].isNull())
				{
					if (!mMonitors[i]->checkHeartbeat())
					{
						// 
						// Heartbeat failed. 
						//
						OmnAlarm << OmnErrId::eAlarmHeartbeatError
							<< "Heartbeat failed for: " 
							<< mMonitors[i]->getName()
							<< " at: " << OmnGetTime()
							<< enderr;

						OmnHeartbeatLog << "Heartbeat failed for: " 
							<< mMonitors[i]->getName()
							<< " at: " << OmnGetTime() << flushlog;

						if (mCallbacks[i])
						{
							// 
							// Chen Ding, 05/18/2003
							//
							// mLock->unlock();
							mCallbacks[i]->heartbeatFailed(i);
							// mLock->lock();
						}
					}
				}
			}

			needToSend = true;
		}

		// 
		// Chen Ding, 05/18/2003
		//
        // mLock->unlock();
    }

    OmnTraceThread << "Leaving OmnHeartbeatMgr::threadFunc" << std::endl;
    return true;
}


bool
OmnHeartbeatMgr::stopMonitor(const OmnString &name, OmnString &err)
{
	OmnHbMonitorPtr monitor = getMonitor(name);
	if (monitor.isNull())
	{
		err = "Monitor not found";
		OmnWarn << OmnErrId::eWarnHeartbeatError
			<< "Monitor not found: " << name << enderr;
		return false;
	}

	OmnTrace << "To stop heartbeat: "
		<< monitor->getName()
		<< endl;
	monitor->stopHeartbeat();
	return true;
}


bool
OmnHeartbeatMgr::isMonitorActive(const OmnString &name) 
{
	OmnHbMonitorPtr monitor = getMonitor(name);
	return monitor.isNull();
}


bool
OmnHeartbeatMgr::startMonitor()
{
	for (int i=0; i<eMaxMonitors; i++)
	{
		if (!mMonitors[i].isNull())
		{
			mMonitors[i]->startHeartbeat();
		}
	}

	return true;
}


bool
OmnHeartbeatMgr::startMonitor(const OmnString &name, OmnString &err)
{
	OmnHbMonitorPtr monitor = getMonitor(name);
	if (monitor.isNull())
	{
		err = "Monitor not found";
		OmnWarn << OmnErrId::eWarnHeartbeatError
			<< "Monitor not found: " << name << enderr;
		return false;
	}

	OmnTrace << "To start heartbeat: "
		<< monitor->getName()
		<< endl;
	monitor->startHeartbeat();
	return true;
}


OmnHbMonitorPtr
OmnHeartbeatMgr::getMonitor(const int monitorId)
{
	if (monitorId < 0 || monitorId >= eMaxMonitors)
	{
		OmnWarn << OmnErrId::eWarnProgError
			<< "Invalid monitorId: " << monitorId << enderr;
		return 0;
	}

	return mMonitors[monitorId];
}


OmnHbMonitorPtr
OmnHeartbeatMgr::getMonitor(const OmnString &name)
{
	mLock->lock();
	OmnHbMonitorPtr monitor;
	for (int i=0; i<eMaxMonitors; i++)
	{
		if (!mMonitors[i].isNull())
		{
			if (mMonitors[i]->getName() == name)
			{
				monitor = mMonitors[i];
				mLock->unlock();
				return monitor;
			}
		}
	}

	mLock->unlock();
	return 0;
}


bool
OmnHeartbeatMgr::signal(const int threadLogicId)
{
	//
	// Do not wake the thread up. Let it wake up itself.
	//
    return true;
}


void
OmnHeartbeatMgr::timeout(const int timerId, const OmnString &name, void *parm)
{
	//
	// Chen Ding, 05/18/2003, 
	// Forgot locking mCondVar before calling its signal() member function.
	//
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
}


//
// E007, 07/13/2003, 2003-0073
//
bool
OmnHeartbeatMgr::checkThread(OmnString &errmsg) const
{
    if (mThreadStatus)
    {
        return true;
    }

    //
    // If the status is false and it has been too long
    // since the time mHeartbeatStatus was set to false,
    // it indicates this thread is in trouble.
    //
    if (OmnTimeSelf->getCrtSec() - mHeartbeatStartSec
        >= mHbTimerSec + 1)
    {
		errmsg = "HeartbeatMgr thread failed. ";
		OmnAlarm << OmnErrId::eAlarmThreadError
			<< errmsg << enderr;
        return false;
    }

    return true;
}


void
OmnHeartbeatMgr::heartbeatFailed(const int monitorId)
{
	if (monitorId < 0 || monitorId >= eMaxMonitors ||
		mMonitors[monitorId].isNull())
	{
		OmnAlarm << OmnErrId::eAlarmProgError
			<< "Invalid monitorID: " << monitorId << enderr;
		return;
	}

	OmnAlarm << OmnErrId::eAlarmHeartbeatError
		<< "Heartbeat failed for: " << mMonitors[monitorId]->getName()
		<< " at: " << OmnGetTime()
		<< enderr;

	OmnHeartbeatLog << "Heartbeat failed for: " 
		<< mMonitors[monitorId]->getName()
		<< " at: " << OmnGetTime() << flushlog;

	if (mCallbacks[monitorId])
	{
		mCallbacks[monitorId]->heartbeatFailed(monitorId);
	}
}


void
OmnHeartbeatMgr::controlMsgRead(const OmnString &name,
								const OmnConnBuffPtr &buff)
{
	OmnWarn << OmnErrId::eWarnCommError
		<< "Read a control message from a heartbeat connection: " 
		<< name << ". "
		<< buff->getBuffer() << enderr;
}


int
OmnHeartbeatMgr::getMonitoredStatus(OmnValList<OmnString> &names,
								   OmnValList<bool> &status)
{
	names.clear();
	status.clear();

	for (int i=0; i<eMaxMonitors; i++)
	{
		if (!mMonitors[i].isNull())
		{
			OmnHbMonitorPtr m = mMonitors[i];
			names.append(m->getName());
			status.append(true);
		}
	}

	return names.entries();
}


OmnRslt     
OmnHeartbeatMgr::config(const OmnSysObjPtr &def)
{
	OmnWarn << OmnErrId::eWarnProgError
		<< "Not implemented yet" << enderr;

	return false;
}


bool
OmnHeartbeatMgr::isMonitoredGood(const int monitorId,
								 bool &status) const
{
	if (monitorId < 0 || 
		monitorId >= eMaxMonitors ||
		mMonitors[monitorId].isNull())
	{
		OmnAlarm << OmnErrId::eAlarmProgError
			<< "Invalid monitor ID: " << monitorId << enderr;
		return false;
	}

	return true;
}


bool
OmnHeartbeatMgr::registerMonitor(const int monitorId, OmnHbObj *callback)
{
	if (monitorId < 0 || monitorId >= eMaxMonitors)
	{
		OmnAlarm << OmnErrId::eAlarmProgError
			<< "Invalid monitor ID: " << monitorId << enderr;
		return false;
	}

	if (mCallbacks[monitorId])
	{
		OmnWarn << OmnErrId::eWarnProgError
			<< "Monitor callback is not null: " 
			<< monitorId << ":" << (int)mCallbacks[monitorId] << enderr;
		return false;
	}

	mCallbacks[monitorId] = callback;
	return true;
}


bool
OmnHeartbeatMgr::restore(const int monitorId)
{
	OmnHbMonitorPtr monitor = getMonitor(monitorId);
	if (monitor.isNull())
	{
		OmnAlarm << OmnErrId::eAlarmProgError
			<< "No monitor found: " << monitorId << enderr;
		return false;
	}

	monitor->restore();
	return true;
}


void
OmnHeartbeatMgr::heartbeatRestored(const int monitorId)
{
	if (monitorId < 0 || monitorId >= eMaxMonitors ||
		mMonitors[monitorId].isNull())
	{
		OmnAlarm << OmnErrId::eAlarmProgError
			<< "Invalid monitorID: " << monitorId << enderr;
		return;
	}

	if (mCallbacks[monitorId])
	{
		mCallbacks[monitorId]->heartbeatRestored(monitorId);
	}
}


#endif
