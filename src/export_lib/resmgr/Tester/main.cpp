////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
// Created by CHK 2007-04-04
////////////////////////////////////////////////////////////////////////////

extern int ResMgrTest(int, char**);
extern int QuotaTester();

int main()
{
	ResMgrTest(0, 0);
	QuotaTester();

	return 0;
}
