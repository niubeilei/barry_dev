///////////////////////////////////////////////////////////////
// Created by:	Chen Ding
// Created:		06/02/2005	
// Comments:
// 
// Change History:
//	06/02/2005 File Created
///////////////////////////////////////////////////////////////
#ifndef Omn_Proxy_Tester_aosProxyTestSuite_h
#define Omn_Proxy_Tester_aosProxyTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/String.h"


class AosProxyTestSuite
{
private:

public:
	AosProxyTestSuite() {}
	~AosProxyTestSuite() {}

	static OmnTestSuitePtr	getSuite(const OmnIpAddr 	&clientAddr,
							    	 const OmnIpAddr 	&serverAddr,
								     const OmnIpAddr 	&localAddr,
								     const OmnIpAddr 	&bouncerAddr,
								     const int			clientPort,								     
									 const int			numOfTries,
									 const int			connNum,
									 const int			repeat,
									 const int			blockSize);
};
#endif

