////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommandMan.h
// Description:
//   
//
// Modification History:
//				Created by Harry Long, 12/5/2006
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CommandMan_CommandMan_h
#define Omn_CommandMan_CommandMan_h

#include "Tracer/TraceEntry.h"
#include "Tracer/Tracer.h"
#include "XmlParser/Ptrs.h"

#include "aosUtil/Types.h"
#include "Util/File.h"
#include <string.h>
#include <stdio.h>
#include <iostream>


extern int aos_Generate_MD5(const char* path, char* md5);
extern int aos_comman_Authenticate(const char* path);


#endif
