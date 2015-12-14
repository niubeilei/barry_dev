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
// This file defines some additional functions for the STL::string class.
//   
//
// Modification History:
// 11/28/2007 created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/a_string.h"

#include "Util/String.h"
#include <string>

std::string & operator << (std::string &lhs, const OmnString &rhs)
{
 	lhs += rhs.data();
 	return lhs;
}


std::string & operator << (std::string &lhs, const std::string &rhs)
{
	lhs += rhs;
	return lhs;
}


std::string & operator << (std::string &lhs, const char *rhs)
{
	if (!rhs) return lhs;
	lhs += rhs;
	return lhs;
}


std::string & operator << (std::string &lhs, const u32 rhs)
{
	char buf[20];
    sprintf(buf, "%u", rhs);
	return lhs += buf;
}


std::string & operator << (std::string &lhs, void *rhs)
{
	char buf[20];
    sprintf(buf, "%lx", (unsigned long)rhs);
	return lhs += buf;
}

std::string aos_string_itos(const int value)
{
	char buff[20];
	sprintf(buff, "%d", value);
	return std::string(buff);
}


