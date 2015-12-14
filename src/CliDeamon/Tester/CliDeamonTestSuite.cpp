///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//
// Change History:
//	06/02/2005 File Created
///////////////////////////////////////////////////////////////
#include "CliDeamon/Tester/CliDeamonTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "CliDeamon/Tester/CliDeamonTestPkg.h"

OmnTestSuitePtr AosCliDeamonTestSuite::getSuite(const OmnIpAddr &serverAddr,
						const int	serverPort,
						const OmnIpAddr	&clientAddr,
						const int	clientPort,
						const int 	maxNumOfLogin,
						const OmnString	&filePath,
						const int	numOfRepeat,
						const int	numOfConcurConn,
						const int	numOfTries)
{
  OmnTestSuitePtr suite = OmnNew 
    OmnTestSuite("AosCliDaemonTestPkg", "Cli Deamon Test Suite");
                            
  //                      
  // Now add all the testers
  //
  suite->addTestPkg(OmnNew AosCliDeamonTestPkg(serverAddr,
						      serverPort,
						      clientAddr,
						      clientPort,
						      maxNumOfLogin,
						      filePath,
						      numOfRepeat,
						      numOfConcurConn,
						      numOfTries));

  return suite;
}
