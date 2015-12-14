////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MySqlInclude.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Porting_MySqlInclude_h
#define Omn_Porting_MySqlInclude_h


// Unix part
#ifdef OMN_PLATFORM_UNIX
#include "mysql/mysql.h"




//
// Microsoft part
//
#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include "mysql.h"


#endif

#endif




