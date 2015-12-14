////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestDriveUser.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


struct net_device *card_test(const char *tester_select, const char *thread_count, const char *buffer_size)
{
	OmnString rslt;
	char cmd[100];

	// 
	// Prepare the command
	//
	sprintf(cmd, "card_test %s %s %s", tester_select, thread_count, buffer_size);

	// 
	// Call the function through the KAPI
	//
	if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
	{
		// 
		// The KAPI call failed. Raise an alarm and return null.
		//
		OmnAlarm << "Failed to run KAPI: card_test" << enderr;
		return 0;
	}
	char *buff = new char[rslt.length()];
	memcpy(buff, rslt.data(), rslt.length());
	return (struct net_device *)buff;
}


struct net_device *get_card_test_result(void)
{
	OmnString rslt;
	char cmd[100];

	// 
	// Prepare the command
	//
	sprintf(cmd, "get_card_test_result");

	// 
	// Call the function through the KAPI
	//
	if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
	{
		// 
		// The KAPI call failed. Raise an alarm and return null.
		//
		OmnAlarm << "Failed to run KAPI: card_test" << enderr;
		return 0;
	}
	char *buff = new char[rslt.length()];
	memcpy(buff, rslt.data(), rslt.length());
	return (struct net_device *)buff;
}
