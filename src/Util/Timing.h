////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timing.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_Timing_h
#define Snt_Util_Timing_h

#include "Porting/TimeOfDay.h"


#ifndef OmnTimingFlag
#define OmnTimingFlag //
#endif

class OmnTiming
{
private:
	struct Timestamps
	{
		timeval	tv;
		char	tag;
	};

	enum
	{
		eMaxEntry = 10
	};
	
	OmnString	mName;
	int			mIndex;
	Timestamps	mEntry[eMaxEntry];

public:
	OmnTiming()
	:
	mIndex(0)
	{
	}

	void	setName(const OmnString &name) {mName = name;}
	void	resetTiming()
			{
				for (int i=0; i<eMaxEntry; i++) 
				{
					mEntry[i].tv.tv_sec = 0;
					mEntry[i].tv.tv_usec = 0;
				}
				OmnGetTimeOfDay(&mEntry[0].tv);
				mIndex = 1;
			}

	void	snaptime(const char c)
			{
				if (mIndex < 0 || mIndex >= eMaxEntry) return;
				
				mEntry[mIndex].tag = c;
				OmnGetTimeOfDay(&mEntry[mIndex++].tv);
			}

	void	printout()
			{
				if (mIndex < 2)
				{
					return;
				}

				for (int i=1; i<mIndex; i++)
				{
					cout << mEntry[i].tag << ":"
						 << mEntry[i].tv.tv_usec - 
							mEntry[i-1].tv.tv_usec
						 << " ";
				}

				cout << endl;
			}
};

#endif

