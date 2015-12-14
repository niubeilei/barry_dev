///////////////////////////////////////////////////////////////
// Created by:	Open Source Society
// Created:		10/03/2000
// Comments:
//
// Change History:
//	10/03/2000
//
///////////////////////////////////////////////////////////////
#ifndef Omn_Thread_ThreadShellMgr_h
#define Omn_Thread_ThreadShellMgr_h

/*
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadShellCaller.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include <deque>
#include <vector>
using namespace std;

OmnDefineSingletonClass(OmnThreadShellMgrSingleton,
                		OmnThreadShellMgr1, 
						OmnThreadShellMgr1Self,
						OmnSingletonObjId::eThreadShellMgr, 
						"ThreadShellMgr1");


class OmnThreadShellMgr1 : public OmnThreadedObj,
					  	   public OmnThreadShellCaller
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultGetShellTimer = 15,
		eMaxThreadShells = 500
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnMutexPtr			mReqLock;
	OmnCondVarPtr		mReqCondVar;
	vector<OmnThreadShellPtr>	mShells;
	deque<OmnThrdShellProcPtr>	mProcessors;
	int					mGetShellTimer;
	OmnThreadPtr		mThread;

	// Do not use the two:
	OmnThreadShellMgr1(const OmnThreadShellMgr1 &rhs);
	OmnThreadShellMgr1 & operator = (const OmnThreadShellMgr1 &rhs);

public:
	OmnThreadShellMgr1();
	~OmnThreadShellMgr1();

	OmnThreadShellPtr	getShell(const uint timerSec);
	bool		proc(const OmnThrdShellProcPtr &proc);
	bool		procAsync(vector<OmnThrdShellProcPtr> &procs);
	bool		procSync(vector<OmnThrdShellProcPtr> &procs);
	bool		returnShell(const OmnThreadShellPtr &theShell);
	bool		msgRcved(const OmnConnBuffPtr &buff, const u32 transId);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &state, 
							const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

	// Singleton class interface
	static OmnThreadShellMgr1 *	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &def);

private:
};
*/
#endif

