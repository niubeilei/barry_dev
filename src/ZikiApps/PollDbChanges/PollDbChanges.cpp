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
// 12/19/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ZikiApps/PollDbChanges/PollDbChanges.h"

#include "alarm_c/alarm.h"
#include "Book/BookMgr.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "TinyXml/tinyxml.h"
#include "Util/File.h"
#include "Util/a_string.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "Util1/Wait.h"
#include "DataStore/DbTrackTable.h"


AosPollDbChanges::AosPollDbChanges()
{
}


AosPollDbChanges::~AosPollDbChanges()
{
}


// This function monitors the changes for the table 'tablename'. 
// If there are change records that is newer than 'snappoint', 
// it retrieves the changes and returns the new snappoint.
//
// Returns:
// It returns 0 if no changes were detected. Otherwise, it returns
// the seqno detected. 
int
AosPollDbChanges::process(
		const u32 snappoint, 
		const OmnString &tablename) 
{
	// This process will poll the database every eFrequence number of
	// ms until 'eNumTries' have been tried. 
	int tries = 0;
	while (tries++ < eNumTries)
	{
		int seqno = AosDbTrackTable::pollChanges(snappoint, tablename);
		if (seqno > 0) return seqno;
		OmnWaitSelf->wait(0, eFrequence);
	}

	// Timed out. Return 0
	return 0;
}

