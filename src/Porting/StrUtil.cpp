////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrUtil.cpp
// Description:
//   
//
// Modification History:
// 		getDomainName(...): Failed to retrieve domain name if it 
// 		does not have breckets. 
////////////////////////////////////////////////////////////////////////////

#include "Porting/StrUtil.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"




void
OmnStrUtil::replaceSubstr(char *target, 
						  const int targetLength, 
						  const int targetMaxLength,
						  const int replacedPos, 
						  const int replacedLength,
					 	  const char *replacingStr, 
						  const int replacingLength)
{
	//
	// It replaces a substring [replacedPos, replacedLength], with 
	// the string 'fromStr'. 
	// 

	// 
	// Sanity checking. 
	//
	if (replacedPos < 0 || replacedPos >= targetLength)
	{
		OmnAlarm << OmnErrId::eAlarmProgramError
			<< "Invalid replacedPos: " << replacedPos << enderr;
		return;
	}

	if (replacedLength < 0 || replacedPos + replacedLength > targetLength)
	{
		OmnAlarm << OmnErrId::eAlarmProgramError
			<< "Invalid replacedLength: " << replacedPos
			<< ":" << replacedLength << ":" << targetLength << enderr;
			return;
	}

	if (replacingLength < 0 || replacingLength > targetMaxLength ||
		(replacingLength > replacedLength && 
			replacingLength - replacedLength > targetMaxLength - targetLength))
	{
		OmnAlarm << OmnErrId::eAlarmProgramError
			<< "Invalid replacingLength: " << replacingLength
			<< ":" << targetLength << ":" << targetMaxLength << enderr;
		return;
	}

	int delta = replacingLength - replacedLength;
	if (delta > 0)
	{
		//
		// Sanity checking
		// 
		if (!replacingStr)
		{
			//
			// Chen Ding, 10/10/2003, 2003-0267
			//
			OmnAlarm << OmnErrId::eAlarmProgramError
				<< "Null repacing string: " << enderr;
			return;
		}

		//
		// The replacing string is longer than the replaced. Need to 
		// make room for the extra bytes. 
		// shift the data first and then replace
		//
		int stopPos = replacedPos + replacedLength;
		for (int i=targetLength; i>=stopPos; i--)
		{
			target[i+delta] = target[i];
		}

		//
		// Copy the address
		//
		memcpy(&target[replacedPos], replacingStr, replacingLength);
		return;
	}

	if (delta < 0)
	{
		//
		// New substring is shorter than the old one.
		//
		int startPos = replacedPos + replacedLength + delta;
		int stopPos = targetLength + delta;
		for (int i=startPos; i<=stopPos; i++)
		{
			target[i] = target[i-delta];
		}

		//
		// Copy the address
		//
		if (replacingStr)
		{
			memcpy(&target[replacedPos], replacingStr, replacingLength);
		}

		//
		// Otherwise, it is to remove instead of replacing the substring
		// Therefore, this second step is not needed.
		// 
		return;
	}

	//
	// Otherwise, does not need to shift.
	//
	if (!replacingStr)
	{
		OmnAlarmProgError << "Null replacing string!" << enderr;
		return;
	}

	memcpy(&target[replacedPos], replacingStr, replacingLength);
}

/*
bool
OmnStrUtil::getDomainName(const OmnString &mgcpEid,
						  OmnString &domainName,
						  OmnString &err)
{
	// 
	// MGCP EID is in the form:
	// 		aaln/x@[xxxxxxxxx]
	// This function retrieves xxxxxxxxx out of it.
	//
	const char *data = mgcpEid.data();
	int length = mgcpEid.length();

	int index = 0;
	while (index < length && data[index] != '@') index++;

	if (index >= length)
	{
		// 
		// Didn't find it. 
		//
		err = "Missing @: ";
		err << mgcpEid;
		return false;
	}

	// 
	// Now data[index] is '@'
	// Next, check whether there is '['
	//
	index++;

	if (data[index] == '[')
	{
		// 
		// Chen Ding, 05/21/2003, Change-0001
		//
		length--;
		index++;
	}

	// 
	// Now, index points to the beginning of the domain name. 
	// Need to confirm the end is ']'. 
	// Domain name starts at index, and length = 'length' - 'index'
	//
	domainName.assign(&data[index], length - index);
	return true;
}
*/
