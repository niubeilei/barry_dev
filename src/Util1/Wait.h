////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Wait.h
// Description:
//	This class uses ::select() to provide a precise sleep() function.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_Wait_h 
#define Omn_Util1_Wait_h

//#include "Config/ConfigEntity.h"
#include "Porting/Timeval.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"





OmnDefineSingletonClass(OmnWaitSingleton,
						OmnWait,
						OmnWaitSelf,
                		OmnSingletonObjId::eWait, 
						"Wait");


class OmnWait
{
private:
    fd_set 				mReadfds;

	int					mTheSock;
    int 				mNfds;

	enum 
	{
		eMinUsec = 10000
	};

public:
	OmnWait();
	virtual ~OmnWait();

	static OmnWait *	getSelf();
	bool	wait(const int sec, const int usec);
	bool	wait(const int msec) {return wait(msec/1000, (msec%1000) * 1000);}
	bool	start();
	bool	stop();

	virtual bool	        config(const AosXmlTagPtr &def);
};
#endif



