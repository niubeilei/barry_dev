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


class AosCliDaemonTestSuite
{
 private:

 public:
  AosCliDaemonTestSuite(){}
  ~AosCliDaemonTestSuite(){}
  static OmnTestSuitePtr	getSuite(const OmnIpAddr &serverAddr,
					 const int	serverPort,
					 const OmnIpAddr	&clientAddr,
					 const int	clientPort,
					 const int 	maxNumOfLogin,
					 const OmnString	&filePath,
					 const OmnString        &logFilePath,
					 const int	numOfRepeat,
					 const int	numOfConcurConn,
					 const int	numOfTries);
};
#endif

