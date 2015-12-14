////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 11/28/2007 created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_string_h
#define Omn_Util_string_h

#include "aosUtil/Types.h"
#include <string>
#include <list>

class OmnString;

typedef std::list<std::string>  	AosStringList;

extern std::string & operator << (std::string &lhs, const OmnString &rhs);
extern std::string & operator << (std::string &lhs, const std::string &rhs);
extern std::string & operator << (std::string &lhs, const char *rhs);
extern std::string & operator << (std::string &lhs, const u32 rhs);
extern std::string & operator << (std::string &lhs, void *);
extern std::string aos_string_itos(const int value);

#endif
