////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//   
//
// Modification History:
// 03/25/2009 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Samples/SharedLibSample/F1.h"

#include <iostream>
using namespace std;

SharedLibObj *theobj;

int main(int argc, char **argv)
{
	char c;
	while (1)
	{
		cout << "0:    Exit" << endl;
		cout << "1:    Create Object" << endl;
		cout << "Please enter your command" << endl;
		cin >> c;
		switch (c)
		{
		case '0': 
			 return 0;

		case '1':
			 {
				 theobj = new SharedLibObj;
				 cout << "Created" << endl;
			 }
			 break;

		default:
			 break;
		}
	}

	return 0;
} 

