#include "CertServerTester.h"

#include "alarm/Alarm.h"
#include "net/tcp.h"
#include "porting/addr.h"
#include "util/file.h"
#include "util/tracer.h"
#include "util/buffer.h"
#include <iostream>
#include <string>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( AosCertServerTest );

static std::string sg_fn1 = "FirstMsg.txt";
static std::string sg_fn2 = "SecondMsg.txt";
static std::string sg_local_addr = "192.168.197.128";
static int		   sg_local_port = 0;
static std::string sg_remote_addr = "192.168.197.128";
static int		   sg_remote_port = 5000;

void 
AosCertServerTest::setUp()
{
	// 
	// Read the first message.
	//
	int ret = aos_file_read_file(sg_fn1.data(), 
			&mFirstMsg, &mFirstMsgLen);
	aos_assert(!ret);

	// Read the second message
	ret = aos_file_read_file(sg_fn2.data(), 
			&mSecondMsg, &mSecondMsgLen);
	aos_assert(!ret);

	// Create the connection
	u32 local_addr;
	aos_assert(!aos_addr_to_u32(sg_local_addr.data(), &local_addr));

	u32 remote_addr;
	aos_assert(!aos_addr_to_u32(sg_remote_addr.data(), &remote_addr));

	aos_assert(!aos_tcp_connect(&mSock, 
			local_addr, sg_local_port, 1,
			remote_addr, sg_remote_port, 1, 
			&mLocalPort, &mRemotePort));	

	aos_trace("Connection created: %d", mSock); 
}


void 
AosCertServerTest::tearDown()
{
}


void 
AosCertServerTest::testOneDialog()
{
	int ret;
   
	// 
	// Send the first message
	//
 	ret = aos_tcp_send_raw(mSock, mFirstMsg, mFirstMsgLen, 0);
	CPPUNIT_ASSERT(!ret);
	aos_trace("Sent first message: %s", mFirstMsg);

	aos_buffer_t *buffer = aos_create_conn_read_buffer();
	CPPUNIT_ASSERT(buffer);

	// 
	// Read the first resposne
	//
	u32 remote_addr;
	u16 remote_port;
	int is_timeout;
	int is_conn_broken;
	ret = aos_conn_read_from(
			mSock, buffer, 0, 0, &is_timeout, 
			&remote_addr, &remote_port, &is_conn_broken);
	CPPUNIT_ASSERT(!ret);
	CPPUNIT_ASSERT(!is_timeout);
	CPPUNIT_ASSERT(!is_conn_broken);

	aos_trace("Read first response: %s", buffer->buff);

	//
	// Send the second message
	//
 	ret = aos_tcp_send_raw(mSock, mSecondMsg, mSecondMsgLen, 0);
	CPPUNIT_ASSERT(!ret);
	aos_trace("Sent Second message: %s", mSecondMsg);

	ret = aos_conn_read_from(
			mSock, buffer, 0, 0, &is_timeout, 
			&remote_addr, &remote_port, &is_conn_broken);
	CPPUNIT_ASSERT(!ret);
	CPPUNIT_ASSERT(!is_timeout);
	CPPUNIT_ASSERT(!is_conn_broken);

	aos_trace("Read second response: %s", buffer->buff);
}

