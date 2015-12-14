//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// HAC supports the following CLI commands:
// 1. xxx
// 2. xxx
// ...
//
// This function tortures HAC CLIs. 
//
// Modification History:
// 4/21/2007 Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#include "HacDaemon/Tester/HacCliTester.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/GenTable.h"


AosHacCliTester::AosHacCliTester()
{
    mBasicTestRepeat = 10000;
}


AosHacCliTester::~AosHacCliTester()
{
}


bool
AosHacCliTester::start()
{
    basicTest(mBasicTestRepeat, mTable);
    return true;
}


bool
AosHacCliTester::basicTest(const u32 repeat, const AosGenTablePtr &table)
{
    return true;
}


bool
AosHacCliTester::config()
{
	// 
	// This function will randomly determine how many CLIs to run. 
	// It then randomly select CLIs to run. 
	//
	// If there are any errors during the execution, it returns false.
	// Otherwise, it returns true.
	//
	return true;
}


bool	
AosHacCliTester::getUser(bool &correct, 
					const AosStreamType::E app, 
					OmnString &user)
{
	// 
	// This function gets either a correct user ('correct' == true)
	// or an incorrect user, for the given application 'app'. 
	// The class is configured in such a way that the application
	// 'app' has a list of valid users. If correct, it selects 
	// a user from the list. Otherwise, it randomly creates a
	// user name and makes sure the user is not in the application
	// user list. 
	//
	// Note that if the application has not users yet, it will 
	// generate an incorrect user even if 'correct' is true.
	// In this case, upon return, it changes 'correct' to false. 
	//
	// This function should always return true unless there are
	// program errors. 
	//
	OmnNotImplementedYet;
	return false;
}


bool	
AosHacCliTester::getPasswd(const bool correct, 
					const OmnString &user, 
					OmnString &passwd)
{
	// 
	// Given the user 'user', this function generates 
	// either a correct password (if 'correct' is true), 
	// or an incorrect password otherwise.
	//
	// If the user is not found, it simply generates a
	// password. 
	//
	// It should always return true, unless there are 
	// program errors.
	//
	OmnNotImplementedYet;
	return false;
}


bool	
AosHacCliTester::getSendIP(const bool correct,
					const OmnString &user, 
					const AosStreamType::E app,
					OmnIpAddr &sendIP)
{
	// 
	// This function gets an either a correct sender IP 
	// ('correct' == true) or incorrect sender IP ('correct' == false).
	//
	// Note that this class assumes the host runs the IP Proxy 
	// service. This service lets this host owns a large number of
	// IP addresses. An IP address is correct if it is owned by
	// this host and the HAC is configured in such a way that 
	// the user 'user' is allowed to access the application 'app' through
	// the send IP. 
	//
	// If the user 'user' is not found, it will simply generate
	// a random password.
	//
	// This function should always return true unless there are 
	// program errors.
	//
	OmnNotImplementedYet;
	return false;
}


bool	
AosHacCliTester::getRecvIPPort(const bool correct, 
					const AosStreamType::E app,
					const OmnString &user,
					OmnIpAddr &recvIP, 
					int &recvPort)
{
	// 
	// This function will generate either a correct receiver IP
	// and port (if 'correct' is true), or an incorrect receiver
	// IP and port. 
	//
	// A receiver IP/Port is correct if it is the IP address and 
	// port of the real server for the application 'app' and 
	// the HAC has been configured to allow the user 'user' to  
	// use the application 'app'. 
	//
	// This function should always return true unless there are
	// program errors.
	//
	OmnNotImplementedYet;
	return false;
}

