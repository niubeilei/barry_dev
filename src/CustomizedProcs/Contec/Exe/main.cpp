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
// 
////////////////////////////////////////////////////////////////////////////

#include <iostream>
#ifdef __amd64__
#include "../../../SeReqProc/Lib.h"
#else
#include "../../../SeReqProc/Lib32.h"
#endif

using namespace std;
  
int 
main(int argc, char **argv)
{
	char *sor = argv[1];
	char *des = argv[2];
	
	int num = UnPackFile(sor, des);

	cout << "parsing ecg, num :" << num << endl;

	return 0;
	
}

