////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransMgr.cpp
// Description:
//	To save CPU cycles, this class runs a thread that runs at mPrecision ms 
//	ticker. This means that it is possible that the timing can be 
//  mPrecision ms off.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "RetransMgr/RetransMgr.h"

#include "Event/Event.h"
#include "Porting/TimeOfDay.h"
#include "RetransMgr/RetransTrans.h"
#include "RetransMgr/RetransRequester.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


OmnSingletonImpl(OmnRetransMgrSingleton,
                 OmnRetransMgr,
                 OmnRetransMgrSelf,
                "OmnRetransMgr");


OmnRetransMgr::OmnRetransMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mTimerSec(eDefaultTimerSec),
mTimerUsec(eDefaultTimerUsec),
mTooManyReqs(false),
mTooManyReqShreshold(eDefaultTooManyReq)
{
	createDefaultSchedules();
}


bool
OmnRetransMgr::start()
{
	//
	// Start the thread
	//
	OmnTrace << "Singleton class OmnRetransMgr started!" << endl;
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "RetransMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
OmnRetransMgr::stop()
{
	// 
	// Stop the thread.
	//
	OmnTrace << "Singleton class OmnRetransMgr stopped!" << endl;
	if (mThread)
	{
		mThread->stop();
		return true;
	}

	OmnAlarm << "Thread not started!" << enderr;
	return false;
}


OmnRslt
OmnRetransMgr::config(const OmnXmlParserPtr &conf)
{
	//
	// 	<RetransMgrConfig>
	//		<Timer>
	//			<TimerSec>
	//			<TimerUsec>
	//		</Timer>
	//		<TooManyReqThreshold>
	//		<Schedules>
	//			<Schedule>
	//				<ScheduleName>
	//				<TriggerPoint>
	//				<TriggerPoint>
	//					..
	//			</Schedule>
	//			...
	//		</Schedules>
	//	</RetransMgrConfig>
	//
	OmnXmlItemPtr def = conf->tryItem(OmnXmlItemName::eRetransMgrConfig);
	if (!def)
	{
		return true;
	}

	// 
	// Configure the timer
	//
	OmnXmlItemPtr timerDef = def->tryItem(OmnXmlItemName::eTimerDef);
	if (timerDef)
	{
		mTimerSec = def->getInt(OmnXmlItemName::eSecond, eDefaultTimerSec);
		if (mTimerSec < 0)
		{
			return OmnWarn << "RetransMgr TimerSec is negative" << enderr;
		}

		mTimerUsec = def->getInt(OmnXmlItemName::eUsec, eDefaultTimerUsec);
		if (mTimerUsec < 0)
		{
			return OmnWarn << "RetransMgr TimerUsec is negative" << enderr;
		}
	}

	mTooManyReqShreshold = def->getInt(OmnXmlItemName::eTooManyReqThreshold, 
		eDefaultTooManyReq);

	//
	// Configure the schedules
	//
	OmnXmlItemPtr schdDef = def->tryItem(OmnXmlItemName::eSchedules);
	if (schdDef)
	{
		// 
		// There are schedules.
		//
		schdDef->reset();
		while (schdDef->hasMore())
		{
			OmnXmlItemPtr schd = schdDef->next();
			if (schd->entries() <= 1)
			{
				OmnAlarm << "Invalid retransmission schedule definition: "
					<< schd->toString() << enderr;
			}
			else
			{
				schd->reset();
				OmnRetransSchedule schedule;
				int lastTriggerPoint = 0;
				bool isGood = false;
				while (schd->hasMore())
				{
					int triggerPoint = schd->next()->getInt(OmnXmlItemName::eTriggerPoint, -1);
					if (triggerPoint < lastTriggerPoint)
					{
						OmnAlarm << "Invalid retransmission schedule definition: "
							<< schd->toString() << enderr;
						isGood = false;
						break;
					}

					schedule.append(triggerPoint);
					lastTriggerPoint = triggerPoint;
				}

				if (isGood)
				{
					// 
					// Successfully created a schedule
					//
					mSchedules.append(schedule);
				}
			}
		}
	}

	return true;
}


OmnRetransMgr::~OmnRetransMgr()
{
}


void
OmnRetransMgr::createDefaultSchedules()
{
	// 
	// We will create one default retransmission schedule:
	//	500		500
	//	1000	1500
	//	2000	3500
	//	4000	7500
	//	8000	15500
	//	16000	31500
	//
	OmnRetransSchedule schedule;
	schedule.append(500);
	schedule.append(1500);
	schedule.append(3500);
	schedule.append(7500);
	schedule.append(15500);
	schedule.append(31500);
	mSchedules.append(schedule);
}


OmnRetransTransPtr 
OmnRetransMgr::addTrans(const OmnMsgPtr &msg, 
						const OmnRetransRequesterPtr &requester,
						void *data)
{
	//
	// Create the transaction. Use the default schedule.
	//
	OmnRetransTransPtr trans = OmnNew OmnRetransTrans(
		msg, requester, mSchedules[eDefaultSchedule], data);

	//
	// Add the newly created handler to the list.
	//
	mLock->lock();
	mList.append(trans);
	mLock->unlock();
	return trans;
}


OmnRetransTransPtr 
OmnRetransMgr::addTrans(const OmnMsgPtr &msg, 
						const OmnRetransRequesterPtr &requester,
						const int scheduleId,
						void *data)
{
	//
	// Create the transaction. If the schedule 'scheduleId' is defined, 
	// we will use that schedule. Otherwise, we will use the default
	// schedule.
	//
	OmnRetransTransPtr trans;
	if (scheduleId < 0 || scheduleId >= mSchedules.entries())
	{
		OmnAlarm << OmnErrId::eInvalidScheduleId
			<< "Invalid schedule ID: " << scheduleId << enderr;

		trans = OmnNew OmnRetransTrans(
			msg, requester, mSchedules[eDefaultSchedule], data);
	}
	else
	{
		trans = OmnNew OmnRetransTrans(
			msg, requester, mSchedules[scheduleId], data);
	}

	//
	// Add the newly created handler to the list.
	//
	mLock->lock();
	mList.append(trans);
	mLock->unlock();

	// 
	// Check whether need to generate an event
	//
	if (mList.entries() >= mTooManyReqShreshold && !mTooManyReqs)
	{
		// 
		// There are too many outstanding retransmission requests. 
		// Raise the event.
		//
		mTooManyReqs = true;
		OmnWarn << OmnErrId::eRetransError
			<< "There are too many retransmission requests: "
			<< mList.entries() << enderr;
	}
	else
	{
		if (mList.entries() < mTooManyReqShreshold && mTooManyReqs)
		{
			// 
			// There were too many requests, but now it is not. Need to 
			// raise another event.
			//
			OmnWarn << OmnErrId::eRetransDroppedToNormal
				<< "Retransmission requests dropped to normal: " 
				<< mList.entries() << enderr;
			mTooManyReqs = false;
		}
	}
	return trans;
}


bool
OmnRetransMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter OmnRetransMgr::threadFunction." << endl;
    OmnRetransMgrPtr thisPtr(this, false);
/* Chen Ding, XXXXX
    while (state == OmnThrdStatus::eActive)
    {
        //
        // Wait 
        //
		mThreadStatus = true;
        OmnWaitSelf->wait(mTimerSec, mTimerUsec);
		mThreadStatus = true;

        //
        // The timer expired
        //
		int64_t msec = OmnTime::getCrtMsec();
		check(msec);
    }

    OmnTrace << "Leave OmnTime::secTickThreadFunc" << endl;
*/
    return true;
}


