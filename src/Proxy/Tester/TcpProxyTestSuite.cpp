///////////////////////////////////////////////////////////////
// Created by:	Chen Ding
// Created:		06/02/2005	
// Comments:
//
// Change History:
//	06/02/2005 File Created
///////////////////////////////////////////////////////////////
#include "Proxy/Tester/TcpProxyTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "Proxy/Tester/TcpProxyTester.h"


OmnTestSuitePtr		
AosProxyTestSuite::getSuite(const OmnIpAddr 	&clientAddr,
					    	const OmnIpAddr 	&serverAddr,
						    const OmnIpAddr 	&localAddr,
						    const OmnIpAddr 	&bouncerAddr,
						    const int			clientPort,								     
						    const int			numOfTries,
							const int			connNum,
							const int			repeat,
							const int			blockSize)
{                           
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("AosProxyTester", "Proxy Library Test Suite");
                            
	//                      
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosTcpProxyTester(clientAddr,
										       serverAddr,
										       localAddr,
										       bouncerAddr,
											   clientPort,
											   numOfTries,
											   connNum,
											   repeat,
											   blockSize));

	return suite;
}

