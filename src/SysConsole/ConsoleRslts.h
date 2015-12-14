////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysConsole_ConsoleRslts_h
#define AOS_SysConsole_ConsoleRslts_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"
#include <vector>
using namespace std;


class AosConsoleRslts : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	struct Entry
	{
		OmnString	info_id;
		OmnString	contents;
		bool		finished;

		Entry(const OmnString &info_id, 
			  const OmnString &contents, 
			  const bool finished)
		:
		info_id(info_id),
		contents(contents),
		finished(finished)
		{
		}
	};

	vector<Entry>	mEntries;

public:
	AosConsoleRslts();
	~AosConsoleRslts();

	bool addResults(const OmnString &info_id, 
					const OmnString &contents, 
					const bool finished);
private:
};
#endif

