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
#ifndef Snt_Util_FileWatcher_h
#define Snt_Util_FileWatcher_h

#include "Util1/TimerObj.h"


#define AOSFILE_COUNTING_WRITE			AosFileWatcher::updateWrite();			
#define AOSFILE_COUNTING_READ			AosFileWatcher::updateRead();	
#define AOSFILE_COUNTING_GETC			AosFileWatcher::updateGetc();	
#define AOSFILE_COUNTING_FLUSH			AosFileWatcher::updateFlush();	
#define AOSFILE_COUNTING_SEEK			AosFileWatcher::updateSeek();	
#define AOSFILE_COUNTING_REWIND			AosFileWatcher::updateRewind();	
#define AOSFILE_COUNTING_GETS			AosFileWatcher::updateGets();	
#define AOSFILE_COUNTING_FORCEFLUSH		AosFileWatcher::updateForceFlush();	


class AosFileWatcher : public virtual OmnTimerObj
{
	OmnDefineRCObject;

private:
	static int	smTotal;
	static int	smWrite;
	static int	smRead;
	static int	smGetc;
	static int	smFlush;
	static int	smSeek;
	static int	smRewind;
	static int	smGets;
	static int	smFcFlush;

	static int	smTotalLast;
	static int	smWriteLast;
	static int	smReadLast;
	static int	smGetcLast;
	static int	smFlushLast;
	static int	smSeekLast;
	static int	smRewindLast;
	static int	smGetsLast;
	static int	smFcFlushLast;

	AosFileWatcher() {}

public:
	virtual void timeout(const int timerid, const OmnString &name, void *parm);

	static void updateWrite() 		{smTotal++; smWrite++;}
	static void updateRead() 		{smTotal++; smRead++;}
	static void updateGetc() 		{smTotal++; smGetc++;}
	static void updateFlush() 		{smTotal++; smFlush++;}
	static void updateSeek() 		{smTotal++; smSeek++;}
	static void updateRewind() 		{smTotal++; smRewind++;}
	static void updateGets() 		{smTotal++; smGets++;}
	static void updateForceFlush() 	{smTotal++; smFcFlush++;}

	static void start();
};
#endif
