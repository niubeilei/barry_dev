///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//
// Change History:
//	06/02/2005 File Created
///////////////////////////////////////////////////////////////
#include "CliDeamon/Tester2/CliDaemonTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "CliDeamon/Tester2/CliDaemonTortureTestPkg.h"

OmnTestSuitePtr AosCliDaemonTestSuite::getSuite(const OmnIpAddr &serverAddr,
						const int	serverPort,
						const OmnIpAddr	&clientAddr,
						const int	clientPort,
						const int 	maxNumOfLogin,
						const OmnString	&filePath,
						const OmnString &logFilePath,
						const int	numOfRepeat,
						const int	numOfConcurConn,
						const int	numOfTries)
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
						      numOfRepeat,
						      numOfConcurConn,
						      numOfTries));

  return suite;
}
