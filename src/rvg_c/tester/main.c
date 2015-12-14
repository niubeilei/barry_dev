////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/tester/rvg_xml_tester.h"

#include "alarm_c/alarm.h"
#include "rvg_c/tester/rsgcharset_tester.h"
#include "util_c/app.h"
#include <stdio.h>

int 
main(int argc, char **argv)
{
	aos_assert_r(!aos_app_init(), -1);
//	aos_rvg_xml_tester();
	aos_assert_r(!aos_rsgcharset_tester(), -1);
	return 0;
} 