bool
OmnRetransMgr::signal(const int threadLogicId)
{
	//
	// Since the thread will wake up at mPrecision number of ms, there
	// is no need to do anything here.
	//
    return true;
}


void
OmnRetransMgr::check(const int64_t &msec)
{
	// 
	// This function checks all the outstanding handles to see whether
	// it needs to retransmit messages. The handler knows whether it 
	// needs to retransmit and how to retransmit. If the handler is too
	// old, this function will also remove it.
	//
	int index = 0;

	OmnDynArray<OmnRetransTransPtr, 5000, 1000> triggered;
	while (index < mList.entries())
	{
		//
		// We block this class to check for eBatch
		// number of handlers. After that, it unlocks
		// to give call processing threads a chance to
		// add more handlers here. 
		//
		mLock->lock();
		int i=0;
		while (i++<100 && index < mList.entries())
		{
			if (!mList[index]->isValid())
			{
				mList.remove(index);
				continue;
			}

			if (mList[index]->isTooOld(msec))
			{
				//
				// This means the handler is too old. Remove it.
				//
				triggered.append(mList[index]);
				mList.remove(index);
				continue;
			}

			// 
			// Check whether it triggers any retransmission
			//
			if (mList[index]->isRetransTriggered(msec))
			{
				// 
				// It needs to retransmit the message. Store it in the array
				// 'triggered[]' first. 
				//
				triggered.append(mList[index]);
			}
			index++;
		}
		mLock->unlock();
	}

	// 
	// Call back the triggered
	//
	for (int i=0; i<triggered.entries(); i++)
	{
		triggered[i]->informRequester(msec);
	}
}


bool
OmnRetransMgr::checkThread(OmnString &errmsg, const int tid) const
{
	if (mThreadStatus)
	{
		return true;
	}

	//	
	// We assume mPrecision is shorter than the timer ThreadMgr 
	// runs. 
	//
    if (OmnTime::getCrtSec() - mHeartbeatStartSec > 2)
    {
		errmsg = "Retrans thread failed. ";
        OmnAlarm << OmnErrId::eAlarmHeartbeatError
            << errmsg << enderr; 
        return false;
    }

    return true;
}
