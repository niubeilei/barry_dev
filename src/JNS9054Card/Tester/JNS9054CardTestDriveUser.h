////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestDriveUser.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef JNS9054Card_Tester_JNS9054CardTestDriveUser_H
#define JNS9054Card_Tester_JNS9054CardTestDriveUser_H

struct net_device *card_test(const char *tester_select, const char *thread_count, const char *buffer_size);
struct net_device *get_card_test_result(void);

#endif 
