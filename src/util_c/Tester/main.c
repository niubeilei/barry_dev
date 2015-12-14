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
// 01/18/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/Tester/list_tester.h"

#include "util_c/Tester/dyn_array_tester.h"


int 
main(int argc, char **argv)
{
	aos_semantics_init();

	aos_list_tester();
	aos_dyn_array_tester();
	aos_value_tester();
	return 0;
} 

