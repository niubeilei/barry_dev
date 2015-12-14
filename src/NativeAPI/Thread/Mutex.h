////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Mutex.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_Mutex_h 
#define Omn_Thread_Mutex_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Semantics/Semantics.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Porting/Mutex.h"
#include <string>

#ifndef AOSLOCK
#define AOSLOCK(x) (x)->lock(__FILE__, __LINE__)
#endif

#ifndef AOSUNLOCK
#define AOSUNLOCK(x) (x)->unlock(__FILE__, __LINE__)
#endif

#ifndef AOSLOCKTRACE
#define AOSLOCKTRACE(x) (x)->trace(__FILE__, __LINE__)
#endif

//#define AOSLOCKAREA(x)  {OmnMutexAgent lock(x);

class OmnMutex : public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosLockMonitorPtr	mMonitor;

private:
	enum
	{
		eMaxLockTime = 10
	};

    OmnMutexType	mLock;
    OmnMutexAttr	mLockAttr;
	bool 			mRecurSive;
	u32				mLockTime;
	OmnString		mFname;
	int				mLine;
	OmnString		mTrace;		// Chen Ding, 10/30/2010

	AosSemanDecl(bool mSemanFlag);
	AosSemanDecl(std::string mClassName);
	AosSemanDecl(std::string mInstName);

	//
	// The following should not be used.
	//
    OmnMutex & operator = (const OmnMutex &);
    OmnMutex(const OmnMutex &);

public:
	OmnMutex();
	explicit OmnMutex(bool recursive);
    OmnMutex(const OmnString &);
    ~OmnMutex();

	// static void * operator new(std::size_t size, const char *fname, const int line, int, int, int);

    bool 			lock();
    bool			unlock();
	bool			trylock(const char*, int);
	bool 			trylock();
    OmnMutexType &	getLock() {return mLock;}

	// AosSemanDecl(void setSemanFlag(bool f) {mSemanFlag = f;});
	void setSemanFlag(bool f);

	bool lock(const OmnString &fname, const int line)
	{
		u32 crttime = OmnGetSecond();
		if (mLockTime > 0 && (crttime > mLockTime + eMaxLockTime))
		{
			u32 locktime = mLockTime;
			u32 ctime = crttime;
			OmnString fn = mFname;
			int ln = mLine;
			OmnString tc = mTrace;
			if (locktime > 0 && ctime > locktime + eMaxLockTime)
			{
				OmnScreen << "************ Lock being hold: " << fname << ":" << line
					<< ":" << fn << ":" << ln 
					<< ":" << locktime << ":" << crttime 
					<< ":" << tc << endl;
			}
		}

		lock();
		mLockTime = OmnGetSecond();
		mFname = fname;
		mLine = line;
		mTrace = "";
		return true;
	}

	bool unlock(const OmnString &fname, const int line)
	{
// Ken Lee, 2013/06/04
//		if (mLockTime == 0)
//		{
//			OmnAlarm << "Double unlocking: " << fname << ":" << line << enderr;
//			return true;
//		}
		mLockTime = 0;
		mFname = "";
		mLine = -1;
		mTrace = "";
		return unlock();
	}

	void trace(const char *fname, const int line)
	{
		mTrace << ":" << fname << ":" << line;
	}

private:
	bool				init();
};


class OmnMutexAgent 
{
	OmnMutexPtr mLock;
public:
	OmnMutexAgent(const OmnMutexPtr &lock):mLock(lock)
	{
		mLock->lock();
	}
	~OmnMutexAgent()
	{
		mLock->unlock();
	}
};

#endif

