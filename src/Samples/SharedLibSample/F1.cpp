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
// Modification History:
// 03/25/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Samples/SharedLibSample/F1.h"

// #include "Tracer/TraceEntry.h"
// #include "aosUtil/Types.h"
// #include "Tracer/Tracer.h"
// #include <string>
#include <iostream>
using namespace std;

int *myarray;
SharedLibObj::SharedLibObj()
{
	cout << "SharedLibObj instance created" << endl;
}


SharedLibObj::~SharedLibObj()
{
}

