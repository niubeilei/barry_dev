///////////////////////////////////////////////////////////////
// Created by:	Open Source Society
// Created:		10/3/2000
// Comments:
//
// Change History:
//	10/03/2000 - File Created
//
///////////////////////////////////////////////////////////////
#ifndef Omn_Thread_ThrdShellProc_h
#define Omn_Thread_ThrdShellProc_h

#include "Debug/Rslt.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Sem.h"
#include "Util/RCObject.h"



class OmnThrdShellProc : virtual public OmnRCObject
{
protected:
	u64					mPoison1;		// Chen Ding, 2013/02/13
	OmnThrdTransPtr		mTrans;
	OmnString			mErrmsg;
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	bool				mFinished;
	OmnString			mName;
	u64					mPoison2;		// Chen Ding, 2013/02/13
	OmnSemPtr			mSem;
	u64					mPoison3;		// Chen Ding, 2013/02/13

public:
	OmnThrdShellProc(const OmnString &name);
	~OmnThrdShellProc();

	virtual bool		run() = 0;
	virtual bool		procFinished() = 0;
	virtual bool		waitUntilFinished();

	// Chen Ding, 2013/02/13
	void 				setSem(const OmnSemPtr &sem);
	void 				postSem();
	OmnString			getName() const {return mName;}
	bool				isSuccessful() const;
	OmnString			getErrmsg() const {return mErrmsg;}
	bool				threadFinished();
	bool				isFinished() const {return mFinished;}
};
#endif

