#ifndef AOS_CERT_SERVER_TESTER_H 
#define AOS_CERT_SERVER_TESTER_H 

#include <cppunit/extensions/HelperMacros.h> 
#include "util/types.h"


class AosCertServerTest : public CppUnit::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( AosCertServerTest ); 
	CPPUNIT_TEST(testOneDialog); 
	CPPUNIT_TEST_SUITE_END(); 
	
private:
	char *			mFirstMsg;
	int				mFirstMsgLen;
	char *			mSecondMsg;
	int				mSecondMsgLen;
	int				mSock;
	u16				mLocalPort;
	u16				mRemotePort;

public: 
	void setUp(); 
	void tearDown(); 
	void testOneDialog(); 
}; 

#endif // AosCertServerTest_H 
