////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2011/05/02	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_IILIdx_h
#define Omn_SEUtil_IILIdx_h

#include "alarm_c/alarm.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"


#define	AOSIILIDX_IDXSEP ','


class AosIILIdx 
{
public:
	enum
	{
		eMaxLevelIndex = 3,
		eMaxNumLevels = eMaxLevelIndex + 1,
		eInvalidIndex = -5,
		eStartIndex = -10,
		eMaxEntries = 2000000
	};

	i64		mIdx[eMaxNumLevels];

public:
	AosIILIdx();
	~AosIILIdx();
	
	void		reset();
	void		setBegin() 
	{
		for (int i=0; i<eMaxNumLevels; i++)
		{
				mIdx[i] = 0;
		}
	}
	void		setEnd()
	{
		for (int i=0; i<eMaxNumLevels; i++)
		{
				mIdx[i] = -5;
		}
	}

	i64			getIdx(const int level) const; 
	bool		setIdx(const int level, const i64 &idx);
	bool		isStart(const int level) const;
	bool		isValid(const int level) const;
	bool		notFound(const int level) const;
	bool		setStart(const int level);
	bool		setInvalid();
	bool		setInvalid(const int level);
	bool 		next(const int level, const bool reverse);
	OmnString	toString() const; 
	bool		setValue(const OmnString &value); 
	bool 		isFinished()
	{
		return mIdx[0] == -5;
	}
	
	void 		setFinished()
	{
		mIdx[0] = -5;
	}

	static	int			cmpPos(const AosIILIdx &idx1, const AosIILIdx &idx2);
	static	int			cmpPos(const AosIILIdx &idx1, const AosIILIdx &idx2, const int cur_level);

	// Chen Ding, 2013/03/03
	bool isAtBeginning(const int level, const bool reverse);
};
#endif
