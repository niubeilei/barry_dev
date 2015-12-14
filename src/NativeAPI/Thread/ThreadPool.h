///////////////////////////////////////////////////////////////
// Created by:	Open Source Society
// Created:		10/03/2000
// Comments:
//
// Change History:
//	10/03/2000
//
// Created By Ken Lee, 2013/04/12	
///////////////////////////////////////////////////////////////
#ifndef Omn_Thread_ThreadPool_h
#define Omn_Thread_ThreadPool_h

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


class OmnThreadPool : public OmnThreadShellCaller
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultGetShellTimer = 15,
		eMaxThreadShells = 32 
	};

	OmnString			mName;
	OmnMutexPtr			mLock;
	//OmnCondVarPtr		mCondVar;
	//OmnMutexPtr			mReqLock;
	//OmnCondVarPtr		mReqCondVar;
	vector<OmnThreadShellPtr>	mShells;
	deque<OmnThrdShellProcPtr>	mProcessors;
	int					mGetShellTimer;
	//OmnThreadPtr		mThread;
	OmnString			mFileName;
	int					mFileLine;

	// Do not use the two:
	OmnThreadPool(const OmnThreadPool &rhs);
	OmnThreadPool & operator = (const OmnThreadPool &rhs);

public:
	OmnThreadPool(const OmnString &name, const OmnString &file_name, const int file_line);
	~OmnThreadPool();

	OmnThreadShellPtr	getShell(const uint timerSec);
	bool		proc(const OmnThrdShellProcPtr &proc);
	bool		procAsync(const vector<OmnThrdShellProcPtr> &procs);
	bool		procSync(const vector<OmnThrdShellProcPtr> &procs);
	bool		returnShell(const OmnThreadShellPtr &theShell);
	//bool		msgRcved(const OmnConnBuffPtr &buff, const u32 transId);

	/*
	// OmnThreadedObj interface
	virtual bool	threadFunc(
						OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool	checkThread(OmnString &errmsg, const int tid) const; 
	*/

	// Chen Ding, 2013/08/18
	static bool	runProcSync(const vector<OmnThrdShellProcPtr> &procs);
	static bool	runProcAsync(const vector<OmnThrdShellProcPtr> &procs);
	static bool	runProcAsync(const OmnThrdShellProcPtr &proc);
};
#endif

