///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//
// Change History:
//	06/02/2005 File Created
///////////////////////////////////////////////////////////////
#include "CliDeamon/Tester3/CliDaemonTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "CliDeamon/Tester3/CliDaemonTortureTestPkg.h"


AosCliDaemonTestSuite::AosCliDaemonTestSuite(const OmnString &suiteId,
                                             const OmnString &desc)
:
OmnObject(OmnClassId::eOmnTestSuite),
OmnDbObj(OmnClassId::eOmnTestSuite, OmnStoreId::eTest),
OmnTestSuite(suiteId,desc),
mLock(OmnNew OmnMutex()),
mFinished(false)
{
        if (mThread.isNull())
        {
                OmnThreadedObjPtr thisPtr(this, false);
                mThread = OmnNew OmnThread(thisPtr, "CliDaemonTortureTest", 0, false, true, __FILE__, __LINE__);
        }
}
AosCliDaemonTestSuite::~AosCliDaemonTestSuite()
{
}



OmnTestSuitePtr AosCliDaemonTestSuite::getSuite(const OmnIpAddr &serverAddr,
						const int	serverPort,
						const OmnIpAddr	&clientAddr,
						const int	clientPort,
						const int 	maxNumOfLogin,
						const OmnString	&filePath,
						const OmnString &logFilePath,
						const int	numOfTries,
						UserListEntry *user,
						OmnMutexPtr mutex)
{
  OmnTestSuitePtr suite = OmnNew 
    OmnTestSuite("AosCliDaemonTorturePkg", "Cli Deamon Test Suite");
                            
  //                      
  // Now add all the testers
  //
  suite->addTestPkg(OmnNew AosCliDaemonTortureTestPkg(serverAddr,
						      serverPort,
						      clientAddr,
						      clientPort,
						      maxNumOfLogin,
						      filePath,
						      logFilePath,
						      numOfTries,
						      user,
						      mutex));

  return suite;
}


int
AosCliDaemonTestSuite::start(const OmnTestMgrPtr &testMgr)
{
	mTestMgr = testMgr;
	if(mThread)
        {
                mThread->start();
      	}
      else
        {
                OmnAlarm << "Thread is null!!!" << enderr;
        }

        return 0;
}


int
AosCliDaemonTestSuite::run()
{
        OmnTrace << "Suite: " << mSuiteName << " with "
                << mPackages.entries() << " packages!" << endl;

        mPackages.reset();
        OmnTestPkgPtr package;

        mTotalTc = 0;
        mTotalFailed = 0;
        while (mPackages.hasMore() && mTotalFailed < eMaxFailed)
          {
                package = mPackages.crtValue();
                mPackages.next();

                mTestMgr->setCrtPkg(package);
                package->setTestMgr(mTestMgr);
                package->start();
                mTestMgr->setCrtPkg(0);

                mTotalTc += package->getTotalTcs();
                mTotalFailed += package->getTotalFailedTcs();
          }

        mTestMgr->addRslt(mTotalTc, mTotalFailed);
        return mTotalTc;
}


bool
AosCliDaemonTestSuite::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    //
    //
    OmnTrace << "Enter OmnKeyouTestSuite::threadFunc." << endl;

    while ( state == OmnThrdStatus::eActive )
     {
        run();
        mFinished = true;
        state = OmnThrdStatus::eStop;
     }

    return true;
}

bool
AosCliDaemonTestSuite::signal(const int threadLogicId)
{
        return true;
}


void
AosCliDaemonTestSuite::heartbeat()
{
        //
        // Currently not managed by ThreadMgr. Do nothing.
        //
}


bool
AosCliDaemonTestSuite::checkThread(OmnString &err, const int thrdLogicId) const
{
        //
        // Currently not managed by ThreadMgr. Always return true
        //
    return true;
}



