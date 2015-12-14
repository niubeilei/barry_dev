///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//			test suite of cliDeamon
// 
// Change History:
//	09/15/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef Omn_CliDaemon_Tester_AosCliDaemonTestSuite_h
#define Omn_CliDaemon_Tester_AosCliDaemonTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "Thread/Mutex.h"
#include "Tester/TestSuite.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "CliDeamon/Tester3/Constants.h"

class AosCliDaemonTestSuite : public OmnThreadedObj, OmnTestSuite
{
 private:
 	 OmnTestMgrPtr mTestMgr;
     OmnThreadPtr mThread;
	 OmnMutexPtr mLock;
	 bool mFinished;
 public:
 	AosCliDaemonTestSuite(const OmnString &suiteId,
                                          const OmnString &desc);
  	virtual ~AosCliDaemonTestSuite();
  	static OmnTestSuitePtr	getSuite(const OmnIpAddr &serverAddr,
					 const int	serverPort,
					 const OmnIpAddr	&clientAddr,
					 const int	clientPort,
					 const int 	maxNumOfLogin,
					 const OmnString	&filePath,
					 const OmnString        &logFilePath,
					 const int	numOfTries,
					 UserListEntry *user,
					 OmnMutexPtr mutex);
					 
	virtual int             start(const OmnTestMgrPtr &testMgr);


        //
        // ThreadedObj interface
        //
   	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
     	virtual bool signal(const int threadLogicId);
    	virtual void heartbeat();
    	virtual bool checkThread(OmnString &err, const int thrdLogicId) const;
private:
        int run();


};
#endif

