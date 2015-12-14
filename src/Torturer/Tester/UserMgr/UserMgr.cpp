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
// Friday, January 18, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////


#include "UserMgr.h"
#include <list>
#include <string>

std::list<std::string> gUserNames;

bool AddUser(char* userName, char* userIp, u16 userPort)
{
	if (userName == NULL)
	{
		return false;
	}

	if (gUserNames.size() >= 1000)
	{
		return false;
	}
	std::list<std::string>::iterator iter = std::find(gUserNames.begin(), gUserNames.end(), userName);
	if (iter != gUserNames.end())
	{
		return false;
	}
	gUserNames.push_back(userName);

	return true;
}


bool DelUser(char* userName)
{
	if (userName == NULL)
	{
		return false;
	}
	std::list<std::string>::iterator iter = std::find(gUserNames.begin(), gUserNames.end(), userName);
	if (iter != gUserNames.end())
	{
		gUserNames.remove(userName);
		return true;
	}
	return false;
}

