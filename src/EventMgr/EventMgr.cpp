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
// Modification History:
// 2011/01/20	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EventMgr/EventMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
//#include "DocClient/DocClient.h"
#include "SEUtil/IILName.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SeSiteMgr/SeSiteMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/SecurityMgrObj.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Tracer/Tracer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosEventMgrSingleton, 
				 AosEventMgr, 
				 AosEventMgrSelf,
				"AosEventMgr");



AosEventMgr::AosEventMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosEventMgr::~AosEventMgr()
{
}


bool
AosEventMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "EventMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosEventMgr::stop()
{
	OmnTrace << "Singleton class AosEventMgr stopped!" << endl;
	if (mThread)
	{
		mThread->stop();
		return true;
	}

	OmnAlarm << "Thread not started!" << enderr;
	return false;
}


bool
AosEventMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosEventMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter AosEventMgr::threadFunc." << endl;

	AosSmartDocObjPtr sdocs;
    while (state == OmnThrdStatus::eActive)
    {
		/* 
		 * Chen Ding, 08/13/2011
		 * Need to re-work
		mLock->lock();
		if (mQueue.size() == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		Entry entry = mQueue.front();
		mQueue.pop();
		sdocs = getAsyncSdocs(entry.hook, entry.rdata);
		mLock->unlock();

		while (sdocs)
		{
			sdocs->run(entry.rdata);
			sdocs = sdocs->getNext();
		}
		*/
		OmnSleep(1000);
    }

    OmnTraceThread << "Leaving AosEventMgr::threadFunc" << endl;
    return true;
}


bool
AosEventMgr::unregisterSdoc(
		const AosEventHook hook,
		const OmnString &key,
		const AosSmartDocObjPtr &sdoc, 
		const AosRundataPtr &rdata) 
{
	// This function unregisters [hook, key, sdoc]. The class SHALL
	// not be locked before calling this function. 
	//
	// All registrations that are in the same bucket are linked.
	// It finds the list. If not, it is an error. Otherwise, 
	// it loops through to find the entry. If not found, it is 
	// an error. Otherwise, it removes the entry from the list.
	aos_assert_r(key != "", false);
	mLock->lock();

	AosSmartDocObjPtr head = mSdocs[hook][key];
	aos_assert_rl(head, mLock, false);

	AosSmartDocObjPtr crt = head;
	do
	{
		if (crt == sdoc)
		{
			// Found it.
			if (crt->getNext() == crt)
			{
				// The list has only one. Need to remove it.
				mSdocs[hook][key] = 0;
				mLock->unlock();
				return true;
			}

			AosSmartDocObjPtr newhead = head;
			if (newhead == crt)
			{
				// It is the first one. Set the new head
				newhead = crt->getNext();
				mSdocs[hook][key] = newhead;
			}

			// The list is longer than one. Remove the current one.
			crt->getPrev()->setNext(crt->getNext());
			crt->getNext()->setPrev(crt->getPrev());
			mLock->unlock();
			return true;
		}

		crt = crt->getNext();

	} while (crt != head);

	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool
AosEventMgr::signal(const int threadLogicId)
{
	//
	// Wake up the thread in case it is sleeping.
	//
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
    return true;
}


void
AosEventMgr::heartbeat(const int tid)
{
	mThreadStatus = false;
}


bool
AosEventMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return mThreadStatus;
}


bool
AosEventMgr::procEvent(
		const AosEventHook hook, 
		const AosHookPoint hookpoint,
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	/*
	 * Chen Ding, 08/13/2011
	 * Need to re-work
	mLock->lock();
	AosSmartDocObjPtr sdocs = getSdocs(hook, key, rdata);
	mQueue.push(Entry(hook, rdata));
	mCondVar->signal();
	mLock->unlock();
	
	while (sdocs)
	{
		sdocs->run(rdata);
		sdocs = sdocs->getNext();
	}

	return true;
	*/
// Chen Ding, 08/16/2011
return true;
	OmnNotImplementedYet;
	return false;
}


