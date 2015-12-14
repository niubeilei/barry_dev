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
// 08/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/FileWatcher.h"

#include "Debug/Debug.h"
#include "Util1/Timer.h"

int	AosFileWatcher::smTotal = 0;
int	AosFileWatcher::smWrite = 0;
int	AosFileWatcher::smRead = 0;
int	AosFileWatcher::smGetc = 0;
int	AosFileWatcher::smFlush = 0;
int	AosFileWatcher::smSeek = 0;
int	AosFileWatcher::smRewind = 0;
int	AosFileWatcher::smGets = 0;
int	AosFileWatcher::smFcFlush = 0;

int	AosFileWatcher::smTotalLast = 0;
int	AosFileWatcher::smWriteLast = 0;
int	AosFileWatcher::smReadLast = 0;
int	AosFileWatcher::smGetcLast = 0;
int	AosFileWatcher::smFlushLast = 0;
int	AosFileWatcher::smSeekLast = 0;
int	AosFileWatcher::smRewindLast = 0;
int	AosFileWatcher::smGetsLast = 0;
int	AosFileWatcher::smFcFlushLast = 0;

void
AosFileWatcher::start()
{
	OmnTimerObjPtr thisptr = OmnNew AosFileWatcher();
	//OmnTimer::getSelf()->startTimer("fw", 3, 0, thisptr, 0);
}


void
AosFileWatcher::timeout(const int timerid, const OmnString &name, void *parm)
{
	cout << __FILE__ << ":" << __LINE__ << endl
		<< "============ File Statistics ============" << endl
		<< "    total:      " << smTotal	<< ":" << smTotal - smTotalLast << endl 
		<< "    write:      " << smWrite 	<< ":" << smWrite - smWriteLast << endl
		<< "    read:       " << smRead 	<< ":" << smRead - smReadLast << endl
		<< "    getc:       " << smGetc 	<< ":" << smGetc - smGetcLast << endl
		<< "    flush:      " << smFlush 	<< ":" << smFlush - smFlushLast << endl
		<< "    seek:       " << smSeek 	<< ":" << smSeek - smSeekLast << endl
		<< "    rewind:     " << smRewind 	<< ":" << smRewind - smRewindLast << endl
		<< "    fgets:      " << smGets 	<< ":" << smGets - smGetsLast << endl
		<< "    forceflush: " << smFcFlush 	<< ":" << smFcFlush - smFcFlushLast 
		<< endl;

	smTotalLast = smTotal;
	smWriteLast = smWrite;
	smReadLast = smRead;
	smGetcLast = smGetc;
	smFlushLast = smFlush;
	smSeekLast = smSeek;
	smRewindLast = smRewind;
	smGetsLast = smGets;
	smFcFlushLast = smFcFlush;

	OmnTimerObjPtr thisptr = OmnNew AosFileWatcher();
	//OmnTimer::getSelf()->startTimer("fw", 3, 0, thisptr, 0);
}
