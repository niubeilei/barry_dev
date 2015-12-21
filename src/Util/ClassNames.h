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
// Modification History:
// 10/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_ClassNames_h
#define AOS_Util_ClassNames_h

#include "Util/String.h"

class AosClassName
{
public:
	enum E
	{
		eInvalid,

		eAosBuff,
		eAosDataRecord,
		eAosDocFileMgr,
		eAosFile,
		eAosRundata,
		eAosTrans,
		eAosXmlTag,
		eAosRaftLogEntry,
		eAosRaftMsg,

		eMax
	};

	static OmnString toString(const E code)
	{
		switch (code)
		{
		case eAosBuff: 			return "AosBuff";
		case eAosFile: 			return "AosFile";
		case eAosDataRecord:	return "AosDataRecord";
		case eAosDocFileMgr:	return "AosDocFileMgr";
		case eAosRundata:   	return "AosRundata";
		case eAosTrans:			return "AosTrans";
		case eAosXmlTag:		return "AosXmlTag";
		case eAosRaftLogEntry:	return "AosRaftLogEntry";
		case eAosRaftMsg:		return "AosRaftMsg";
		default: break;
		}
		return "InvalidClass";
	}

	static bool isValid(const E code)
	{
		return (code > eInvalid && code < eMax);
	}
};

#endif
				
