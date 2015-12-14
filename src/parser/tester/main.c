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
#include "parser/tester/parser_tester.h"

#include "util_c/global_data.h"

int 
main(int argc, char **argv)
{
	aos_global_data_init();
	aos_parser_tester();
	return 0;
} 

