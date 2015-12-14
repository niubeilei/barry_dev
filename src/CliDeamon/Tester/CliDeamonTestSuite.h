///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//			test suite of cliDeamon
// 
// Change History:
//	09/15/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef Omn_CliDeamon_Tester_AosCliDeamonTestSuite_h
#define Omn_CliDeamon_Tester_AosCliDeamonTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/String.h"


class AosCliDeamonTestSuite
{
 private:

 public:
  AosCliDeamonTestSuite(){}
  ~AosCliDeamonTestSuite(){}
  static OmnTestSuitePtr	getSuite(const OmnIpAddr &serverAddr,
					 const int	serverPort,
					 const OmnIpAddr	&clientAddr,
					 const int	clientPort,
					 const int 	maxNumOfLogin,
					 const OmnString	&filePath,
					 const int	numOfRepeat,
					 const int	numOfConcurConn,
					 const int	numOfTries);
};
#endif