/*
bool
AosEventMgr::addToIILLocked(
		const AosEventHook hook, 
		const OmnString &key, 
		const OmnString &sdoc_objid, 
		const AosRundataPtr &rdata)
{
	// Each hook has an IIL (str IIL) to store all its registered
	// hooks. The value portion is the key and the docid portion
	// is the smartdoc's docid. The IIL is identified by hook.

	// Retrieve the smartdoc.
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(rdata->getSiteid(), 
			sdoc_objid, rdata);
	if (!doc)
	{
		rdata->setError() << "Smartdoc undefined: " << sdoc_objid;
		return false;
	}

	u64 sdoc_docid = doc->getAttrU64(AOSTAG_DOCID, 0);

	return addToIILLocked(hook, key, sdoc_docid, rdata);
}


bool
AosEventMgr::addToIILLocked(
		const AosEventHook hook,
		const OmnString &key,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// All registrations are stored in IILs. There is one IIL
	// per [hook, siteid]. The class SHALL not be locked when
	// calling this function.
	OmnString iilname = AosIILName::composeHookIILName(hook);
	//bool rslt = AosIILClient::getSelf()->addValueDoc(
			iilname, key, docid, false, true, rdata);
	if (!rslt)
	{
		// This means that the entry is already in the list.
		rdata->setError() << "Entry already in the list";
		return false;
	}
	return true;
}
*/


bool
AosEventMgr::registerSdoc(
		const OmnString &hookstr,
		const OmnString &key, 
		const OmnString &sdoc_objid, 
		const AosRundataPtr &rdata) 
{
	/*
	// This function reigsters an entry [hook, key, sdoc_objid]. 
	// The class SHALL not be locked before calling this
	// function.
	if (key == "")
	{
		rdata->setError() << "Hook key is empty";
		return false;
	}

	AosEventHook hook = AosEventHookToEnum(hookstr);
	if (!AosIsValidHook(hook))
	{
		rdata->setError() << "Unregognized hook: " << hookstr;
		return false;
	}

	// Check the security
	if (!AosSecurityMgrObj::getSecurityMgr()->checkRegisterHook(
			hook, key, sdoc_objid, rdata))
	{
		return false;
	}

	mLock->lock();

	// Add to the IIL
	bool rslt = addToIILLocked(hook, key, sdoc_objid, rdata);
	if (!rslt) 
	{
		mLock->unlock();
		return false;
	}

	// Add to the queue
	try
	{
		AosSmartDocObjPtr smartdoc = OmnNew AosSmartDoc(sdoc_objid, rdata);
		bool rslt = addToQueueLocked(hook, key, smartdoc, rdata);
		mLock->unlock();
		return rslt;
	}

	catch (OmnExcept &e)
	{
		mLock->unlock();
		rdata->setError() << "Smartdoc incorrect";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mLock->unlock();
	rdata->setOk();
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosEventMgr::addToQueueLocked(
		const AosEventHook hook,
		const OmnString &key, 
		const AosSmartDocObjPtr &sdoc, 
		const AosRundataPtr &rdata) 
{
	// This function adds the registration [hook, key, sdoc] 
	// into the queue.  There is a maximum on how many 
	// smartdocs can register on each hook.
	aos_assert_r(key != "", false);

	if (mSdocs[hook].size() > mMaxRegs[hook])
	{
		rdata->setError() << "Too many registrations: " << hook;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosSmartDocObjPtr head = mSdocs[hook][key];
	if (head)
	{
		// Appended it to the tail of the list. 
		AosSmartDocObjPtr tail = head->getPrev();
		tail->setNext(sdoc);
		sdoc->setPrev(tail);
		sdoc->setNext(head);
		head->setPrev(sdoc);
		return true;
	}

	// Not in the hash yet. 
	sdoc->setPrev(sdoc);
	sdoc->setNext(sdoc);
	mSdocs[hook][key] = sdoc;
	return true;
}	


AosSmartDocObjPtr
AosEventMgr::getSdocs(
		const AosEventHook hook, 
		const OmnString &key, 
		const AosRundataPtr &rdata)
{
	vector<AosSmartDocObjPtr> docs;
	if (hook <= eAosHookInvalid && hook >= eAosHookMax)
	{
		rdata->setError() << "Invalid hook!";
		return 0;
	}

	AosSmartDocObjPtr sdoc = mSdocs[hook][key];
	return sdoc;
}


AosSmartDocObjPtr
AosEventMgr::getAsyncSdocs(
		const AosEventHook hook, 
		const AosRundataPtr &rdata)
{
	//OmnNotImplementedYet;
	return 0;
}

